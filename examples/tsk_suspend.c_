#include <stm32f4_discovery.h>
#include <os.h>

OS_TSK_DEF(cons)
{
	LED_Tick();
}

OS_TSK_DEF(prod)
{
	tsk_suspend(cons);
	tsk_delay(SEC);
	tsk_resume(cons);
}

int main()
{
	LED_Init();

	tsk_start(cons);
	tsk_start(prod);
	tsk_sleep();
}
