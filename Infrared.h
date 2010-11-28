//infrared.h
// ***********************************************************************
// ************************* Infrared Rangers ****************************
// ***********************************************************************
#define debug_IR;     //define to dump some data
class IR {
public: 
  int pingPin;
  float aref;
  float voltsPerUnit;
  IR(void) { 
  }
  virtual void initialize(int , float); 
  virtual const int getRaw(void);
};
// ***********************************************************************
// ************************** Sharp Analog *******************************
// ***********************************************************************
// requires 1 analog pin
class IR_SharpAnalog : 
public IR { 
public: 
  IR_SharpAnalog() : 
  IR(){
  }
  virtual const int measure(void);
  void initialize(int pin , float initaref) {
    aref = initaref;
    pingPin = pin;
    voltsPerUnit = aref / 1024;                   // for 10bit ADC
#ifdef debug_IR
    Serial.print("! Init IR  Arduino pin=");
    Serial.print(pingPin);
    Serial.print("  ARef=");
    Serial.print(aref);
    Serial.print("  VperUnit~=.00");
    Serial.print(voltsPerUnit);
#endif
  }
  const int getRaw(void) {
    int rawData;
    rawData = analogRead(pingPin);
    return rawData; 
  } 
};

// ***********************************************************************
// ************************* Sharp GP2Y0A02 ******************************
// ***********************************************************************
class IR_GP2Y0A02 : 
public IR_SharpAnalog { 
private:
  int rawData;
  float volts;
  int cm;
  float fcm;
public: 
  IR_GP2Y0A02() : 
  IR_SharpAnalog(){
  }
  const int measure(void) {
    rawData=getRaw();
#ifdef debug_IR
    Serial.print("\t"); 
    Serial.print(rawData);
#endif
    // equation simulates the degrading exponential - uses float pow()
    // http://luckylarry.co.uk/arduino-projects/arduino-using-a-sharp-ir-sensor-for-distance-calculation/
    // these constants seem to work best with a 5V aref
    volts = rawData * voltsPerUnit;
    cm = 65 * pow(volts, -1.10);                // constants recommended by larry
#ifdef debug_IR
    Serial.print("\t"); 
    Serial.print((int)(volts*100));
    Serial.print("\t"); 
    Serial.print(cm);
#endif

    cm = 60.495 * pow(volts, -1.1904);          // constants recommended by Honks source
#ifdef debug_IR
    Serial.print("\t"); 
    Serial.print(cm);
#endif

    // piecewise linear approximation - int only
    // http://forum.pololu.com/viewtopic.php?f=6&t=2235&start=12
#ifdef debug_IR
    Serial.print("\t"); 
    Serial.print(read_sharpIR(volts * 1000));
#endif

    // equation uses the fact that the inverse of the readings is linear - there are float and int only versions
    // http://www.acroname.com/robotics/info/articles/irlinear/irlinear.html - to calc constants there is a 
    // spreadsheet at http://forums.parallax.com/attachment.php?s=c7fa36a97cbba2633c0f4a030278ff6d&attachmentid=721&d=128171517
    // aref is part of the calculation if you are using an EXTERNAL aref you may need to generate your own
    // it could be a good idea to do your own anyway my cm's into the spreadsheet could be a different length than yours
    // *** int only version
    if (aref == 2.56) {cm = (21405/(rawData-3))-2; }
    if (aref == 2.8)  {cm = (19570/(rawData-3))-2; }
    if (aref == 3)    {cm = (18266/(rawData-3))-2; }
    if (aref == 3.27) {cm = (16758/(rawData-3))-2; }
    if (aref == 5)    {cm = (10959/(rawData-2))-2; }
#ifdef debug_IR
    Serial.print("\t"); 
    Serial.print(cm);
#endif
    
   // *** float version 
    if (aref == 2.56) {fcm = (1 / (rawData * 4.67E-05 - 0.00015))-2; }
    if (aref == 2.8)  {fcm = (1 / (rawData * 5.11E-05 - 0.00015))-2; }
    if (aref == 3)    {fcm = (1 / (rawData * 5.47E-05 - 0.00015))-2; }
    if (aref == 3.27) {fcm = (1 / (rawData * 5.97E-05 - 0.00015))-2; }
    if (aref == 5)    {fcm = (1 / (rawData * 9.12E-05 - 0.00015))-2; }
#ifdef debug_IR
    Serial.print("\t"); 
    Serial.print(fcm);
#endif
    return cm; 
  }

  int autoscale( int originalMin, int originalMax, int newBegin, int newEnd, int inputValue){
    long zeroRefOriginalMax = 0;
    long zeroRefnewEnd = 0;
    long zeroRefCurVal = 0;
    long rangedValue = 0;
    boolean invFlag = 0;
    // Check for out of range inputValues
    if (inputValue < originalMin) {
      inputValue = originalMin;
    }
    if (inputValue > originalMax) {
      inputValue = originalMax;
    }
    // Zero Refference the values
    zeroRefOriginalMax = originalMax - originalMin;
    if (newEnd > newBegin){ 
      zeroRefnewEnd = newEnd - newBegin;
    }
    else {
      zeroRefnewEnd = newBegin - newEnd; 
      invFlag = 1;
    }
    zeroRefCurVal = inputValue - originalMin;
    // Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine 
    if (originalMin > originalMax ) {
      return 0;
    }
    if (invFlag == 0){
      rangedValue =  ((zeroRefCurVal * zeroRefnewEnd) /
        zeroRefOriginalMax) + newBegin ; 
    }
    else     // invert the ranges
    {   
      rangedValue =  newBegin - ((zeroRefCurVal * zeroRefnewEnd) /
        zeroRefOriginalMax)  ; 
    }
    return rangedValue;
  } 

  int read_sharpIR(int IR_dist_measure_mv)
  {
    int corr_cm[16]=    {
      20 ,30 ,40 ,50 ,60 ,70 ,80 ,90 ,100,110,120,130,140,150,160,170                      };
    int corr_mv[16] = {
      2500,2000,1550,1250,1050,900,800,715,650,600,550,500,475,450,425,400                      };
    int distance_calculated_in_cm;
    unsigned char i;
    if (IR_dist_measure_mv >= corr_mv[0])
    {
      distance_calculated_in_cm = corr_cm[0];
      IR_dist_measure_mv = corr_mv[0];
    }
    else if (IR_dist_measure_mv <= corr_mv[15])
    {
      distance_calculated_in_cm = corr_cm[15];
      IR_dist_measure_mv = corr_mv[15];
    }
    else
    {
      for (i = 1; i < 16; i++)
      {
        if (IR_dist_measure_mv >= corr_mv[i])
        {
          distance_calculated_in_cm = autoscale(corr_mv[i], corr_mv[i-1],corr_cm[i],corr_cm[i-1], IR_dist_measure_mv);
          break;
        }
      }
    }

    return distance_calculated_in_cm;
  }
};

// ***********************************************************************
// ************************** Sharp GP2D120 ******************************
// ***********************************************************************
class IR_GP2D120 : 
public IR_SharpAnalog { 
private:
  float volts;
  int rawData;
  long cm;
public: 
  IR_GP2D120() : 
  IR_SharpAnalog(){
  }
  const int measure(void) {
    rawData=getRaw();

    cm = (2914 / (rawData + 5)) - 1;   // recomended by Acroname for GP2D120
    Serial.print("\t"); 
    Serial.print(cm);

    volts = rawData * voltsPerUnit;
    cm = 12.21*pow(volts, -1.15);      // recommended by larry's mate for GP2D120
    Serial.print("\t"); 
    Serial.print(cm); 
    return cm; 
  } 
};





