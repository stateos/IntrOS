#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

auto led = Led();
auto tsk = Task([]{});

int main()
{
	ThisTask::flip([]
	{
		ThisTask::sleepNext(SEC);
		tsk.startFrom([]
		{
			ThisTask::flip([]
			{
				led.tick();
				ThisTask::stop();
			});
		});
		tsk.join();
	});
}
