#include <stm32f4_discovery.h>
#include <os.h>

OS_ONE(one);

void leds_off( void )
{
	LEDs = 0;
}

OS_TSK_DEF(prod)
{
	for (;;)
	{
		one_call(one, leds_off);
		tsk_delay(SEC);
		LED_Tick();
	}
}

int main()
{
	LED_Init();

	tsk_start(prod);
	tsk_stop();
}
