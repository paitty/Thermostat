
// which analog pin to connect
#define THERMISTORPIN A7
// resistance at 25 degrees C
#define THERMISTORNOMINAL 10000
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 5
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950
// the value of the 'other' resistor
#define SERIESRESISTOR 7500 

#include <Time.h>

const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
float average = 0;                // the average

float steinhart;
float oldsteinhart=23;

int state=0;
int mode=0;
int switchPin = 7;
int buttonPin = 6;
int buttonState = 0;
int time=0;

void setup()   /****** SETUP: RUNS ONCE ******/
{
  Serial.begin(9600);
    pinMode(buttonPin, INPUT);  
  setTime(20, 16, 0, 0, 0, 0);
  
  pinMode(switchPin, OUTPUT);
  digitalWrite(switchPin, HIGH);
  
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0;
}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{

  if ((buttonState == HIGH) && (digitalRead(buttonPin) == LOW)) {     
    // turn LED on:    
    mode=1;  
  }
  
  buttonState = digitalRead(buttonPin);
  
  total= total - readings[index];         
  // read from the sensor:  
  readings[index] = analogRead(THERMISTORPIN); 
  // add the reading to the total:
  total= total + readings[index];       
  // advance to the next position in the array:  
  index = index + 1;                    

  // if we're at the end of the array...
  if (index >= numReadings)              
    // ...wrap around to the beginning: 
    index = 0;                           

  // calculate the average:
  average = total / numReadings;         
   Serial.println(average);

 
  // convert the value to resistance
  average = (1023 / average)  - 1;
  average = SERIESRESISTOR * average;
//  Serial.print("Thermistor resistance "); 
//  Serial.println(reading);
 

  steinhart = average / THERMISTORNOMINAL; // (R/Ro)
  steinhart = log(steinhart); // ln(R/Ro)
  steinhart /= BCOEFFICIENT; // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart; // Invert
  steinhart -= 273.15; // convert to C
  
 Serial.println(steinhart);
 
 if (mode==1){
       state=1-state;
       mode=0;
 }
 Serial.println(hour());
 if ((hour()>=17 && hour()<22) || (hour()>=6 && hour()<9)){
   time=1;
   if ((steinhart<19))
       state=1;
   if ((steinhart>20))
       state=0;
  } else {
    if (time==1)
    {
    state=0;
    time=0;
    }
  }
  
  if (state==1){
    Serial.println("on");
    digitalWrite(switchPin, LOW);
  } else {
    Serial.println("off");
    digitalWrite(switchPin, HIGH);
  }
  oldsteinhart=steinhart;
  
  delay(50);
}
