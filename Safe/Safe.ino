/*
   Kristian Savov
   Student no.: 4170024
   P-CB-06
*/

#include <DHT11.h>
#include <Display.h>
#include <TM1637Display.h>

//map pins to variables
const int LEFT_BUTTON = 9;  //input button
const int RIGHT_BUTTON = 8; //confirm buttonA
const int LDR = A2;
const int LED_RED = 4;
const int LED_GREEN = 5;
const int BUZZER = 3;
const int MIN_LDR_VALUE = 0;
const int MAX_LDR_VALUE = 1023;
const int MIN_LDR_THRESHOLD = 250;
const int MAX_LDR_THRESHOLD = 900;
bool isUnlocked = false;


//button debounce
const int BUTTON_BOUNCE = 50;     //declare delay from beginning
int lastButtonStateLeft = HIGH;
int lastButtonStateRight = HIGH;
int buttonStateLeft = HIGH;
int buttonStateRight = HIGH;
unsigned long lastDelay = 0;

int codeNumbers[] = {1, 2, 3, 4};   //available numbers at each position
int posDisplay = 0;   //digit postition on display

int password[] = {4, 2, 1, 3};    //predefined password
int inputPass[] = {};   //password input by user
int indexPass = 0;




//used for button debounce
int read_button()
{
  int button = 0;
  int buttonLeft = digitalRead(LEFT_BUTTON);
  int buttonRight = digitalRead(RIGHT_BUTTON);

  if (buttonLeft != lastButtonStateLeft)
  {
    lastDelay = millis();
  }
  if (buttonRight != lastButtonStateRight)
  {
    lastDelay = millis();
  }

  if ((millis() - lastDelay) > BUTTON_BOUNCE)
  {

    if (buttonStateLeft != buttonLeft)
    {
      buttonStateLeft = buttonLeft;
      if (lastButtonStateLeft == LOW)
        button = 1;   //left button
    }
    if (buttonStateRight != buttonRight)
    {
      buttonStateRight = buttonRight;
      if (lastButtonStateRight == LOW)
        button = 2;   //right button
    }
  }
  lastButtonStateLeft = buttonLeft;
  lastButtonStateRight = buttonRight;
  return button;
}

void setup()
{
  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(LDR, INPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  Serial.begin(9600);
  Display.clear();
  Display.show("----");
}

//button == 1 - left button
//button == 2 - right button

  int alarm() 
  {
    tone(BUZZER, 450);
    delay(20);
    tone(BUZZER, 900);
    delay(20);
  }
  
void loop()
{
  int readLDR = analogRead(LDR);
  int button = read_button();
  //Display.show("----");

//trigger alarm when LDR value changes above threshhold
if (readLDR <= MAX_LDR_THRESHOLD)
{
  //iterate through display digits in order to input password
  if (button == 1 && posDisplay == 0) //first digit
  {
    Display.show(codeNumbers[indexPass] * 1000);
    inputPass[posDisplay] = codeNumbers[indexPass];
    indexPass++;
    indexPass %= 4;
    Serial.println(codeNumbers[indexPass]);
  }
  else if (button == 2 && posDisplay == 0)
  {
    indexPass = 0;
    posDisplay++;
    Serial.println(inputPass[posDisplay]);
  }
  else if (button == 1 && posDisplay == 1) //second digit
  {
    Display.show(codeNumbers[indexPass] * 100);
    inputPass[posDisplay] = codeNumbers[indexPass];
    indexPass++;
    indexPass %= 4;
    //Serial.println(codeNumbers[indexPass]);
  }
  else if (button == 2 && posDisplay == 1)
  {
    indexPass = 0;
    posDisplay++;
  }
  else if (button == 1 && posDisplay == 2) //third digit
  {
    Display.show(codeNumbers[indexPass] * 10);
    inputPass[posDisplay] = codeNumbers[indexPass];
    indexPass++;
    indexPass %= 4;
    Serial.println(codeNumbers[indexPass]);
  }
  else if (button == 2 && posDisplay == 2)
  {
    indexPass = 0;
    posDisplay++;
    Serial.println(inputPass[posDisplay]);
  }

  else if (button == 1 && posDisplay == 3) //fourth digit
  {
    Display.show(codeNumbers[indexPass]);
    inputPass[posDisplay] = codeNumbers[indexPass];
    indexPass++;
    indexPass %= 4;
    Serial.println(codeNumbers[indexPass]);
  }
  else if (button == 2 && posDisplay == 3)
  {
    indexPass = 0;
    posDisplay++;
    Serial.println(inputPass[posDisplay]);
  }
}
else 
{
  Display.show("----");
  digitalWrite(LED_RED, HIGH);
  alarm();
  delay(5000);
  digitalWrite(LED_RED, LOW);
  noTone(BUZZER);
}

  //  if button-left = pressed:
  //    Display.show(codeNumbers[index] * 1000)
  //    input[pos] = codeNumbers[index];
  //    index += 1
  //    index %= 4
  //  if button-right = pressed
  //    index = 0
  //    pos += 1

  //  if ( button == 1)
  //  {
  //    Display.showCharAt(pos, '0' + codePos0);
  //    pos++;
  //    codePos0++;
  //    if ( pos > 3)
  //    {
  //      pos = 0;
  //    }
  //  }
  //  else if ( button == 2)
  //  {
  //    digitalWrite(LED_GREEN, LOW);
  //    digitalWrite(LED_RED, HIGH);
  //    delay(5000);
  //    digitalWrite(LED_RED, LOW);
  //  }
  //  //Serial.println(readLDR);
}
