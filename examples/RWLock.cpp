#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;
using namespace std::chrono_literals;

auto led = Led();
auto rwl = RWLock();

void consumer( void )
{
	auto lock = WriteLock(rwl);
	led.tick();
}

void producer( void )
{
	auto lock = ReadLock(rwl);
	thisTask::sleepNext(1s);
}

int main()
{
	auto prod = Task::Start(producer);
	auto cons = Task::Start(consumer);
	cons.join();
	prod.join();
}
