#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

auto led = Led();
auto mtx = Mutex();
auto cnd = ConditionVariable();

void consumer()
{
	for (;;)
	{
		auto lck = LockGuard(mtx);
		cnd.wait(mtx);
		led.tick();
	}
}

void producer()
{
	thisTask::delay(SEC);
	cnd.give(cndOne);
}

auto cons = Task(consumer);
auto prod = Task(producer);

int main()
{
	cons.start();
	prod.start();

	thisTask::stop();
}
