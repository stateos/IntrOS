#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

auto led = Led();
auto sem = Semaphore(0);

struct Consumer : public Task
{
	Consumer(): Task(run) { start(); }

	private:
	static void run()
	{
		sem.wait();
		led.tick();
	}
};
	
struct Producer : public Task
{
	Producer(): Task(run) { start(); }

	private:
	static void run()
	{
		thisTask::sleepFor(SEC);
		sem.give();
	}
};

auto cons = Consumer();
auto prod = Producer();

int main()
{
	thisTask::sleep();
}
