#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

//#include <libwispbase/wisp-base.h>
//#include <wisp-base.h>
#include <libipos-gcc-dma/ipos.h>
#include <libmspbuiltins/builtins.h>
#include <libio/log.h>
#include <libmsp/mem.h>
#include <libmsp/periph.h>
#include <libmsp/clock.h>
#include <libmsp/watchdog.h>
#include <libmsp/gpio.h>

#ifdef CONFIG_LIBEDB_PRINTF
#include <libedb/edb.h>
#endif

#include "pins.h"

void task_1();
void task_2();

__p unsigned a[512] = {0};
__p unsigned b[512] = {0};
__p unsigned c[512] = {0};

void task_1() {
	P(a[0]) = 1;
	P(c[0]) = 1;
	PRINTF("%u %u\r\n", P(a[0]));
	PRINTF("%u %u\r\n", P(c[0]));
	os_jump(1);
}
void task_1() {
	PRINTF("%u %u\r\n", P(a[0]));
	PRINTF("%u %u\r\n", P(c[0]));
	exit(0);
}

static void init_hw()
{
	msp_watchdog_disable();
	msp_gpio_unlock();
	msp_clock_setup();
}

void init()
{
	init_hw();

#ifdef CONFIG_EDB
	edb_init();
#endif

	INIT_CONSOLE();

	__enable_interrupt();

//	PRINTF(".%u.\r\n", curctx->task->idx);
}

int main(void) {
	init();
	taskId tasks[] = {{task_1, 0}, {task_2, 0}};

	//This function should be called only once
	os_addTasks(1, tasks );

	os_scheduler();
	return 0;
}
