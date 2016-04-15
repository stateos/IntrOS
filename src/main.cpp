#include <stm32f4_discovery.h>
#include <os.h>

void proc( unsigned &led, unsigned timePoint )
{
	for (;;)
	{
		ThisTask::sleepUntil(timePoint);
		timePoint += SEC/2;
		led++;
	}
}

auto led = Led();
auto grn = GreenLed();

auto t1 = startTask([]{ proc(led[0], 500*MSEC); });
auto t2 = startTask([]{ proc(led[1], 625*MSEC); });
auto t3 = startTask([]{ proc(led[2], 750*MSEC); });
auto t4 = startTask([]{ proc(led[3], 875*MSEC); });
auto t5 = startTask([]{ proc(grn,   1000*MSEC); });

int main()
{
	ThisTask::stop();
}
