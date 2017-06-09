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



//lcd initialisation 
#include <LiquidCrystal.h>
const int lcdRows = 4;
const int lcdCols = 20;
//LCD pins= RS = 34, E = 30, D4 = 28, D5 = 26, D6 = 24, D7 =  22
LiquidCrystal lcd(34, 30, 28, 26, 24, 22);

int previousmillisLCD = 0; //variables used to control refresh rate of lcd and Voltrage sampling rate
int currentmillisLCD;


int plusVoltage = 0;		//digital values to be writtedn to DAC0
int minusVoltage = 0;		//digital values to be writtedn to DAC1

volatile int encoderValue = 0;		
const int coarseStep = 50;		//Adjustment step of coarse encoder
const int fineStep = 10;		//Adjustment step of fine encoder

const int encoderCoarsePinA = 15;
const int encoderCoarsePinB = 14;
const int encoderFinePinA = 17;
const int encoderFinePinB = 18;

#define VoltmeterPlus A6	// Positive output voltage is sampled at this pin 
#define VoltmeterMinus A7	// Negative output voltage is sampled at this pin 
short sampleNumbers = 32;	// take average of this number of samples of analog reading to make stable reading.
int voltmeterPlusValue = 0;  //+output voltage to be displayed on lcd (in decimal)
int voltmeterMinusValue = 0; //-output voltage to be displayed on lcd (in decimal)	


#define CONVERSION_FACTOR_VPLUS 157.0

//divide the analog read value of  VoltmeterPlus by this factor obtained by series divider of 15k ohm and 2.2k ohm
//2.2/(15+2.2)*(4096/(3.3))

//the power suppply work in four modes mode A = dual tracking
//											 B = positive only adjust
//											 C = Negative only adjust
//											 D = Outputs are zero
volatile char mode = 'A';			//default is dual tracking

void setup()
{
	//use due's 12bit resolution for read and write
	analogReadResolution(12);
	analogWriteResolution(12);

	const int RWpin = 32;			//RW pin(5) of LCD
	pinMode(RWpin, OUTPUT);			//RW pin is not connected to GND as usual
	digitalWrite(RWpin, LOW);		//make it low instead
	lcd.begin(lcdCols, lcdRows);

	pinMode(encoderCoarsePinA, INPUT);
	pinMode(encoderCoarsePinB, INPUT);
	attachInterrupt(encoderCoarsePinA, doEncoderCoarse, CHANGE);

	pinMode(encoderFinePinA, INPUT);
	pinMode(encoderFinePinB, INPUT);
	attachInterrupt(encoderFinePinA, doEncoderFine, CHANGE);


	analogWrite(DAC0, 0);	//set output voltage to zero at begining
	analogWrite(DAC1, 0);
	delay(100);
}

void loop()
{
	

	//the power suppply work in four modes mode A = dual tracking
	//											 B = positive only adjust
	//											 C = Negative only adjust
	//											 D = Outputs are zero
	
	char keypressed = myKeypad.getKey();	
	
	if (keypressed != NO_KEY)
	{
		//Look only for mode keys ie A, B, C or D
		if ((keypressed == 'A') || (keypressed == 'B') || (keypressed == 'C') || (keypressed == 'D'))
		{
			mode = keypressed;
		}
				
	}
	
	minusVoltage = encoderValue;		
	plusVoltage = (encoderValue)*18.33 / 20.0;  //to alingn same voltage per channel

	switch (mode)
	{
	case 'A':
		
		analogWrite(DAC0, plusVoltage);
		analogWrite(DAC1, minusVoltage + 10);	//some trimming to equalize voltages
		break;

	case 'B':
		
		analogWrite(DAC0, plusVoltage);
		break;

	case 'C':
		
		analogWrite(DAC1, minusVoltage + 10);	//some trimming to equalize voltages
		break;

	case 'D':							
		encoderValue = 0;					//All outputs are set to zero
		analogWrite(DAC0, 0);
		analogWrite(DAC1, 0);
		break;
	}

	currentmillisLCD = millis();
	if (currentmillisLCD - previousmillisLCD > 300)
	{


		double vPlus = 0;
		double vMinus = 0;

		//Take samples and average them for a stable value
		for (int i = 0; i < sampleNumbers; i++)
		{
			vPlus += analogRead(VoltmeterPlus);
			vMinus += analogRead(VoltmeterMinus);
		}
		voltmeterPlusValue = vPlus / sampleNumbers;
		voltmeterMinusValue = vMinus / sampleNumbers;

		lcd.clear();
		lcd.print(" V+ =  ");
		lcd.print(voltmeterPlusValue / CONVERSION_FACTOR_VPLUS);

		lcd.setCursor(0, 1);
		lcd.print(" V- = ");

		lcd.print(((voltmeterMinusValue*3.3 / 4096) - 3.2986) / 0.14037);
		lcd.setCursor(1, 3);
		if (mode == 'A')
		{
			lcd.print("DUAL");
		}
		else if (mode == 'B')
		{
			lcd.print("+AJD");
		}
		else if (mode == 'C')
		{
			lcd.print("-ADJ");
		}
		else if (mode == 'D')
		{
			lcd.print("ZERO");
		}

		previousmillisLCD = currentmillisLCD;
	}

}

void doEncoderCoarse()
{
	delayMicroseconds(300);			//Tweaked the delay to reduce jitter on coarse encoder
	if (digitalRead(encoderCoarsePinA) == HIGH)
	{
		delayMicroseconds(300);		//Tweaked the delay to reduce jitter on coarse encoder
		if (digitalRead(encoderCoarsePinB) == LOW)
		{
			if (encoderValue <= (4095 - coarseStep))
			{
				encoderValue = encoderValue + coarseStep;
			}

		}
		else
		{
			if (encoderValue >= (0 + coarseStep))
			{
				encoderValue = encoderValue - coarseStep;
			}

		}
	}
	else
	{
		delayMicroseconds(300);			//Tweaked the delay to reduce jitter on coarse encoder
		if (digitalRead(encoderCoarsePinB) == HIGH)
		{
			if (encoderValue <= (4095 - coarseStep))
			{
				encoderValue = encoderValue + coarseStep;
			}
		}
		else
		{
			if (encoderValue >= (0 + coarseStep))
			{
				encoderValue = encoderValue - coarseStep;
			}
		}
	}
}

void doEncoderFine()
{
	delayMicroseconds(100);
	if (digitalRead(encoderFinePinA) == HIGH)
	{
		delayMicroseconds(100);
		if (digitalRead(encoderFinePinB) == LOW)
		{
			if (encoderValue <= (4095 - fineStep))
			{
				encoderValue = encoderValue + fineStep;
			}
		}
		else
		{
			if (encoderValue >= (0 + fineStep))
			{
				encoderValue = encoderValue - fineStep;
			}
		}
	}
	else
	{
		delayMicroseconds(100);
		if (digitalRead(encoderFinePinB) == HIGH)
		{
			if (encoderValue <= (4095 - fineStep))
			{
				encoderValue = encoderValue + fineStep;
			}
		}
		else
		{
			if (encoderValue >= (0 + fineStep))
			{
				encoderValue = encoderValue - fineStep;
			}
		}
	}
}