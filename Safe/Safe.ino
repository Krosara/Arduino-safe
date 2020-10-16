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
const int MIN_LDR_THRESHOLD = 150;
const int MAX_LDR_THRESHOLD = 900;

String passCode = "4312";   //predefined password
String inputCode = "1---";
String finalInputCode = "";
int inputFailed = 0;    //failed attempts at the code

int currentInputState;
int currentConfirmState;
int lastInputState;
int lastConfirmState;


int valueDigit = 0;     //for entering the code 1-4
int positionDigit = 1;  //position on the display

bool alreadyExecuted = false;
bool detectInput = false;     //checks if all conditions have been met
bool detectConfirm = false;   //checks if all conditions have been met

int buttonInput()   //left (input) button press with button debounce
{
  lastInputState = currentInputState;
  currentInputState = digitalRead(LEFT_BUTTON);
  if (currentInputState == LOW && lastInputState == HIGH)
  {
    delay(10);
    if (currentInputState == LOW)
    {
      detectInput = true;
    }
  }
}

int buttonConfirm()   //right (confirm) button press with button debounce
{
  lastConfirmState = currentConfirmState;
  delay(10);
  currentConfirmState = digitalRead(RIGHT_BUTTON);
  if (currentConfirmState == LOW && lastConfirmState == HIGH)
  {
    if (currentConfirmState == LOW)
    {
      detectConfirm = true;
    }
  }
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
  //Display.clear();
}


void loop()
{
  int readLDR = analogRead(LDR);

  if (readLDR <= MAX_LDR_THRESHOLD)
  {
    if (inputFailed < 3)
    {
      if (positionDigit <= 4)
      {
        buttonInput();
        if (detectInput)
        {
          valueDigit = valueDigit % 4 + 1;    //cycle numbers 1-4 on display
          inputCode = String(finalInputCode) + String(valueDigit) + "---";
          detectInput = false;
        }
        Display.show(inputCode);
        buttonConfirm();
        if (detectConfirm)
        {
          finalInputCode += valueDigit;
          positionDigit++;
          valueDigit = 1;
          inputCode = String(finalInputCode) + "1---";
          detectConfirm = false;
        }
      }
      else
      {
        finalInputCode.remove(4, 3);
        if (finalInputCode == passCode)
        {
          if (!alreadyExecuted)
          {
            digitalWrite(LED_GREEN, HIGH);
            delay(500);
            digitalWrite(LED_GREEN, LOW);
          }
          else
          {
            valueDigit = 1;
            positionDigit = 1;
            inputCode = "1---";
            finalInputCode = "";
            digitalWrite(LED_GREEN, HIGH);
            delay(500);
            digitalWrite(LED_GREEN, LOW);
          }
          buttonInput();
        }
        else
        {
          inputFailed++;
          digitalWrite(LED_RED, HIGH);
          delay(500);
          digitalWrite(LED_RED, LOW);
          //reset the code
          valueDigit = 1;
          positionDigit = 1;
          inputCode = "1---";
          finalInputCode = "";
        }
        if (LDR >= MIN_LDR_THRESHOLD)
        {
          valueDigit = 1;
          positionDigit = 1;
          inputCode = "1---";
          finalInputCode = "";
          digitalWrite(LED_GREEN, LOW);
        }
      }
    }
    else
    {
      digitalWrite(LED_RED, HIGH);
      delay(2500);
      digitalWrite(LED_RED, LOW);
      //reset the code
      valueDigit = 1;
      positionDigit = 1;
      inputCode = "1---";
      finalInputCode = "";
    }
  }
  else
  {
    tone(BUZZER, 500);
    digitalWrite(LED_RED, HIGH);
    delay(5000);
    noTone(BUZZER);
    digitalWrite(LED_RED, LOW);
    valueDigit = 1;
    positionDigit = 1;
    inputCode = "1---";
    finalInputCode = "";
  }
}
