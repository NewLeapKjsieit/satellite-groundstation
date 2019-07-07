
/*
 when stepper moves for 1.8 deg the gear moves for 1 deg for el and az
*/
#include <AccelStepper.h>
#include <MultiStepper.h>

#define AZdirpin 2    //azimuth direction
#define ELdirpin 4    //elevation direction
#define AZsteppin 3   //azimuth step
#define ELsteppin 5   //elevation step

#define enableAZ_motor_pin 6    //Enable pin
#define enableEL_motor_pin 7    //Enable pin

#define AZhome 8    //AZ homing switch
#define ELhome 9    //EL homing switch

AccelStepper AZstepper = AccelStepper(1, AZsteppin, AZdirpin);
AccelStepper ELstepper = AccelStepper(1, ELsteppin, ELdirpin);

int AZ_current_pos=0, EL_current_pos=0;     //variables to store current values
int AZ_previous_pos=0, EL_previous_pos=0;   //variables to store previous values

String values = "", AZ_value = "", EL_value="";
int AZ_int=0, EL_int=0;
int az_count1=0;

void homing(){
  digitalWrite(AZdirpin, LOW);
  digitalWrite(ELdirpin, LOW);
  while(digitalRead(AZhome) != LOW)
  {
    AZstepper.move(1);
    AZstepper.run();
  }
  while(digitalRead(ELhome) != LOW)
  {
    ELstepper.move(1);
    ELstepper.runSpeed();
  }
  ELstepper.setCurrentPosition(0);
  AZstepper.setCurrentPosition(0);
}

void setup() {
  Serial.begin(9600);

  AZstepper.setMaxSpeed(1000);
  AZstepper.setAcceleration(50.0);

  ELstepper.setMaxSpeed(1000);
  ELstepper.setAcceleration(50.0);

  pinMode(AZdirpin, OUTPUT);
  pinMode(ELdirpin, OUTPUT);
  pinMode(AZsteppin, OUTPUT);
  pinMode(ELsteppin, OUTPUT);

  pinMode(enableAZ_motor_pin, OUTPUT);
  pinMode(enableEL_motor_pin, OUTPUT);

  pinMode(AZhome, INPUT);
  pinMode(ELhome, INPUT);

  homing();

}

void loop() {
  digitalWrite(AZdirpin, HIGH);
  digitalWrite(ELdirpin, HIGH);
  //Code to parse PREDICT values
  if(Serial.available()!=0){
  char val = Serial.peek();
  if (val == 'A' or val == 'Z' or val == 'E' or val == 'L' ){       
    if (val == 'A' or val == 'Z'){
      if(val == 'A'){
        Serial.readStringUntil('Z');
            AZ_value = Serial.readStringUntil(' ');
            AZ_int = AZ_value.toInt();
          }
          else{
            Serial.readStringUntil(' ');
            AZ_value = Serial.readStringUntil(' ');
            AZ_int = AZ_value.toInt();
          }   
        }
        if (val == 'E' or val == 'L'){
          if(val == 'E'){
            Serial.readStringUntil('L');
            EL_value = Serial.readStringUntil(' ');
            EL_int = EL_value.toInt();
          }
          else{
            Serial.readStringUntil(' ');
            EL_value = Serial.readStringUntil(' ');
            EL_int = EL_value.toInt();
          }
      }
    }
  //End of the code for parsing PREDICT values
  
  AZ_current_pos = AZ_int;
  EL_current_pos = EL_int;
  Serial.println(EL_int);
  if(EL_int<=3){/*if(EL_previous_pos == 3){*/homing();}//}
  else{stepper_move(AZ_int, EL_int);
  Serial.println("Here");}
}
}

void stepper_move(int AZ_value, int EL_value)
{
  Serial.println("in stepper move");
  //To move azimuth
  int az_count = AZ_value - AZ_previous_pos;
  az_count1=az_count1+az_count;
  //if(AZ_previous_pos == 0){
    while(AZstepper.currentPosition() != (az_count1)){
      Serial.print("az");
      Serial.println(AZstepper.currentPosition());
      delay(2000);
    AZstepper.move(1);
    AZstepper.run();
    AZstepper.setCurrentPosition(AZstepper.currentPosition());
    AZ_previous_pos = AZ_current_pos;
    }
  //}
  /*else{
    while(AZstepper.currentPosition() != (az_count)){
      Serial.print( "az");
      Serial.println(AZstepper.currentPosition());
      delay(1000);
    AZstepper.move(1); 
    AZstepper.run();
    AZstepper.setCurrentPosition(AZstepper.currentPosition());
    AZ_previous_pos = AZ_current_pos;
    }
  }*/
  //Azimuth moving ended
  
/*  //To move elevation
  
  if(EL_previous_pos < EL_current_pos){
    int el_count = EL_value-EL_previous_pos;
    while(ELstepper.currentPosition() != el_count){
      Serial.print("el");
      Serial.println(ELstepper.currentPosition());
      delay(2000);
    ELstepper.setSpeed(100); // move by difference
    ELstepper.runSpeed();
    ELstepper.setCurrentPosition(ELstepper.currentPosition());
    EL_previous_pos = EL_current_pos;
    }
  }
  else if(EL_previous_pos > EL_current_pos){
    int el_count = EL_previous_pos - EL_value;
    while(ELstepper.currentPosition() != el_count){
      Serial.print("el");
      Serial.println(ELstepper.currentPosition());
      delay(2000);
      ELstepper.setSpeed(100);
      ELstepper.runSpeed();
      ELstepper.setCurrentPosition(ELstepper.currentPosition());
      EL_previous_pos = EL_current_pos;
    } //move to ccw side
  }
  //Elevation moving ended*/
  Serial.println("going out of stepper move");
}
