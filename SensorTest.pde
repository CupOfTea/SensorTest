//obstacle.pde
#include "WProgram.h"         //don't know why I need this?
#include "Infrared.h"
#include "UltraSonic.h"

float aref;                      

IR_GP2Y0A02 frontSense;
sonar_SRF05 downSense;

void setup() {

    analogReference(DEFAULT);       // Use default 5v AREF
    aref = 5;                       // Use default 5v AREF
    //analogReference(INTERNAL2V56);    // 2.56v AREF built into the mega 
    //aref = 2.56;                      // 2.56v AREF built into the mega

    // analogReference(EXTERNAL);     // Use external voltage reference connected to AREF pin  *read* http://arduino.cc/en/Reference/AnalogReference?from=Reference.AREF
    // aref = 2.8;                    // Using the arduino 3.3v supply with 5k resister so aref can be selected in code
                                      // the aref 32k internal resister forms voltage divider = 3.3 * 32 / (32 + 4.7) = 2.8773842v
                                      // Got 2.8 on DMM though
    // aref= 3.27                     // Honks' Micrel MIC5207 3V3 regulator using an internal bandgap reference for stabilizing the voltage.                                           


    Serial.begin(57600);
  
  Serial.println("*Sonar\tRaw\tVolts\tLarry\tHonk\tTable\tAcronam\tAfloat");
 
  downSense.initialize(36);
  frontSense.initialize(7,aref);
}

void loop() {
  delay(100);
  int front;
  int down;
  down=downSense.measure();
  Serial.print(down);
  front=frontSense.measure();
  Serial.println("");
}


