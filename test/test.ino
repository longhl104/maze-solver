const int pinLeftLED = 9 ;
const int pinRightLED = 2;
const int pinMidLED = 6;
const int pinLeftReceiver = 10;
const int pinRightReceiver = 3 ;
const int pinMidReceiver = 7;
const int frequency = 38000;

void setup() {
  // put your setup code here, to run once:
  pinMode(pinLeftLED, OUTPUT);  pinMode(pinLeftReceiver, INPUT);
  pinMode(pinRightLED, OUTPUT);  pinMode(pinRightReceiver, INPUT);
  pinMode(pinMidLED, OUTPUT); pinMode(pinMidReceiver, INPUT);
  Serial.begin(9600);
}



int irDetect(int irLedPin, int irReceiverPin, int frequency)
{
  tone(irLedPin, frequency, 8);              // IRLED 38 kHz for at least 1 ms
  delay(1);                                  // Wait 1 ms
  int ir = digitalRead(irReceiverPin);       // IR receiver -> ir variable
  delay(1);                                  // Down time before recheck
  return ir;                                // Return 1 no detect, 0 detect
}

void loop() {
  // put your main code here, to run repeatedly:
  int left = irDetect(pinLeftLED, pinLeftReceiver, frequency);
  int mid = irDetect(pinMidLED, pinMidReceiver, frequency);
  int right = irDetect(pinRightLED, pinRightReceiver, frequency);
  Serial.print(left);Serial.print(" ");Serial.print(mid);Serial.print(" ");Serial.println(right);
  delay(500);
}
