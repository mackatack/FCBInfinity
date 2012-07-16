/*

This is the main Arduino/Teensyduino file for the FCBInfinity project.
Please note that this is VERY much still a work in progress and you should
consider the code below still in Alpha/Testing phase. Use at work risk

-Mackatack

*/

#include <MIDI.h>
#include <Bounce.h>
#include <LedControl.h>
#include <LiquidCrystalFast.h>
 
// Initialisation of the LCD
// link here
//                   (RS, RW, Enable, D4, D5, D6, D7)
LiquidCrystalFast lcd(19, 18, 38, 39, 40, 41, 42);

// Initialisation of the MAX chip, this chip controls all the leds on the system,
// the three LED-digits and all the led indicators for the buttons etc.
// http://www.pjrc.com/teensy/td_libs_LedControl.html
//                               (DIN_pin, CLK_pin, LOAD_pin, number_of_chips);
LedControl mydisplay = LedControl(43, 21, 20, 1);

// Below a struct that should ease the controlling of the leds and buttons,
// we should probably move this to another file and keep this main file
// as clean as possible.
typedef struct {
  int x;
  int y;
  Bounce btn;
  boolean ledStatus;
} button;
button upperRow[] = {
  {1, 6, Bounce(38, 20), false},
  {1, 2, Bounce(39, 20), false},
  {1, 3, Bounce(40, 20), false},
  {1, 7, Bounce(41, 20), false},
  {1, 0, Bounce(42, 20), false}
};
button lowerRow[] = {
  {6, 0, Bounce(15, 20), false},
  {6, 7, Bounce(14, 20), false},
  {6, 3, Bounce(13, 20), false},
  {6, 2, Bounce(12, 20), false},
  {6, 6, Bounce(11, 20), false}
};
Bounce btnBankUp = Bounce(43, 10);
Bounce btnBankDown = Bounce(10, 10);

// the setup routine runs once when you press reset:
void setup() {
  // Set the button pins to INPUT_PULLUP, this is a Teensy extension that isn't
  // available on arduino boards. This allows me to directly hook up all the buttons
  // to the Teensy instead of having to add a pull-up resistor for every button in the project.
  //for(int i=10; i<=15; ++i)
  //  pinMode(i, INPUT_PULLUP);
  //for(int i=38; i<=43; ++i)
  //  pinMode(i, INPUT_PULLUP);
  
  // Init midi  
  //Serial.begin(31250);
  //MIDI.begin(MIDI_CHANNEL_OMNI); 
  //MIDI.turnThruOff();
  //Serial.begin(57600);
  //Serial.println("MIDI Input Test");  
  
  // Set all the leds on that are connected to the MAX chip.
  mydisplay.shutdown(0, false);  // turns on display
  mydisplay.setIntensity(0, 2);  // 15 = brightest
  for(int i=0; i<=7; i++) {
      mydisplay.setDigit(0, i, 8, true);
  } 
  
  // Set the pins for the Stompbox bank RGB-led to output
  // pins 27, 0 and 1 are PWM pins, so an AnalogWrite() will set the
  // led intensity for this led.
  pinMode(27, OUTPUT);
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  //analogWrite(0,  80);   // Red    
  //analogWrite(27, 80);   // Green
  //analogWrite(1,  80);   // Blue
  
  // Write something on the LCD
  lcd.begin(20, 2);
  lcd.println(" FCB-Infinity v1.0");
  lcd.println("         Mackatack");
  /*
  delay(800);
  lcd.clear();
  lcd.setCursor(0,0);
  */  
}

// *************************************************************
// Everything below this line is just garbage testing code and
// will be modified dramatically soon
// *************************************************************

// Some debugging milisecond timers
elapsedMillis boardLedTimer;
elapsedMillis LCDTimer;

boolean ledStatus = true;
int i = 0;
int x = 0;
int y = 0;
int m_intBank = 0;

int msgCount = 0;
int ledIndicator = 0;
int ledPin = 0;

int digit1 = 1;
int digit2 = 2;
int digit3 = 3;
boolean noDigitStatus = true;
elapsedMillis digitElapsed;


// the loop routine runs over and over again forever:
void loop() {
  
  if (digitElapsed>100) {
     digitElapsed = 0;
      
     lcd.setCursor(0,0);
     lcd.print("    ");
     lcd.setCursor(0,0);    
     int r = safeAnalogRead(7);
     lcd.print(r);
     lcd.setCursor(0,1);
     lcd.print("            ");
     lcd.setCursor(0,1);
     r = smoothInterpolate(r, 26, 990, 0, 128);
     lcd.print(r);
     analogWrite(1, r);  // Blue
     //lcd.setCursor(0,1);
     //lcd.print(analogRead(pinExp2));   
  }
  
  
  
  /*btnBankUp.update();
  if (btnBankUp.fallingEdge()) {
    goBank(m_intBank+1);
  }
  
  btnBankDown.update();
  if (btnBankDown.fallingEdge()) {
    goBank(m_intBank-1);
  }
  
  for (int i=0; i<5; ++i) {
    upperRow[i].btn.update();
    if (upperRow[i].btn.fallingEdge()) {
      lcd.setCursor(0,0);
      lcd.print(i);
      upperRow[i].ledStatus = !upperRow[i].ledStatus;
      mydisplay.setLed(0, upperRow[i].y, upperRow[i].x, upperRow[i].ledStatus);
    }
    
    lowerRow[i].btn.update();
    if (lowerRow[i].btn.fallingEdge()) {
      lcd.setCursor(0,1);
      lcd.print(i);
      lowerRow[i].ledStatus = !lowerRow[i].ledStatus;
      mydisplay.setLed(0, lowerRow[i].y, lowerRow[i].x, lowerRow[i].ledStatus);
    }
  }*/
  
}

int safeAnalogRead(pin) {
  analogRead(pin);
  delay(15);
  return analogRead(pin);
}

int smoothInterpolate(int value, int valueMin, int valueMax, int targetMin, int targetMax) {
  value -= valueMin;
  value = (float)value * (targetMax-targetMin) / (valueMax-valueMin) + targetMin;
  if (value<targetMin) return targetMin;
  if (value>targetMax) return targetMax;
  return value; 
}


void goBank(int bank) {
  if (bank<0) bank=0;
  if (bank>999) bank=999;
  
  analogWrite(27, 0);
  analogWrite(0, 0);
  analogWrite(1, 0);
  lcd.setCursor(1,1);
  lcd.print(' ');
  lcd.setCursor(1,1);
  switch (bank%3) {
    case 0:
      analogWrite(27, 100);  // R
      lcd.print('R');
      break;
    case 1:
      analogWrite(0, 100);  // B
      lcd.print('B');
      break;
    case 2:
      analogWrite(1, 150);  // G
      lcd.print('G');
      break;
  }
  
  m_intBank = bank;
  lcd.setCursor(20-3,1);
  if (bank<10)
    lcd.print(' ');
  if (bank<100)
    lcd.print(' ');
  lcd.print(m_intBank);  
}
