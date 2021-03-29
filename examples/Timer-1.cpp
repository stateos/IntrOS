#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

Led led = {};

void proc()
{
	led.tick();
}

Timer tmr = {proc};

int main()
{
	tmr.startPeriodic(SEC);
	thisTask::stop();
}
