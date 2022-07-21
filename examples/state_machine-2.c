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

hsm_state_t *StateOff;
hsm_state_t *StateOn;

unsigned StateOffHandler(hsm_t *hsm, unsigned event)
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
}

unsigned StateOnHandler(hsm_t *hsm, unsigned event)
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
}

hsm_state_t *StateOff = HSM_STATE_CREATE(NULL, StateOffHandler);
hsm_state_t *StateOn  = HSM_STATE_CREATE(NULL, StateOnHandler);

hsm_t *blinker = HSM_CREATE(1);
tsk_t *dispatcher = TSK_CREATE(NULL);

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
