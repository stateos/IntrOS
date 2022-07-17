#include <stm32f4_discovery.h>
#include <os.h>

enum
{
	hsmSwitch = hsmUser,
	hsmTick,
};

unsigned StateOffHandler(hsm_t *, unsigned);
unsigned StateOnHandler (hsm_t *, unsigned);

OS_HSM_STATE(StateOff, NULL, StateOffHandler);
OS_HSM_STATE(StateOn,  NULL, StateOnHandler);

OS_HSM(blinker, 1);

unsigned StateOffHandler(hsm_t *hsm, unsigned event)
{
	switch (event)
	{
	case hsmInit:
		LEDs = 0;
		return hsmOK;
	case hsmSwitch:
		hsm_transition(hsm, StateOn);
		return hsmOK;
	}
	return event;
}

unsigned StateOnHandler(hsm_t *hsm, unsigned event)
{
	switch (event)
	{
	case hsmSwitch:
		hsm_transition(hsm, StateOff);
		return hsmOK;
	case hsmTick:
		LED_Tick();
		return hsmOK;
	}
	return event;
}

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
