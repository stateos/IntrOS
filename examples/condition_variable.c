#include <stm32f4_discovery.h>
#include <os.h>

OS_MTX(mtx);
OS_CND(cnd);

void consumer()
{
	mtx_wait(mtx);
	for (;;)
	{
		cnd_wait(cnd, mtx);
		LED_Tick();
	}
}

void producer()
{
	mtx_wait(mtx);
	tsk_delay(SEC);
	cnd_give(cnd);
	mtx_give(mtx);
}

OS_TSK(cons, consumer);
OS_TSK(prod, producer);

int main()
{
	LED_Init();

	tsk_start(cons);
	tsk_start(prod);

	tsk_stop();
}
