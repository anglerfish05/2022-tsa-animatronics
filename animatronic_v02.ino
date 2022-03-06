/*

*/
#include <Wire.h>
#include <Servo.h>
#include <SD.h>
#include <SPI.h>
#include <pcmRF.h>
#include <pcmConfig.h>
#include <TMRpcm.h>


// pir sensor constants
int pirPin = 2;
int val = 0;
bool motionState = false; // start with no motion detected

// SD shield constants
int sdPin = 3;
TMRpcm tmrpcm;

// music loop constants
unsigned long startMillis;
unsigned long currentMillis;

// trunk motor
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
        if ((pos >= 100) || (pos <= 0)) // end of sweep
        {
          // reverse direction
          increment = -increment;
        }
      }
    }
};

Motor motor1(5); // increase # slows down, decrease faster

void setup() {
  startMillis = millis(); // initial start time
  //Serial.begin(9600); // sets up serial monitor

  // trunk motor
  motor1.Attach(4);

  // SD and speaker
  tmrpcm.speakerPin = 11;
  if (!SD.begin(sdPin) ) {
    Serial.println( "SD fails" );
    return;
  }
  tmrpcm.setVolume(7); // volume 0-7
  // play();

  // pir
  pinMode(pirPin, INPUT); // declare pir as input

  // i2c
  Wire.begin(); // start I2C Bus as Master

}

// void play() { // play class
// tmrpcm.stopPlayback();

// tmrpcm.play( "1.wav" ); // replace with file name
// delay(240000); // the time from where the song has to be played


void loop() {
  // pir code
  val = digitalRead(pirPin);

  if (val == HIGH) { // if motion is detected:
    boolean system1 = true;
    boolean system2 = false;
    boolean system3 = false;
    // enter function when motion is detected here

    // SYSTEM 1 - INTRO
    while (system1) {
      // send data to slave i2c 9 (piano light on)
      Wire.beginTransmission(9); // transmit to slave i2c #9
      Wire.write(0); // data (0) for light up all lights
      Wire.endTransmission();
      // request data back
      Wire.requestFrom(9, 2); // request from slave i2c #9, answersize 2

      // add characters to string
      String response = " ";
      while (Wire.available()) {
        char x = Wire.read();
        response += x;
      }
      if ( response == "on" or response == "ON" or response == "On" ) {
        // file plays intro
        tmrpcm.play( "1.wav" ); // replace with file name
        motor1.Update();
        while (true) {
          currentMillis = millis(); // get current time
          if ( currentMillis - startMillis >= 1000 ) { // change to sound duration
            tmrpcm.disable(); // disable after set time millis
            break;
            startMillis = currentMillis;
          }
        }
        system2 = true;
        break;
      }

    }
    // SYSTEM 2 - SONG
    while (system2) {
      // send data to slave i2c 9 (piano led and song)
      Wire.beginTransmission(9); // transmit to slave i2c #9
      Wire.write(1); // data (1) led and song
      Wire.endTransmission();
      // need delay for music duration and trunk movement
      while (true) {
        motor1.Update();
        while (true) {
          currentMillis = millis(); // get current time
          if ( currentMillis - startMillis >= 1000 ) { // change to music duration
            break;
          }
        }
        break;
      }
      // request data back
      Wire.requestFrom(9, 2); // request from slave i2c #9, answersize 2
      // add characters to string
      String response = " ";
      while (Wire.available()) {
        char x = Wire.read();
        response += x;
      }

      if ( response == "on" or response == "ON" or response == "On" ) {
        // file plays (after music is over)
        tmrpcm.play( "1.wav" ); // replace with file name
        motor1.Update();
        while (true) {
          currentMillis = millis(); // get current time
          if ( currentMillis - startMillis >= 1000 ) { // change to sound duration
            tmrpcm.disable(); // disable after set time millis
            break;
            startMillis = currentMillis;
          }
        }
        break;
      }
    }

  }


  if (motionState == false) {
    Serial.println( "Motion is detected" );
    motionState = true;
  }

  else {
    // enter function when motion is not detected here
    if (motionState == true) {
      Serial.println( "Mation ended" );
      motionState = false;
    }
  }
}
