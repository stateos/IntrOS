#include <stm32f4_discovery.h>
#include <os.h>

void consumer()
{
	static uint32_t time = 0;

	tsk_sleepUntil(time += SEC);
	tsk_stop();
}

OS_TSK(cons, consumer);

void producer()
{
	tsk_start(cons);
	tsk_join(cons);

	LED_Tick();
}

OS_TSK(prod, producer);

int main()
{
	LED_Init();

	tsk_start(prod);
	tsk_join(prod);
}
