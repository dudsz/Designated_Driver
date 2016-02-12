/* 
  Markus Erlach
  Functions at the end for US/IR/WE are not completly 
  done by us. Have changed minor things but most were
  retrieved to test the components
*/

// Include libraries 
#include <Servo.h> 
#include <Wire.h>       

// US
int US_Front= 0x72;     // Addresses for US sensors 
int US_Front_R = 0x70;
#define cmdByte 0x00              
#define lightByte 0x01  // Byte to read light sensor
#define rangeByte 0x02  // Byte for start of ranging data
#define cm 0x51         // Choose if you want cm 
#define inch 0x50       // or inches
byte highByte = 0x00;   // Stores high byte from ranging
byte lowByte = 0x00;    // Stored low byte from ranging
 
#define servoPin 8      // Defining pins
#define escPin 9
#define encoder0Pin 2
  
#define ESC_HIGH 2000   // High and low of ESC
#define ESC_LOW 1000    

// IR                   
int a = 2914;               
int distance, distance2;   
int ir_back = 14;          // Addresses for IR sensors
int ir_rfront = 13;
int ir_rback = 15;
int voltage, voltage2;    

// Defining Data variables
String to_Send, IRP1, IRP2, IRP3, IR_FR_Str, IR_BR_Str, IR_Back_Str, 
    USP1, USP2, US_FR_Str, US_FC_Str, CSum_toPad, CSum_Str, 
    len_toPad, len_Str, WE_Str, WE_toPad;
char inData[255];      
char c; // -1;
int i = 0;
int one, two, three, four, five, checksum, check, the, sum, 
  US_FC_Int, US_FR_Int, IR_FR_Int, IR_BR_Int, toCheck,
  IR_Back_Int, len_Int, CSum_Int, speedy, steering, WE_Int;
// Set default WheelEncoder, ESC and Servo start.
int angle = 90;
int throttle = 1500;
volatile int count = 0, countOld = 0;

Servo servo, esc;

void setup() 
{ 
  pinMode(ir_rfront, INPUT);
  pinMode(ir_rback, INPUT);
  pinMode(ir_back, INPUT);
  pinMode(encoder0Pin, INPUT_PULLUP); 
  attachInterrupt(0, sensor1, FALLING);
  Wire.begin();
  servo.attach(servoPin); 
  esc.attach(escPin);
  Serial.begin(38400);
  delay(1000);
}

