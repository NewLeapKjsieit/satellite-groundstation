
/*
 when stepper moves for 1.8 deg the gear moves for 1 deg for el and az

*/
#include <AccelStepper.h>
#include <MultiStepper.h>

#define azDirpin 2 //azimuth direction
#define elDirpin 4 //elevation direction
#define azSteppin 3 //azimuth step
#define elSteppin 5 //elevation step

#define enableAZ_motor_pin 6   //Enable pin
#define enableEL_motor_pin 7   //Enable pin

#define homeAz 8  //AZ homing switch
#define homeEl 9  //EL homing switch

//AccelStepper AZstepper(1, azSteppin, azDirpin);
//AccelStepper ELstepper(1, elSteppin, elDirpin);
AccelStepper AZstepper = AccelStepper(1, azSteppin, azDirpin);
AccelStepper ELstepper = AccelStepper(1, elSteppin, elDirpin);

//variables to store current values
int current_az_pos=0, current_el_pos=0;

//variables for storing previous values
int previous_az_pos=0, previous_el_pos=0;

String values = "", az_value = "", el_value="";
int int_az=0, int_el=0;
void homing()
{
  digitalWrite(azDirpin, LOW);
  digitalWrite(elDirpin, LOW);
  while(digitalRead(homeAz) != HIGH)
  {
    //AZstepper.move(1);
    AZstepper.runSpeed(100);
  }
  while(digitalRead(homeEl) != HIGH)
  {
    //ELstepper.move(1);
    ELstepper.runSpeed();
  }
    AZstepper.setCurrentPosition(0);
    ELstepper.setCurrentPosition(0);
}

void stepper_move(int az_value, int el_value)
{
  //To move azimuth

  if(previous_az_pos == 0){
    int az_count = az_value-previous_az_pos;
    AZstepper.move(az_count); // move by difference
    previous_az_pos = current_az_pos;
  }
  else{
    int az_count = az_value-previous_el_pos;
    AZstepper.move(az_count); // move by difference
    previous_az_pos = current_az_pos;
  }
  //Azimuth moving ended
  
  //To move elevation
  if(previous_el_pos < current_el_pos){
    int el_count = el_value-previous_el_pos;
    ELstepper.move(el_count); // move by difference
    previous_el_pos = current_el_pos;
  }
  else{
    int el_count = el_value - previous_el_pos;
    ELstepper.move(el_count); //move to ccw side
    previous_el_pos = current_el_pos;
  }
  //Elevation moving ended

  AZstepper.run();
  ELstepper.run();
}

void setup() {
  Serial.begin(9600);

  AZstepper.setMaxSpeed(100.0);
  AZstepper.setAcceleration(50.0);

  ELstepper.setMaxSpeed(100.0);
  ELstepper.setAcceleration(50.0);

  pinMode(azDirpin, OUTPUT);
  pinMode(elDirpin, OUTPUT);
  pinMode(azSteppin, OUTPUT);
  pinMode(elSteppin, OUTPUT);

  pinMode(enableAZ_motor_pin, OUTPUT);
  pinMode(enableEL_motor_pin, OUTPUT);

  pinMode(homeAz, INPUT);
  pinMode(homeEl, INPUT);

  void homing();

}

void loop() {
  digitalWrite(azDirpin, HIGH);
  digitalWrite(elDirpin, HIGH);
  //Code to parse GS232B values
  if(Serial.available()>0){
    values = Serial.readString();
    if(values == 'B'){Serial.print("EL=");Serial.println(previous_el_pos);}
    else if(values == 'C'){Serial.print("AZ=");Serial.println(previous_az_pos);}
    else if(values == "C2"){Serial.print("AZ=");Serial.print(previous_az_pos);
                            Serial.print(" EL=");Serial.println(previous_el_pos);}
    else if(values == "S"){AZstepper.stop();ELstepper.stop();}
    else if(values == "A"){AZstepper.stop();}
    else if(values == "E"){ELstepper.stop();}
    else if(values == "L" || "R" || "D" || "U" || "X1" || "X2" || "X3" || "X4" || "O" || "F" || "O2" || "F2" || "P36" || "P45" || "Z" || "H"){}
    else if(values.substring(0, 1) == 'M'){az_value = values.substring(1, 4);}
    else if(values.substring(0, 1) == 'W'){az_value = values.substring(1, 4);
                                           el_value = values.substring(5, 8);}

  }
  //End of parsing GS232B protocol
  //Code to parse PREDICT values
  if(Serial.available()>0){
    values = Serial.readString();
    while (values != "") {
      int  n = values.indexOf(' ');
      if (n > 0) {
        az_value = values.substring(0,n);
        el_value = values.substring(n+1);
        el_value.trim();
      }
    }
  }
  //End of the code for parsing PREDICT values
int_az = az_value.toInt();
int_el = el_value.toInt();
current_az_pos = int_az;
current_el_pos = int_el;
if(int_el<3){
  if(previous_el_pos == 3){homing();}
}
else{stepper_move(int_az, int_el);}


}
/*
void getValues(){
  if(Serial.available()>0){
    values = Serial.readString();
    if(values == 'B'){Serial.print("EL=");Serial.println(previous_el_pos);}
    else if(values == 'C'){Serial.print("AZ=");Serial.println(previous_az_pos);}
    else if(values == "C2"){Serial.print("AZ=");Serial.print(previous_az_pos);
                            Serial.print(" EL=");Serial.println(previous_el_pos);}
    else if(values == "S"){AZstepper.stop();ELstepper.stop();}
    else if(values == "A"){AZstepper.stop();}
    else if(values == "E"){ELstepper.stop();}
    else if(values == "L" || "R" || "D" || "U" || "X1" || "X2" || "X3" || "X4" || "O" || "F" || "O2" || "F2" || "P36" || "P45" || "Z" || "H"){}
    else if(values.substring(0, 1) == 'M'){az_value = values.substring(1, 4);}
    else if(values.substring(0, 1) == 'W'){az_value = values.substring(1, 4);
                                           el_value = values.substring(5, 8);}
return(az_value, el_value);
    }
}
*/

/*int stepper_move(int az1, int el1){
  if(current_az_pos == 0 && current_el_pos == 0){
    AZstepper.move(az1);
    ELstepper.move(el1);
    previous_az_pos = current_az_pos;
    previous_el_pos = current_el_pos;
  }
  else{
    AZstepper.move(az1);
    ELstepper.move(el1);
  }
  return 0;

  }*/
