#include <stm32f4_discovery.h>
#include <os.h>

enum
{
	hsmSwitch = hsmUser,
	hsmTick,
};

hsm_state_t StateOff;
hsm_state_t StateOn;
hsm_t       blinker;
hsm_event_t blinker_queue[100];
tsk_t       dispatcher;
stk_t       dispatcher_stack[100];

unsigned StateOffHandler(hsm_t *hsm, unsigned event)
{
	switch (event)
	{
	case hsmInit:
		LEDs = 0;
		return hsmOK;
	case hsmSwitch:
		hsm_transition(hsm, &StateOn);
		return hsmOK;
	}
	return event;
}

unsigned StateOnHandler(hsm_t *hsm, unsigned event)
{
	switch (event)
	{
	case hsmSwitch:
		hsm_transition(hsm, &StateOff);
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

	hsm_initState(&StateOff, NULL, StateOffHandler);
	hsm_initState(&StateOn,  NULL, StateOnHandler);
	hsm_init(&blinker, blinker_queue, sizeof(blinker_queue));
	wrk_init(&dispatcher, NULL, dispatcher_stack, sizeof(dispatcher_stack));
	hsm_start(&blinker, &dispatcher, &StateOff);
	hsm_send(&blinker, hsmSwitch, NULL);
	for (;;)
	{
		tsk_delay(SEC);
		hsm_send(&blinker, hsmTick, NULL);
	}
}
