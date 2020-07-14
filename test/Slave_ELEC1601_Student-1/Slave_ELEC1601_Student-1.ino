#include <SoftwareSerial.h>   //Software Serial Port
#include <StackArray.h>
#include <Servo.h>

// https://www.pololu.com/file/0J195/line-maze-algorithm.pdf

Servo servoLeft;
Servo servoRight;
const int pinServoLeft = 13;
const int pinServoRight = 12;
const int pinLeftLED = 9 ;
const int pinRightLED = 2;
const int pinMidLED = 4;
const int pinLeftReceiver = 10;
const int pinRightReceiver = 3 ;
const int pinMidReceiver = 5;
const int frequency = 38000;
bool onTrace = false;
int cc = 0;
// StackArray <int> stack;
String mem = "                                                                                                    ";
int memPointer = 0;
#define RxD 7

#define TxD 6



#define DEBUG_ENABLED  1



SoftwareSerial blueToothSerial(RxD, TxD);

void moveForward() {
  servoLeft.attach(pinServoLeft);
  servoRight.attach(pinServoRight);
  servoLeft.writeMicroseconds(1700);
  servoRight.writeMicroseconds(1300);
  delay(1450);
  servoLeft.detach();                            // Stop servo signals
  servoRight.detach();
  delay(1000);
}

void reverse() {
  servoLeft.attach(pinServoLeft);
  servoRight.attach(pinServoRight);
  servoLeft.writeMicroseconds(1700);
  servoRight.writeMicroseconds(1700);
  delay(1000);
  servoLeft.detach();                            // Stop servo signals
  servoRight.detach();
  delay(1000);
}

void turnLeft() {
  servoLeft.attach(pinServoLeft);
  servoRight.attach(pinServoRight);
  servoLeft.writeMicroseconds(1300);
  servoRight.writeMicroseconds(1300);
  delay(500);
  servoLeft.detach();                            // Stop servo signals
  servoRight.detach();
  delay(1000);
}

void turnRight() {
  servoLeft.attach(pinServoLeft);
  servoRight.attach(pinServoRight);
  servoLeft.writeMicroseconds(1700);
  servoRight.writeMicroseconds(1700);
  delay(480);
  servoLeft.detach();                            // Stop servo signals
  servoRight.detach();
  delay(1000);
}

void hold() {
  servoLeft.attach(pinServoLeft);
  servoRight.attach(pinServoRight);
  servoLeft.writeMicroseconds(1500);
  servoRight.writeMicroseconds(1500);
  servoLeft.detach();                            // Stop servo signals
  servoRight.detach();
  delay(1000);
}

int irDetect(int irLedPin, int irReceiverPin, int frequency)
{
  tone(irLedPin, frequency, 8);              // IRLED 38 kHz for at least 1 ms
  delay(1);                                  // Wait 1 ms
  int ir = digitalRead(irReceiverPin);       // IR receiver -> ir variable
  delay(1);                                  // Down time before recheck
  return ir;                                // Return 1 no detect, 0 detect
}

int state() {
  int d[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  for (int i = 0; i < 10; ++i) {
    int irLeft = irDetect(pinLeftLED, pinLeftReceiver, frequency);       // Check for object on left
    int irRight = irDetect(pinRightLED, pinRightReceiver, frequency);       // Check for object on right
    int irMid = irDetect(pinMidLED, pinMidReceiver, frequency);
    int value = irLeft * 4 + irRight * 2 + irMid;
    d[value] = d[value] + 1;
    delay(10);
  }
  int maxd = -1, res = 0;
  for (int i = 0; i < 8; ++i)
    if (maxd < d[i]) {
      maxd = d[i];
      res = i;
    }

  return res;
}
int translate(int l, int r, int m) {
  return l * 4 + r * 2 + m;
}

void setup()
{

  Serial.begin(9600);
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);
  setupBlueToothConnection();
  servoLeft.attach(pinServoLeft);
  servoRight.attach(pinServoRight);
  pinMode(pinLeftLED, OUTPUT);  pinMode(pinLeftReceiver, INPUT);
  pinMode(pinRightLED, OUTPUT);  pinMode(pinRightReceiver, INPUT);
  pinMode(pinMidLED, OUTPUT); pinMode(pinMidReceiver, INPUT);
  mem[memPointer] = 's';

}

void simplify_path()
{
  // only simplify the path if the second-to-last turn was a 'B'
  if (memPointer < 2 || mem[memPointer - 1] != 'u')
    return;

  int total_angle = 0;
  for (int i = 1; i <= 3; ++i)
  {
    switch (mem[memPointer - i + 1])
    {
      case 'r':
        total_angle += 90;
        break;
      case 'l':
        total_angle += 270;
        break;
      case 'u':
        total_angle += 180;
        break;
    }
  }

  // Get the angle as a number between 0 and 360 degrees.
  total_angle = total_angle % 360;

  // Replace all of those turns with a single one.
  switch (total_angle)
  {
    case 0:
      mem[memPointer - 2] = 's';
      break;
    case 90:
      mem[memPointer - 2] = 'r';
      break;
    case 180:
      mem[memPointer - 2] = 'u';
      break;
    case 270:
      mem[memPointer - 2] = 'l';
      break;
  }

  // The path is now two steps shorter.
  memPointer -= 2;
}



