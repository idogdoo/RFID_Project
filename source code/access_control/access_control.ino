/*
    Projektarbeit
    Hochschule Ostwestfalen-Lippe
    Fachbereich 5 - Eletrotechnik und Technische Informatik
    Projekt: RFID-Zugangskontrolle
    Autor: Cihan Aydogdu
    Version: 1.0.3
    Information: Due to GitHub and YouTube publication
                 I added comments in English.
*/

// Including required libraries
#include <SPI.h> // SPI library
#include <MFRC522.h> // RFID library
#include <LiquidCrystal.h> // LCD library
#include <Time.h> // Time library

// Defining the time and date
/* Before compiling the sketch and write the program on
the Arduino Mega chip you have to set the time and date.
It is stored as long as the Arduino has power supply.
Of course you can also attach a battery. You can decide
for yourself. */
#define HR 13
#define MN 12
#define SC 0
#define DY 14
#define MT 6
#define YR 16

// Defining the RFID pins
#define RFID_SDA_PIN 53
#define RFID_RST_PIN 7

// Defining the LCD pins
#define LCD_DATA7 2
#define LCD_DATA6 3
#define LCD_DATA5 4
#define LCD_DATA4 5
#define LCD_ENABLE 11
#define LCD_REGISTER_SELECT 12

// Definig the relay pin
#define RELAY_IN 38

// Array list with valid and invalid IDs and dummy IDs
/*
Valid ID:		1509570 (This ID in the array below)
Invalid ID:	1504830 (No access allowed)
Dummy ID1:		0000000 (Only for demonstration purposes)
Dummy ID2:		1111111 (                "              )
Dummy ID3:		2222222 (                "              )
*/
unsigned long int id_list[] = { 1509570, 0000000, 1111111, 2222222 };

// Naming the RFID receiver and specify pin
MFRC522 mfrc522(RFID_SDA_PIN, RFID_RST_PIN);

// Naming the LCD and specify pin
LiquidCrystal lcd(LCD_REGISTER_SELECT, LCD_ENABLE, LCD_DATA4, LCD_DATA5, LCD_DATA6, LCD_DATA7);

// Setup function (Initialization of the components)
void setup()
{
  Serial.begin(9600); // Starting the serial connection (monitor)
  SPI.begin(); // Starting the SPI connection
  mfrc522.PCD_Init(); // Initialization of the RFID receiver
  pinMode(RELAY_IN, OUTPUT); // Initialization of the relay
  setTime(HR,MN,SC,DY,MT,YR); // Time and date are set at program start
  delay(500); // Short wait time to solve the "flickering" problem on the LCD display
  lcd.begin(16, 2);  // Initialization of the LCD
}

// Query function (is ID in array list or not)
bool is_code_in_array_list(long int candidate){
    int arraysize = sizeof(id_list) / sizeof(long int);
    int i;
    for (i=0; i < arraysize; i++)
    {
      if (id_list[i] == candidate)
      {
        return true;
      }
    }
    return false;
}

// loop function (main part which is repeating during the operation)
void loop()
{
  // At this point use NO lcd.clear() otherwise the LCD flackers
  lcd.setCursor(0, 0);
  lcd.print("Bitte ID-Karte"); // e.g. you can change it to: Please show your ID.
  lcd.setCursor(0, 1);
  lcd.print("vorhalten...");
  
  if ( ! mfrc522.PICC_IsNewCardPresent()) // When a card is in range...
  {
    return; // continue to next if...
  }
  
  if ( ! mfrc522.PICC_ReadCardSerial()) // When a RFID transmitter has been selected...
  {
    return;
  }

  // The ID is stored in this variable later
  unsigned long int code = 0;

  // All 4 blocks will be readouted and summarized
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    code = ((code+mfrc522.uid.uidByte[i])*10);
  }

  // Output on serial monitor
  Serial.print("Die ID des RFID-Tags lautet:  ");
  // UID of the RFID tag will be readouted
  // The output can be in decimal, hexadecimal or binary
  for (byte j = 0; j < mfrc522.uid.size; j++)
  {
    Serial.print(mfrc522.uid.uidByte[j], DEC); // decimal format
    //Serial.print(mfrc522.uid.uidByte[j], HEX); // hexadecimal format
    //Serial.print(mfrc522.uid.uidByte[j], BIN); // binary format
    Serial.print(" ");
  }
  Serial.println();
  Serial.print("Berechnete Kennung:           ");
  Serial.print(code);
  Serial.println();

  // Is ID in array list
  if (is_code_in_array_list(code))
  {
    // Output on serial monitor (e.g. for record/protocol in a company)
    Serial.println("Autorisierung:                JA");
    digitalClockDisplay();
    Serial.println();

    // Output on LCD (e.g. for employees)
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Zutritt gew\xE1hrt");
    lcd.setCursor(0, 1);
    lcd.print("Ihre ID: ");
    lcd.print(code);

    // Unlock the door and lock it after 5 seconds
    digitalWrite(RELAY_IN, HIGH);
    delay(5000); // wait 5 seconds
    digitalWrite(RELAY_IN, LOW);

    // 1,5 Sekunden warten, damit LCD nicht "flackert" und dann LCD-Anzeige loeschen
    // Wait for 1,5 seconds to prevent a LCD flackering and then clear the LCD display
    delay(1500);
    lcd.clear();
    
  }
  else
  {
    // Output on serial monitor
    Serial.println("Autorisierung:                NEIN");
    digitalClockDisplay();
    Serial.println();

    // Output on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Kein Zutritt!");
    lcd.setCursor(0, 1);
    lcd.print("Ihre ID: ");
    lcd.print(code);

    delay (5000); // wait 5 seconds
    lcd.clear();
  }  
}

// Function for date and time
void digitalClockDisplay(){
  Serial.print("Zeitpunkt:                    ");
  Serial.print(hour());
  Serial.print(":");
  printDigits(minute());
  Serial.print(":");
  printDigits(second());
  Serial.print("  ");
  printDigits(day());
  Serial.print(".");
  printDigits(month());
  Serial.print(".");
  Serial.print(year());
  Serial.println();
}

// Function to write a zero before the single digits
// e.g. 10:2:9 1.3.2016 --> 10:02:09 01.03.2016
void printDigits(int digits){
  if(digits < 10)
  {
    Serial.print('0');
  }
  Serial.print(digits);
}

