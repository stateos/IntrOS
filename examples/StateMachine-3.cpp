#include <stm32f4_discovery.h>
#include <os.h>
#include <vector>
#include <chrono>

enum
{
	EventALL    = hsmALL,
	EventStop   = hsmStop,
	EventExit   = hsmExit,
	EventEntry  = hsmEntry,
	EventInit   = hsmInit,
	EventSwitch = hsmUser,
	EventTick,
};

auto led        = device::Led();
auto dispatcher = intros::Task(nullptr);
auto StateOff   = intros::State();
auto StateOn    = intros::State();
auto blinker    = intros::StateMachineT<10>();

std::vector<intros::Action> tab =
{
	{ StateOff, EventInit,   []( hsm_t *, unsigned ){ led = 0; } },
	{ StateOff, EventSwitch, StateOn },
	{ StateOn,  EventSwitch, StateOff },
	{ StateOn,  EventTick,   []( hsm_t *, unsigned ){ led.tick(); } },
};

int main()
{
	for (auto& a: tab) a.link();

	blinker.start(dispatcher, StateOff);
	blinker.send(EventSwitch);
	for (;;)
	{
		intros::thisTask::delay(std::chrono::seconds(1));
		blinker.send(EventTick);
	}
}
