#include <stm32f4_discovery.h>
#include <os.h>

void action( unsigned signo )
{
	switch (signo)
	{
	case 12: LEDs = 0x01; break;
	case 13: LEDs = 0x02; break;
	case 14: LEDs = 0x04; break;
	case 15: LEDs = 0x08; break;
	default: break;
	}
}

void consumer()
{
	tsk_setAction(SELF, action);
	tsk_suspend(SELF);
}

OS_TSK(cons, consumer);

void producer()
{
	unsigned x = 0;

	for (;;)
	{
		tsk_delay(SEC);
		tsk_give(cons, 12 + x);
		x = (x + 1) % 4;
	}
}

OS_TSK(prod, producer);

int main()
{
	LED_Init();

	tsk_start(cons);
	tsk_start(prod);
	tsk_stop();
}
