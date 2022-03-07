#include <pcmRF.h>
#include <pcmConfig.h>
#include <TMRpcm.h>
#include <SPI.h>
#include <SD.h>
#include <Servo.h>
#include <Wire.h>

// led pin contants
int ledG1Pin = 2;
int ledCPin = 3;
int ledDPin = 4;
int ledEPin = 5;
int ledFPin = 6;
int ledG2Pin = 7;
int ledState = LOW;

// millis() constants

unsigned long startMillis;
unsigned long currentMillis;

// i2c constants
String answer = "on";
byte x = 0;

// SD shield constants
int sdPin = 8;
TMRpcm tmrpcm;

// motor constants
class Motor
{
    Servo servo;              // the servo
    int pos;              // current servo position
    int increment;        // increment to move for each interval
    int  updateInterval;      // interval between updates
    unsigned long lastUpdate; // last update of position

  public:
    Motor(int interval)
    {
      updateInterval = interval;
      increment = 1;
    }

    void Attach(int pin)
    {
      servo.attach(pin);
    }

    void Detach()
    {
      servo.detach();
    }

    void Update()
    {
      if ((millis() - lastUpdate) > updateInterval) // time to update
      {
        lastUpdate = millis();
        pos += increment;
        servo.write(pos);
        Serial.println(pos);
        if ((pos >= 160) || (pos <= 0)) // end of sweep
        {
          // reverse direction
          increment = -increment;
        }
      }
    }
};

Motor motor1(5); // head motor
Motor motor2(5); // lArm motor
Motor motor3(5); // rArm motor

void setup() {
  startMillis = millis();
  // Serial.begin(9600);

  // sd and speaker
  tmrpcm.speakerPin = 9;
  if (!SD.begin(sdPin) ) {
    Serial.println( "SD fails" );
    return;
  }
  tmrpcm.setVolume(7);
  // play();

  // leds
  pinMode(ledG1Pin, OUTPUT);
  pinMode(ledCPin, OUTPUT);
  pinMode(ledDPin, OUTPUT);
  pinMode(ledEPin, OUTPUT);
  pinMode(ledFPin, OUTPUT);
  pinMode(ledG2Pin, OUTPUT);

  // i2c
  Wire.begin(9); // set up i2c Bus as slave #9
  Wire.onRequest(requestEvent); // function to run when data is requested
  Wire.onReceive(receiveEvent); // function to run when data is received

  // motors
  motor1.Attach(10);
  motor2.Attach(11);
  motor3.Attach(12);
}

void receiveEvent() { // receive event class
  x = Wire.read();
}

void requestEvent() { // request event class
  byte response[2]; // setup response array

  for (byte i = 0; i < 2; i++) { // format answer as array
    response[i] = (byte)answer.charAt(i);
  }
  // send response back to Master
  Wire.write(response, sizeof(response));
}


void loop() {
  // put your main code here, to run repeatedly:

  if ( x == 0 ) {
    // code for light up all lights
    digitalWrite(ledG1Pin, HIGH);
    digitalWrite(ledCPin, HIGH);
    digitalWrite(ledDPin, HIGH);
    digitalWrite(ledEPin, HIGH);
    digitalWrite(ledFPin, HIGH);
    digitalWrite(ledG2Pin, HIGH);
    }
  if (x == 1) {
    digitalWrite(ledG1Pin, LOW);
    digitalWrite(ledCPin, LOW);
    digitalWrite(ledDPin, LOW);
    digitalWrite(ledEPin, LOW);
    digitalWrite(ledFPin, LOW);
    digitalWrite(ledG2Pin, LOW);
    }
}
