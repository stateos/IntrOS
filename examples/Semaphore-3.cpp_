#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

auto led = Led();
auto sem = Semaphore(0);
auto cons = Task::Start([]{ sem.wait(); led.tick(); });
auto prod = Task::Start([]{ ThisTask::sleepFor(SEC); sem.give(); });

int main()
{
	ThisTask::stop();
}
