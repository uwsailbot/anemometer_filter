#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

#include <Servo.h> 
#include <ros.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Int32.h>
#include <math.h>

#define WindVanePin (A4)       // The pin the wind vane sensor is connected to

// Constants for anemometer filter
const float expWeightingFactor = 0.07;
const int medFactor = 50;
const int timeInterval = 100;

ros::NodeHandle  nh;
std_msgs::Int32 winddir;

Servo servo_rudder;
Servo servo_winch;
int VaneValue;       // raw analog value from wind vane
int CalDirection;    
int FilteredDirection;
int LastValue;
int count=0;

void servo_cb( const std_msgs::Float32& cmd_msg){
    servo_rudder.write(cmd_msg.data); 
}

void winch_cb( const std_msgs::Int32& pos_msg){
    int position_msg = map(pos_msg.data, 0, 2160, 1000, 2000);
    servo_winch.writeMicroseconds(position_msg);
}

ros::Subscriber<std_msgs::Float32> sub_rudder("rudder", servo_cb);
ros::Subscriber<std_msgs::Int32> sub_winch("winch",winch_cb);
ros::Publisher anemometer("anemometer", &winddir);

//Prototypes for anemometer filter
int exponentialFilter(int reading);
int medianFilter(int reading);

void setup(){
    nh.initNode();
    nh.subscribe(sub_rudder);
    nh.subscribe(sub_winch);
    nh.advertise(anemometer);
  
    servo_rudder.attach(3); //attach it to pin 3
    servo_winch.attach(4);
    LastValue = 0;

    pinMode(WindVanePin, INPUT);
}

unsigned long previousMillis = 0;
void loop(){

    if ((millis() - previousMillis) > timeInterval){
    VaneValue = analogRead(WindVanePin);
        CalDirection = map(VaneValue, 0, 1023, 0, 360);
        CalDirection = ((CalDirection % 360) + 360) % 360;
    
        FilteredDirection = medianFilter(ExponentialFilter(CalDirection));
    
          // Only update the display if change greater than 5 degrees. 
        if(abs(FilteredDirection - LastValue) > 5)
        { 
             LastValue = FilteredDirection;
             winddir.data = FilteredDirection;
             anemometer.publish(&winddir);
           
        }
        previousMillis = millis();
    }
  
    nh.spinOnce();
}


int expValue = 0;
bool expFirstValueFlag = true;
int exponentialFilter(int reading){
   //first value
   if (expFirstValueFlag == true){
      expValue = reading;
      expFirstValueFlag = false;
   }else{
      //deal with vaue wrapping
      if (reading < 90 && expValue > 270){
         expValue -= 360;
      }else if (reading > 270 && expValue < 90){
         expValue += 360;
      }
   expValue = expWeightingFactor*reading + (1 - expWeightingFactor)*expValue;
   }
    
   return (expValue + 360) % 360;
}

int medValue[medFactor+1];
int medValueSorted[medFactor+1];
bool medFirstValueFlag = true;
int medianFilter(int reading){
   if (medFirstValueFlag){
      for (int i = 0; i < medFactor; i++){
         medValue[i] = reading;
         medValueSorted[i] = reading;
      }
        
      medFirstValueFlag = false;
   }

   // wrap data
   if (reading < 90 && medValue[0] > 270){
      for (int i = 0; i < medFactor; i++){
         medValue[i] = medValue[i] - 360;
         medValueSorted[i] = medValueSorted[i] - 360;
      }
   }else if (reading > 270 && medValue[0] < 90){
      for (int i = 0; i < medFactor; i++){
         medValue[i] = medValue[i] + 360;
         medValueSorted[i] = medValueSorted[i] + 360;
      }
   }

   //remove last value
   for (int i = 0; i < medFactor-1; i++){
      if (medValueSorted[i] >= medValue[medFactor-1]){
         medValueSorted[i] = medValueSorted[i+1];
      }
   }

   //insert new value
   int i = medFactor-1;
   while (i > 0 && medValueSorted[i-1] > reading){
      medValueSorted[i] = medValueSorted[i-1];
      i--;
   }
   medValueSorted[i] = reading;

   //shift chronological values
   for (int i = medFactor-1; i > 0; i--){
      medValue[i] = medValue[i-1];
   }
   medValue[0] = reading;
    
   return (medValueSorted[int(medFactor/2)] +360) % 360;
}
