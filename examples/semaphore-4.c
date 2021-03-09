#include <stm32f4_discovery.h>
#include <os.h>

OS_SEM(sem, 0);

OS_TSK_START(cons)
{
	sem_wait(sem);
	LED_Tick();
}

OS_TSK_START(prod)
{
	tsk_delay(SEC);
	sem_give(sem);
}

int main()
{
	LED_Init();
	tsk_sleep();
}
