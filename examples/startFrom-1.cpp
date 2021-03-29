#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

auto led = Led();
auto tsk = Task([]{});

int main()
{
	thisTask::flip([]
	{
		thisTask::sleepNext(SEC);
		tsk.startFrom([]
		{
			led.tick();
			thisTask::stop();
		});
		tsk.join();
	});
}
