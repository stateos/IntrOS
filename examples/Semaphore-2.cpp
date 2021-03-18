#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

auto led = Led();
auto sem = Semaphore(0);
auto cons = Task([]{ sem.wait(); led.tick(); });
auto prod = Task([]{ this_task::sleepFor(SEC); sem.give(); });

int main()
{
	cons.start();
	prod.start();

	this_task::stop();
}
