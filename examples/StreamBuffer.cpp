#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

auto led = Led();
auto stm = StreamBufferTT<1, unsigned>();

void consumer()
{
	unsigned x;

	for (;;)
	{
		stm.wait(&x);
		led = x;
	}
}

void producer()
{
	unsigned x = 1;

	for (;;)
	{
		this_task::delay(SEC);
		stm.send(&x);
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
