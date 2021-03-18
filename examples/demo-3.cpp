#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

void proc( unsigned &led, Clock::time_point timePoint )
{
	this_task::sleepUntil(timePoint);
	for (;;)
	{
		this_task::sleepNext(std::chrono::milliseconds{500});
		led++;
	}
}

int main()
{
	auto led = Led();
	auto grn = GreenLed();
	auto now = Clock::now();

	auto t1 = Task::Start(proc, std::ref(led[0]), now + std::chrono::milliseconds{  0});
	auto t2 = Task::Start(proc, std::ref(led[1]), now + std::chrono::milliseconds{125});
	auto t3 = Task::Start(proc, std::ref(led[2]), now + std::chrono::milliseconds{250});
	auto t4 = Task::Start(proc, std::ref(led[3]), now + std::chrono::milliseconds{375});
	auto t5 = Task::Start(proc, std::ref(grn),    now + std::chrono::milliseconds{500});

	this_task::stop();
}
