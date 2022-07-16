#include <stm32f4_discovery.h>
#include <os.h>

enum
{
	hsmSwitch = hsmUser,
	hsmTick,
};

hsm_state_t *StateOff;
hsm_state_t *StateOn;

unsigned StateOffHandler(hsm_t *hsm, unsigned event)
{
	switch (event)
	{
	case hsmInit:
		LEDs = 0;
		return hsmOK;
	case hsmSwitch:
		hsm_transition(hsm, StateOn, NULL);
		return hsmOK;
	}
	return event;
}

unsigned StateOnHandler(hsm_t *hsm, unsigned event)
{
	switch (event)
	{
	case hsmSwitch:
		hsm_transition(hsm, StateOff, NULL);
		return hsmOK;
	case hsmTick:
		LED_Tick();
		return hsmOK;
	}
	return event;
}

hsm_state_t *StateOff = HSM_STATE_CREATE(NULL, StateOffHandler);
hsm_state_t *StateOn  = HSM_STATE_CREATE(NULL, StateOnHandler);

hsm_t *blinker = HSM_CREATE(1);

int main()
{
	LED_Init();

	hsm_start(blinker, StateOff);
	hsm_send(blinker, hsmSwitch, NULL);
	for (;;)
	{
		tsk_delay(SEC);
		hsm_send(blinker, hsmTick, NULL);
	}
}
