#include <stm32f4_discovery.h>
#include <os.h>

enum
{
	hsmSwitch = hsmUser,
	hsmTick,
};

hsm_handler_t StateOffHandler;
hsm_handler_t StateOnHandler;

OS_HSM_STATE(StateOff, NULL, StateOffHandler);
OS_HSM_STATE(StateOn,  NULL, StateOnHandler);

OS_HSM(blinker, 1);
OS_TSK(dispatcher, NULL);

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

	hsm_start(blinker, dispatcher, StateOff);
	hsm_send(blinker, hsmSwitch, NULL);
	for (;;)
	{
		tsk_delay(SEC);
		hsm_send(blinker, hsmTick, NULL);
	}
}
