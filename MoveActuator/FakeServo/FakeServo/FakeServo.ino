#include <Settings.h>
#include <SPI.h>

#define INPUT_LOW 1000
#define INPUT_HIGH 2000
#define LEFTS_LOW 55
#define LEFTS_HIGH 255
#define RIGHTS_LOW 55
#define RIGHTS_HIGH 255

#define INVALID_DATA 0L

#define STEER_IN_PIN 2

#define ProcessFallOfINT()  RC_elapsed=(micros()-RC_rise)
#define ProcessRiseOfINT() RC_rise=micros()

volatile unsigned long RC_rise;
volatile unsigned long RC_elapsed;
volatile bool RC_Done;

void ISR_RDR_rise(){
  noInterrupts();
  ProcessRiseOfINT();
  digitalWrite(6, HIGH);
  attachInterrupt(digitalPinToInterrupt(STEER_IN_PIN), ISR_RDR_fall, FALLING);
  interrupts();
}

void ISR_RDR_fall(){
  noInterrupts();
  ProcessFallOfINT();
  RC_Done = 1;
  digitalWrite(6, LOW);
  attachInterrupt(digitalPinToInterrupt(STEER_IN_PIN), ISR_RDR_rise, RISING);
  interrupts();
}

int input = 1500;
int leftgoal, rightgoal;
float leftS, rightS;
const float maxstep = 5.5158;

// Select IC 3 DAC (channels C and D)
#define SelectCD 49 
// Select IC 2 DAC (channels A and B)
#define SelectAB 53

void setup() {
  Serial.begin(9600);
  SPI.begin(); 
  attachInterrupt(digitalPinToInterrupt(STEER_IN_PIN),  ISR_RDR_rise,  RISING);
  RC_rise = INVALID_DATA;
  RC_elapsed = INVALID_DATA;
  RC_Done = 0;
  
  Serial.println("Setup");
}

void loop() {
  Serial.print(RC_elapsed); Serial.print("\t");
  if(input != RC_elapsed)
  {
    input = RC_elapsed > INPUT_HIGH ? INPUT_HIGH : RC_elapsed;
    input = input < INPUT_LOW ? INPUT_LOW : input;    
  
    leftgoal = map(input, INPUT_LOW, INPUT_HIGH, LEFTS_LOW, LEFTS_HIGH);
    rightgoal = map(input, INPUT_LOW, INPUT_HIGH, RIGHTS_LOW, RIGHTS_HIGH);
  }

  leftS += leftS < leftgoal ? min(leftgoal - leftS, maxstep) : -min(leftS - leftgoal, maxstep);
  rightS += rightS < rightgoal ? min(rightgoal - rightS, maxstep) : -min(rightS - rightgoal, maxstep);

  DAC_Write(0, (int)leftS);
  DAC_Write(1, (int)rightS);
  
  Serial.print(input); Serial.print("\t");
  Serial.print(leftS); Serial.print("\t");
  Serial.print(rightS); Serial.print("\t");
  Serial.println();
}

/* DAC_Write applies value to address, producing an analog voltage.
  // address: 0 for chan A; 1 for chan B; 2 for chan C; 3 for chan D
  // value: digital value converted to analog voltage
  // Output goes to mcp 4802 Digital-Analog Converter Chip via SPI
  // There is no input back from the chip.
  
  REGISTER 5-3: WRITE COMMAND REGISTER FOR MCP4802 (8-BIT DAC)
  A/B — GA SHDN D7 D6 D5 D4 D3 D2 D1 D0 x x x x
  bit 15 bit 0
  bit 15 A/B: DACA or DACB Selection bit
  1 = Write to DACB
  0 = Write to DACA
  bit 14 — Don’t Care
  bit 13 GA: Output Gain Selection bit
  1 = 1x (VOUT = VREF * D/4096)
  0 = 2x (VOUT = 2 * VREF * D/4096), where internal VREF = 2.048V.
  bit 12 SHDN: Output Shutdown Control bit
  1 = Active mode operation. VOUT is available.
  0 = Shutdown the selected DAC channel. Analog output is not available at the channel that was shut down.
  VOUT pin is connected to 500 k (typical)
  bit 11-0 D11:D0: DAC Input Data bits. Bit x is ignored.
  With 4.95 V on Vcc, observed output for 255 is 4.08V.
  This is as documented; with gain of 2, maximum output is 2 * Vref
*/
void DAC_Write(int address, int value){
  int byte1 = ((value & 0xF0) >> 4) | 0x10; // active mode, bits D7-D4
  int byte2 = (value & 0x0F) << 4; // D3-D0

  if (address < 2)
  {
    // take the SS pin low to select the chip:
    digitalWrite(SelectAB, LOW);
    if (address >= 0)
    {
      if (address == 1)
        byte1 |= 0x80; // second channnel
      SPI.transfer(byte1);
      SPI.transfer(byte2);
    }
    // take the SS pin high to de-select the chip:
    digitalWrite(SelectAB, HIGH);
  }
  else
  {
    // take the SS pin low to select the chip:
    digitalWrite(SelectCD, LOW);
    if (address <= 3)
    {
      if (address == 3)
        byte1 |= 0x80; // second channnel
      SPI.transfer(byte1);
      SPI.transfer(byte2);
    }
    // take the SS pin high to de-select the chip:
    digitalWrite(SelectCD, HIGH);
  }
}
