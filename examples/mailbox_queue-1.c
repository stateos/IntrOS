#include <stm32f4_discovery.h>
#include <os.h>

OS_BOX(box, 1, sizeof(unsigned));

void consumer()
{
	unsigned x;

	for (;;)
	{
		box_wait(box, &x);
		LEDs = x & 0x0FUL;
	}
}

void producer()
{
	unsigned x = 1;

	for (;;)
	{
		tsk_delay(SEC);
		box_give(box, &x);
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
