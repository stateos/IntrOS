#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

auto led = Led();
auto tsk = Task([]{});

int main()
{
	this_task::flip([]
	{
		this_task::sleepNext(SEC);
		tsk.startFrom([]
		{
			this_task::flip([]
			{
				led.tick();
				this_task::stop();
			});
		});
		tsk.join();
	});
}
