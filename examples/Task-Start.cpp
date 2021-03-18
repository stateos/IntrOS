#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

int main()
{
	auto led = Led();
	auto sem = Semaphore::Binary();
	auto cons = Task::Make ([&]{ sem.wait(); led.tick(); });
	auto prod = Task::Start([&]{ this_task::sleepFor(SEC); sem.give(); });
	cons.start();
	cons.join();
}
