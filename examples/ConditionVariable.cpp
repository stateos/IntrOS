#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

auto led = Led();
auto mtx = Mutex();
auto cnd = ConditionVariable();

void consumer()
{
	mtx.wait();
	for (;;)
	{
		cnd.wait(mtx);
		led.tick();
	}
}

void producer()
{
	mtx.wait();
	thisTask::delay(SEC);
	cnd.give();
	mtx.give();
}

auto cons = Task(consumer);
auto prod = Task(producer);

int main()
{
	cons.start();
	prod.start();

	thisTask::stop();
}
