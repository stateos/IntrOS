#include <stm32f4_discovery.h>
#include <os.h>

OS_STM(stm, sizeof(unsigned));

OS_TSK_DEF(cons)
{
	unsigned x;

	for (;;)
	{
		stm_wait(stm, &x, sizeof(x));
		LEDs = x & 0x0FUL;
	}
}

OS_TSK_DEF(prod)
{
	unsigned x = 1;

	for (;;)
	{
		tsk_delay(SEC);
		stm_send(stm, &x, sizeof(x));
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