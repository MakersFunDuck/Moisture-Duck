// ===================================================================================
// Libraries and Definitions
// ===================================================================================

// Libraries
#include <avr/io.h>             // for gpio
#include <avr/sleep.h>          // for the sleep modes
#include <avr/interrupt.h>      // for the interrupts
#include <avr/wdt.h>            // for the watch dog timer
#include <util/delay.h>         // for delays




// Pin definitions
#define PWR_PIN  PB0        // power pin output
#define BUZZER_PIN  PB4       // buzzer pin output
#define TEST_PIN  PB1       // test button input
#define SOIL_PIN  PB3       // Soil  pin input
#define PWR_CHK  PB2        // battery check input

// ADC port definitions
#define Soil_Check  3               // ADC port of measurement
#define VCC_Check  2        // ADC port of measurement

uint8_t count = 0;          //Watchdog counter
#define V_REF 1100          // internal reference voltage in mV

//Functions
void resetWatchdog(void);
void sleep(void);
uint8_t get_vcc(void);
void beep(int num_times);
uint16_t readADC(uint8_t port);
void delay_ms(uint16_t ms);

uint8_t vcccount = 0; //VCC measurement count


// ===================================================================================
// Watchdog and Sleep Implementation
// ===================================================================================



// Reset watchdog timer
void resetWatchdog(void) {
  cli();                                  // timed sequence coming up
  wdt_reset();                            // reset watchdog
  MCUSR = 0;                              // clear various "reset" flags
  WDTCR = (1<<WDCE)|(1<<WDE)|(1<<WDTIF);  // allow changes, disable reset
  WDTCR = (1<<WDTIE)|(1<<WDP3)|(1<<WDP0); // set WDT interval 8 seconds
  sei();                                  // interrupts are required now
      
  
}

// Go to sleep in order to save energy, wake up by watchdog timer or pin change interrupt
void sleep(void) {
  GIFR |= (1<<PCIF);                      // clear any outstanding interrupts
  resetWatchdog();                        // get watchdog ready
  sleep_mode();                           // sleep
}





// ===================================================================================
// Additional Functions
// ===================================================================================

void beep(int num_times) {
  for (int i = 0; i < num_times; i++) {
    for (int j = 0; j < 80; j++) {
      PORTB |= (1 << BUZZER_PIN);  // turn on the buzzer
      _delay_us(100);              // wait for 100 ms
      PORTB &= ~(1 << BUZZER_PIN); // turn off the buzzer
      _delay_us(100);              // wait for 100 ms
      }
    _delay_ms(100);
  }
}


// Analog read ADC
uint16_t readADC(uint8_t port) {
  ADMUX   = (port & 0x03);                // set port against Vcc
  ADCSRA |= (1<<ADSC);                    // start ADC conversion
  while(ADCSRA & (1<<ADSC));              // wait for ADC conversion complete
  return(ADC);                            // read and return measurement
}

// Analog read VCC 
//0,1uF capacitor first will be charged, and soil check pin ADC value will be read
uint16_t readVCC() {
   DDRB |= (1 << PWR_CHK);         // Set PWR_CHK as output
   PORTB |= (1 << PWR_CHK);       // Set PWR_CHK high
   _delay_ms(50);              // Wait for charging
   //DDRB |= ~(1 << PWR_CHK);   // Set PWR_CHK as input
  return readADC(VCC_Check);
  //PORTB &= ~(1 << PWR_CHK);  // Set PWR_CHK low
  //DDRB |= ~(1 << PWR_CHK);   // Set PWR_CHK as input
  
}


// Timer/Counter0 Compare Match A interrupt handler
ISR(TIM0_COMPA_vect)
{
  // Decrement timer value
  OCR0A--;
}
// Watchdog interrupt service routine
ISR(WDT_vect) {
  wdt_disable();                          // disable watchdog
  if (count > 15) // if interrupted 15. times ( after 15*8sec = 120s = 2min )
    {
      count = 0;      
    }
  else{
    count++;
    sleep();
    }
}



// ===================================================================================
// Main Function
// ===================================================================================

int main(void) {
  
  // Reset watchdog timer
  resetWatchdog();                        // do this first in case WDT fires

  // Disable unused peripherals and set sleep mode to save power
  ACSR   = (1<<ACD);                      // disable analog comparator
  DIDR0 |= (1<<SOIL_PIN )|(1<<PWR_CHK);   // disable digital input buffer on ADC pins

  // Setup pins
  DDRB  = (1<<BUZZER_PIN)|(1<<PWR_PIN);       // set output pins
  PORTB = (1<<TEST_PIN);                  // set pull-ups

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // set sleep mode to power down

  // Setup pin change interrupt
  GIMSK = (1<<PCIE);                      // pin change interrupts enable
  PCMSK = (1<<TEST_PIN);                  // set pins for pin change interrupt
  sei();                  // enable global interrupts
       
                                
  
  
  // Loop
  while(1) {
    
    if(~PINB & (1<<TEST_PIN))   beep(1);        // alarm if test button is pressed
    ADCSRA |=  (1<<ADEN);               // enable ADC
    //if (vcccount < 15) vcccount = vcccount++;     // don't always read the vcc  to save energy
    //if (vcccount > 15){
        //uint16_t vcc = readVCC(); 
        //if (vcc<1768) beep(8);            //battery low alarm
        
    /*  
    
    Voltage = ADC_value * Vref / 1024
    = 1768 * 1.1V / 1024
    = 1,89V
    */        
     // vcccount = 0;
     // }
          
     PORTB  |=  (1<<PWR_PIN);          // power measurement circuit
     _delay_us(125);              // give it all a little time  
    do {
      if((readADC(Soil_Check)) < 512   )beep(2);  // beep twice if dry soil
    } while(~PINB & (1<<TEST_PIN));         // repeat if test button is hold
      
    /*
    
    Voltage = ADC_value * Vref / 1024
    = 512 * 1.1V / 1024
    = 0.55V
    */
    
    PORTB  &= ~(1<<PWR_PIN)|~(1<<SOIL_PIN);         // power off to prevent corrosion   
    ADCSRA &= ~(1<<ADEN);         // disable ADC
    sleep();                // sleep for a while ...
  }
}
