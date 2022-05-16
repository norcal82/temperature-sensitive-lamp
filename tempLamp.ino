#include <OneWire.h>		 // one wire library for temp sensors - http://www.pjrc.com/teensy/td_libs_OneWire.html && http://bildr.org/2011/07/ds18b20-arduino/
#include <DallasTemperature.h> // dallas temp lib for simplifying temp readings and setting sensor to 9-bits vs 12. http://milesburton.com/Dallas_Temperature_Control_Library

bool DEBUG = true;

//RGB Pin Variables
// red and blue led pins. attiny85 only has two PWM's 
byte redPin = 9,
     bluePin = 10;

// DS18S20 Temperature chip i/o plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2

// setup a oneWire instance to communicate with any onewire devices (not just maxim/dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;


// you need to consider your average ambient temp. this is set to get the full color range throughout a day.
float tmpHigh = 78.00,
      tmpLow  = 62.00;

float tmpMid = (tmpLow + tmpHigh) / 2;

void setup(){

  if (DEBUG) 
     Serial.begin(9600);

  sensors.begin();

  if (DEBUG) {
     if (!sensors.getAddress(insideThermometer, 0)) 
          Serial.println("Unable to find address for Device 0"); 
  }
     
  pinMode(redPin, OUTPUT);
  pinMode(bluePin, OUTPUT);  

}

void loop() {

  sensors.requestTemperatures();
  
  if (DEBUG)
     Serial.println(printTemperature(insideThermometer));
  
  setTmpColor(printTemperature(insideThermometer));

  delay(1000);
}

void setColor(byte red, byte blue){
  analogWrite(redPin, red);
  analogWrite(bluePin, blue);
}

float printTemperature(DeviceAddress deviceAddress) {
  return DallasTemperature::toFahrenheit(sensors.getTempC(deviceAddress)); // Converts tempC to Fahrenheit
} 

void setTmpColor(float tmp) {
     
  if(tmp < tmpLow)  // set full blue
    setColor(0.00, 255.00);

  if(tmp > tmpLow && tmp < tmpMid) {
    
    float data = map(tmp, tmpLow, tmpMid, 0.00, 255.00); // set blue amount
    
    setColor(data, 255.00);
  }

  if(tmp > tmpMid && tmp < tmpHigh){

    float data = map(tmp, tmpMid, tmpHigh, 0.00, 255.00); // set red amount
    
    setColor(255.00, (255.00 - data));
  }

  if(tmp > tmpHigh)
    setColor(255.00, 0.00); //set full red
}

//map is not supported on the tiny. copied from http://arduino.cc/en/reference/map
float map(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
