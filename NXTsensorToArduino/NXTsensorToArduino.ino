// Intended to allow an Arduino to mimic a Hitech IRRceiver for Radio Control
// http://www.hitechnic.com/cgi-bin/commerce.cgi?preadd=action&key=NIR1032
// Edit the ServoDecode.h and set MAX_CHANNELS to correct number. Otherwise the transmitter must be turned on before receiver.
// ServoDecode Library @ http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1228137503/0
// RC PPM input on pin 8
// NXT Connector pinout http://en.wikipedia.org/wiki/Lego_Mindstorms_NXT#Connector
// Arduino ground -> NXT pin 2 and 3
// Arduino 5V -> NXT cable pin 4
// Arduino pin A4 -> NXT cable pin 6
// Arduino pin A5 -> NXT cable pin 5

#include <Wire.h>
#include <ServoDecode.h>

//  Address for the Arduino to announce itself as
#define ID 0x01
#define ADDRESS ID
#define ADDRESS_SEND 2
#define ADDRESS_RECV (ADDRESS_SEND - 1)

// Original information
// uint8_t SensorVersion[9] = " V1.1   ";
// uint8_t SensorName[9] =    "HiTechnc";
// uint8_t SensorType[9] =    "IRRecv  ";

uint8_t SensorVersion[9] = " V0.1   ";
uint8_t SensorName[9] =    "Arduino ";
uint8_t SensorType[9] =    "RadioCon";

byte x;
byte buffer[9] = {
  0,0,0,0,0,0,0,0};
byte sendbuffer[] = {
  0, 0};
//  Lego Power Functions IR values.  -128 is Break.
int PWM_MODE[] = {
  0, 16, 30, 44, 58, 72, 86, 100, -128};

// RC receiver channels to sample
int PanChan = 1;
int ThrustChan = 2;
int TiltChan = 3;
int SteerChan = 4;
int SwitchChan = 5;

int PPM_Min = 990;
float Tolerance = 0.05;

//unsigned long InitialPan;
unsigned long InitialThrust;
//unsigned long InitialTilt;
unsigned long InitialSteer;
//unsigned long InitialSwitch;

unsigned long Steer;
unsigned long Thrust;
unsigned long Pan;
unsigned long Tilt;
unsigned long Switch;

unsigned long S;
unsigned long T;
unsigned long Left;
unsigned long Right;

//Steering and Thrust High and Low Tolerances, used for channel mixing.
int S_H_Tol;
int T_H_Tol;
int S_L_Tol;
int T_L_Tol;

//  If the Failed samples count is greater than 10 then disable.  If the Ready count is greater than 1000 then enable.
int Fail_count = 0;
int Ready_count = 0;

void setup() {  
  Serial.begin(57600);             
  pinMode(8, INPUT); // RC input
  Wire.begin(ID);                
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);  
  ServoDecode.begin();
  delay(500);
  ServoDecode.setFailsafe(1,1500); 
  ServoDecode.setFailsafe(2,1500);
  ServoDecode.setFailsafe(3,1500); 
  ServoDecode.setFailsafe(4,1500); 
  ServoDecode.setFailsafe(5,1500); 
}

