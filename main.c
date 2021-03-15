#include <MKL25Z4.h>
#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
 

{
#endif

#define DAC_Pin 30

static unsigned  int pot_vall;	

void DAC_init(void);
void DAC_write(unsigned int data); 
void ADC_Init(void);
unsigned int adc_read(unsigned char ch);



int main(void) {
DAC_init();
ADC_Init();	
	while(1){
pot_vall = adc_read(0); 													// read channel zero and store it in the pot_Vall 
DAC_write(pot_vall);		
	}
}

void DAC_write(unsigned int data){
	DAC0->DAT[0].DATL = (data & 0xFF);							// write the LS 8 bits   
	DAC0->DAT[0].DATH = ((data >> 8)&0x0F);					// write the MS 4 bits
}
void DAC_init(){
	SIM->SCGC6 |= SIM_SCGC6_DAC0_MASK;      				// Enable clock gating for DAC0
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;		  				// Eanble clock gating for PORTE 
	PORTE->PCR[DAC_Pin] &= ~(7U<<8); 								// Enable Analog Mode for PortE pin30 (DAC Pin)
	DAC0->C1 =0x00; 											      		// Disable Buffer Mode
	DAC0->C2 =0x00; 																		
	DAC0->C0 = DAC_C0_DACEN(1) |										// DAC Enable 
	DAC_C0_DACTRGSEL(1);														// DAC Software trigger & DAC buffer disable
}

void ADC_Init(){
	
				// Enable clocks
	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;							//  ADC 0 clock enable 
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;							// port E clock enable 

				// select analog for pin
	PORTE->PCR[20] &= ~PORT_PCR_MUX_MASK; 
	PORTE->PCR[20] |= PORT_PCR_MUX(0); 
	
	ADC0->SC1[0] = 0x00 ; 													// select channel 0 
					
				// Configure ADC
	ADC0->CFG1 = 0x00; 															// Reset register CFG1
	ADC0->CFG1 |=   ADC_CFG1_ADLPC(0)|							// Low power Consumption
	ADC_CFG1_MODE(1) | 															// single ended 12 bits mode 
	~ADC_CFG1_ADIV_MASK|														//  The divide ratio is 1 and the clock rate is input clock.
	~ADC_CFG1_ADICLK_MASK ;													// Input Bus Clock (20-25 MHz out of reset)
	
	ADC0->CFG2 = 0x00; 															// Reset register CFG2
	ADC0->CFG2 = 	ADC_CFG2_ADLSTS(0)|								// Default longest sample time
	ADC_CFG2_MUXSEL(0)|															// select channel A 
	ADC_CFG2_ADHSC(0);															// Normal conversion sequence selected.
								
															
	
	ADC0->SC3 = 0x00;																// One conversion & Hardware average function disabled

	ADC0->SC2 = ADC_SC2_REFSEL(0)| 									// Default voltage reference pin pair, that is, external pins VREFH and VREFL
	ADC_SC2_ADTRG(0); 															// Software trigger selected
																									// & DMA is disabled 

}
unsigned int adc_read(unsigned char ch)
{
	ADC0->SC1[0] = ch ;   													// Write to SC1A to start conversion
	while(ADC0->SC2 & ADC_SC2_ADACT_MASK); 					// Conversion in progress
	while(!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));   	// Run until the conversion is complete
	return ADC0->R[0];
}