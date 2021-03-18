#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

int main()
{
	auto led = Led();

	for (;;)
	{
		this_task::delay(SEC);
		led.tick();
	}
}
