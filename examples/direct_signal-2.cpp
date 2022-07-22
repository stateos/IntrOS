#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

int main()
{
	auto led = Led();

	auto cons = Task::Start([&]
	{
		thisTask::setAction([&](unsigned signo)
		{
			thisTask::flip([&]
			{
				led = SIGSET(signo);
				thisTask::suspend();
			});
		});
		thisTask::suspend();
	});

	auto prod = Task::Start([&]
	{
		unsigned x = 0;

		for (;;)
		{
			thisTask::sleepNext(SEC);
			cons.signal(x);
			x = (x + 1) % 4;
		}
	});

	thisTask::suspend();
}
