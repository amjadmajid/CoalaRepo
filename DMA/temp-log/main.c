
/*
    The output should be like this
    compressed block:
    0000 0001 0000 0002 0000 0003 0000 0106
    0100 0101 0102 0103 0104 0105 0107 010E
    0108 0109 010A 010B 010C 010D 010F 0116
    0110 0111 0112 0113 0114 0115 0117 011E
    0118 0119 011A 011B 011C 011D 011F 0126
    0120 0121 0122 0123 0124 0125 0127 012E
    0128 0129 012A 012B 012C 012D 012F 0136
    0130 0131 0132 0133 0134 0135 0137 013E

    rate: samples/block: 353/64

    The result should look like this. If you can't print, you can do assert with this

 */

#include <msp430.h>
#include <ipos.h>


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

__p node_t _v_dict[DICT_SIZE];
__p letter_t _v_letter;
__p unsigned _v_letter_idx;
__p sample_t _v_prev_sample;
__p index_t _v_out_len;
__p index_t _v_node_count;
__p sample_t _v_sample;
__p index_t _v_sample_count;
__p index_t _v_sibling;
__p index_t _v_child;
__p index_t _v_parent;
__p index_t _v_parent_next;
__p node_t _v_parent_node;
__p node_t _v_compressed_data[BLOCK_SIZE];
__p node_t _v_sibling_node;
__p index_t _v_symbol;

static sample_t acquire_sample(letter_t prev_sample)
{
    letter_t sample = (prev_sample + 1) & 0x03;
    return sample;
}

void task_init()
{
    WVAR(_v_parent_next, 0);
    WVAR(_v_out_len, 0);
    WVAR(_v_letter, 0);
    WVAR(_v_prev_sample, 0);
    WVAR(_v_letter_idx, 0);
    WVAR(_v_sample_count, 1);
    // os_jump(1);
}

void task_init_dict()
{
    letter_t _p_letter = RVAR(_v_letter);

    node_t node = {
        .letter = _p_letter,
        .sibling = NIL, // no siblings for 'root' nodes
        .child = NIL, // init an empty list for children
    };
    int i = _p_letter;
    WVAR(_v_dict[i], node);
    _p_letter++;
    WVAR(_v_letter, _p_letter);
    if (_p_letter < NUM_LETTERS) {
        os_jump(0);
    } else {
        WVAR(_v_node_count, NUM_LETTERS);
        // os_jump(1);
    }
}

void task_sample()
{
    unsigned _p_letter_idx = RVAR(_v_letter_idx);

    unsigned next_letter_idx = _p_letter_idx + 1;
    if (next_letter_idx == NUM_LETTERS_IN_SAMPLE)
        next_letter_idx = 0;

    if (_p_letter_idx == 0) {
        WVAR(_v_letter_idx, next_letter_idx);
        // os_jump(1);
    } else {
        WVAR(_v_letter_idx, next_letter_idx);
        os_jump(2);
    }
}

void task_measure_temp()
{
    sample_t prev_sample;
    prev_sample = RVAR(_v_prev_sample);

    sample_t sample = acquire_sample(prev_sample);
    prev_sample = sample;
    WVAR(_v_prev_sample, prev_sample);
    WVAR(_v_sample, sample);
    // os_jump(1);
}

void task_letterize()
{
    unsigned letter_idx = RVAR(_v_letter_idx);
    sample_t _p_sample = RVAR(_v_sample);
    if (letter_idx == 0)
        letter_idx = NUM_LETTERS_IN_SAMPLE;
    else
        letter_idx--;
    unsigned letter_shift = LETTER_SIZE_BITS * letter_idx;
    letter_t letter = (_p_sample & (LETTER_MASK << letter_shift)) >> letter_shift;

    WVAR(_v_letter, letter);
    // os_jump(1);
}

void task_compress()
{
    node_t parent_node;

    // pointer into the dictionary tree; starts at a root's child
    index_t parent = RVAR(_v_parent_next);

    parent_node = RVAR(_v_dict[parent]);

    WVAR(_v_sibling, parent_node.child);
    WVAR(_v_parent_node, parent_node);
    WVAR(_v_parent, parent);
    WVAR(_v_child, parent_node.child);
    index_t _p_sample_count = RVAR(_v_sample_count);
    _p_sample_count++;
    WVAR(_v_sample_count, _p_sample_count);

    // os_jump(1);
}

