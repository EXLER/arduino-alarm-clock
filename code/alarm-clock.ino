#include <LiquidCrystal.h>
// #include <Time.h> for hardware

// Initialize the library with interface pins
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

#define PLUS_PIN 8
#define MINUS_PIN 12
#define SELECT_PIN 11
#define ALARM_PIN 10

#define LED_PIN 9

bool setAlarmMode = false;
bool isAlarm = false;
bool alarmSet = false;

signed int hours = 22;
signed int minutes = 50;
signed int setHours;
signed int setMinutes;
signed int alarmHours;
signed int alarmMinutes;

void setup()
{
    // Opens serial port, sets data rate to 9600 bps
    Serial.begin(9600);
    // (int: columns, int: rows)
    lcd.begin(16, 2);
    lcd.setCursor(2, 0);
    lcd.print("Alarm  Clock");

    pinMode(PLUS_PIN, INPUT);
    digitalWrite(PLUS_PIN, HIGH);
    pinMode(MINUS_PIN, INPUT);
    digitalWrite(MINUS_PIN, HIGH);
    pinMode(SELECT_PIN, INPUT);
    digitalWrite(SELECT_PIN, HIGH);
    pinMode(ALARM_PIN, INPUT);
    digitalWrite(ALARM_PIN, HIGH);

    pinMode(LED_PIN, OUTPUT);

    Serial.print("Start time: ");
    printTimeToSerial(hours, minutes);

    delay(1000);
}

void loop()
{
    lcd.clear();
    while (!setAlarmMode)
    {
        lcd.setCursor(6, 0);
        printTimeToLCD(hours, minutes);

        if (digitalRead(ALARM_PIN) == LOW)
        {
            isAlarm = !isAlarm;

            if (!isAlarm)
            {
                lcd.clear();
                printTimeToLCD(hours, minutes);
            }
        }

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

            if (digitalRead(SELECT_PIN) == LOW)
            {
                Serial.print("Changed into set alarm mode");
                delay(100); // Hack for weird timing shit
                setAlarmMode = true;
                break;
            }
        }
    }

    lcd.clear();
    setHours = hours;
    setMinutes = minutes;
    while (setAlarmMode)
    {
        printTimeToLCD(setHours, setMinutes);

        lcd.setCursor(4, 1);
        lcd.print("Set Alarm");

        if (digitalRead(SELECT_PIN) == LOW)
        {
            Serial.print("Alarm set!\n");
            setAlarmMode = false;
            alarmSet = true;
            alarmHours = setHours;
            alarmMinutes = setMinutes;
            break;
        }

        if (digitalRead(PLUS_PIN) == LOW)
        {
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
        if (digitalRead(MINUS_PIN) == LOW)
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