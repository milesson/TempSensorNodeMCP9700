/**
 * == Temperature MCP1700 ==
 * This analog sensor is powered from a digital pin, so that is possible to minimize 
 * power consumption during sleep. Be sure to configure the power pin as as an input 
 * during sleep to avoid current leakage. 
 * 
 * Turn-on Time: 800 µs
 * Output Voltage, TA = 0°C V0°C — 500 — mV
 * Temperature Coefficient TC = 10.0 — mV/°C 
 *
 * D2 - Temp Vcc, to power the sensor
 * A0 - Temp sensor output
 *
 * == Moisture ==
 * Soil moisture is measured by applying a voltage over a know resistor and trough 
 * the soil itself (the unknown resistor). By measuring the reference voltage between 
 * these two you can calculate the unknown resistor which represents the mositure in 
 * the soil. Two different resistor can be used to gain accuracy in moist or dry soil 
 * which provide quick a large range of resistance. Recommended  values R1 30k, R2 150k.
 * 
 * D3 - Vcc for R1 
 * D4 - Vcc for R2 
 * A1 - Moisture sensor reference out
 */

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <Arduino.h>
#include "Sensor_config.h"
#include "Sleepy.h"
#include "LowPass.h"
#include <avr/power.h>

//Pin defines 
#define TEMP_VCC 2
#define TEMP_OUT A0
#define MOIST_VCC1 3
#define MOIST_VCC2 4
#define MOIST_REF_OUT A1

// nRF24L01(+) radio attached using Getting Started board 
RF24 radio(9,10);

// Network uses that radio
RF24Network network(radio);

// Sleep mode helper
Sleepy sleep;

// Inital address for this node
unsigned int node_id = SENSOR_NODE_SLAVE;

// Low pass filtered measurment points
LowPass lp_temp;
LowPass lp_moisture1(0.2);
LowPass lp_moisture2(0.2);
LowPass lp_battery;

void setup(void)
{

  Serial.begin(57600);
  
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ node_id);
  
  //Todo: assign a dynamic node address and write to eeprom
 
  // Setup sensor pins
  pinMode(TEMP_VCC, OUTPUT);
  pinMode(TEMP_OUT, INPUT);
  pinMode(MOIST_VCC1, OUTPUT);
  pinMode(MOIST_VCC2, OUTPUT);
  pinMode(MOIST_REF_OUT, INPUT);
  
  //analogReference(2); //INTERNAL1V1
  analogReference(3); //INTERNAL2V56
  
  Serial.println("Setup done!");
  
}

void loop(void)
{
  
  // Pump the network regularly
  network.update();
  
  /*  Read and set sensor values */
  
  bool valueChanged = false; // To keep track if we have to send an update to the gatway
  valueChanged |= lp_moisture1.SetValue(MeasureMoisture1());
  Serial.print(valueChanged);
  
  valueChanged |= lp_moisture2.SetValue(MeasureMoisture2());
  Serial.print(valueChanged);
  
  valueChanged |= lp_temp.SetValue(MeasureTemp()); 
  Serial.print(valueChanged);
  
  valueChanged |= lp_battery.SetValue(MeasureBattery());
  Serial.println(valueChanged);
  
  //Print data
  Serial.print("moisture1: ");
  Serial.print(lp_moisture1.GetValue());
  Serial.print(" moisture2: ");
  Serial.println(lp_moisture2.GetValue());
  Serial.print(" temp: ");
  Serial.print(lp_temp.GetValue());
  
  
  if(valueChanged) {
    package_t package = { node_id /*node id*/, lp_temp.GetValue(), lp_moisture1.GetValue(), lp_moisture2.GetValue(), lp_battery.GetValue()};
    RF24NetworkHeader header(/*to node*/ SENSOR_NODE_MASTER);
    bool ok = network.write(header,&package,sizeof(package));
    //Todo, try resending a few time and the give up.
    
    Serial.println("Sent!");
  }
  
  Serial.flush();
 
  PowerSave();
  radio.powerDown();
  sleep.SleepSeconds(30);
  radio.powerUp();
  PowerUp();
}



void PowerSave() 
{

    //Disable ADC to save power
    ADCSRA &= ~(1<<ADEN);
    
    // Set outputs low
    digitalWrite(TEMP_VCC, LOW);
    digitalWrite(MOIST_VCC1, LOW);
    digitalWrite(MOIST_VCC2, LOW);

    //Configure all pins at inputs 
    pinMode(TEMP_VCC, INPUT);
    pinMode(TEMP_OUT, INPUT);
    pinMode(MOIST_VCC1, INPUT);
    pinMode(MOIST_VCC2, INPUT);
    pinMode(MOIST_REF_OUT, INPUT);
}

void PowerUp() 
{
      
    // Restore sensor pins setup
    pinMode(TEMP_VCC, OUTPUT);
    pinMode(MOIST_VCC1, OUTPUT);
    pinMode(MOIST_VCC2, OUTPUT);
    
    //Eanble ADC to save power
    ADCSRA &= ~(1<<ADEN);
}


// How many measurements to take.  64*1024 = 65536, so 64 is the max we can fit in a uint16_t.
const int num_measurements = 64;

int MeasureTemp()
{
    
    digitalWrite(TEMP_VCC, HIGH); // Temp sensor on
    
    int i = num_measurements;
    uint32_t reading = 0;
    while(i--)
      reading += analogRead(TEMP_OUT);
      
      
    digitalWrite(TEMP_VCC, LOW); // Temp sensor off
    
    // Convert the reading to celcius*256
    // This is the formula for MCP9700.
    // C = reading * 1.1
    // C = ( V - 1/2 ) * 100
    //
    // Then adjust for the calibation value on this sensor
    return ( ( ( ( reading * 0x120 ) - 0x800000 ) * 0x64 ) >> 16 ); // + this_node.temp_calibration;
}

int MeasureMoisture1()
{
  // Todo
  return -1;
}

int MeasureMoisture2()
{
  // Todo
  return -1;
}

int MeasureBattery()
{
  // Todo
  return -1;
}

