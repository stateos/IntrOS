#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

Led led;

void proc1( void );
void proc2( void );

void proc1()
{
	led.tick();
	thisTimer::flip(proc2);
}

void proc2()
{
	led.tick();
	thisTimer::flip(proc1);
}

auto tmr = Timer();

int main()
{
	tmr.startFrom(SEC, SEC, proc1);
	thisTask::stop();
}
