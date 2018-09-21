#include <Servo.h>

// Global Values //
bool debug = true;
bool inDarkRoom = false;
const int TRACE_DIST = 14;
const int MIN_TRACE_DIST = 6;

// Motors //
Servo rightservo;
Servo leftservo;
int i = 0;
const int dur = 16;

// Photo Resistor  //
int lightValue;
const int pResistor = A0;
const int pResistorRight = A1;
const int pResistorLeft =A2;

// Ping Sensors //
const int rightPing = 2;
const int leftPing = 13;

//************** SETUP FUNCTIONS *****************//
void setup() {
  if (debug == true){
    Serial.begin(9600);
  }
  motorSetup();
  PRSetup();
  initializeLocation();
}

void motorSetup() {
  rightservo.attach(10);
  leftservo.attach(12);
}

void PRSetup() {
  pinMode(pResistor, INPUT);
}

//************** MAIN LOOP **********************//
void loop() {
  // Robot Movement Control  //
  delay(500);
  if(!inDarkRoom){
    moveThroughCourse();
  }
  else {
    darkRoomMove();
  }
}

//************* High Level Algorithm Functions **************//
// Initial Robot Orientation
void initializeLocation() {
  if (!somethingLeft()) {
    turnLeft(90);
    while(!somethingFront()) {
      moveforward(1);
    }
    turnRight(90);
  }
}

// Course Movement
void moveThroughCourse() {
  if(lightLevel()>150) {
    if(somethingLeft()){
      if(lightLevel() > 800) {
        turnRight(15);
      }
      else if(lightLevel() < 350) {
        turnLeft(15);
      }
      else if(somethingFront()){
        turnRight(92);
        moveforward(1);
      }
      else if(tooCloseLeft()){
        Serial.println("turn");
        turnRight(7);
        moveforward(1);
      }
      else if(tooFarLeft()){
        turnLeft(5);
        moveforward(1);
      }
      else {
        Serial.println("straight");
        moveforward(1);
      }
    }
    else {
      turnLeft(87);
      moveforward(3);
    }
  }
  else {
    inDarkRoom = true;
  }
}

// Dark Room Movement
void darkRoomMove() {
  int range = 120;
  int angleSlice = 10;
  int whichAngle = 0;
  turnLeft(range/2);
  int highestLight = analogRead(pResistor);
  for(int i = 0; i < range/angleSlice; i++) {
   turnRight(angleSlice);
   if(analogRead(pResistor) > highestLight) {
     highestLight = analogRead(pResistor);
     whichAngle = i;
   }
  }
  turnLeft(range);
  turnRight(whichAngle * angleSlice);
  moveforward(5);
}

//*************** Hardware Operation Functions ***********//
// Photo Resistor Functions //
int rightLight(){
  int value = analogRead(pResistorRight);
  return value;
}
int leftLeft(){
  int value = analogRead(pResistorLeft);
  return value;
}
int lightLevel(){
  int value = analogRead(pResistor);
  return value;
}

// Ping Detector Functions //
long pingDistance(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delayMicroseconds(2);
  digitalWrite(pin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pin, LOW);
  pinMode(pin, INPUT);
  int duration = pulseIn(pin, HIGH);
  int inches = microsecondsToInches(duration);
  return inches;
}

long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}

bool somethingLeft() {
  int dist = pingDistance(leftPing);
  return dist < TRACE_DIST;
}

bool somethingFront() {
  int dist = pingDistance(rightPing);
  return dist < TRACE_DIST-2;
}

bool tooCloseLeft() {
  int dist = pingDistance(leftPing);
  Serial.println(dist);
  return dist < MIN_TRACE_DIST;
}

bool tooFarLeft() {
  int dist = pingDistance(leftPing);
  return dist > 10 && dist < TRACE_DIST;
}

// Robot Motor Control Functions //
void moveforward(int dist) {
  for (i = 0; i < 2; i++ ) { 
    rightservo.writeMicroseconds(1500);    // motor stop 
    leftservo.writeMicroseconds(1500);
  }
  
  for (i = 0; i < dist*(dur+2); i++) {
    rightservo.writeMicroseconds(1625);     // full speed forward
    leftservo.writeMicroseconds(1625);
    delay(15);  
  } 
  for (i = 0; i < 2; i++ ) { 
    rightservo.writeMicroseconds(1500);    // motor stop 
    leftservo.writeMicroseconds(1500);
  }
}

void movebackward(int dist) {
    for (i = 0; i < 2; i++) { 
    rightservo.writeMicroseconds(1500);    // motor stop 
    leftservo.writeMicroseconds(1500);
    delay(15); 
  }
  
  for (i = 0; i < dist*(dur+2); i++) {
    rightservo.writeMicroseconds(1375);     // full speed backward
    leftservo.writeMicroseconds(1375);
    delay(15);  
  } 
  for (i = 0; i < 2; i++) { 
    rightservo.writeMicroseconds(1500);    // motor stop 
    leftservo.writeMicroseconds(1500);
    delay(15); 
  }
}

void turnLeft(int deg) {
  for (i = 0; i < 2; i++ ) { 
    rightservo.writeMicroseconds(1500);    // motor stop 
    leftservo.writeMicroseconds(1500);
    delay(15); 
  }
  
  for (i = 0; i < 1.05*deg; i++) {
    rightservo.writeMicroseconds(1625);     // 
    leftservo.writeMicroseconds(1375);
    delay(15);  
  } 
  for (i = 0; i < 2; i++ ) { 
    rightservo.writeMicroseconds(1500);    // motor stop 
    leftservo.writeMicroseconds(1500);
    delay(15); 
  }
}

void turnRight(int deg) {
  for (i = 0; i < 2; i++ ) { 
    rightservo.writeMicroseconds(1500);    // motor stop 
    leftservo.writeMicroseconds(1500);
    delay(15); 
  }
  
  for (i = 0; i < 1.11*deg; i++) {
    rightservo.writeMicroseconds(1375);     // 
    leftservo.writeMicroseconds(1625);
    delay(15);  
  } 
  for (i = 0; i < 2; i++ ) { 
    rightservo.writeMicroseconds(1500);    // motor stop 
    leftservo.writeMicroseconds(1500);
    delay(15); 
  }
}

//*************** Test Functions ****************//
void driveforwardtest(){
  moveforward(1);
}

void driveforwardpingtest(int R, int L){
  if (R > 12 && L > 12) {
    moveforward(1);
  }
}

void driveforwardlighttest(int light) {
  if (light > 150) {
    moveforward(1);
  }
}

void printlightdata(int light) {
  Serial.print("light: ");
  Serial.print(light);
  Serial.println();
}
