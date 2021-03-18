#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

auto led = Led();
auto sem = Semaphore(0);
auto cons = Task::Start([]{ sem.wait(); led.tick(); });
auto prod = Task::Start([]{ this_task::sleepFor(SEC); sem.give(); });

int main()
{
	this_task::stop();
}