/*
  Loop to write to ESC and Servo
  Then read in data from sensors to store it
  and send it to HLB
*/
void loop() {
  delay(10);
  esc.writeMicroseconds(throttle);
  servo.write(angle);
  delay(10);
  /*
  // US
  US_FR_Int = getUS(US_Front);    // 12;
  US_FC_Int = getUS(US_Front_R);  // 11;
  // IR
  IR_FR_Int = getIR(ir_rfront);
  //IR_BR_Int = getIR(ir_rback);
  IR_Back_Int = getIR(ir_back);
  // WheelEncoder
  WE_Int = getWE();
  delay(10);
  
  // US sensors
  if (US_FR_Int >= 100) { // US_Range_Int2 >= 100
    US_FR_Int = 99;
    US_FR_Str = String(US_FR_Int);
  } if (US_FR_Int >= 0 && US_FR_Int < 10) {
    USP1 = String(US_FR_Int);
    US_FR_Str = "0" + USP1;  
  } else {
    US_FR_Str = String(US_FR_Int);
  }
  if (US_FC_Int >= 100) { 
    US_FC_Int = 99;
    US_FC_Str = String(US_FC_Int);
  } if (US_FC_Int >= 0 && US_FC_Int < 10) {
    USP2 = String(US_FC_Int);
    US_FC_Str = "0" + USP2;  
  } else {
    US_FC_Str = String(US_FC_Int);
  }
  // IR sensors
  if (IR_FR_Int >= 100) {             // IR_BR >= 100 & IR_Back >= 100
    IR_FR_Int = 99;                   // IR_BR = 99; IR_Back = 99;
    IR_FR_Str = String(IR_FR_Int);    // IR_BR_Str = String(IR_BR); IR_Back_Str = String(IR_Back);
  } 
  if (IR_FR_Int < 10 && IR_FR_Int > 0) {
    IRP1 = String(IR_FR_Int);         // IR_To_Pad2 = String(IR_BR); IR_To_Pad3 = String(IR_Back); 
    IR_FR_Str = "0" + IRP1;       // IR_BR_Str = "0" + IR_To_Pad2; IR_Back_Str = "0" + IR_To_Pad3;  
  } else { 
    IR_FR_Str = String(IR_FR_Int);    // IR_BR_Str = String(IR_BR); IR_Back_Str = String(IR_Back);
  }
  
/*  if (IR_BR_Int >= 100) {             
    IR_BR_Int = 99;                   
    IR_BR_Str = String(IR_BR_Int);    
  } 
  if (IR_BR_Int < 10 && IR_BR_Int > 0) {
    IRP2 = String(IR_BR_Int);    
    IR_BR_Str = "0" + IRP2;         
  } else { 
    IR_BR_Str = String(IR_BR_Int);    
  }
  */
  /*
  if (IR_Back_Int >= 100) {             
    IR_Back_Int = 99;                   
    IR_Back_Str = String(IR_Back_Int);    
  } 
  if (IR_Back_Int < 10 && IR_Back_Int > 0) {
    IRP3 = String(IR_Back_Int);    
    IR_Back_Str = "0" + IRP3;         
  } else { 
    IR_Back_Str = String(IR_Back_Int);    
  }
  
  // Wheel Encoder
  if (WE_Int >= 100) {     
    WE_Str = String(WE_Int);    
  } 
  if (WE_Int >= 10 && WE_Int < 100) {
    WE_toPad = String(WE_Int);    
    WE_Str = "0" + WE_toPad;         
  } else if (WE_Int < 10) { 
    WE_toPad = String(WE_Int);
    WE_Str = "00" + WE_toPad;  
  }
  
  //delay(10);
  //Checksum
  CSum_Int = US_FR_Int + US_FC_Int + IR_FR_Int + IR_Back_Int + WE_Int; 
   // + IR_Back_Int + WE_Int;
  if (CSum_Int < 10 && CSum_Int > 0) { 
    CSum_toPad = String(CSum_Int);    
    CSum_Str = "00" + CSum_toPad;    
  } 
  if (CSum_Int < 100 && CSum_Int >= 10) {
    CSum_toPad = String(CSum_Int);    
    CSum_Str = "0" + CSum_toPad;         
  } else { 
    CSum_Str = String(CSum_Int);    
  }
  // Create length to send
  len_Int = US_FR_Str.length() + US_FC_Str.length() 
    + IR_FR_Str.length() //+ IR_BR_Str.length() 
    + IR_Back_Str.length() + CSum_Str.length() + WE_Str.length() +3;
  // Check length
  if (len_Int < 10 && len_Int > 0) {
    CSum_toPad = String(CSum_Int);    
    CSum_Str = "0" + len_toPad;         
  } else { 
    len_Str = String(len_Int);    
  }
  len_Str = String(len_Int);
  
  /* To send
  will be: <Len:CSum_Send:IR1IR2IR3US1US2WE  
  Example: <16:044:0304021520001>             
  */
  /*
  to_Send = "<" + len_Str + ":" + CSum_Str + ":" +IR_FR_Str //+ IR_BR_Str 
    + IR_Back_Str + US_FR_Str + US_FC_Str + WE_Str + ">";   
  */
  String str = "<17:080:20202020000>";
  char test[str.length()+1];
  str.toCharArray(test, str.length()+1);
  /*
  char byteArray[to_Send.length() + 1];
  to_Send.toCharArray(byteArray, to_Send.length()+1);
  Serial.write(byteArray);
  */
  Serial.write(test);
}

