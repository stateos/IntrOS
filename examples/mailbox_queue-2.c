#include <stm32f4_discovery.h>
#include <os.h>

box_id box = BOX_CREATE(1, sizeof(unsigned));

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

tsk_id cons = WRK_CREATE(consumer, 256);
tsk_id prod = WRK_CREATE(producer, 256);

int main()
{
	LED_Init();

	tsk_start(cons);
	tsk_start(prod);
	tsk_stop();
}
