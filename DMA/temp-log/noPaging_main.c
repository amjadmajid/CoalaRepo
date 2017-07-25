#include <msp430.h> 
#include <libio/log.h>
#include <libmspbuiltins/builtins.h>
#include <libio/log.h>
#include <libmsp/mem.h>
#include <libmsp/periph.h>
#include <libmsp/clock.h>
#include <libmsp/watchdog.h>
#include <libmsp/gpio.h>

#define NIL 0 // like NULL, but for indexes, not real pointers

#define DICT_SIZE         512
#define BLOCK_SIZE         64

#define NUM_LETTERS_IN_SAMPLE        2
#define LETTER_MASK             0x00FF
#define LETTER_SIZE_BITS             8
#define NUM_LETTERS (LETTER_MASK + 1)

typedef unsigned index_t;
typedef unsigned letter_t;
typedef unsigned sample_t;

// NOTE: can't use pointers, since need to ChSync, etc
typedef struct _node_t {
    letter_t letter; // 'letter' of the alphabet
    index_t sibling; // this node is a member of the parent's children list
    index_t child;   // link-list of children
} node_t;


unsigned overflow=0;
#ifdef BOARD_MSP_TS430
__attribute__((interrupt(51)))
    void TimerB1_ISR(void){
        TBCTL &= ~(0x0002);
        if(TBCTL && 0x0001){
            overflow++;
            TBCTL |= 0x0004;
            TBCTL |= (0x0002);
            TBCTL &= ~(0x0001);
        }
    }
__attribute__((section("__interrupt_vector_timer0_b1"),aligned(2)))
void(*__vector_timer0_b1)(void) = TimerB1_ISR;
#endif
///// function Prototypes
void task_init();
void task_init_dict();
void task_sample();
void task_measure_temp();
void task_letterize();
void task_compress();
void task_find_sibling();
void task_add_node();
void task_add_insert();
void task_append_compressed();
void task_print();
void task_done();

__nv letter_t _v_letter;
__nv unsigned _v_letter_idx;
__nv sample_t _v_prev_sample;
__nv index_t _v_out_len;
__nv index_t _v_node_count;
__nv node_t _v_dict[DICT_SIZE];
__nv sample_t _v_sample;
__nv index_t _v_sample_count;
__nv index_t _v_sibling;
__nv index_t _v_child;
__nv index_t _v_parent;
__nv index_t _v_parent_next;
__nv node_t _v_parent_node;
__nv node_t _v_compressed_data[BLOCK_SIZE];
__nv node_t _v_sibling_node;
__nv index_t _v_symbol;

static sample_t acquire_sample(letter_t prev_sample)
{
    letter_t sample = (prev_sample + 1) & 0x03;
    return sample;
}

void task_init()
{
    LOG("init\r\n");
    _v_parent_next = 0;
    LOG("init: start parent %u\r\n", _v_parent);
    _v_out_len = 0;
    _v_letter = 0;
    _v_prev_sample = 0;
    _v_letter_idx = 0;;
    _v_sample_count = 1;
    os_jump(1);
}

void task_init_dict()
{
    LOG("init dict: letter %u\r\n", _v_letter);

    node_t node = {
        .letter = _v_letter,
        .sibling = NIL, // no siblings for 'root' nodes
        .child = NIL, // init an empty list for children
    };
    int i = _v_letter;
    _v_dict[i] = node;
    _v_letter++;
    if (_v_letter < NUM_LETTERS) {
        os_jump(0);
    } else {
        _v_node_count = NUM_LETTERS;
        os_jump(1);
    }
}

void task_sample()
{
    LOG("sample: letter idx %u\r\n", _v_letter_idx);

    unsigned next_letter_idx = _v_letter_idx + 1;
    if (next_letter_idx == NUM_LETTERS_IN_SAMPLE)
        next_letter_idx = 0;

    if (_v_letter_idx == 0) {
        _v_letter_idx = next_letter_idx;
        os_jump(1);
    } else {
        _v_letter_idx = next_letter_idx;
        os_jump(2);
    }
}

void task_measure_temp()
{
    sample_t prev_sample;
    prev_sample = _v_prev_sample;

    sample_t sample = acquire_sample(prev_sample);
    LOG("measure: %u\r\n", sample);
    prev_sample = sample;
    _v_prev_sample = prev_sample;
    _v_sample = sample;
    os_jump(1);
}

void task_letterize()
{
    unsigned letter_idx = _v_letter_idx;
    if (letter_idx == 0)
        letter_idx = NUM_LETTERS_IN_SAMPLE;
    else
        letter_idx--;
    unsigned letter_shift = LETTER_SIZE_BITS * letter_idx;
    letter_t letter = (_v_sample & (LETTER_MASK << letter_shift)) >> letter_shift;

    LOG("letterize: sample %x letter %x (%u)\r\n", _v_sample, letter, letter);

    _v_letter = letter;
    os_jump(1);
}

void task_compress()
{
    node_t parent_node;

    // pointer into the dictionary tree; starts at a root's child
    index_t parent = _v_parent_next;

    LOG("compress: parent %u\r\n", parent);

    parent_node = _v_dict[parent];

    LOG("compress: parent node: l %u s %u c %u\r\n", parent_node.letter, parent_node.sibling, parent_node.child);

    _v_sibling = parent_node.child;
    _v_parent_node = parent_node;
    _v_parent = parent;
    _v_child = parent_node.child;
    _v_sample_count++;

    os_jump(1);
}

