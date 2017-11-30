#include <msp430.h> 
#include <mspReseter.h>
#include "mspProfiler.h"
#include "mspDebugger.h"
#include <ipos.h>

//#define TSK_SIZ
#define AUTO_RST
//#define LOG_INFO



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
__p node_t _v_dict[DICT_SIZE];


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
    WP(_v_parent_next) = 0;
    WP(_v_out_len) = 0;
    WP(_v_letter) = 0;
    WP(_v_prev_sample) = 0;
    WP(_v_letter_idx) = 0;
    WP(_v_sample_count) = 1 ;

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

    uint16_t i = RP(_v_letter);

    // uart_sendStr("_v_letter \0");
    //     uart_sendHex16(_v_letter);
    // uart_sendStr("\n\r \0");

    WP(_v_dict[i].letter) = i ;
    WP(_v_dict[i].sibling) =  NIL;
    WP(_v_dict[i].child) = NIL;
    WP(_v_letter)++;
    if (i < NUM_LETTERS) {
        os_jump(0);
    } else {
        WP(_v_node_count) =  NUM_LETTERS;
//        os_jump(1);
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

    // uart_sendStr("task_sample \0");
    // uart_sendStr("\n\r \0");

    unsigned next_letter_idx = RP(_v_letter_idx) + 1;
    if (next_letter_idx == NUM_LETTERS_IN_SAMPLE)
        next_letter_idx = 0;

    if (RP(_v_letter_idx) == 0) {
        WP(_v_letter_idx) = next_letter_idx;
        // os_jump(1);
    } else {
        WP(_v_letter_idx) = next_letter_idx;
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

    // uart_sendStr("task_measure_temp \0");
    // uart_sendStr("\n\r \0");

    sample_t prev_sample;
    prev_sample = RP(_v_prev_sample);

    sample_t sample = acquire_sample(prev_sample);
    prev_sample = sample;
    WP(_v_prev_sample) = prev_sample;
    WP(_v_sample) = sample;
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

    // uart_sendStr("task_letterize \0");
    // uart_sendStr("\n\r \0");

    unsigned letter_idx = RP(_v_letter_idx);
    if (letter_idx == 0)
        letter_idx = NUM_LETTERS_IN_SAMPLE;
    else
        letter_idx--;
    unsigned letter_shift = LETTER_SIZE_BITS * letter_idx;
    letter_t letter = (RP(_v_sample) & (LETTER_MASK << letter_shift)) >> letter_shift;

    WP(_v_letter) = letter;
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

    // uart_sendStr("task_compress \0");
    // uart_sendStr("\n\r \0");

    // pointer into the dictionary tree; starts at a root's child
    index_t parent = RP(_v_parent_next);

    uint16_t __cry;

    // uart_sendStr("_v_dict[parent].child \0");
    // uart_sendHex16(_v_dict[parent].child);
    // uart_sendStr("\n\r \0");

    __cry = RP(_v_dict[parent].child);
    WP(_v_sibling) = __cry ;
    __cry = RP(_v_dict[parent].letter);
    WP(_v_parent_node.letter) =  __cry;
    __cry = RP(_v_dict[parent].sibling);
    WP(_v_parent_node.sibling) = __cry;
    __cry = RP(_v_dict[parent].child);
    WP(_v_parent_node.child) = __cry;
    WP(_v_parent) = parent;
    __cry = RP(_v_dict[parent].child);
    WP(_v_child) = __cry;
    (WP(_v_sample_count))++;

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

    if (RP(_v_sibling) != NIL) {
        int i = RP(_v_sibling);

        uint16_t __cry = RP(_v_letter);
        if (RP(_v_dict[i].letter) == __cry ) { // found

            __cry = RP(_v_sibling);
            WP(_v_parent_next) = __cry;

            os_jump(10);
            return;
        } else { // continue traversing the siblings
            if(RP(_v_dict[i].sibling) != 0){
                __cry = RP(_v_dict[i].sibling);
                WP(_v_sibling) = __cry;
                os_jump(0);
                return;
            }
        }

    }

    index_t starting_node_idx = (index_t)RP(_v_letter);
    WP(_v_parent_next) = starting_node_idx;

    if (RP(_v_child) == NIL) {
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

    int i = RP(_v_sibling);


    if (RP(_v_dict[i].sibling) != NIL) {
        index_t next_sibling = RP(_v_dict[i].sibling);
        WP(_v_sibling) = next_sibling;
        os_jump(0);

    } else { // found last sibling in the list

//        LOG("add node: found last\r\n");
        uint16_t __cry;

         __cry = RP(_v_dict[i].letter);
         WP(_v_sibling_node.letter) = __cry;
        __cry = RP(_v_dict[i].sibling);
        WP(_v_sibling_node.sibling) = __cry;
        __cry = RP(_v_dict[i].child);
        WP(_v_sibling_node.child) = __cry;

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

    if (RP(_v_node_count) == DICT_SIZE) { // wipe the table if full
        while (1);
    }

    index_t child = RP(_v_node_count);
    uint16_t __cry;
    if (RP(_v_parent_node.child) == NIL) { // the only child

        WP(_v_parent_node.child) = child;
        int i = RP(_v_parent);


        __cry = RP(_v_parent_node.letter);
        WP(_v_dict[i].letter) = __cry;
        __cry  = RP(_v_parent_node.sibling);
        WP(_v_dict[i].sibling) = __cry;
        __cry = RP(_v_parent_node.child);
        WP(_v_dict[i].child) = __cry;

    } else { // a sibling

        index_t last_sibling = RP(_v_sibling);

        WP(_v_sibling_node.sibling) = child;
        __cry = RP(_v_sibling_node.letter);
        WP(_v_dict[last_sibling].letter) = __cry;
        __cry = RP(_v_sibling_node.sibling);
        WP(_v_dict[last_sibling].sibling) = __cry;
        __cry  = RP(_v_sibling_node.child);
        WP(_v_dict[last_sibling].child) = __cry;
    }
    __cry = RP(_v_letter);
    WP(_v_dict[child].letter) = __cry;
    WP(_v_dict[child].sibling) = NIL;
    WP(_v_dict[child].child) = NIL;
    __cry = RP(_v_parent);
    WP(_v_symbol) = __cry;
    (RP(_v_node_count))++;
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
    int i = RP(_v_out_len);
    __cry = RP(_v_symbol);
    WP(_v_compressed_data[i].letter) = __cry;

    if ( (++(RP(_v_out_len))) == BLOCK_SIZE) {
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
        index_t index = RP(_v_compressed_data[i].letter);
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

#if 1
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
    mr_auto_rand_reseter(50000); // every 12 msec the MCU will be reseted
#endif

}

int main(void) {
    init();

    taskId tasks[] = {{task_init,   1,  1  },
        {task_init_dict,            2,  3  },
        {task_sample,               3,  1  },
        {task_measure_temp,         4,  1  },
        {task_letterize,            5,  5  },
        {task_compress,             6,  2  },
        {task_find_sibling,         7,  3  },
        {task_add_node,             8,  3  },
        {task_add_insert,           9,  5  },
        {task_append_compressed,    10, 2 },
        {task_print,                11, 3 },
        {task_done,                 12, 1 }};
    //This function should be called only once
    os_addTasks(12, tasks );

    os_scheduler();
    return 0;
}











