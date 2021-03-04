#include <stm32f4_discovery.h>
#include <os.h>

using namespace device;
using namespace intros;

void proc( unsigned &led, cnt_t timePoint )
{
	ThisTask::sleepUntil(timePoint);
	for (;;)
	{
		ThisTask::sleepNext(SEC/2);
		led++;
	}
}

auto led = Led();
auto grn = GreenLed();

auto t1 = Task::Start([]{ proc(led[0], SEC/8*0); });
auto t2 = Task::Start([]{ proc(led[1], SEC/8*1); });
auto t3 = Task::Start([]{ proc(led[2], SEC/8*2); });
auto t4 = Task::Start([]{ proc(led[3], SEC/8*3); });
auto t5 = Task::Start([]{ proc(grn,    SEC/8*4); });

int main()
{
	ThisTask::stop();
}
