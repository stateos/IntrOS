#include <stm32f4_discovery.h>
#include <os.h>

auto led = device::Led();
auto bar = os::Barrier(2);

void consumer()
{
	bar.wait();
	led.tick();
}

void producer()
{
	os::thisTask::delay(SEC);
	bar.wait();
}

auto cons = os::Task(consumer);
auto prod = os::Task(producer);

int main()
{
	cons.start();
	prod.start();
	os::thisTask::stop();
}
