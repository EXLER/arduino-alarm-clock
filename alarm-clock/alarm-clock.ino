#include <LiquidCrystal_I2C.h>
#include <OneButton.h>
#include <TimeLib.h>

// Initialize the library with interface pins
LiquidCrystal_I2C lcd(0x27, 20, 4);

#define PLUS_PIN 8
#define MINUS_PIN 12
#define SELECT_PIN 11
#define ALARM_PIN 10

#define LED_PIN 9

#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

bool setAlarmMode = false;
bool isAlarm = false;
bool alarmSet = false;

signed int hours = 19;
signed int minutes = 32;
signed int setHours;
signed int setMinutes;
signed int alarmHours;
signed int alarmMinutes;

OneButton plusBtn = OneButton(
                      PLUS_PIN,  // Input pin for the button
                      true,        // Button is active LOW
                      true         // Enable internal pull-up resistor
                    );
OneButton minusBtn = OneButton(
                       MINUS_PIN,  // Input pin for the button
                       true,        // Button is active LOW
                       true         // Enable internal pull-up resistor
                     );
OneButton selectBtn  = OneButton(
                         SELECT_PIN,  // Input pin for the button
                         true,        // Button is active LOW
                         true         // Enable internal pull-up resistor
                       );
OneButton alarmBtn = OneButton(
                       ALARM_PIN,  // Input pin for the button
                       true,        // Button is active LOW
                       true         // Enable internal pull-up resistor
                     );

void setup()
{
  // Opens serial port, sets data rate to 9600 bps
  Serial.begin(9600);
  // (int: columns, int: rows)
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.setCursor(2, 0);
  lcd.print("Alarm  Clock");

  setSyncProvider(requestSync);  // Set function to call when sync required

  plusBtn.attachClick(pressPlusButton);
  minusBtn.attachClick(pressMinusButton);
  selectBtn.attachClick(pressSelectButton);
  alarmBtn.attachClick(pressAlarmButton);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  Serial.print("Start time: ");
  printTimeToSerial(hours, minutes);

  delay(1000);
}

void loop()
{
  lcd.clear();
  while (!setAlarmMode)
  {
    plusBtn.tick();
    minusBtn.tick();
    selectBtn.tick();
    alarmBtn.tick();
    if (Serial.available()) {
      processSyncMessage();
    }
    lcd.setCursor(6, 0);
    printTimeToLCD(hours, minutes);

    if (alarmSet)
    {
      if (hours == alarmHours && minutes == alarmMinutes)
      {
        isAlarm = true;
      }
    }

    if (isAlarm)
    {
      Serial.print("ALARM!\n");
      digitalWrite(LED_PIN, HIGH);
      lcd.setCursor(6, 1);
      lcd.print("Alarm");
    }
    else
    {
      digitalWrite(LED_PIN, LOW);
    }
  }

  lcd.clear();
  setHours = hours;
  setMinutes = minutes;
  while (setAlarmMode)
  {
    plusBtn.tick();
    minusBtn.tick();
    selectBtn.tick();
    alarmBtn.tick();
    printTimeToLCD(setHours, setMinutes);

    lcd.setCursor(4, 1);
    lcd.print("Set Alarm");


  }
}

void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

  if (Serial.find(TIME_HEADER)) {
    pctime = Serial.parseInt();
    if (pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
      setTime(pctime); // Sync Arduino clock to the time received on the serial port
      hours = hour();
      minutes = minute();
    }
  }
}

void pressPlusButton()
{
  if (setAlarmMode) {
    Serial.print("Plus pressed\n");
    setMinutes++;
    if (setMinutes > 59)
    {
      setMinutes = 0;
      setHours++;

      if (setHours > 23)
      {
        setHours = 0;
      }
    }
  }
}

void pressMinusButton()
{
  if (setAlarmMode)
  {
    Serial.print("Minus pressed\n");
    setMinutes--;
    if (setMinutes < 0)
    {
      setMinutes = 59;
      setHours--;

      if (setHours < 0)
      {
        setHours = 23;
      }
    }
  }
}

void pressSelectButton()
{
  if (setAlarmMode)
  {
    Serial.print("Alarm set!\n");
    setAlarmMode = false;
    alarmSet = true;
    alarmHours = setHours;
    alarmMinutes = setMinutes;
  }
  else
  {
    if (!isAlarm) {
      Serial.print("Changed into set alarm mode");
      delay(100); // Hack for weird timing shit
      setAlarmMode = true;
    }
  }
}

void pressAlarmButton()
{
  if (!setAlarmMode) {
    isAlarm = !isAlarm;
    alarmSet = false;

    if (!isAlarm)
    {
      lcd.clear();
      printTimeToLCD(hours, minutes);
    }
  }
}

time_t requestSync()
{
  Serial.write(TIME_REQUEST);
  return 0; // the time will be sent later in response to serial mesg
}

void printTimeToSerial(int hours, int minutes)
{
  Serial.print(hours);
  Serial.print(":");
  Serial.print(minutes);
  Serial.print("\n");
}

void printTimeToLCD(int hours, int minutes)
{
  lcd.setCursor(6, 0);
  if (hours < 10)
  {
    lcd.print("0");
    lcd.print(hours);
  }
  else
  {
    lcd.print(hours);
  }
  lcd.print(":");
  if (minutes < 10)
  {
    lcd.print("0");
    lcd.print(minutes);
  }
  else
  {
    lcd.print(minutes);
  }
}
