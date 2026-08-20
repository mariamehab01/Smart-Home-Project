#include <avr/io.h>
#include "Std_Types.h"
#include "BIT_MATH.h"
#include "DIO_Interface.h"
#include "PWM_Config.h"
#include "PWM_Private.h"
#include "PWM_Interface.h"
void PWM_INIT(void)
{
	/* PB3 / OC0 = Output */
	DIO_voidSetPinDirection(PWM_PORT, PWM_PIN, PIN_OUTPUT);
	/* Fast PWM Mode */
	SET_BIT(TCCR0, WGM00);
	SET_BIT(TCCR0, WGM01);
	/* Non-Inverting Mode */
	SET_BIT(TCCR0, COM01);
	CLR_BIT(TCCR0, COM00);
	/* Prescaler = 64 */
	SET_BIT(TCCR0, CS00);
	SET_BIT(TCCR0, CS01);
	CLR_BIT(TCCR0, CS02);
	/* Motor initially OFF */
	OCR0 = 0;
}
void PWM_SET_DUTY_CYCLE(u8 DutyCycle)
{
	if (DutyCycle > 100)
	{
		DutyCycle = 100;
	}
	OCR0 = ((u16)DutyCycle * 255) / 100;
}