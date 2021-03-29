#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

auto led = Led();
auto tmr = Timer::StartPeriodic(SEC, []{ led.tick(); });

int main()
{
	thisTask::sleep();
}
