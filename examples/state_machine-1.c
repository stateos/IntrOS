#include <stm32f4_discovery.h>
#include <os.h>

enum
{
	hsmSwitch = hsmUser,
	hsmTick,
};

hsm_state_id StateOff;
hsm_state_id StateOn;

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

OS_HSM_STATE(StateOff, NULL, StateOffHandler);
OS_HSM_STATE(StateOn,  NULL, StateOnHandler);

OS_HSM(blinker, 1);

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
