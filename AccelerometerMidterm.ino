#include <Wire.h>

long xAccelRaw, yAccelRaw, zAccelRaw;
float xAccelNormal, yAccelNormal, zAccelNormal;

long xGyroRaw, yGyroRaw, zGyroRaw;
float xGyroNormal,yGyroNormal, zGyroNormal;

int buttonPin = 7;
int buttonState = 0;
int redPin = 11;
int greenPin = 10;
int bluePin = 9;
boolean beepMode = true;

void setup()
{
  pinMode(buttonPin, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  Wire.begin();
  Serial.begin(9600);
  
  //Let's set up the MPU to wake up
  Wire.beginTransmission(0b1101000); //Slave address. LSB 0 because AD0 pulled to ground.
  Wire.write(107); //Access Power Management register because device automatically comes in sleep mode
  Wire.write(0b00000000); //Turn off sleep mode and sets internal oscillator as clock source.
  Wire.endTransmission();

  //Configure sensitivity of Gyroscope
  Wire.beginTransmission(0b1101000); //Slave address
  Wire.write(27); //Gyrscope configuration register
  Wire.write(0b00000000); //Range of +- 250 degrees per second.
  Wire.endTransmission();

  //Configure sensitivity for Accelerometer
  Wire.beginTransmission(0b1101000); //Slave address
  Wire.write(28); //Accelerometer configuration register
  Wire.write(0b00000000); //Range of +-2g
  Wire.endTransmission();
}

void loop()
{
  buttonState = digitalRead(buttonPin);
  getAcceleration();
  getGyroscope();
  normalizeValues();

  if(buttonState == HIGH) //Button released
  {
    
    delay(5);
    beepMode = true;
  }
  else
  {
    delay(5);
    beepMode = false;
  }

  if(beepMode)
  {
      if( (abs(xAccelNormal) > .25 && abs(xAccelNormal) <.35) || (abs(yAccelNormal) > .25 && abs(yAccelNormal) < .35) )
    {
      //Sound low sound
      tone(8, 500, 100);
    }
    else if( (abs(xAccelNormal) > .35 && abs(xAccelNormal) <.45) || (abs(yAccelNormal) > .35 && abs(yAccelNormal) < .45) )
    {
      //Sound medium sound
      tone(8,1000, 100);
    }
    else if( (abs(xAccelNormal) > .45 && abs(xAccelNormal) <.55) || (abs(yAccelNormal) > .45 && abs(yAccelNormal) < .55) )
    {
      tone(8,1500, 100);
    }
    else if( (abs(xAccelNormal) > .55) || (abs(yAccelNormal) > .55) )
    {
      tone(8, 2000, 100);
    }

    analogWrite(redPin, 0);
    analogWrite(greenPin, 0);
    analogWrite(bluePin, 0);

  }
  else
  {
    if(abs(xAccelNormal) > .6) //Aqua
    {
      analogWrite(redPin, 0);
      analogWrite(greenPin, 255);
      analogWrite(bluePin, 255);
    }
    else if(abs(yAccelNormal) >.6) //Yellow
    {
      analogWrite(redPin, 255);
      analogWrite(greenPin, 255);
      analogWrite(bluePin, 0);
    }
    else if(abs(zAccelNormal) > .6) //Purple
    {
      analogWrite(redPin, 80);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, 80);
    }
    
    

  }

  
  
}

void getAcceleration()
{
  Wire.beginTransmission(0b1101000);
  Wire.write(59); //Begin communication with accelerometer values register
  Wire.endTransmission();
  Wire.requestFrom(0b1101000, 6); // Requesting accelerometer data from registers 59-64
  while(Wire.available() < 6); //Wait until all 6 registers have transmitted the data
  xAccelRaw = Wire.read() <<8 | Wire.read(); //Put x axis accelerometer value back together (16 bits)
  yAccelRaw = Wire.read() <<8 | Wire.read();// Put y accelerometer value back together
  zAccelRaw = Wire.read() <<8 | Wire.read(); //Z accelerometer value
}

void getGyroscope()
{
  Wire.beginTransmission(0b1101000);
  Wire.write(67);
  Wire.endTransmission();
  Wire.requestFrom(0b1101000, 6);
  while(Wire.available() < 6); //Wait until all 6 registers' values load
  xGyroRaw = Wire.read() << 8 | Wire.read();
  yGyroRaw = Wire.read() << 8 | Wire.read();
  zGyroRaw = Wire.read() << 8 | Wire.read();
}

void normalizeValues()
{
  xAccelNormal = (xAccelRaw / 16384.0); //Convert it to G's
  yAccelNormal = (yAccelRaw / 16384.0);
  zAccelNormal = (zAccelRaw / 16384.0);

  xGyroNormal = (xGyroRaw / 131.0); //Degrees per second
  yGyroNormal = (yGyroRaw / 131.0);
  zGyroNormal = (zGyroRaw / 131.0);
}


