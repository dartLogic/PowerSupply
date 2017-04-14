
//lcd initialisation 
#include <LiquidCrystal.h>
const int lcdRows = 4;
const int lcdCols = 20;

//LCD pins= RS = 34, E = 30, D4 = 28, D5 = 26, D6 = 24, D7 =  22
LiquidCrystal lcd(34, 30, 28, 26, 24, 22);


//keypad initialisation
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

int voltage = 0;
int vHundreds;
int vTens;
int vUnits;
int dacValue = 0;
int digitPlace = 2;
#define VoltmeterPlus A6	// Positive output voltage is sampled at this pin 
#define VoltmeterMinus A7	// Negative output voltage is sampled at this pin 
int voltmeterPlusValue = 0;
int voltmeterMinusValue = 0;
int previousmillisLCD = 0;
int currentmillisLCD;
void setup()
{
	Serial.begin(9600);
	const int RWpin = 32;			//RW pin(5) of LCD
	pinMode(RWpin, OUTPUT);			//RW pin is not connected to GND as usual
	digitalWrite(RWpin, LOW);		//make it low instead


	lcd.begin(lcdCols, lcdRows);
	lcd.print("Test");
	delay(1000);
	lcd.clear();
	analogWrite(DAC0, dacValue);
	analogWrite(DAC1, dacValue);	
	delay(500);
}

void loop()
{
	//use due's 12bit resolution for read and write
	analogReadResolution(12);
	analogWriteResolution(12);

	char keypressed = myKeypad.getKey();
	if (keypressed != NO_KEY)
	{
		
		if ((keypressed - '0') >= 0 && (keypressed - '0') <= 9)
		{
			lcd.clear();
			//lcd.print(keypressed);
			switch (digitPlace)
			{
			case 2:
				vHundreds = 100 * (keypressed - '0');
				break;
			case 1:
				vTens = 10 * (keypressed - '0');
				break;
			case 0:
				vUnits = keypressed - '0';
			}
			digitPlace--;
		}
		if (keypressed == 'D')
		{
			dacValue = vHundreds + vTens + vUnits;
			//lcd.println("  ");
			//lcd.print(dacValue);

			analogWrite(DAC0, dacValue);
			analogWrite(DAC1, dacValue);
			digitPlace = 2;
		}
		
		
	}
	currentmillisLCD = millis();

	//Read output voltages at every 300ms
	if (currentmillisLCD - previousmillisLCD > 300)
	{
		int vPlus=0;
		int vMinus = 0;

	//Take 10 samples and average them for a stable value
		for (int i = 0; i < 10; i++)
		{
			vPlus += analogRead(VoltmeterPlus);
			vMinus += analogRead(VoltmeterMinus);
		}
		voltmeterPlusValue = vPlus / 10;
		voltmeterMinusValue = vMinus / 10;

		lcd.clear();
		lcd.setCursor(1, 0);
		lcd.print("V+ = ");
		lcd.print(voltmeterPlusValue);
		lcd.print(" V- = ");
		lcd.print(voltmeterMinusValue);
		previousmillisLCD = currentmillisLCD;
	}

}
