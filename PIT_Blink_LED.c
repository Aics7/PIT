//Uses PIT to blink an LED every 400ms. After blinking 5 times, it blinks at 100ms  for 5x and repeat

#include <MKL25Z4.h>

void initialize(void);
void initialize_timer(void);
void blink(void);

#define BLUE         (1)    //PTD1
#define scope        (7)    //PTD7 for scope observation
#define MASK(X)      (1<<X)
volatile int counter = 0;

int main()
{
  initialize();
  initialize_timer();
  while(1)
	{
    blink();
  }
}

void initialize()
{
  SIM->SCGC5 |=SIM_SCGC5_PORTD_MASK;
  PORTD->PCR[BLUE ] &= ~PORT_PCR_MUX_MASK;  //Clear mux
  PORTD->PCR[scope] &= ~PORT_PCR_MUX_MASK;  //Clear mux
  
  PORTD->PCR[BLUE ] |= PORT_PCR_MUX(1);  //setup to be GPIO
  PORTD->PCR[scope] |= PORT_PCR_MUX(1);  //setup to be GPIO
  
  PTD->PDDR |= MASK(BLUE) ;
  PTD->PDDR |= MASK(scope) ;
  
  PTD->PSOR =MASK(BLUE);  //turn off BLUE LED
	PTD->PCOR =MASK(scope);  //set scope output low
}

void initialize_timer()
{
  //Clock gate
  SIM->SCGC6 |=SIM_SCGC6_PIT_MASK;
  //enable PIT timer
  PIT_MCR &= ~PIT_MCR_MDIS_MASK;
  //select channel 0 and load 0x00422444 (400ms)
  PIT->CHANNEL[0].LDVAL =  0x00422444;
  //enable PIT timer, interrupt and chain mode 
  PIT_TCTRL0 |= PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK | PIT_TCTRL_CHN_MASK;
  //clear flag
  PIT_TFLG0 |= PIT_TFLG_TIF_MASK;

  //configure PIT IRQ
  NVIC_ClearPendingIRQ(PIT_IRQn);
  NVIC_SetPriority(PIT_IRQn, 3);
  NVIC_EnableIRQ(PIT_IRQn);
  //__enable_irq();
}

void PIT_IRQHandler()
{
  PTD->PTOR |= MASK(BLUE);    // toggle BLUE LED
  PTD->PTOR |= MASK(scope);    //toggle PTD7 for scope observation
  PIT_TFLG0 |= PIT_TFLG_TIF_MASK;   //clear the interrupt flag
  counter ++;
}

void blink()
{
	if (counter == 0)
	{
		PIT->CHANNEL[0].LDVAL = 0x00422444;    //set time to 400ms
	}
	if(counter == 10)
	{
		PIT->CHANNEL[0].LDVAL = 0x00100011;    //set time to 100ms
	}
	if(counter == 20)
	{
			counter = 0;    //reset count
	}
}
