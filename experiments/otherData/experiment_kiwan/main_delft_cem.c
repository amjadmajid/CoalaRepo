#include <msp430.h> 
#include <libipos-gcc-dma/ipos.h>
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

__p letter_t _v_letter;
__p unsigned _v_letter_idx;
__p sample_t _v_prev_sample;
__p index_t _v_out_len;
__p index_t _v_node_count;
__p node_t _v_dict[DICT_SIZE];
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
	LOG("init\r\n");
	PW(_v_parent_next) = 0;
	LOG("init: start parent %u\r\n", P(_v_parent));
	PW(_v_out_len) = 0;
	PW(_v_letter) = 0;
	PW(_v_prev_sample) = 0;
	PW(_v_letter_idx) = 0;;
	PW(_v_sample_count) = 1;

	os_jump(1);
}

void task_init_dict()
{
	LOG("init dict: letter %u\r\n", P(_v_letter));

	uint16_t i = P(_v_letter);	
	PW(_v_dict[i].letter) = P(_v_letter); 
	PW(_v_dict[i].sibling) = NIL; 
	PW(_v_dict[i].child) = NIL; 
	PW(_v_letter) = P(_v_letter) + 1;
	if (P(_v_letter) < NUM_LETTERS) {
		os_jump(0);
	} else {
		PW(_v_node_count) = NUM_LETTERS;
		os_jump(1);
	} 
}

void task_sample()
{
	LOG("sample: letter idx %u\r\n", P(_v_letter_idx));

	unsigned next_letter_idx = P(_v_letter_idx) + 1;
	if (next_letter_idx == NUM_LETTERS_IN_SAMPLE)
		next_letter_idx = 0;

	if (P(_v_letter_idx) == 0) {
		PW(_v_letter_idx) = next_letter_idx;
		os_jump(1);
	} else {
		PW(_v_letter_idx) = next_letter_idx;
		os_jump(2);
	}
}

void task_measure_temp()
{
	sample_t prev_sample;
	prev_sample = P(_v_prev_sample);

	sample_t sample = acquire_sample(prev_sample);
	LOG("measure: %u\r\n", sample);
	prev_sample = sample;
	PW(_v_prev_sample) = prev_sample;
	PW(_v_sample) = sample;
	os_jump(1);
}

void task_letterize()
{
	unsigned letter_idx = P(_v_letter_idx);
	if (letter_idx == 0)
		letter_idx = NUM_LETTERS_IN_SAMPLE;
	else
		letter_idx--;
	unsigned letter_shift = LETTER_SIZE_BITS * letter_idx;
	letter_t letter = (P(_v_sample) & (LETTER_MASK << letter_shift)) >> letter_shift;

	LOG("letterize: sample %x letter %x (%u)\r\n", P(_v_sample), letter, letter);

	PW(_v_letter) = letter;
	os_jump(1);
}

void task_compress()
{
	// pointer into the dictionary tree; starts at a root's child
	index_t parent = P(_v_parent_next);

	LOG("compress: parent %u\r\n", parent);

	LOG("compress: parent node: l %u s %u c %u\r\n", P(_v_dict[parent].letter), P(_v_dict[parent].sibling), P(_v_dict[parent].child));

	PW(_v_sibling) = P(_v_dict[parent].child);
	PW(_v_parent_node.letter) = P(_v_dict[parent].letter);
	PW(_v_parent_node.sibling) = P(_v_dict[parent].sibling);
	PW(_v_parent_node.child) = P(_v_dict[parent].child);
	PW(_v_parent) = parent;
	PW(_v_child) = P(_v_dict[parent].child);
	(PW(_v_sample_count))++;

	os_jump(1);
}

void task_find_sibling()
{
	LOG("find sibling: l %u s %u\r\n", P(_v_letter), P(_v_sibling));

	if (P(_v_sibling) != NIL) {
		int i = P(_v_sibling);

		LOG("find sibling: l %u, sn: l %u s %u c %u\r\n", P(_v_letter),
				P(_v_dict[i].letter), P(_v_dict[i].sibling), P(_v_dict[i].child));

		if (P(_v_dict[i].letter) == P(_v_letter)) { // found
			LOG("find sibling: found %u\r\n", P(_v_sibling));

			PW(_v_parent_next) = P(_v_sibling);

			os_jump(10);
			return;
		} else { // continue traversing the siblings
			if(P(_v_dict[i].sibling) != 0){
				PW(_v_sibling) = P(_v_dict[i].sibling);
				os_jump(0);
				return;
			}
		}

	} 
	LOG("find sibling: not found\r\n");

	index_t starting_node_idx = (index_t)P(_v_letter);
	PW(_v_parent_next) = starting_node_idx;

	LOG("find sibling: child %u\r\n", P(_v_child));


	if (P(_v_child) == NIL) {
		os_jump(2);
	} else {
		os_jump(1);
	}
}

