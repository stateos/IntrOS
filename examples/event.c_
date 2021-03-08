#include <stm32f4_discovery.h>
#include <os.h>

OS_EVT(evt);

void consumer()
{
	LEDs = evt_wait(evt) & 0x0FUL;
}

void producer()
{
	unsigned x = 1;

	for (;;)
	{
		tsk_delay(SEC);
		evt_give(evt, x);
		x = (x << 1) | (x >> 3);
	}
}

OS_TSK(cons, consumer, 256);
OS_TSK(prod, producer, 256);

int main()
{
	LED_Init();

	tsk_start(cons);
	tsk_start(prod);
	tsk_stop();
}
