#include <msp430.h> 
#include <mspReseter.h>
#include "mspProfiler.h"
#include "mspDebugger.h"
#include <ipos.h>

#define TSK_SIZ
#define AUTO_RST
#define LOG_INFO



__nv uint8_t pinCont = 0;
#define NIL 0 // like NULL, but for indexes, not real pointers

#define DICT_SIZE         512
#define BLOCK_SIZE         64

#define NUM_LETTERS_IN_SAMPLE        2
#define LETTER_MASK             0x00FF
#define LETTER_SIZE_BITS             8
#define NUM_LETTERS (LETTER_MASK + 1)


typedef uint16_t index_t;
typedef uint16_t letter_t;
typedef uint16_t sample_t;

// NOTE: can't use pointers, since need to ChSync, etc
typedef struct _node_t {
    letter_t letter; // 'letter' of the alphabet
    index_t sibling; // this node is a member of the parent's children list
    index_t child;   // link-list of children
} node_t;


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
__p node_t _v_sibling_node;
__p index_t _v_symbol;
__p node_t _v_compressed_data[BLOCK_SIZE];

static sample_t acquire_sample(letter_t prev_sample)
{
    letter_t sample = (prev_sample + 1) & 0x03;
    return sample;
}

void task_init()
{

#ifdef TSK_SIZ
    cp_reset();
#endif

    pinCont=1;
    WVAR(_v_parent_next, 0);
    WVAR(_v_out_len,0);
    WVAR(_v_letter, 0);
    WVAR(_v_prev_sample, 0);
    WVAR(_v_letter_idx, 0);
    WVAR(_v_sample_count,1);

#ifdef TSK_SIZ
    cp_sendRes("task_init \0");
#endif

//    os_jump(1);
}

void task_init_dict()
{
#ifdef TSK_SIZ
    cp_reset();
#endif

    int i = RVAR(_v_letter);
    WVAR(_v_dict[i].letter,i);
    WVAR(_v_dict[i].sibling, NIL);
    WVAR(_v_dict[i].child, NIL);
    i++;
    WVAR(_v_letter, i);
    if (i < NUM_LETTERS) {
        os_jump(0);
    } else {
        WVAR(_v_node_count, NUM_LETTERS);
//        os_jump(1);
        ;
    }
#ifdef TSK_SIZ
    cp_sendRes("task_init_dict \0");
#endif
}

void task_sample()
{
#ifdef TSK_SIZ
    cp_reset();
#endif

    unsigned next_letter_idx = RVAR(_v_letter_idx) + 1;
    if (next_letter_idx == NUM_LETTERS_IN_SAMPLE)
        next_letter_idx = 0;

    WVAR(_v_letter_idx,next_letter_idx);
    if (RVAR(_v_letter_idx) == 0) {
//        os_jump(1);
        ;
    } else {
        os_jump(2);
    }
#ifdef TSK_SIZ
    cp_sendRes("task_sample \0");
#endif
}

void task_measure_temp()
{
#ifdef TSK_SIZ
    cp_reset();
#endif

    sample_t prev_sample;
    prev_sample = RVAR(_v_prev_sample);

    sample_t sample = acquire_sample(prev_sample);
    prev_sample = sample;
    WVAR(_v_prev_sample, prev_sample);
    WVAR(_v_sample, sample);
//    os_jump(1);

#ifdef TSK_SIZ
    cp_sendRes("task_measure_temp \0");
#endif
}

void task_letterize()
{
#ifdef TSK_SIZ
    cp_reset();
#endif

    unsigned letter_idx = RVAR(_v_letter_idx);
    if (letter_idx == 0)
        letter_idx = NUM_LETTERS_IN_SAMPLE;
    else
        letter_idx--;
    unsigned letter_shift = LETTER_SIZE_BITS * letter_idx;
    letter_t letter = (RVAR(_v_sample) & (LETTER_MASK << letter_shift)) >> letter_shift;

    WVAR(_v_letter, letter);
//    os_jump(1);
#ifdef TSK_SIZ
    cp_sendRes("task_letterize \0");
#endif
}

void task_compress()
{
#ifdef TSK_SIZ
    cp_reset();
#endif

    // pointer into the dictionary tree; starts at a root's child
    index_t parent = RVAR(_v_parent_next);

    uint16_t __cry;
    __cry = RVAR(_v_dict[parent].child);
    WVAR(_v_sibling, __cry) ;
    __cry = RVAR(_v_dict[parent].letter);
    WVAR(_v_parent_node.letter, __cry);
    __cry = RVAR(_v_dict[parent].sibling);
    WVAR(_v_parent_node.sibling, __cry);
    __cry = RVAR(_v_dict[parent].child);
    WVAR(_v_parent_node.child, __cry);
    WVAR(_v_parent, parent);
    __cry = RVAR(_v_dict[parent].child);
    WVAR(_v_child, __cry);
    (RVAR(_v_sample_count))++;

//    os_jump(1);
#ifdef TSK_SIZ
    cp_sendRes("task_compress \0");
#endif
}

