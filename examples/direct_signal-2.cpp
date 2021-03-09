#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

int main()
{
	auto led = Led();

	auto cons = Task::Start([&]
	{
		ThisTask::action([&](unsigned signo)
		{
			ThisTask::flip([&]
			{
				led = SIGSET(signo);
				ThisTask::suspend();
			});
		});
		ThisTask::suspend();
	});

	auto prod = Task::Start([&]
	{
		unsigned x = 0;

		for (;;)
		{
			ThisTask::sleepNext(SEC);
			cons.signal(x);
			x = (x + 1) % 4;
		}
	});

	ThisTask::suspend();
}
