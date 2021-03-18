#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

auto led = Led();
auto evq = EventQueueT<1>();

void consumer()
{
	for (;;)
	{
		led = evq.wait();
	}
}

void producer()
{
	unsigned x = 1;

	for (;;)
	{
		this_task::delay(SEC);
		evq.give(x);
		x = (x << 1) | (x >> 3);
	}
}

auto cons = Task(consumer);
auto prod = Task(producer);

int main()
{
	cons.start();
	prod.start();

	this_task::stop();
}
