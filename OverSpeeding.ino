#include <SoftwareSerial.h>
#include <Servo.h>
#include <Sim800L.h>

#define INTERRUPT 0
#define intPin 2
#define IN1 5
#define IN2 6
#define EN 4
#define circum 18.5 // in centimeters

byte angle = 90, LED[3] = {14, 15, 16};
String to;
char *number = "+250739082281";
String message = "Imodoka NO RAD125Z YARENGEJE UMUVUDUKO WAGENWE ";
bool Sent = false, error;
volatile int pulseCount = 0;
float speed, rotation;
unsigned long oldTime;

Servo servo;
SoftwareSerial BT(7, 8);
Sim800L SIM800(10, 11);

void setup()
{
  for (byte i = 0; i < sizeof(LED); i++)
    pinMode(LED[i], OUTPUT);
  Serial.begin(9600), SIM800.begin(9600), BT.begin(9600);
  pinMode(EN, OUTPUT), pinMode(IN1, OUTPUT), pinMode(IN2, OUTPUT);
  pinMode(intPin, INPUT), digitalWrite(intPin, HIGH);
  digitalWrite(EN, HIGH), servo.attach(9);
  servo.write(angle), attachInterrupt(INTERRUPT, pulseCounter, FALLING);
}

void loop()
{
  speedoMeter();
  while (BT.available() > 0)
  {
    to += char(BT.read());
  }
  if (to.indexOf('a') != -1)
  {
    Serial.println("DRIVING FORWARD"), forward();
  }
  else if (to.indexOf('e') != -1)
  {
    Serial.println("DRIVING REVERSE"), reverse();
  }
  else if (to.indexOf('c') != -1)
  {
    Serial.println("TOGGLED THE CAR"), switchCar();
  }
  else if (to.indexOf('d') != -1)
  {
    Serial.println("TURN LEFT"), turnLeft();
  }
  else if (to.indexOf('b') != -1)
  {
    Serial.println("TURN RIGHT"), turnRight();
  }
  to = "";
  if (speed > 500)
  {
    for (byte i = 0; i < sizeof(LED); i++)
      digitalWrite(LED[i], HIGH);
    sendSms();
  }
  else
    for (byte i = 0; i < sizeof(LED); i++)
      digitalWrite(LED[i], LOW);
};

void pulseCounter()
{
  pulseCount++;
};

void speedoMeter()
{
  if ((millis() - oldTime) >= 1000)
  {
    detachInterrupt(INTERRUPT);
    rotation = ((1000.0 / (millis() - oldTime)) * pulseCount) / 60; //per sec
    oldTime = millis(), speed = circum * 3600 * rotation / 2000;
    Serial.println(String(speed) + "m/h");
    pulseCount = 0, attachInterrupt(INTERRUPT, pulseCounter, FALLING);
  }
};

void switchCar()
{
  digitalWrite(EN, !digitalRead(EN));
};

void carStop()
{
  digitalWrite(EN, LOW);
};

void forward()
{
  digitalWrite(IN1, HIGH), digitalWrite(IN2, 0);
};

void reverse()
{
  digitalWrite(IN1, LOW), digitalWrite(IN2, HIGH);
};

void turnLeft()
{
  angle = servo.read();
  if (angle < 180)
  {
    angle += 10, servo.write(angle);
  }
};

void turnRight()
{
  angle = servo.read();
  if (angle > 0)
  {
    angle -= 10, servo.write(angle);
  }
};

void sendSms()
{
  SIM800.listen();
  if (!Sent)
  {
    Sent = SIM800.sendSms(number, message + " iragenda kuri " + String(speed) + " meters/hour");
    BT.listen();
  }
};