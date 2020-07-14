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
int oldDir;
// StackArray <int> stack;
String mem="                                                                                                    ";
int memPointer=0;

void moveForward() {
    servoLeft.attach(pinServoLeft);
    servoRight.attach(pinServoRight);
    servoLeft.writeMicroseconds(1700);
    servoRight.writeMicroseconds(1300);
    delay(1500);
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
    delay(450);
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
    int d[8] = {0,0,0,0,0,0,0,0};
    for(int i=0;i<10;++i) {
        int irLeft = irDetect(pinLeftLED, pinLeftReceiver, frequency);       // Check for object on left
        int irRight = irDetect(pinRightLED, pinRightReceiver, frequency);       // Check for object on right
        int irMid = irDetect(pinMidLED, pinMidReceiver, frequency);
        int value = irLeft * 4 + irRight * 2 + irMid;
        d[value] = d[value]+1;
        delay(10);
    }
    int maxd = -1,res=0;
    for(int i=0;i<8;++i)
    if(maxd<d[i]) {
        maxd=d[i];
        res=i;
    }

    return res;
}
int translate(int l, int r, int m) {
    return l * 4 + r * 2 + m;
}

void setup() {
    // put your setup code here, to run once:
    servoLeft.attach(pinServoLeft);
    servoRight.attach(pinServoRight);
    pinMode(pinLeftLED, OUTPUT);  pinMode(pinLeftReceiver, INPUT);
    pinMode(pinRightLED, OUTPUT);  pinMode(pinRightReceiver, INPUT);
    pinMode(pinMidLED, OUTPUT); pinMode(pinMidReceiver, INPUT);
    Serial.begin(9600);
   
    mem[memPointer]='s';
    //  Serial.print(stack.peek());

}

void simplify_path()
{
    // only simplify the path if the second-to-last turn was a 'B'
    if(memPointer < 2 || mem[memPointer-1] != 'B')
        return;
 
    int total_angle = 0;
    for(int i=1;i<=3;++i)
    {
        switch(mem[memPointer-i+1])
        {
			case 'R':
				total_angle += 90;
				break;
			case 'L':
				total_angle += 270;
				break;
			case 'B':
				total_angle += 180;
				break;
        }
    }
 
    // Get the angle as a number between 0 and 360 degrees.
    total_angle = total_angle % 360;
 
    // Replace all of those turns with a single one.
    switch(total_angle)
    {
		case 0:
			mem[memPointer - 2] = 'S';
			break;
		case 90:
			mem[memPointer - 2] = 'R';
			break;
		case 180:
			mem[memPointer - 2] = 'B';
			break;
		case 270:
			mem[memPointer - 2] = 'L';
			break;
    }
 
    // The path is now two steps shorter.
    memPointer -= 2;
}
void loo1p() {
//  reverse();
    Serial.println(state());
    //  moveForward();
//    turnLeft();
//    servoLeft.detach();
//    servoRight.detach();
//    delay(1000);
//    servoLeft.attach(pinServoLeft);
//    servoRight.attach(pinServoRight);
//    moveForward();
//    servoLeft.detach();
//    servoRight.detach();
    delay(500);
}
void loop() {
    // put your main code here, to run repeatedly:
    //  Serial.println(onReverse);
    //  delay(2000);
    if(onTrace==false) {
        int current = state();
        if (current == 1) {
            moveForward();
        } else if (current == 0) {
            reverse();
            ++memPointer;
            mem[memPointer]='u';
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
                mem[memPointer]='l';
            }
            else if (current ==  translate(0, 1, 1)) {
                moveForward();
				        ++memPointer;
                mem[memPointer]='s';
            } else if (current == translate(1, 1, 0)) {
                turnLeft();
                moveForward();
                ++memPointer;
                mem[memPointer]='l';
            } else if (current == translate(1, 1, 1)) {
                turnLeft();
                moveForward();
                ++memPointer;
                mem[memPointer]='l';
            }
        }
		    simplify_path();
    } else 
    {
		  Serial.println(mem);
		  delay(100000);
    }

}
