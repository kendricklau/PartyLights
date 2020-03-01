/* Kendrick Lau
 */

#include<math.h>

int analogPin = 1; // MSGEQ7 OUT
int strobePin = 2; // MSGEQ7 STROBE
int resetPin = 3; // MSGEQ7 RESET
int prevSpectrumValue[7];
int glideValue[7];
int spectrumValue[7];

int prevArray1[7];
int prevArray2[7];
int prevArray3[7];
// MSGEQ7 OUT pin produces values around 50-80
// when there is no input, so use this value to
// filter out a lot of the chaff.
int filterValue = 70;
 
// LED pins connected to the PWM pins on the Arduino

int ledPinR = 10;
int ledPinG = 9;
int ledPinB = 11;

String space = " ";


void setup()
{
  Serial.begin(9600);
  // Read from MSGEQ7 OUT
  pinMode(analogPin, INPUT);
  // Write to MSGEQ7 STROBE and RESET
  pinMode(strobePin, OUTPUT);
  pinMode(resetPin, OUTPUT);
 
  // Set analogPin's reference voltage
  analogReference(DEFAULT); // 5V
 
  // Set startup values for pins
  digitalWrite(resetPin, LOW);
  digitalWrite(strobePin, HIGH);
}
 
void loop()
{
  // Set reset pin low to enable strobe
  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW);
 
  // Get all 7 spectrum values from the MSGEQ7
  for (int i = 0; i < 7; i++)
  {
    digitalWrite(strobePin, LOW);
    delayMicroseconds(30); // Allow output to settle
 
    spectrumValue[i] = analogRead(analogPin);
    
    spectrumValue[i] = constrain(spectrumValue[i], filterValue, 1023);
    
    // Remap the value to a number between 0 and 255
    spectrumValue[i] = map(spectrumValue[i], filterValue, 1023, 0, 255);
    //filter out static noise
    if(spectrumValue[i] < 60){
      spectrumValue[i] = 10;
    }
    // Remove serial stuff after debugging
    Serial.print(spectrumValue[i]);
    Serial.print(" ");
    digitalWrite(strobePin, HIGH);
   }
   Serial.println();
   averageArray(spectrumValue);
   writeGlide(spectrumValue);
}

void averageArray(int (& myArray) [7])
{
  memcpy(prevArray1, prevArray2, sizeof(prevArray1));
  memcpy(prevArray2, prevArray3, sizeof(prevArray2));
  memcpy(prevArray3, myArray, sizeof(prevArray3));
  for (int i = 0; i < 7; i++)
  {
    myArray[i] = (prevArray1[i] + prevArray2[i] + prevArray3[i])/3;
  }
}

void writeGlide(int (& myArray) [7])
{
  memcpy(prevSpectrumValue, myArray, sizeof(prevSpectrumValue));
  
  for (int j = 0; j < 125; j++)
  {
    for (int i = 0; i < 7; i++)
    {
      glideValue[i] = (myArray[i] - prevSpectrumValue[i])/100;
      myArray[i] = myArray[i] + glideValue[i];
    }
    
    analogWrite(ledPinB, (myArray[1] + myArray[2])/2); 
    analogWrite(ledPinR, (myArray[4] + myArray[3])/2); 
    analogWrite(ledPinG, (myArray[6] + myArray[5])/2); 
  }
}

//void writeGlide2(int (& myArray) [7])
//{
//  memcpy(prevSpectrumValue, myArray, sizeof(prevSpectrumValue));
//  for (int i = 0; i < 7; i++)
//  {
//    glideValue[i] = myArray[i] - prevSpectrumValue[i];
//  }
//  for (int j = 0; j < 100; j++)
//  {
//    for (int i = 0; i < 7; i++)
//    {
//      glideValue[i] = (myArray[i] - prevSpectrumValue[i])/100;
//      myArray[i] = myArray[i] + 1;
//    }
//    analogWrite(ledPinR, (myArray[1] + myArray[2])/2); 
//    analogWrite(ledPinG, (myArray[4] + myArray[3])/2); 
//    analogWrite(ledPinB, (myArray[6] + myArray[5])/2); 
//  }
//}
