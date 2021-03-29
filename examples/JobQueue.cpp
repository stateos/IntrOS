#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

auto led = Led();
auto job = JobQueueT<1>();
auto cons = Task::Start([]{ job.wait(); });
auto tmr = Timer::StartPeriodic(SEC, []{ job.give([]{ led.tick(); }); });

int main()
{
	thisTask::sleep();
}
