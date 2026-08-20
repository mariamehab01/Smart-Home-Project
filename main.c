/*
 * smart_home_gp.c
 *
 * Created: 8/20/2026 8:02:03 PM
 * Author : PC
 */ 

#define F_CPU 8000000UL

#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "DIO_Interface.h"
#include "ADC_Interface.h"
#include "EXT_Interface.h"
#include "GIE_Interface.h"
#include "UART_Interface.h"

#include "LCD_Interface.h"
#include "PWM_Interface.h"

#define RAIN_SENSOR_PORT     PORTC_ID
#define RAIN_SENSOR_PIN      PIN6

#define RAIN_LED_PORT        PORTC_ID
#define RAIN_LED_PIN         PIN7

#define GREEN_LED_PORT       PORTB_ID
#define GREEN_LED_PIN        PIN0

#define RED_LED_PORT         PORTB_ID
#define RED_LED_PIN          PIN1

#define BUZZER_PORT          PORTB_ID
#define BUZZER_PIN           PIN2

#define MANUAL_BUTTON_PORT   PORTB_ID
#define MANUAL_BUTTON_PIN    PIN4

#define MANUAL_SW_PORT       PORTD_ID
#define MANUAL_SW_PIN        PIN4

#define WIRELESS_SW_PORT     PORTD_ID
#define WIRELESS_SW_PIN      PIN5

#define RELAY_PORT           PORTD_ID
#define RELAY_PIN            PIN6

#define LDR_SW_PORT          PORTD_ID
#define LDR_SW_PIN           PIN7

volatile u8 Alarm_Flag = 0;

void Flame_Gas_CallBack(void)
{
	Alarm_Flag = 1;
}

u16 Get_LCD_Temperature(void)
{
	u16 ADC_Value;
	u16 Temperature;

	ADC_Value = ADC_Read_value(ADC_CHANNEL0);

	Temperature =
	((u32)ADC_Value * 500UL) / 1024UL;

	return Temperature;
}

u16 Get_LDR_Value(void)
{
	return ADC_Read_value(ADC_CHANNEL1);
}

u16 Get_Fan_Temperature(void)
{
	u16 ADC_Value;
	u16 Temperature;

	ADC_Value = ADC_Read_value(ADC_CHANNEL3);

	Temperature =
	((u32)ADC_Value * 500UL) / 1023UL;

	return Temperature;
}

