#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

auto led = Led();
auto evt = Event();

void consumer()
{
	led = evt.wait();
}

void producer()
{
	unsigned x = 1;

	for (;;)
	{
		ThisTask::delay(SEC);
		evt.give(x);
		x = (x << 1) | (x >> 3);
	}
}

auto cons = Task(consumer);
auto prod = Task(producer);

int main()
{
	cons.start();
	prod.start();

	ThisTask::stop();
}