void task_find_sibling()
{
    node_t *sibling_node;
    letter_t _p_letter = RVAR(_v_letter);
    index_t _p_sibling = RVAR(_v_sibling);
    index_t _p_child = RVAR(_v_child);

    if (_p_sibling != NIL) {
        int i = _p_sibling;
        node_t _p_dict_i = RVAR(_v_dict[i]);
        sibling_node = &_p_dict_i;

        if (sibling_node->letter == _p_letter) { // found

            WVAR(_v_parent_next, _p_sibling);
            os_jump(10);
            return;
        } else { // continue traversing the siblings
            if(sibling_node->sibling != 0){
                WVAR(_v_sibling, sibling_node->sibling);
                os_jump(0);
                return;
            }
        }
    }

    index_t starting_node_idx = (index_t)_p_letter;
    WVAR(_v_parent_next, starting_node_idx);

    if (_p_child == NIL) {
        os_jump(2);
    } else {
        // os_jump(1);
    }
}

void task_add_node()
{
    node_t *sibling_node;
    index_t _p_sibling = RVAR(_v_sibling);
    int i = _p_sibling;
    node_t _p_dict_i = RVAR(_v_dict[i]);
    sibling_node = &_p_dict_i;

    if (sibling_node->sibling != NIL) {
        index_t next_sibling = sibling_node->sibling;
        WVAR(_v_sibling, next_sibling);
        os_jump(0);

    } else { // found last sibling in the list

        node_t sibling_node_obj = *sibling_node;
        WVAR(_v_sibling_node, sibling_node_obj);
        // os_jump(1);
    }
}

void task_add_insert()
{
    index_t _p_node_count = RVAR(_v_node_count);
    letter_t _p_letter = RVAR(_v_letter);
    node_t _p_parent_node = RVAR(_v_parent_node);
    node_t _p_sibling_node = RVAR(_v_sibling_node);
    index_t _p_parent = RVAR(_v_parent);
    index_t _p_sibling = RVAR(_v_sibling);

    if (_p_node_count == DICT_SIZE) { // wipe the table if full
        while (1);
    }

    index_t child = _p_node_count;
    node_t child_node = {
        .letter = _p_letter,
        .sibling = NIL,
        .child = NIL,
    };

    if (_p_parent_node.child == NIL) { // the only child

        node_t parent_node_obj = _p_parent_node;
        parent_node_obj.child = child;
        int i = _p_parent;
        WVAR(_v_dict[i], parent_node_obj);

    } else { // a sibling

        index_t last_sibling = _p_sibling;
        node_t last_sibling_node = _p_sibling_node;

        last_sibling_node.sibling = child;
        WVAR(_v_dict[last_sibling], last_sibling_node);
    }
    WVAR(_v_dict[child], child_node);
    WVAR(_v_symbol, _p_parent);
    _p_node_count++;
    WVAR(_v_node_count, _p_node_count);
    // os_jump(1);
}

void task_append_compressed()
{
    index_t _p_symbol = RVAR(_v_symbol);
    index_t i = RVAR(_v_out_len);

    WVAR(_v_compressed_data[i].letter, _p_symbol);

    if (++i == BLOCK_SIZE) {
        WVAR(_v_out_len, i);
        // os_jump(1);
    } else {
        WVAR(_v_out_len, i);
        os_jump(5);
    }
}

void task_print()
{
    unsigned i;


    for (i = 0; i < BLOCK_SIZE; ++i) {
        index_t index = RVAR(_v_compressed_data[i].letter);
        if (i > 0 && (i + 1) % 8 == 0){
        }
    }
    // os_jump(1);
}

void task_done()
{
    //  TRANSITION_TO(task_init);
//    while (1);
    P3OUT |=BIT5;
    P3OUT &=~BIT5;
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
      WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
//     Disable the GPIO power-on default high-impedance mode to activate previously configured port settings.
      PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.

#ifdef CONFIG_EDB
    edb_init();
#endif

    CSCTL0_H = CSKEY >> 8;                // Unlock CS registers
//    CSCTL1 = DCOFSEL_4 |  DCORSEL;                   // Set DCO to 16MHz
    CSCTL1 = DCOFSEL_6;                   // Set DCO to 8MHz
    CSCTL2 =  SELM__DCOCLK;               // MCLK = DCO
    CSCTL3 = DIVM__1;                     // divide the DCO frequency by 1
    CSCTL0_H = 0;

      P3DIR |=BIT5;

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


