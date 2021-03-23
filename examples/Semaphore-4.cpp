#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

void consumer(Semaphore &sem, Led &led)
{
	for (;;)
	{
		sem.wait();
		led.tick();
	}
}

void producer(Semaphore &sem)
{
	for (;;)
	{
		this_task::sleepFor(SEC);
		sem.give();
	}
}

int main()
{
	Led led;
	Semaphore sem;
	auto cons = Task::Start([&]{ consumer(sem, led); });
	auto prod = Task::Start([&]{ producer(sem); });
	this_task::sleep();
}
