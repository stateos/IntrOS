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

#define State extern hsm_state_id

State StateOff;
State StateOn;

OS_HSM_STATE(StateOff, NULL, [](hsm_t *hsm, unsigned event)->unsigned
{
	switch (event)
	{
	case EventInit:
		LEDs = 0;
		return EventOK;
	case EventSwitch:
		hsm_transition(hsm, StateOn);
		return EventOK;
	}
	return event;
});

OS_HSM_STATE(StateOn, NULL, [](hsm_t *hsm, unsigned event)->unsigned
{
	switch (event)
	{
	case EventSwitch:
		hsm_transition(hsm, StateOff);
		return EventOK;
	case EventTick:
		LED_Tick();
		return EventOK;
	}
	return event;
});

OS_HSM(blinker, 1);
OS_TSK(dispatcher, NULL);

int main()
{
	LED_Init();

	hsm_start(blinker, dispatcher, StateOff);
	hsm_send(blinker, EventSwitch, NULL);
	for (;;)
	{
		tsk_delay(SEC);
		hsm_send(blinker, EventTick, NULL);
	}
}