void task_find_sibling()
{
#ifdef TSK_SIZ
    cp_reset();
#endif

    if (RVAR(_v_sibling) != NIL) {
        int i = RVAR(_v_sibling);

        uint16_t __cry = RVAR(_v_letter);
        if (RVAR(_v_dict[i].letter) == __cry ) { // found

            __cry = RVAR(_v_sibling);
            WVAR(_v_parent_next, __cry);

            os_jump(10);
            return;
        } else { // continue traversing the siblings
            if(RVAR(_v_dict[i].sibling) != 0){
                __cry = RVAR(_v_dict[i].sibling);
                WVAR(_v_sibling, __cry);
                os_jump(0);
                return;
            }
        }

    }

    index_t starting_node_idx = (index_t)RVAR(_v_letter);
    WVAR(_v_parent_next, starting_node_idx);

    if (RVAR(_v_child) == NIL) {
        os_jump(2);
    }
//    else {
//        os_jump(1);
//    }

#ifdef TSK_SIZ
    cp_sendRes("task_find_sibling \0");
#endif
}

void task_add_node()
{
#ifdef TSK_SIZ
    cp_reset();
#endif

    int i = RVAR(_v_sibling);


    if (RVAR(_v_dict[i].sibling) != NIL) {
        index_t next_sibling = RVAR(_v_dict[i].sibling);
        WVAR(_v_sibling, next_sibling);
        os_jump(0);

    } else { // found last sibling in the list

//        LOG("add node: found last\r\n");
        uint16_t __cry;

         __cry = RVAR(_v_dict[i].letter);
         WVAR(_v_sibling_node.letter, __cry);
        __cry = RVAR(_v_dict[i].sibling);
        WVAR(_v_sibling_node.sibling, __cry);
        __cry = RVAR(_v_dict[i].child);
        WVAR(_v_sibling_node.child, __cry);

//        os_jump(1);
    }

#ifdef TSK_SIZ
    cp_sendRes("task_add_node \0");
#endif

}

void task_add_insert()
{
#ifdef TSK_SIZ
    cp_reset();
#endif

    if (RVAR(_v_node_count) == DICT_SIZE) { // wipe the table if full
        while (1);
    }

    index_t child = RVAR(_v_node_count);
    uint16_t __cry;
    if (RVAR(_v_parent_node.child) == NIL) { // the only child

        WVAR(_v_parent_node.child, child);
        int i = RVAR(_v_parent);


        __cry = RVAR(_v_parent_node.letter);
        WVAR(_v_dict[i].letter, __cry);
        __cry  = RVAR(_v_parent_node.sibling);
        WVAR(_v_dict[i].sibling, __cry);
        __cry = RVAR(_v_parent_node.child);
        WVAR(_v_dict[i].child, __cry);

    } else { // a sibling

        index_t last_sibling = RVAR(_v_sibling);

        WVAR(_v_sibling_node.sibling, child);
        __cry = RVAR(_v_sibling_node.letter);
        WVAR(_v_dict[last_sibling].letter, __cry);
        __cry = RVAR(_v_sibling_node.sibling);
        WVAR(_v_dict[last_sibling].sibling, __cry);
        __cry  = RVAR(_v_sibling_node.child);
        WVAR(_v_dict[last_sibling].child, __cry);
    }
    __cry = RVAR(_v_letter);
    WVAR(_v_dict[child].letter, __cry);
    WVAR(_v_dict[child].sibling, NIL);
    WVAR(_v_dict[child].child, NIL);
    __cry = RVAR(_v_parent);
    WVAR(_v_symbol, __cry);
    (RVAR(_v_node_count))++;
#ifdef TSK_SIZ
    cp_sendRes("task_add_insert \0");
#endif

//    os_jump(1);
}

void task_append_compressed()
{
#ifdef TSK_SIZ
    cp_reset();
#endif

    uint16_t __cry;
    int i = RVAR(_v_out_len);
    __cry = RVAR(_v_symbol);
    WVAR(_v_compressed_data[i].letter, __cry);

    if ( (++(RVAR(_v_out_len))) == BLOCK_SIZE) {
//        os_jump(1);
        ;
    } else {
        os_jump(5);
    }
#ifdef TSK_SIZ
    cp_sendRes("task_append_compressed \0");
#endif

}

void task_print()
{
#ifdef TSK_SIZ
    cp_reset();
#endif


    unsigned i;

    for (i = 0; i < BLOCK_SIZE; ++i) {
        index_t index = RVAR(_v_compressed_data[i].letter);
    }

#ifdef TSK_SIZ
    cp_sendRes("task_print \0");
#endif

//    os_jump(0);
}

void task_done()
{

#ifdef TSK_SIZ
       cp_reset();
#endif

    if (pinCont){
        P3OUT |=BIT5;
        P3OUT &=~BIT5;
    }
    pinCont=0;

#ifdef TSK_SIZ
     cp_sendRes("task_done \0");
#endif
}



void init()
{

      WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings.
      PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.

      P3OUT &=~BIT5;
      P3DIR |=BIT5;

#if 0
      CSCTL0_H = CSKEY >> 8;                // Unlock CS registers
  //    CSCTL1 = DCOFSEL_4 |  DCORSEL;                   // Set DCO to 16MHz
      CSCTL1 = DCOFSEL_6;                   // Set DCO to 8MHz
      CSCTL2 =  SELM__DCOCLK;               // MCLK = DCO
      CSCTL3 = DIVM__1;                     // divide the DCO frequency by 1
      CSCTL0_H = 0;
#endif

#ifdef TSK_SIZ
    cp_init();
#endif

#ifdef LOG_INFO
    uart_init();
#endif

#ifdef AUTO_RST
    mr_auto_rand_reseter(13000); // every 12 msec the MCU will be reseted
#endif

}

int main(void) {
    init();

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

