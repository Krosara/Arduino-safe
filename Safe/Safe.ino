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
const int MIN_LDR_THRESHOLD = 100;
const int MAX_LDR_THRESHOLD = 900;

String passCode = "4312";   //predefined password
String inputCode = "1---";
String finalInputCode = "";
int inputFailed = 0;    //failed attempts at the code
bool isLocked = true;

//button debounce
int currentInputState;
int currentConfirmState;
int lastInputState;
int lastConfirmState;

int valueDigit = 0;     //for entering the code 1-4
int positionDigit = 1;  //position on the display

int buzTime = 5000;   //time for buzzer

//blink led
int ledState = LOW;
unsigned long previousMillis = 0;
const unsigned long interval = 300;
unsigned long startMillis;

//button debounce
bool detectInput = false;
bool detectConfirm = false;

//keep the mode
int mode = 1;
unsigned long StartTime;
unsigned long curr;
unsigned long ElapsedTime;


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

void blinkRedLED()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;                  // save the last time you blinked the LED
    if (ledState == LOW)                             // if the LED is off turn it on and vice-versa:
    {
      ledState = HIGH;
    }
    else
    {
      ledState = LOW;
    }
    // set the LED with the ledState of the variable:
    digitalWrite(LED_RED, ledState);
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
}


void loop()
{
  StartTime = millis();
  Serial.print("StartTime:");
  Serial.println(StartTime);
  int readLDR = analogRead(LDR);
  if (readLDR  <= MAX_LDR_THRESHOLD && readLDR > MIN_LDR_THRESHOLD && inputFailed < 3 && isLocked == true) {
    mode = 1;
  }
  else if (readLDR > MAX_LDR_THRESHOLD) {
    mode = 2;
  }
  else if (inputFailed >= 3) {
    mode = 3;
  }
  else if (isLocked == false) {
    mode = 5;
  }

  ///////////////////////
  //Normal working mode//
  ///////////////////////

  if (mode == 1) {
    if (positionDigit <= 4 && isLocked == true)
    {
      buttonInput();
      //input button function
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
      if (positionDigit > 4) {
        finalInputCode.remove(4, 3);
        if (finalInputCode == passCode)
        {
          if (isLocked == true)
          {
            digitalWrite(LED_GREEN, HIGH);
            mode = 5;
            isLocked = false;
          }
          buttonInput();
        }
        else {
          mode = 4;
        }
      }
    }
  }

  ////////////////////////
  //Burglar working mode//
  ////////////////////////

  if (mode == 2) {
    tone(BUZZER, 500);
    digitalWrite(LED_RED, HIGH);
    delay(5000);
    noTone(BUZZER);
    digitalWrite(LED_RED, LOW);
    valueDigit = 1;
    positionDigit = 1;
    inputCode = "1---";
    finalInputCode = "";
    inputFailed = 0;
  }

  /////////////////
  //3 times wrong//
  /////////////////

  if (mode == 3) {
    //Serial.println("ALARM SAFE 4312");
    curr = millis();
    Serial.print("Curr:");
    Serial.println(curr);
    if (StartTime - (curr - 5000) == 5000) {

      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval)
      {
        previousMillis = currentMillis;                 // save the last time you blinked the LED
        if (ledState == LOW)                            // if the LED is off turn it on and vice-versa:
        {
          ledState = HIGH;
        }
        else
        {
          ledState = LOW;
        }
        // set the LED with the ledState of the variable:
        digitalWrite(LED_RED, ledState);
      }
    }
    else {
      tone(BUZZER, 600, buzTime);
      //reset the code

      valueDigit = 1;
      positionDigit = 1;
      inputCode = "1---";
      finalInputCode = "";
      inputFailed = 0;
    }

  }

  //////////////
  //Wrong code//
  //////////////

  if (mode == 4) {
    digitalWrite(LED_RED, HIGH);
    delay(500);
    digitalWrite(LED_RED, LOW);
    inputFailed++;
    //reset the code
    valueDigit = 1;
    positionDigit = 1;
    inputCode = "1---";
    finalInputCode = "";
  }

  ////////////////
  //Correct code//
  ////////////////

  if (mode == 5) {

    readLDR = analogRead(LDR);
    digitalWrite(LED_GREEN, HIGH);
    if (readLDR < MIN_LDR_THRESHOLD && isLocked == false)
    {
      valueDigit = 1;
      positionDigit = 1;
      inputCode = "1---";
      finalInputCode = "";
      digitalWrite(LED_GREEN, LOW);
      isLocked = true;
      mode = 1;
    }
  }
}
