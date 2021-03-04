#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

int main()
{
	auto sem = Semaphore::Binary();

	auto cons = Task::Start([&]
	{
		Led led;

		for (;;)
		{
			sem.wait();
			led.tick();
		}
	});

	auto prod = Task::Start([&]
	{
		for (;;)
		{
			ThisTask::sleepFor(SEC);
			sem.give();
		}
	});

	ThisTask::sleep();
}
