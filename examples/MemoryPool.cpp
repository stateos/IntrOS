#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

auto led = Led();
auto lst = ListTT<unsigned>();
auto mem = MemoryPoolTT<1, unsigned>();

void consumer()
{
	unsigned*p;

	for (;;)
	{
		p = lst.wait();
		led = *p;
		mem.give(p);
	}
}

void producer()
{
	unsigned*p;
	unsigned x = 1;

	for (;;)
	{
		this_task::delay(SEC);

		p = mem.wait();
		*p=x;
		lst.give(p);
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
