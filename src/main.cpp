#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

auto led = Led();
auto sem = Semaphore::Binary();
auto cons = Task::Start([]{ sem.wait(); led.tick(); });
auto prod = Task::Start([]{ thisTask::sleepNext(std::chrono::seconds{1}); sem.give(); });

int main()
{
	thisTask::suspend();
}
