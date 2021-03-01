/*
counter with threshold
original sw by Nicu FLORICA (niq_ro)
*/

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
  
volatile int provimp; // measures sensor pulses
unsigned int totalimp = 0;  // total impulses;
unsigned int totalimp0 = 10;  // previous impulses;
int prag = 50;  // default threshold

#define senzor 2 // Sensor Input (D2)
#define sw1 12  // pin for increase the threshold 
#define sw2 11 // pin for decrease the threshold 
#define releu 13 // pin for relay

#define atras LOW
#define repaos HIGH


void numarare() // Interrupt function
   {
   provimp++;
   }

byte starebuton1,starebuton2;             // the current reading from the input pin
byte citirestarebuton1, citirestarebuton2;   // 
byte ultimastarebuton1 = HIGH;   // the previous reading from the input pin
byte ultimastarebuton2 = HIGH;
unsigned long ultimtpdebounce1 = 0;  // the last time the output pin was toggled
unsigned long ultimtpdebounce2 = 0;
unsigned long tpdebounce = 50;    // the debounce time; increase if the output flickers
unsigned long tpactionare = 0;  // momment at activate relay
unsigned long tpcomanda = 1000;  // activate relay time in ms

void setup()
{
  lcd.begin(); // initialize the LCD
  lcd.clear(); // clear the screen
  lcd.backlight(); // Turn on the blacklight and print a message.
  lcd.setCursor(0,0);
  lcd.print(" Sistem automat ");
  lcd.setCursor(0,1);
  lcd.print("sw: Nicu FLORICA");
  
  pinMode(senzor, INPUT);
  digitalWrite(senzor, HIGH); // Optional Internal Pull-Up
  pinMode(sw1, INPUT);
  pinMode(sw2, INPUT);
  digitalWrite(sw1, HIGH);
  digitalWrite(sw2, HIGH);  
  pinMode(releu, OUTPUT);
  digitalWrite(releu, repaos);

   delay(3000);
   lcd.clear();
   Serial.begin(9600);
   Serial.println(" ");Serial.println(" ");Serial.println(" ");
   Serial.println("------------------------------------------");    
   Serial.println("counter with threshold");
   attachInterrupt(0, numarare, RISING); // Setup Interrupt
   sei(); // Enable interrupts
}

void loop ()
{

// button reading
citirestarebuton1 = digitalRead(sw1);  // read the state of the switch into a local variable: 
  if (citirestarebuton1 != ultimastarebuton1) // If the switch changed, due to noise or pressing:
  {
    ultimtpdebounce1 = millis();  // reset the debouncing timer
  }
  if ((millis() - ultimtpdebounce1) > tpdebounce) 
  {
    if (citirestarebuton1 != starebuton1) // if the button state has changed
    {
      starebuton1 = citirestarebuton1;         
      if (starebuton1 == LOW) // only toggle the LED if the new button state is LOW
      {
        Serial.println("sw1");
        prag = prag + 5;
        if (prag > 100) prag = 5;
        totalimp = 0;  
      }
    }
  }

citirestarebuton2 = digitalRead(sw2);  // read the state of the switch into a local variable: 
  if (citirestarebuton2 != ultimastarebuton2) // If the switch changed, due to noise or pressing:
  {
    ultimtpdebounce2 = millis();  // reset the debouncing timer
  }
  if ((millis() - ultimtpdebounce2) > tpdebounce) 
  {
    if (citirestarebuton2 != starebuton2) // if the button state has changed
    {
      starebuton2 = citirestarebuton2;         
      if (starebuton2 == LOW) // only toggle the LED if the new button state is LOW
      {
      Serial.println("sw2");
      prag = prag - 5; 
      if (prag < 5) prag = 100;
      totalimp = 0;
      }
    }
  }

// count part
      totalimp = totalimp + provimp;  // count all impulses
      provimp = 0; // Reset Counter
      if (totalimp != totalimp0)
      {
      Serial.print(totalimp, DEC); // Print litres/hour
      Serial.println(" pulses -> ");
      Serial.println("------------------------------------------");   
      }
      lcd.setCursor(0,0);
      //lcd.print("n: "); 
      if (totalimp < 1000) lcd.print(" ");
      if (totalimp < 100) lcd.print(" ");
      if (totalimp < 10) lcd.print(" ");
      lcd.print(totalimp,DEC); // Print total pulses  
      lcd.print(" / ");
      lcd.print(prag); // Print threshold
      lcd.print("   ");
totalimp0 = totalimp;

// decision part
if (totalimp >= prag)
{
  totalimp = 0;
  digitalWrite(releu, atras);
  lcd.setCursor(15,0);
  lcd.print("*");
  tpactionare = millis();
}
if (millis() - tpactionare > tpcomanda)
{
  digitalWrite(releu, repaos);
  lcd.setCursor(15,0);
  lcd.print(" ");
}

ultimastarebuton1 = citirestarebuton1;
ultimastarebuton2 = citirestarebuton2;             

}  // end main loop
