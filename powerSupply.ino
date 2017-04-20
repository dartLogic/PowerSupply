//lcd initialisation 
#include <LiquidCrystal.h>
const int lcdRows = 4;
const int lcdCols = 20;
//LCD pins= RS = 34, E = 30, D4 = 28, D5 = 26, D6 = 24, D7 =  22
LiquidCrystal lcd(34, 30, 28, 26, 24, 22);

int previousmillisLCD = 0;
int currentmillisLCD;

volatile int encoderValue = 0;
int plusVoltage = 0;
int minusVoltage = 0;
const int coarseStep = 50;
const int fineStep = 5;

const int encoderCoarsePinA = 15;
const int encoderCoarsePinB = 14;
const int encoderFinePinA = 17;
const int encoderFinePinB = 18;

#define VoltmeterPlus A6	// Positive output voltage is sampled at this pin 
#define VoltmeterMinus A7	// Negative output voltage is sampled at this pin 
int voltmeterPlusValue = 0;
int voltmeterMinusValue = 0;


#define CONVERSION_FACTOR_VPLUS 157.0

//divide the analog read value of  VoltmeterPlus by this factor obtained by series divider of 15k ohm and 2.2k ohm
//2.2/(15+2.2)*(4096/(3.3))


void setup()
{
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
}

void loop()
{
	//use due's 12bit resolution for read and write
	analogReadResolution(12);
	analogWriteResolution(12);
	minusVoltage = encoderValue;
	plusVoltage = (encoderValue)*18.33 / 20.0;  //to alingn same voltage per channel 
	analogWrite(DAC0, plusVoltage);
	analogWrite(DAC1, minusVoltage + 10);

	currentmillisLCD = millis();
	if (currentmillisLCD - previousmillisLCD > 300)
	{


		double vPlus = 0;
		double vMinus = 0;

		//Take 10 samples and average them for a stable value
		for (int i = 0; i < 10; i++)
		{
			vPlus += analogRead(VoltmeterPlus);
			vMinus += analogRead(VoltmeterMinus);
		}
		voltmeterPlusValue = vPlus / 10;
		voltmeterMinusValue = vMinus / 10;

		lcd.clear();
		lcd.print(" V+ =  ");
		lcd.print(voltmeterPlusValue / CONVERSION_FACTOR_VPLUS);

		lcd.setCursor(0, 1);
		lcd.print(" V- = ");

		lcd.print(((voltmeterMinusValue*3.3 / 4096) - 3.2986) / 0.14037);
		lcd.setCursor(0, 2);
		lcd.print(voltmeterMinusValue);
		previousmillisLCD = currentmillisLCD;
	}

}

void doEncoderCoarse()
{
	if (digitalRead(encoderCoarsePinA) == HIGH)
	{
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
	if (digitalRead(encoderFinePinA) == HIGH)
	{
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