/* 
  Function to read in serial data and 
  use the retrieved data. Is run between each iteration of
  the loop
*/
void serialEvent() { 
  while (Serial.available() > 0) {
      c = Serial.read();
      inData[i] = c;
      i++;
      delay(2);
  }
  inData[i] = '\0';
  // 12308115
  if (isdigit(inData[0]) && isdigit(inData[1]) && isdigit(inData[2])
    && isdigit(inData[3]) && isdigit(inData[4])) {
    setData();
  }
  i = 0;
  // Additional if statements for simple steering
  // and driving from HLB for oral examination
  if (inData[0] == 's' || inData[0] == 'S') {
    throttle = throttle - 10;  
  }
  i = 0;
  if (inData[0] == 'w' || inData[0] == 'W') {
    throttle = 1560;  
  }
  i = 0;
  if (inData[0] == 'f' || inData[0] == 'F') {
    throttle = throttle + 10;  
  }
  i = 0;
  if (inData[0] == 'r' || inData[0] == 'R') {
    throttle = 1270;  
  }
  i = 0;
  if (inData[0] == 'n' || inData[0] == 'N') {
    throttle = 1500;  
  }
  i = 0;
  if (inData[0] == 'h' || inData[0] == 'H') {
    angle = 115;  
  }
  i = 0;
  if (inData[0] == 'v' || inData[0] == 'V') {
    angle = 65;  
  }
  i = 0;
  if (inData[0] == 'm' || inData[0] == 'M') {
    angle = 90;  
  }
  i = 0;
}

// Get US sensors data
int getUS(int address) {
  int range = 0;

  Wire.beginTransmission(address);     // Start communticating with SRF08
  Wire.write((byte)cmdByte);           // Send Command Byte
  Wire.write(cm);                      // Choose between inch and cm
  Wire.endTransmission();
 
  delay(70);                           // Wait for ranging to be complete
 
  Wire.beginTransmission(address);     // start communicating with SRFmodule
  Wire.write(rangeByte);               // Call the register for start of ranging data
  Wire.endTransmission();
 
  Wire.requestFrom(address, 2);        // Request 2 bytes from SRF module
  while (Wire.available() < 2);
  byte highByte = Wire.read();
  byte lowByte = Wire.read();
  
  range = (highByte << 8) + lowByte;
   
  return(range); 

}

// Function to set Speed and Servo angle
void setData() {
  
  check = inData[0] - '0',
  the = inData[1] - '0',
  sum = inData[2] - '0',
  one = inData[3] - '0',
  two = inData[4] - '0',
  three = inData[5] - '0',
  four = inData[6] - '0',
  five = inData[7] - '0';
  
  i = 0; // Reset i

  // Checksum
  checksum = ((check * 100) + (the * 10) + sum);
  //10008092
  // Speed and Angle
  // Simon Lobo Roca
  speedy = ((one * 10) + two);
  // Markus Erlach
  steering = ((three * 100) + (four * 10) + five);  
  toCheck = speedy + steering;
  if (checksum == toCheck) {
    angle = steering;
    throttle = 1571;
    // Simon Lobo Roca
    if(speedy > 7){
      throttle = 1600;
    }
    if (speedy > 4 && speedy <= 7) {
      throttle = 1572;
    }
    if (speedy < 1) {
      throttle = 1500;  
    }     
    // Markus Erlach
    // Max and min values
    if (throttle > 1620) {
      throttle = 1620; 
    }
    if (angle > 120) {
     angle = 120; 
    } if (angle < 60) {
      angle = 60;
    }
  }
}

// Get IR Distance
int getIR(int ir) {
  voltage = analogRead(ir);
  distance = (a/(voltage+5))-1; 
  delay(20);
  return (distance);
}

// Get Wheel Encoder
int getWE() {
  if (countOld != count)
  {
    //Serial.println(count);
       countOld = count;
  }
  return (countOld);
}

void sensor1()
{
  count++;
}
