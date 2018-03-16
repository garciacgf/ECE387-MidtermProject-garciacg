#include <Wire.h>

long xAccelRaw, yAccelRaw, zAccelRaw;
float xAccelNormal, yAccelNormal, zAccelNormal;

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
  buttonState = digitalRead(buttonPin); //Using polling. Unefficient but sufficient for the project
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

  //Taking absolute values because we do not care what direction the acceleration is acting upon
  if(beepMode)
  { 
    if( (abs(xAccelNormal) > .25 && abs(xAccelNormal) <.35) || (abs(yAccelNormal) > .25 && abs(yAccelNormal) < .35) )
    {
      tone(8, 500, 100);
    }
    else if( (abs(xAccelNormal) > .35 && abs(xAccelNormal) <.45) || (abs(yAccelNormal) > .35 && abs(yAccelNormal) < .45) )
    {
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

    analogWrite(redPin, 0); //Makes sure when in this mode that the LED will not turn on
    analogWrite(greenPin, 0);
    analogWrite(bluePin, 0);

  }
  else //Only when the button is pressed
  {
    //Even though gravity acting on an axis would be equal to 1g, the threshold is set at .6g because of factory offsets and because we
    //want the light to turn as gravity begins to noticeably pull on a certain axis and not just when it is fully acting on that axis.
    
    if(abs(xAccelNormal) > .6) //Aqua when gravity acting on x-axis
    {
      analogWrite(redPin, 0);
      analogWrite(greenPin, 255);
      analogWrite(bluePin, 255);
    }
    else if(abs(yAccelNormal) >.6) //Yellow when gravity acting on y-axis
    {
      analogWrite(redPin, 255);
      analogWrite(greenPin, 255);
      analogWrite(bluePin, 0);
    }
    else if(abs(zAccelNormal) > .6) //Purple when gravity acting on z-axis
    {
      analogWrite(redPin, 80);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, 80);
    }
  }  
}

void getAcceleration()
{
  Wire.beginTransmission(0b1101000); //Begin I2C communication
  Wire.write(59); //Begin communication with accelerometer values register
  Wire.endTransmission();
  Wire.requestFrom(0b1101000, 6); // Requesting accelerometer data from registers 59-64
  while(Wire.available() < 6); //Wait until all 6 registers have transmitted the data
  xAccelRaw = Wire.read() <<8 | Wire.read(); //Put x axis accelerometer value back together (16 bits)
  yAccelRaw = Wire.read() <<8 | Wire.read();// Put y accelerometer value back together
  zAccelRaw = Wire.read() <<8 | Wire.read(); //Z accelerometer value
}

void normalizeValues()
{
  xAccelNormal = (xAccelRaw / 16384.0); //Converts it to G's. The divisor depends on the range of the measurements selected (+-2g)
  yAccelNormal = (yAccelRaw / 16384.0);
  zAccelNormal = (zAccelRaw / 16384.0);
}


