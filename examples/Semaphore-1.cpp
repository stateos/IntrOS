#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

Led       led;
Semaphore sem{0};

void consumer()
{
	for (;;)
	{
		sem.wait();
		led.tick();
	}
}

void producer()
{
	for (;;)
	{
		this_task::sleepFor(SEC);
		sem.give();
	}
}

TaskT<256> cons{consumer};
TaskT<256> prod{producer};

int main()
{
	cons.start();
	prod.start();

	this_task::stop();
}
