/*==================[inclusions]=============================================*/

#include "board.h"
#include "my_rtos.h"
#include "my_rtos_vectors.h"

#include <string.h>

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

static user_irq_handler_t user_irq_handlers[QEI_IRQn+1];

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/
extern osState_t osState;
extern bool osSwitchRequired;
/*==================[internal functions definition]==========================*/

static void MyRtos_IRQHandler(LPC43XX_IRQn_Type irq) {

   // Save previous OS state
   osState_t prevState = osState;
	
   // Change OS state to IRQ
   osState = STATE_IRQ;

   // If user has defined a handler call it
	if (user_irq_handlers[irq] != NULL) {
		user_irq_handlers[irq]();
	}

   // Restore previous OS state
   osState = prevState;

   // Check if a context switching is required
   if (osSwitchRequired) {
      MyRtos_SchedulerUpdate();
   }
}

/*==================[external functions definition]==========================*/

int32_t MyRtos_AttachIRQ(LPC43XX_IRQn_Type irq, user_irq_handler_t callback) {
	int32_t rv = -1;
	if (user_irq_handlers[irq] == NULL) {
		user_irq_handlers[irq] = callback;
		NVIC_ClearPendingIRQ(irq);
		NVIC_EnableIRQ(irq);
		rv = 0;
	}
	return rv;
}

int32_t MyRtos_DetachIRQ(LPC43XX_IRQn_Type irq) {
	int32_t rv = -1;
	if (user_irq_handlers[irq] != NULL) {
		user_irq_handlers[irq] = NULL;
		NVIC_ClearPendingIRQ(irq);
		NVIC_DisableIRQ(irq);
		rv = 0;
	}
	return rv;
}


/*==================[interrupt service routines]=============================*/

void DAC_IRQHandler(void){MyRtos_IRQHandler(         DAC_IRQn         );}
void M0APP_IRQHandler(void){MyRtos_IRQHandler(       M0APP_IRQn       );}
void DMA_IRQHandler(void){MyRtos_IRQHandler(         DMA_IRQn         );}
void FLASH_EEPROM_IRQHandler(void){MyRtos_IRQHandler(RESERVED1_IRQn   );}
void ETH_IRQHandler(void){MyRtos_IRQHandler(         ETHERNET_IRQn    );}
void SDIO_IRQHandler(void){MyRtos_IRQHandler(        SDIO_IRQn        );}
void LCD_IRQHandler(void){MyRtos_IRQHandler(         LCD_IRQn         );}
void USB0_IRQHandler(void){MyRtos_IRQHandler(        USB0_IRQn        );}
void USB1_IRQHandler(void){MyRtos_IRQHandler(        USB1_IRQn        );}
void SCT_IRQHandler(void){MyRtos_IRQHandler(         SCT_IRQn         );}
void RIT_IRQHandler(void){MyRtos_IRQHandler(         RITIMER_IRQn     );}
void TIMER0_IRQHandler(void){MyRtos_IRQHandler(      TIMER0_IRQn      );}
void TIMER1_IRQHandler(void){MyRtos_IRQHandler(      TIMER1_IRQn      );}
void TIMER2_IRQHandler(void){MyRtos_IRQHandler(      TIMER2_IRQn      );}
void TIMER3_IRQHandler(void){MyRtos_IRQHandler(      TIMER3_IRQn      );}
void MCPWM_IRQHandler(void){MyRtos_IRQHandler(       MCPWM_IRQn       );}
void ADC0_IRQHandler(void){MyRtos_IRQHandler(        ADC0_IRQn        );}
void I2C0_IRQHandler(void){MyRtos_IRQHandler(        I2C0_IRQn        );}
void SPI_IRQHandler(void){MyRtos_IRQHandler(         I2C1_IRQn        );}
void I2C1_IRQHandler(void){MyRtos_IRQHandler(        SPI_INT_IRQn     );}
void ADC1_IRQHandler(void){MyRtos_IRQHandler(        ADC1_IRQn        );}
void SSP0_IRQHandler(void){MyRtos_IRQHandler(        SSP0_IRQn        );}
void SSP1_IRQHandler(void){MyRtos_IRQHandler(        SSP1_IRQn        );}
// void UART0_IRQHandler(void){MyRtos_IRQHandler(       USART0_IRQn      );}
// void UART1_IRQHandler(void){MyRtos_IRQHandler(       UART1_IRQn       );}
// void UART2_IRQHandler(void){MyRtos_IRQHandler(       USART2_IRQn      );}
// void UART3_IRQHandler(void){MyRtos_IRQHandler(       USART3_IRQn      );}
void I2S0_IRQHandler(void){MyRtos_IRQHandler(        I2S0_IRQn        );}
void I2S1_IRQHandler(void){MyRtos_IRQHandler(        I2S1_IRQn        );}
void SPIFI_IRQHandler(void){MyRtos_IRQHandler(       RESERVED4_IRQn   );}
void SGPIO_IRQHandler(void){MyRtos_IRQHandler(       SGPIO_INT_IRQn   );}
void GPIO0_IRQHandler(void){MyRtos_IRQHandler(       PIN_INT0_IRQn    );}
void GPIO1_IRQHandler(void){MyRtos_IRQHandler(       PIN_INT1_IRQn    );}
void GPIO2_IRQHandler(void){MyRtos_IRQHandler(       PIN_INT2_IRQn    );}
void GPIO3_IRQHandler(void){MyRtos_IRQHandler(       PIN_INT3_IRQn    );}
void GPIO4_IRQHandler(void){MyRtos_IRQHandler(       PIN_INT4_IRQn    );}
void GPIO5_IRQHandler(void){MyRtos_IRQHandler(       PIN_INT5_IRQn    );}
void GPIO6_IRQHandler(void){MyRtos_IRQHandler(       PIN_INT6_IRQn    );}
void GPIO7_IRQHandler(void){MyRtos_IRQHandler(       PIN_INT7_IRQn    );}
void GINT0_IRQHandler(void){MyRtos_IRQHandler(       GINT0_IRQn       );}
void GINT1_IRQHandler(void){MyRtos_IRQHandler(       GINT1_IRQn       );}
void EVRT_IRQHandler(void){MyRtos_IRQHandler(        EVENTROUTER_IRQn );}
void CAN1_IRQHandler(void){MyRtos_IRQHandler(        C_CAN1_IRQn      );}
void ADCHS_IRQHandler(void){MyRtos_IRQHandler(       ADCHS_IRQn       );}
void ATIMER_IRQHandler(void){MyRtos_IRQHandler(      ATIMER_IRQn      );}
void RTC_IRQHandler(void){MyRtos_IRQHandler(         RTC_IRQn         );}
void WDT_IRQHandler(void){MyRtos_IRQHandler(         WWDT_IRQn        );}
void M0SUB_IRQHandler(void){MyRtos_IRQHandler(       M0SUB_IRQn       );}
void CAN0_IRQHandler(void){MyRtos_IRQHandler(        C_CAN0_IRQn      );}
void QEI_IRQHandler(void){MyRtos_IRQHandler(         QEI_IRQn         );}

/*==================[end of file]============================================*/
