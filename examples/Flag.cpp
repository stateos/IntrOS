#include <stm32f4_discovery.h>
#include <os.h>

auto led = device::Led();
auto flg = intros::Flag();

void consumer()
{
	flg.wait(3);
	led.tick();
}

void producer()
{
	intros::thisTask::delay(SEC/2);
	flg.give(1);
	intros::thisTask::delay(SEC/2);
	flg.give(2);
}

auto cons = intros::Task(consumer);
auto prod = intros::Task(producer);

int main()
{
	cons.start();
	prod.start();
	intros::thisTask::stop();
}
