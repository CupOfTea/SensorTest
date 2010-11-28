//ultraSonic.h
// ***********************************************************************
// *********************** Ultra Sonic Rangers ***************************
// ***********************************************************************
class sonar {
public: 
  int pingPin;
  int smoothFactor;
  
  sonar(void) { 
  }

  virtual void initialize(int); 
  virtual const int measure(void);

  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled. (duration/58)
};

// ***********************************************************************
// ************************ Devantech SRF05 *****************************
// ***********************************************************************
// Assumes device is connected in single pin mode:  ie requires 1 digital pin
class sonar_SRF05 : public sonar { 
private:
  long duration;
  int distance;
  int cm;
public: 
  sonar_SRF05() : sonar(){
  }
  void initialize(int pin) {
      pingPin = pin;
      smoothFactor=0.5;
  }
  const int measure (void) { 
     // The PING))) is triggered by a HIGH pulse of 10 or more microseconds.
     // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
     pinMode(pingPin, OUTPUT);
     digitalWrite(pingPin, LOW);
     delayMicroseconds(2);                // timing reqd by device
     digitalWrite(pingPin, HIGH);          // trigger pulse
     delayMicroseconds(10);                // timing reqd by device
     digitalWrite(pingPin, LOW);
     pinMode(pingPin, INPUT);              // pin to input and count till pong
     duration = pulseIn(pingPin, HIGH);
     cm = duration / 58;
     // distance = distance * (1.0 - smoothFactor) + (cm * smoothFactor); // same as AQ smooth()
     return cm; 
  } 
};
