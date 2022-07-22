#include <stm32f4_discovery.h>
#include <os.h>

enum
{
	EventOK     = hsmOK,
	EventStop   = hsmStop,
	EventExit   = hsmExit,
	EventEntry  = hsmEntry,
	EventInit   = hsmInit,
	EventSwitch = hsmUser,
	EventTick,
};

auto blinker    = intros::StateMachineT<10>();
auto dispatcher = intros::Task(nullptr);

extern intros::State StateOff;
extern intros::State StateOn;

intros::State StateOff([](hsm_t *hsm, unsigned event)->unsigned
{
	switch (event)
	{
	case EventInit:
		LEDs = 0;
		return EventOK;
	case EventSwitch:
		hsm->transition(StateOn);
		return EventOK;
	}
	return event;
});

intros::State StateOn([](hsm_t *hsm, unsigned event)->unsigned
{
	switch (event)
	{
	case EventSwitch:
		hsm->transition(StateOff);
		return EventOK;
	case EventTick:
		LED_Tick();
		return EventOK;
	}
	return event;
});

int main()
{
	LED_Init();

	blinker.start(dispatcher, StateOff);
	blinker.send(EventSwitch);

	for (;;)
	{
		tsk_delay(SEC);
		blinker.send(EventTick);
	}
}
