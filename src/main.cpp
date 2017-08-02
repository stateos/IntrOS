#include <stm32f4_discovery.h>
#include <os.h>

void proc( unsigned &led, uint32_t timePoint )
{
	for (;;)
	{
		ThisTask::sleepUntil(timePoint += SEC/2);
		led++;
	}
}

auto led = Led();
auto grn = GreenLed();

auto t1 = startTask([] { proc(led[0], SEC/8*0); });
auto t2 = startTask([] { proc(led[1], SEC/8*1); });
auto t3 = startTask([] { proc(led[2], SEC/8*2); });
auto t4 = startTask([] { proc(led[3], SEC/8*3); });
auto t5 = startTask([] { proc(grn,    SEC/8*4); });

int main()
{
	ThisTask::stop();
}
