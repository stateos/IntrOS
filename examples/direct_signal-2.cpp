#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

int main()
{
	auto led = Led();

	auto cons = Task::Start([&]
	{
		this_task::action([&](unsigned signo)
		{
			this_task::flip([&]
			{
				led = SIGSET(signo);
				this_task::suspend();
			});
		});
		this_task::suspend();
	});

	auto prod = Task::Start([&]
	{
		unsigned x = 0;

		for (;;)
		{
			this_task::sleepNext(SEC);
			cons.signal(x);
			x = (x + 1) % 4;
		}
	});

	this_task::suspend();
}
