#include <stm32f4_discovery.h>
#include <os.h>

OS_RWL(rwl);

void consumer( void )
{
	rwl_lockWrite(rwl);
	LED_Tick();
	rwl_unlockWrite(rwl);
}

void producer( void )
{
	rwl_lockRead(rwl);
	tsk_sleepNext(SEC);
	rwl_unlockRead(rwl);
}

OS_TSK(cons, consumer);
OS_TSK(prod, producer);

int main()
{
	LED_Init();
	tsk_start(prod);
	tsk_start(cons);
	tsk_sleep();
}
