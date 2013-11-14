#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_dma.h"
#include "misc.h"

/* FORWARD DECLARATIONS */
void RCC_configure(void);
void NVIC_configure(void);
void DMA_configure(void);
void stopwatch (uint32_t* pMicro);

/* Global variables */
uint32_t g_usCounter = 0;
uint32_t g_usCounterMCU = 0;
uint32_t g_usCounterDMA = 0;

#define SIZE 5 * 1024
uint32_t g_source[SIZE];
uint32_t g_dest[SIZE];

/*
 * MAIN
 */
int main()
{
	/* Setup SysTick Timer for 1 us interrupts */
	SysTick_Config(SystemCoreClock / 1000 / 1000);

	/* Needed configuration */
	RCC_configure();
	DMA_configure();
	NVIC_configure();

	/* Enable DMA1 Channel transfer */
	DMA_Cmd(DMA1_Channel1, ENABLE);

	/* Transfer using MCU */
	uint32_t i = 0;
	for (i = 0; i < SIZE; i++)
	{
	    g_dest[i] = g_source[i];
	}

	stopwatch(&g_usCounterMCU);

	/* PUT HERE A BREAKPOINT AND CHECK VALUE OF g_usCounterMCU AND g_usCounterDMA
	 * YOU WILL SEE THAT DMA HAS BEEN FASTER TO TRANSFER DATA */
    while(1)
    {
    }

    /* Control should never come here */
    return 0;
}

/*
 * System clock configuration
 */
void RCC_configure(void)
{
	// Enable clock for DMA1 peripheral
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

/*
 * Configure Nested Vector Interrupt Controller (NVIC)
 */
void NVIC_configure(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	// Enable DMA1 channel IRQ Channel
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
 * Configure DMA
 */
void DMA_configure(void)
{
	DMA_InitTypeDef  DMA_InitStructure;

	DMA_DeInit(DMA1_Channel1);

	// DMA channel used for memory to memory transfer
	DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;

	// Normal mode (not circular)
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;

	// Priority
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;

	// Source and destination data size word=32bit
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;

	// Automatic memory increment enable for destination and source
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;

	// Location assigned to peripheral register (actually memory) will be source
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;

	// Size of data to be transfered
	DMA_InitStructure.DMA_BufferSize = SIZE;

	// Source and destination base addresses
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)g_source;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)g_dest;

	// Send values to DMA registers
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	// Enable DMA1 Channel Transfer Complete interrupt
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
}

/*
 * Overwrite the default handler for DMA1 interrupts
 */
void DMA1_Channel1_IRQHandler(void)
{
  // Handle only transfer complete (TC) type interrupts
  if(DMA_GetITStatus(DMA1_IT_TC1))
  {
	  // Clear the pending bits
	  DMA_ClearITPendingBit(DMA1_IT_GL1);

	  // Transfer completed, stop the stopwatch
	  stopwatch(&g_usCounterDMA);
  }
}

/*
 * Overwrite weak handler for systick interrupts
 */
void SysTick_Handler(void)
{
		g_usCounter++;
}

/*
 * Stop the stopwatch
 * @param
 */
void stopwatch (uint32_t* pMicro)
{
	*pMicro = g_usCounter;
}
