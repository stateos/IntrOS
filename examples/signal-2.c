#include <stm32f4_discovery.h>
#include <os.h>

sig_id sig = SIG_CREATE(0);

void consumer()
{
	unsigned x;

	x = sig_wait(sig, sigAll);
	LEDs = SIGSET(x) & 0x0FUL;
}

void producer()
{
	unsigned x = 0;

	for (;;)
	{
		tsk_delay(SEC);
		sig_give(sig, x);
		x = (x + 1) % 4;
	}
}

int main()
{
	LED_Init();

	tsk_start((tsk_id)TSK_CREATE(consumer));
	tsk_start((tsk_id)TSK_CREATE(producer));
	tsk_sleep();
}
