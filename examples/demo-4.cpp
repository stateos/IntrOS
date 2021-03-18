#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;
using namespace std::chrono_literals;

void proc( unsigned &led, Clock::duration start )
{
	this_task::sleepUntil(Clock::time_point(start));
	for (;;)
	{
		this_task::sleepNext(500ms);
		led++;
	}
}

int main()
{
	auto led = Led();
	auto grn = GreenLed();

	auto t1 = Task::Start([&]{ proc(led[0],   0ms); });
	auto t2 = Task::Start([&]{ proc(led[1], 125ms); });
	auto t3 = Task::Start([&]{ proc(led[2], 250ms); });
	auto t4 = Task::Start([&]{ proc(led[3], 375ms); });
	auto t5 = Task::Start([&]{ proc(grn,    500ms); });

	this_task::stop();
}
