#include <stm32f4_discovery.h>
#include <os.h>

OS_MEM(mem, 1, sizeof(unsigned));
OS_LST(lst);

void consumer()
{
	unsigned *p;

	for (;;)
	{
		p = lst_wait(lst);
		LEDs = *p & 0x0FUL;
		mem_give(mem, p);
	}
}

void producer()
{
	unsigned *p;
	unsigned x = 1;

	for (;;)
	{
		tsk_delay(SEC);
		p = mem_wait(mem);
		*p = x;
		lst_give(lst, p);
		x = (x << 1) | (x >> 3);
	}
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