void task_add_node()
{
	int i = P(_v_sibling);

	LOG("add node: s %u, sn: l %u s %u c %u\r\n", P(_v_sibling),
			P(_v_dict[i].letter), P(_v_dict[i].sibling), P(_v_dict[i].child));

	if (P(_v_dict[i].sibling) != NIL) {
		index_t next_sibling = P(_v_dict[i].sibling);
		PW(_v_sibling) = next_sibling;
		os_jump(0);

	} else { // found last sibling in the list

		LOG("add node: found last\r\n");

		PW(_v_sibling_node.letter) = P(_v_dict[i].letter);
		PW(_v_sibling_node.sibling) = P(_v_dict[i].sibling);
		PW(_v_sibling_node.child) = P(_v_dict[i].child);

		os_jump(1);
	}
}

void task_add_insert()
{
	LOG("add insert: nodes %u\r\n", P(_v_node_count));

	if (P(_v_node_count) == DICT_SIZE) { // wipe the table if full
		while (1);
	}
	LOG("add insert: l %u p %u, pn l %u s %u c%u\r\n", P(_v_letter), P(_v_parent),
			P(_v_parent_node.letter), P(_v_parent_node.sibling), P(_v_parent_node.child));

	index_t child = P(_v_node_count);

	if (P(_v_parent_node.child) == NIL) { // the only child
		LOG("add insert: only child\r\n");

		PW(_v_parent_node.child) = child;
		int i = P(_v_parent);
		PW(_v_dict[i].letter) = P(_v_parent_node.letter);
		PW(_v_dict[i].sibling) = P(_v_parent_node.sibling);
		PW(_v_dict[i].child) = P(_v_parent_node.child);

	} else { // a sibling

		index_t last_sibling = P(_v_sibling);
		LOG("add insert: sibling %u\r\n", last_sibling);

		PW(_v_sibling_node.sibling) = child;
		PW(_v_dict[last_sibling].letter) = P(_v_sibling_node.letter);
		PW(_v_dict[last_sibling].sibling) = P(_v_sibling_node.sibling);
		PW(_v_dict[last_sibling].child) = P(_v_sibling_node.child);
	}
	PW(_v_dict[child].letter) = P(_v_letter);
	PW(_v_dict[child].sibling) = NIL;
	PW(_v_dict[child].child) = NIL;
	PW(_v_symbol) = P(_v_parent);
	(PW(_v_node_count))++;

	os_jump(1);
}

void task_append_compressed()
{
	LOG("append comp: sym %u len %u \r\n", P(_v_symbol), P(_v_out_len));
	int i = P(_v_out_len);
	PW(_v_compressed_data[i].letter) = P(_v_symbol);

	if (++PW(_v_out_len) == BLOCK_SIZE) {
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
		index_t index = P(_v_compressed_data[i].letter);
		BLOCK_PRINTF("%04x ", index);
		if (i > 0 && (i + 1) % 8 == 0){
			BLOCK_PRINTF("\r\n");
		}
	}
	BLOCK_PRINTF("\r\n");
	BLOCK_PRINTF("rate: samples/block: %u/%u\r\n", P(_v_sample_count), BLOCK_SIZE);
	BLOCK_PRINTF_END();
	os_jump(1);
}

void task_done()
{
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
	//	*timer &= ~(0x0020); //set bit 5 to zero(halt!)
#endif
	//  WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
	// Disable the GPIO power-on default high-impedance mode to activate previously configured port settings.
	//  PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.
	init_hw();

#ifdef CONFIG_EDB
	edb_init();
#endif

	INIT_CONSOLE();
	//	P3DIR |=BIT5;
	//	P1DIR |=BIT0;
	__enable_interrupt();
	PRINTF(".0.\r\n");
}

int main(void) {
	init();
	//	PRINTF("INIT\r\n");
	//	for (unsigned k = 0; k < arr_len; ++k) {
	//		PRINTF("arr[%u]: %u\r\n", k, arr[k]);
	//	}
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