void lo1op()
{
  int cnt = 0;
  char cur;
  while (cnt < 6) {
    if (blueToothSerial.available()) {
      cur = blueToothSerial.read();
      delay(1);
    }

    cnt += 1;
    delay(1);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  //  Serial.println(onReverse);
  //  delay(2000);
  char cur;
  int cnt = 0;
  while (cnt < 6) {
    if (blueToothSerial.available()) {
      cur = blueToothSerial.read();
      delay(1);
    }
    if (cur == '.') {
      if (!onTrace) {
        Serial.println("a;sdjfa");
        blueToothSerial.println("The robot has stopped. Now place it at the begin!");
        blueToothSerial.println(mem);
        onTrace = true;
        cc = 0;
      }
      else {
        Serial.println("a;sdjfa");
        blueToothSerial.println("The robot has finished its shortest path!");
        memPointer = 0;
        String mem = "                                                                                                    ";
        onTrace = false;
      }
      hold();
      break;
    }
    cnt += 1;
    delay(50);
  }
  while (true) {
    if (blueToothSerial.available()) {
      cur = blueToothSerial.read();
      delay(1);
    }
    if (cur == '`') {
      break;
    } else if (cur == '[') {
      servoLeft.attach(pinServoLeft);
      servoRight.attach(pinServoRight);
      servoLeft.writeMicroseconds(1300);
      servoRight.writeMicroseconds(1300);
      delay(25);
      servoLeft.detach();                            // Stop servo signals
      servoRight.detach();
    } else if (cur == ']') {
      servoLeft.attach(pinServoLeft);
      servoRight.attach(pinServoRight);
      servoLeft.writeMicroseconds(1700);
      servoRight.writeMicroseconds(1700);
      delay(25);
      servoLeft.detach();                            // Stop servo signals
      servoRight.detach();
    } else if (cur == '=') {
      servoLeft.attach(pinServoLeft);
      servoRight.attach(pinServoRight);
      servoLeft.writeMicroseconds(1700);
      servoRight.writeMicroseconds(1300);
      delay(25);
      servoLeft.detach();                            // Stop servo signals
      servoRight.detach();
    }
    delay(50);
  }

  if (onTrace == false) {
    int current = state();
    blueToothSerial.print(current);
    if (current == 1) {
      moveForward();
    } else if (current == 0) {
      reverse();
      moveForward();
      ++memPointer;
      mem[memPointer] = 'u';
    } else {
      if (current == translate(1, 0, 0)) {
        turnLeft();
        moveForward();
      }
      else if (current == translate(0, 1, 0)) {
        turnRight();
        moveForward();
      }
      else if (current == translate(1, 0, 1)) {
        turnLeft();
        moveForward();
        ++memPointer;
        mem[memPointer] = 'l';
      }
      else if (current ==  translate(0, 1, 1)) {
        moveForward();
        ++memPointer;
        mem[memPointer] = 's';
      } else if (current == translate(1, 1, 0)) {
        turnLeft();
        moveForward();
        ++memPointer;
        mem[memPointer] = 'l';
      } else if (current == translate(1, 1, 1)) {
        turnLeft();
        moveForward();
        ++memPointer;
        mem[memPointer] = 'l';
      }
    }
    simplify_path();
  } else {
    int current = state();
    blueToothSerial.print(current);
    if (current == 1) {
      moveForward();
    } else  if (current == translate(1, 0, 0)) {
      turnLeft();
      moveForward();
    } else if (current == translate(0, 1, 0)) {
      turnRight();
      moveForward();
    } else {
      if (mem[cc] == 's') {
        moveForward();
      } else if (mem[cc] == 'l') {
        turnLeft();
        moveForward();
      } else if (mem[cc] == 'r') {
        turnRight();
        moveForward();
      }
      ++cc;
    }
  }

}



void setupBlueToothConnection()
{
  blueToothSerial.begin(38400);                           // Set BluetoothBee BaudRate to default baud rate 38400
  blueToothSerial.print("\r\n+STWMOD=0\r\n");             // set the bluetooth work in slave mode
  blueToothSerial.print("\r\n+STNA=Slave30+\r\n");    // set the bluetooth name as "SeeedBTSlave"
  blueToothSerial.print("\r\n+STOAUT=1\r\n");             // Permit Paired device to connect me
  blueToothSerial.print("\r\n+STAUTO=0\r\n");             // Auto-connection should be forbidden here
  delay(2000);                                            // This delay is required.
  blueToothSerial.print("\r\n+INQ=1\r\n");                // make the slave bluetooth inquirable
  Serial.println("The slave bluetooth is inquirable!");
  delay(2000);                                            // This delay is required.

  blueToothSerial.flush();

}
