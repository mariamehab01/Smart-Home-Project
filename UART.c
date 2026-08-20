#define F_CPU 8000000UL

#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "UART_Private.h"
#include "UART_Configh.h"
#include "UART_Interface.h"


void USART_Init(u16 UART_BAUDRATE)
{
    u8 LOC_Var = 0;

    u16 BAUD_PRESCALE =
        (((F_CPU / (UART_BAUDRATE * 16UL))) - 1);

    CLR_BIT(UCSRA, 1);

    UBRRH = (u8)(BAUD_PRESCALE >> 8);
    UBRRL = (u8)BAUD_PRESCALE;

    SET_BIT(UCSRB, 3);
    SET_BIT(UCSRB, 4);

    CLR_BIT(LOC_Var, 6);

    CLR_BIT(LOC_Var, 4);
    CLR_BIT(LOC_Var, 5);

    CLR_BIT(LOC_Var, 3);

    SET_BIT(LOC_Var, 1);
    SET_BIT(LOC_Var, 2);

    SET_BIT(LOC_Var, 7);

    UCSRC = LOC_Var;
}


void USART_Transmit(u8 data)
{
    while (GET_BIT(UCSRA, 5) == 0);

    UDR = data;
}


u8 USART_Receive(void)
{
    while (GET_BIT(UCSRA, 7) == 0);

    return UDR;
}


u8 USART_ReceiveNonBlocking(u8 *copy_u8Data)
{
    /* Check the RX Complete (RXC) flag in UCSRA register */
    if (GET_BIT(UCSRA, 7) == 1)
    {
        /* Read the received data from the UDR buffer and store it */
        *copy_u8Data = UDR;

        /* Return 1 indicating data was received */
        return 1;
    }

    /* Return 0 immediately if the receive buffer is empty */
    return 0;
}