void task_find_sibling()
{
    node_t *sibling_node;

    LOG("find sibling: l %u s %u\r\n", _v_letter, _v_sibling);

    if (_v_sibling != NIL) {
        int i = _v_sibling;
        sibling_node = &_v_dict[i];

        LOG("find sibling: l %u, sn: l %u s %u c %u\r\n", _v_letter,
                sibling_node->letter, sibling_node->sibling, sibling_node->child);

        if (sibling_node->letter == _v_letter) { // found
            LOG("find sibling: found %u\r\n", _v_sibling);

            _v_parent_next = _v_sibling;
            os_jump(10);
            return;
        } else { // continue traversing the siblings
            if(sibling_node->sibling != 0){
                _v_sibling = sibling_node->sibling;
                os_jump(0);
                return;
            }
        }

    }
    LOG("find sibling: not found\r\n");

    index_t starting_node_idx = (index_t)_v_letter;
    _v_parent_next = starting_node_idx;

    LOG("find sibling: child %u\r\n", _v_child);


    if (_v_child == NIL) {
        os_jump(2);
    } else {
        os_jump(1);
    }
}

void task_add_node()
{
    node_t *sibling_node;

    int i = _v_sibling;
    sibling_node = &_v_dict[i];

    LOG("add node: s %u, sn: l %u s %u c %u\r\n", _v_sibling,
            sibling_node->letter, sibling_node->sibling, sibling_node->child);

    if (sibling_node->sibling != NIL) {
        index_t next_sibling = sibling_node->sibling;
        _v_sibling = next_sibling;
        os_jump(0);

    } else { // found last sibling in the list

        LOG("add node: found last\r\n");

        node_t sibling_node_obj = *sibling_node;
        _v_sibling_node = sibling_node_obj;
        os_jump(1);
    }
}

void task_add_insert()
{
    LOG("add insert: nodes %u\r\n", _v_node_count);

    if (_v_node_count == DICT_SIZE) { // wipe the table if full
        while (1);
    }
    LOG("add insert: l %u p %u, pn l %u s %u c%u\r\n", _v_letter, _v_parent,
            _v_parent_node.letter, _v_parent_node.sibling, _v_parent_node.child);

    index_t child = _v_node_count;
    node_t child_node = {
        .letter = _v_letter,
        .sibling = NIL,
        .child = NIL,
    };

    if (_v_parent_node.child == NIL) { // the only child
        LOG("add insert: only child\r\n");

        node_t parent_node_obj = _v_parent_node;
        parent_node_obj.child = child;
        int i = _v_parent;
        _v_dict[i] = parent_node_obj;

    } else { // a sibling

        index_t last_sibling = _v_sibling;
        node_t last_sibling_node = _v_sibling_node;

        LOG("add insert: sibling %u\r\n", last_sibling);

        last_sibling_node.sibling = child;
        _v_dict[last_sibling] = last_sibling_node;
    }
    _v_dict[child] = child_node;
    _v_symbol = _v_parent;
    _v_node_count++;

    os_jump(1);
}

void task_append_compressed()
{
    LOG("append comp: sym %u len %u \r\n", _v_symbol, _v_out_len);
    int i = _v_out_len;
    _v_compressed_data[i].letter = _v_symbol;

    if (++_v_out_len == BLOCK_SIZE) {
        os_jump(1);
    } else {
        os_jump(5);
    }
}

void task_print()
{
    unsigned i;

    PRINTF("TIME end is 65536*%u+%u\r\n",overflow,(unsigned)TBR);
    BLOCK_PRINTF_BEGIN();
    BLOCK_PRINTF("compressed block:\r\n");
    for (i = 0; i < BLOCK_SIZE; ++i) {
        index_t index = _v_compressed_data[i].letter;
        BLOCK_PRINTF("%04x ", index);
        if (i > 0 && (i + 1) % 8 == 0){
            BLOCK_PRINTF("\r\n");
        }
    }
    BLOCK_PRINTF("\r\n");
    BLOCK_PRINTF("rate: samples/block: %u/%u\r\n", _v_sample_count, BLOCK_SIZE);
    BLOCK_PRINTF_END();
    os_jump(1);
}

void task_done()
{
    //  TRANSITION_TO(task_init);
    while (1);
}

static void init_hw()
{
    msp_watchdog_disable();
    msp_gpio_unlock();
    msp_clock_setup();
}

void init()
{
#ifdef BOARD_MSP_TS430
    TBCTL &= 0xE6FF; //set 12,11 bit to zero (16bit) also 8 to zero (SMCLK)
    TBCTL |= 0x0200; //set 9 to one (SMCLK)
    TBCTL |= 0x00C0; //set 7-6 bit to 11 (divider = 8);
    TBCTL &= 0xFFEF; //set bit 4 to zero
    TBCTL |= 0x0020; //set bit 5 to one (5-4=10: continuous mode)
    TBCTL |= 0x0002; //interrupt enable
    //  *timer &= ~(0x0020); //set bit 5 to zero(halt!)
#endif
    //  WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings.
    //  PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.
    init_hw();

#ifdef CONFIG_EDB
    edb_init();
#endif

    INIT_CONSOLE();
    //  P3DIR |=BIT5;
    //  P1DIR |=BIT0;
    __enable_interrupt();
}

int main(void) {
    init();
    //  PRINTF("INIT\r\n");
    //  for (unsigned k = 0; k < arr_len; ++k) {
    //      PRINTF("arr[%u]: %u\r\n", k, arr[k]);
    //  }
    taskId tasks[] = {{task_init, 0},
        {task_init_dict, 0},
        {task_sample, 0},
        {task_measure_temp, 0},
        {task_letterize, 0},
        {task_compress, 0},
        {task_find_sibling, 0},
        {task_add_node, 0},
        {task_add_insert, 0},
        {task_append_compressed, 0},
        {task_print, 0},
        {task_done, 0}};
    //This function should be called only once
    os_addTasks(12, tasks );

    os_scheduler();
    return 0;
}

