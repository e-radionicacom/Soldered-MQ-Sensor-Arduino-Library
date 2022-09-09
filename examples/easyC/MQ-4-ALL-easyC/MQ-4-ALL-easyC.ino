/*
  MQUnifiedsensor Library - reading an MQ4

  Demonstrates the use a MQ4 sensor with easyC.
  Library originally added 01 may 2019
  by Miguel A Califa, Yersson Carrillo, Ghiordy Contreras, Mario Rodriguez

  Added example
  modified 23 May 2019
  by Miguel Califa

  Updated library usage
  modified 26 March 2020
  by Miguel Califa

  Example is modified by Soldered.com

 This example code is in the public domain.

*/

// Include the library
#include "MQ-Sensor-SOLDERED.h"

#define RatioMQ4CleanAir (4.4) // RS / R0 = 60 ppm

MQ4 mq4; // If this sensor is easyC, you don't need to specify on which
         // pin is connected

void setup()
{
    // Init the serial port communication at 115200 bauds. It's used to print out measured data.
    Serial.begin(115200);

    // Set math model to calculate the PPM concentration and the value of constants
    mq4.setRegressionMethod(1); //_PPM =  a*ratio^b


    /*****************************  MQ Init ********************************************/
    // Remarks: Initializes MQ easyC library.
    /************************************************************************************/
    mq4.begin(0x30); // Here you need to specify I2C address of sensor.
                     // By default it is set to 0x30, but you can change
                     // Using onboard switches labeled with ADDR
    /*
      //If the RL value is different from 10K please assign your RL value with the following method:
      mq4.setRL(10);
    */
    /*****************************  MQ Calibration ********************************************/
    // Explanation:
    // In this routine the sensor will measure the resistance of the sensor supposing before was pre-heated
    // and now is on clean air (Calibration conditions), and it will setup R0 value.
    // We recomend execute this routine only on setup or on the laboratory and save on the eeprom of your arduino
    // This routine not need to execute to every restart, you can load your R0 if you know the value
    // Acknowledgements: https://jayconsystems.com/blog/understanding-a-gas-sensor
    Serial.print("Calibrating please wait.");
    float calcR0 = 0;
    for (int i = 1; i <= 10; i++)
    {
        mq4.update(); // Update data, the arduino will be read the voltage on the analog pin
        calcR0 += mq4.calibrate(RatioMQ4CleanAir);
        Serial.print(".");
    }
    mq4.setR0(calcR0 / 10);
    Serial.println("  done!.");

    if (isinf(calcR0))
    {
        Serial.println("Warning: Conection issue founded! Check easyC cable, connector and I2C address!");
        while (1)
            ;
    }
    if (calcR0 == 0)
    {
        Serial.println("Warning: Conection issue founded! Check easyC cable, connector and I2C address!");
        while (1)
            ;
    }
    /*****************************  MQ Calibration ********************************************/

    Serial.println("*************** Lectures from MQ-4 **********************");
    Serial.println("|    LPG   |  CH4 |   CO    |    Alcohol    |   Smoke    |");
}

void loop()
{
    mq4.update(); // Update data, the arduino will be read the voltage on the analog pin

    /*
      Exponential regression:
    Gas    | a      | b
    LPG    | 3811.9 | -3.113
    CH4    | 1012.7 | -2.786
    CO     | 200000000000000 | -19.05
    Alcohol| 60000000000 | -14.01
    smoke  | 30000000 | -8.308
    */
    mq4.setA(3811.9);
    mq4.setB(-3.113);             // Configurate the ecuation values to get CH4 concentration
    float LPG = mq4.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before
                                  // or in the setup

    mq4.setA(1012.7);
    mq4.setB(-2.786);             // Configurate the ecuation values to get CH4 concentration
    float CH4 = mq4.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before
                                  // or in the setup

    mq4.setA(200000000000000);
    mq4.setB(-19.05);            // Configurate the ecuation values to get CH4 concentration
    float CO = mq4.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before
                                 // or in the setup

    mq4.setA(60000000000);
    mq4.setB(-14.01);                 // Configurate the ecuation values to get CH4 concentration
    float Alcohol = mq4.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted
                                      // before or in the setup

    mq4.setA(30000000);
    mq4.setB(-8.308);               // Configurate the ecuation values to get CH4 concentration
    float Smoke = mq4.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted
                                    // before or in the setup

    Serial.print("|    ");
    Serial.print(LPG);
    Serial.print("    |    ");
    Serial.print(CH4);
    Serial.print("    |    ");
    Serial.print(CO);
    Serial.print("    |    ");
    Serial.print(Alcohol);
    Serial.print("    |    ");
    Serial.print(Smoke);
    Serial.println("    |");

    delay(500); // Sampling frequency
}
