#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

void consumer(Mutex &mtx, Led &led)
{
	auto lock = Lock<Mutex>(mtx);
	led.tick();
}

void producer(Mutex &mtx)
{
	auto lock = Lock<Mutex>(mtx);
	This::sleepFor(SEC);
}

int main()
{
	Led led;
	Mutex mtx;
	Task prod = Task::Start([&mtx]      { producer(mtx); });
	Task cons = Task::Start([&mtx, &led]{ consumer(mtx, led); });
	This::sleep();
}
