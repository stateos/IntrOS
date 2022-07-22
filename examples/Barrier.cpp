#include <stm32f4_discovery.h>
#include <os.h>

auto led = device::Led();
auto bar = intros::Barrier(2);

void consumer()
{
	bar.wait();
	led.tick();
}

void producer()
{
	intros::thisTask::delay(SEC);
	bar.wait();
}

auto cons = intros::Task(consumer);
auto prod = intros::Task(producer);

int main()
{
	cons.start();
	prod.start();
	intros::thisTask::stop();
}
