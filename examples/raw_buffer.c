#include <stm32f4_discovery.h>
#include <os.h>

OS_RAW(raw, sizeof(unsigned));

OS_TSK_DEF(cons)
{
	unsigned x;

	for (;;)
	{
		raw_wait(raw, &x, sizeof(x));
		LEDs = x & 0x0FUL;
	}
}

OS_TSK_DEF(prod)
{
	unsigned x = 1;

	for (;;)
	{
		tsk_delay(SEC);
		raw_send(raw, &x, sizeof(x));
		x = (x << 1) | (x >> 3);
	}
}

int main()
{
	LED_Init();

	tsk_start(cons);
	tsk_start(prod);
	tsk_sleep();
}
