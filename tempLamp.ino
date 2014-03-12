#include <OneWire.h>		       // one wire library for temp sensors - http://www.pjrc.com/teensy/td_libs_OneWire.html && http://bildr.org/2011/07/ds18b20-arduino/
#include <DallasTemperature.h> // dallas temp lib for simplifying temp readings and setting sensor to 9-bits vs 12. http://milesburton.com/Dallas_Temperature_Control_Library


//RGB Pin Variables
//only need red and blue. attiny85 only has two PWM's 
byte redPin = 9,
     bluePin = 10;

/* DS18S20 Temperature chip i/o */

// data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

// setup a oneWire instance to communicate with any onewire devices (not just maxim/dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

DeviceAddress insideThermometer;


// you need to consider your average ambient temp.
// this is set to get the full color range throughout a day.
float tmpHigh = 78.00,
      tmpMid  = 70.00,
      tmpLow  = 62.00;

void setup(){

  //Serial.begin(9600);

  sensors.begin();

  //if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0"); 
  
  // rgb strip schematic w/ transistor array
  // http://siliconrepublic.blogspot.com/2011/02/arduino-based-pc-ambient-lighting.html
  // intialize the rgb pins	
  pinMode(redPin, OUTPUT);
  pinMode(bluePin, OUTPUT);  

}

void loop(){

  sensors.requestTemperatures();
  
  //Serial.println(printTemperature(insideThermometer));
  
  setTmpColor(printTemperature(insideThermometer)); // function to print out the data;

  delay(1000);

}


// set the color of the rgb led only R & B because we dont need green

void setColor(byte red, byte blue){

  analogWrite(redPin, red);
  analogWrite(bluePin, blue);  

}

// function to print the temperature for a device
float printTemperature(DeviceAddress deviceAddress){
  
  // method 2 - faster
  //float tempC = sensors.getTempC(deviceAddress);
  //return DallasTemperature::toFahrenheit(tempC); // Converts tempC to Fahrenheit

  return DallasTemperature::toFahrenheit(sensors.getTempC(deviceAddress)); // Converts tempC to Fahrenheit

} 

void setTmpColor(float tmp){

  if(tmp < tmpLow)  //less than 62 set blue
    setColor(0.00, 255.00);

  if(tmp > tmpLow && tmp < tmpMid){ //lower bound. set amount of red
    
    float data = map(tmp, tmpLow, tmpMid, 0.00, 255.00);
    
    setColor(data, 255.00);
  }

  if(tmp > tmpMid && tmp < tmpHigh){ //upper bound. set amount of blue

    float data = map(tmp, tmpMid, tmpHigh, 0.00, 255.00);
    
    setColor(255.00, (255.00 - data)); //we subtract 255 because we need the opposite for the upper
  }

  if(tmp > tmpHigh)
    setColor(255.00, 0.00); //hotter than 78 stay red

}


//just in case map is not supported on the tiny
//http://arduino.cc/en/reference/map
float map(float x, float in_min, float in_max, float out_min, float out_max) //gotta save that space. analog reading calls 1023..:(
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}