int main(void)
{
	u16 LCD_Temperature;
	u16 Fan_Temperature;
	u16 LDR_Value;

	u8 Rain_State;

	u8 ManualSw;
	u8 WirelessSw;
	u8 LdrSw;

	u8 BtnState;

	u8 RxData = 0;
	u8 BtState = 0;

	u8 RelayState;

	u8 TargetDuty = 0;
	u8 CurrentDuty = 0;
	u8 RampCounter = 0;

	u16 PreviousTemperature = 1000;

	u8 PreviousDayState = 2;
	u8 PreviousRainState = 2;

	u8 NormalScreenInit = 1;

	u8 AlarmScreenShown = 0;
	u16 AlarmCounter = 0;

	DIO_voidSetPinDirection(
	GREEN_LED_PORT,
	GREEN_LED_PIN,
	PIN_OUTPUT
	);

	DIO_voidSetPinDirection(
	RED_LED_PORT,
	RED_LED_PIN,
	PIN_OUTPUT
	);

	DIO_voidSetPinDirection(
	BUZZER_PORT,
	BUZZER_PIN,
	PIN_OUTPUT
	);

	DIO_voidSetPinDirection(
	RELAY_PORT,
	RELAY_PIN,
	PIN_OUTPUT
	);

	DIO_voidSetPinDirection(
	RAIN_LED_PORT,
	RAIN_LED_PIN,
	PIN_OUTPUT
	);

	DIO_voidSetPinDirection(
	PORTD_ID,
	PIN2,
	PIN_INPUT
	);

	DIO_voidSetPinDirection(
	PORTA_ID,
	PIN0,
	PIN_INPUT
	);

	DIO_voidSetPinDirection(
	PORTA_ID,
	PIN1,
	PIN_INPUT
	);

	DIO_voidSetPinDirection(
	PORTA_ID,
	PIN3,
	PIN_INPUT
	);

	DIO_voidSetPinDirection(
	RAIN_SENSOR_PORT,
	RAIN_SENSOR_PIN,
	PIN_INPUT
	);

	DIO_voidSetPinDirection(
	MANUAL_BUTTON_PORT,
	MANUAL_BUTTON_PIN,
	PIN_INPUT
	);

	DIO_voidSetPinDirection(
	MANUAL_SW_PORT,
	MANUAL_SW_PIN,
	PIN_INPUT
	);

	DIO_voidSetPinDirection(
	WIRELESS_SW_PORT,
	WIRELESS_SW_PIN,
	PIN_INPUT
	);

	DIO_voidSetPinDirection(
	LDR_SW_PORT,
	LDR_SW_PIN,
	PIN_INPUT
	);

	DIO_voidSetPinValue(
	GREEN_LED_PORT,
	GREEN_LED_PIN,
	PIN_HIGH
	);

	DIO_voidSetPinValue(
	RED_LED_PORT,
	RED_LED_PIN,
	PIN_LOW
	);

	DIO_voidSetPinValue(
	BUZZER_PORT,
	BUZZER_PIN,
	PIN_LOW
	);

	DIO_voidSetPinValue(
	RELAY_PORT,
	RELAY_PIN,
	PIN_LOW
	);

	DIO_voidSetPinValue(
	RAIN_LED_PORT,
	RAIN_LED_PIN,
	PIN_LOW
	);

	ADC_INIT();

	LCD_voidInit();

	HLCD_voidClearLCD();

	USART_Init(9600);

	PWM_INIT();

	PWM_SET_DUTY_CYCLE(0);

	EXT_voidInit(
	EXT0,
	RISING_EDGE
	);

	EXT_voidSetCallBack(
	Flame_Gas_CallBack,
	EXT0
	);

	GIE_Enable();

	while (1)
	{
		LCD_Temperature =
		Get_LCD_Temperature();

		LDR_Value =
		Get_LDR_Value();

		Fan_Temperature =
		Get_Fan_Temperature();

		Rain_State =
		DIO_voidGetBitValue(
		RAIN_SENSOR_PORT,
		RAIN_SENSOR_PIN
		);

		if (Rain_State == PIN_HIGH)
		{
			DIO_voidSetPinValue(
			RAIN_LED_PORT,
			RAIN_LED_PIN,
			PIN_HIGH
			);
		}
		else
		{
			DIO_voidSetPinValue(
			RAIN_LED_PORT,
			RAIN_LED_PIN,
			PIN_LOW
			);
		}

		if (Fan_Temperature < 10)
		{
			TargetDuty = 0;
			CurrentDuty = 0;
			RampCounter = 0;
		}
		else if (Fan_Temperature < 20)
		{
			TargetDuty = 50;
		}
		else if (Fan_Temperature < 30)
		{
			TargetDuty = 70;
		}
		else
		{
			TargetDuty = 100;
		}

		if (Fan_Temperature >= 10)
		{
			RampCounter++;

			if (RampCounter >= 5)
			{
				RampCounter = 0;

				if (CurrentDuty < TargetDuty)
				{
					CurrentDuty++;
				}
				else if (CurrentDuty > TargetDuty)
				{
					CurrentDuty--;
				}
			}
		}

		PWM_SET_DUTY_CYCLE(CurrentDuty);

		RelayState = 0;

		ManualSw =
		DIO_voidGetBitValue(
		MANUAL_SW_PORT,
		MANUAL_SW_PIN
		);

		WirelessSw =
		DIO_voidGetBitValue(
		WIRELESS_SW_PORT,
		WIRELESS_SW_PIN
		);

		LdrSw =
		DIO_voidGetBitValue(
		LDR_SW_PORT,
		LDR_SW_PIN
		);

		if (ManualSw == PIN_HIGH)
		{
			BtnState =
			DIO_voidGetBitValue(
			MANUAL_BUTTON_PORT,
			MANUAL_BUTTON_PIN
			);

			if (BtnState == PIN_HIGH)
			{
				RelayState = 1;
			}
		}

		if (WirelessSw == PIN_HIGH)
		{
			if (
			USART_ReceiveNonBlocking(
			&RxData
			) == 1
			)
			{
				if (RxData == '1')
				{
					BtState = 1;
				}
				else if (RxData == '0')
				{
					BtState = 0;
				}
			}

			if (BtState == 1)
			{
				RelayState = 1;
			}
		}

		if (LdrSw == PIN_HIGH)
		{
			if (LDR_Value < 500)
			{
				RelayState = 1;
			}
		}

		if (RelayState == 1)
		{
			DIO_voidSetPinValue(
			RELAY_PORT,
			RELAY_PIN,
			PIN_HIGH
			);
		}
		else
		{
			DIO_voidSetPinValue(
			RELAY_PORT,
			RELAY_PIN,
			PIN_LOW
			);
		}

		if (Alarm_Flag == 1)
		{
			if (AlarmScreenShown == 0)
			{
				AlarmScreenShown = 1;

				AlarmCounter = 0;

				DIO_voidSetPinValue(
				RED_LED_PORT,
				RED_LED_PIN,
				PIN_HIGH
				);

				DIO_voidSetPinValue(
				GREEN_LED_PORT,
				GREEN_LED_PIN,
				PIN_LOW
				);

				DIO_voidSetPinValue(
				BUZZER_PORT,
				BUZZER_PIN,
				PIN_HIGH
				);

				HLCD_voidClearLCD();

				Lcd_Goto_Row_Column(0,0);

				Lcd_DisplayStr(
				(u8*)"!!! ALARM !!!"
				);

				Lcd_Goto_Row_Column(1,0);

				Lcd_DisplayStr(
				(u8*)"FIRE / GAS"
				);
			}

			AlarmCounter++;

			if (AlarmCounter >= 300)
			{
				DIO_voidSetPinValue(
				RED_LED_PORT,
				RED_LED_PIN,
				PIN_LOW
				);

				DIO_voidSetPinValue(
				BUZZER_PORT,
				BUZZER_PIN,
				PIN_LOW
				);

				DIO_voidSetPinValue(
				GREEN_LED_PORT,
				GREEN_LED_PIN,
				PIN_HIGH
				);

				Alarm_Flag = 0;

				AlarmScreenShown = 0;

				NormalScreenInit = 1;
			}
		}
		else
		{
			if (NormalScreenInit == 1)
			{
				HLCD_voidClearLCD();

				PreviousTemperature = 1000;

				PreviousDayState = 2;

				PreviousRainState = 2;

				Lcd_Goto_Row_Column(3,0);

				Lcd_DisplayStr(
				(u8*)"System: SAFE        "
				);

				NormalScreenInit = 0;
			}

			if (
			LCD_Temperature !=
			PreviousTemperature
			)
			{
				Lcd_Goto_Row_Column(0,0);

				Lcd_DisplayStr(
				(u8*)"Temp:"
				);

				Lcd_Displaynum(
				LCD_Temperature
				);

				Lcd_DisplayStr(
				(u8*)" C          "
				);

				PreviousTemperature =
				LCD_Temperature;
			}

			if (LDR_Value > 500)
			{
				if (PreviousDayState != 1)
				{
					Lcd_Goto_Row_Column(1,0);

					Lcd_DisplayStr(
					(u8*)"State: DAY          "
					);

					PreviousDayState = 1;
				}
			}
			else
			{
				if (PreviousDayState != 0)
				{
					Lcd_Goto_Row_Column(1,0);

					Lcd_DisplayStr(
					(u8*)"State: NIGHT        "
					);

					PreviousDayState = 0;
				}
			}

			if (
			Rain_State !=
			PreviousRainState
			)
			{
				Lcd_Goto_Row_Column(2,0);

				if (Rain_State == PIN_HIGH)
				{
					Lcd_DisplayStr(
					(u8*)"It's raining        "
					);
				}
				else
				{
					Lcd_DisplayStr(
					(u8*)"No rain             "
					);
				}

				PreviousRainState =
				Rain_State;
			}
		}
	}

	return 0;
}