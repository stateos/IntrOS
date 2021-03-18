#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

auto led = Led();

auto cons = Task::Start([]
{
	this_task::action([](unsigned signo){ led = SIGSET(signo); });
	this_task::suspend();
});

auto prod = Task::Start([]
{
	unsigned x = 0;

	for (;;)
	{
		this_task::sleepFor(SEC);
		cons.signal(x);
		x = (x + 1) % 4;
	}
});

int main()
{
	this_task::stop();
}
