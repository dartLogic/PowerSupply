
//lcd initialisation 
#include <LiquidCrystal.h>
const int lcdRows = 4;
const int lcdCols = 16;

//LCD pins= RS = 34, E = 30, D4 = 28, D5 = 26, D6 = 24, D7 =  22
LiquidCrystal lcd(34, 30, 28, 26, 24, 22);

const int RWpin = 32; //RW pin(5) of LCD make it low in setup function


////////////////////////////////
//keypad initialisation

unsigned currentMillis;
unsigned prevMillis = 0;

#include <Keypad.h>

const byte keypadRows = 4; //number of rows on the keypad
const byte keypadCols = 4; //number of columns on the keypad

//keymap defines the key pressed according to the row and columns just as appears on the keypad
char keymap[keypadRows][keypadCols] =
{
	{ '1', '2', '3', 'A' },
	{ '4', '5', '6', 'B' },
	{ '7', '8', '9', 'C' },
	{ '*', '0', '#', 'D' }
};

//Code that shows the the keypad connections to the arduino terminals
byte rowPins[keypadRows] = { 52, 50, 48, 46 }; //Rows 0 to 3
byte colPins[keypadCols] = { 44, 42, 40, 38 }; //Columns 0 to 3
//initializes an instance of the Keypad class
Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, keypadRows, keypadCols);


void setup()
{
	Serial.begin(9600);

	pinMode(RWpin, OUTPUT);			//RW pin is not connected to GND as usual
	digitalWrite(RWpin, LOW);		//make it low instead


	lcd.begin(lcdCols, lcdRows);
	lcd.print("Test");
	delay(1000);
	lcd.clear();
	
}

void loop()
{
	char keypressed = myKeypad.getKey();
	if (keypressed != NO_KEY)
	{
		lcd.print(keypressed);
	}
}