void loop(){
  if (ServoDecode.getState()!= READY_state) {
    ++Fail_count;
    Fail_count = constrain(Fail_count, 0, 10);
    if (Fail_count == 10){
      Ready_count = 0;
      InitialThrust = 1500;
      InitialSteer = 1500;
      digitalWrite(13,LOW); // Turn LED off
    }
  }
  else{
    ++Ready_count;
    Ready_count = constrain(Ready_count, 0, 1000);
    if (Ready_count == 1000){
      Fail_count = 0;
      digitalWrite(13,HIGH); // Light an LED When Ready
    }
  }
//  Ready count to try to prevent any false activation
  if (Ready_count == 999){ 
    InitialThrust = ServoDecode.GetChannelPulseWidth(ThrustChan) - PPM_Min;
    InitialSteer = ServoDecode.GetChannelPulseWidth(SteerChan) - PPM_Min;
  };
  if ( Ready_count == 1000) {
    Steer = constrain((ServoDecode.GetChannelPulseWidth(SteerChan) - PPM_Min), 0, 1000);
    Thrust = constrain((ServoDecode.GetChannelPulseWidth(ThrustChan) - PPM_Min), 0, 1000);
    Pan = constrain((ServoDecode.GetChannelPulseWidth(PanChan) - PPM_Min), 0, 1000);
    Tilt = constrain((ServoDecode.GetChannelPulseWidth(TiltChan) - PPM_Min), 0, 1000);
    Switch = constrain((ServoDecode.GetChannelPulseWidth(SwitchChan) - PPM_Min), 0, 1000);

    // invert stick if needed
    Tilt = 1000 - Tilt; 

    //-------------------------------------Channel Mixing------------------------------------------//

    S_H_Tol = InitialSteer + InitialSteer * Tolerance;
    S_L_Tol = InitialSteer - InitialSteer * Tolerance;

    T_H_Tol = InitialThrust + InitialThrust * Tolerance;
    T_L_Tol = InitialThrust - InitialThrust * Tolerance;

    S = Steer;
    T = Thrust;

    // Mix Steer and Thrust into Left and Right
    // Forward Right
    if ((T > T_H_Tol) && (S > S_H_Tol)){
      Right = T - (S - InitialSteer);
      Left = T;
    }
    //Forward Left
    if (( T > T_H_Tol) && (S < S_L_Tol) ){
      Right = T;
      Left = T - (InitialSteer - S);
    }
    //Back Right
    if (( T < T_L_Tol) && (S > S_H_Tol) ){
      Right = T - (InitialSteer - S);
      Left = T;
    }
    //Back Left
    if (( T < T_L_Tol) && (S < S_L_Tol) ){
      Right = T;
      Left = T - (S - InitialSteer);
    }   
    // Turn on spot
    if (T <= T_H_Tol && T >= T_L_Tol){
      Right = 2 * InitialSteer - S;
      Left = S;
    }
    //Forward and Back
    if (S <= S_H_Tol && S >= S_L_Tol){
      Right = T;
      Left = T;
    }
    if (Left <= T_H_Tol && Left >= T_L_Tol && Right <= T_H_Tol && Right >= T_L_Tol) {
      Left = InitialThrust;
      Right = InitialThrust;
    }   

    Left =   constrain(Left, 0, 1000);
    Right =  constrain(Right, 0, 1000);

    //--------------------------------------------------------------------------------------//

//    PWM_trans(Steer, 0);
//    PWM_trans(Thrust, 1);
    PWM_trans(Left, 0);
    PWM_trans(Right, 1);
    PWM_trans(Pan, 2);    
    PWM_trans(Tilt, 3);
    PWM_trans(Switch, 4);
  }
}

void PWM_trans(int val, int out){
  if ( val > 940){  //7 FWD
    buffer[out] = PWM_MODE[7];
  }
  else if (val >= 880 && val <= 940){ // 6 FWD
    buffer[out] = PWM_MODE[6];
  }
  else if (val >= 820 && val <= 880){ // 5 FWD
    buffer[out] = PWM_MODE[5];
  }
  else if (val >= 760 && val <= 820){ // 4 FWD
    buffer[out] = PWM_MODE[4];
  }
  else if (val >= 700 && val <= 760){ // 3 FWD
    buffer[out] = PWM_MODE[3];
  }
  else if (val >= 640 && val <= 700){ // 2 FWD
    buffer[out] = PWM_MODE[2];
  }
  else if (val >= 580 && val <= 640){ // 1 FWD
    buffer[out] = PWM_MODE[1];
  }
  else if (val >= 420 && val <= 580){ // FLT
    buffer[out] = PWM_MODE[0];
  }
  else if (val >= 360 && val <= 420){ // 1 REV
    buffer[out] = 0 - PWM_MODE[1];
  }
  else if (val >= 300 && val <= 360){ // 2 REV
    buffer[out] = 0 - PWM_MODE[2];
  }
  else if (val >= 240 && val <= 300){ // 3 REV
    buffer[out] = 0 - PWM_MODE[3];
  }
  else if (val >= 180 && val <= 240){ // 4 REV
    buffer[out] = 0 - PWM_MODE[4];
  }
  else if (val >= 120 && val <= 180){ // 5 REV
    buffer[out] = 0 - PWM_MODE[5];
  }
  else if (val >= 60 && val <= 120){ // 6 REV
    buffer[out] = 0 - PWM_MODE[6];
  }
  else if (val < 60){// 7 REV
    buffer[out] = 0 - PWM_MODE[7];
  }
}

//---------------------------------I2C Events-------------------------------//

void receiveEvent(int howMany){
  x = Wire.receive(); // receive byte as an integer
}

void requestEvent(){
  if (x == 0x00){
    Wire.send(SensorVersion, 8);
  }
  else if (x == 0x08){
    Wire.send(SensorName, 8);
  }
  else if (x == 0x10){
    Wire.send(SensorType, 8);
  }
  else if (x == 0x42){// Channel 1, Motor 1A and 1B
    sendbuffer[0] = buffer[0];
    sendbuffer[1] = buffer[1];
    Wire.send(sendbuffer, 2);
  }
  else if (x == 0x44){// Channel 2, Motor 2A and 2B
    sendbuffer[0] = buffer[2];
    sendbuffer[1] = buffer[3];
    Wire.send(sendbuffer, 2);
  }
  else if (x == 0x46){// Channel 3, Motor 3A and 3B
    sendbuffer[0] = buffer[4];
    sendbuffer[1] = buffer[5];
    Wire.send(sendbuffer, 2);
  }
  else if (x == 0x48){// Channel 4, Motor 4A and 4B
    sendbuffer[0] = buffer[6];
    sendbuffer[1] = buffer[7];
    Wire.send(sendbuffer, 2);
  }
}