  //libraries
#include <Adafruit_NeoPixel.h>


	//software definitions
#define lednum 8			// number of leds to be set in chain
#define hddweight 5			//persentage weight for hdd activity indication

	//hardware definitions
#define ledpin 3			// serial line to addressable leds
#define resetbutton 4			// connected to reset button on front panel
#define powerbutton 5
#define sidepanel 8			// switch detecting presence of side panel on chassis
#define hddled 9			// connected to hdd activity indicator on front panel (pwm required)
#define resetpin 10			// mb reset line
#define powerpin 11

	//process definitions
#define mode_usr_a 0		//	common mode serial set a
#define mode_usr_b 1		//	common mode serial set b
#define mode_usr_c 2		//	common mode serial set c
#define mode_usr_d 3		//	common mode serial set d
#define mode_slow_shift 4	//	shifts colours from led to led
#define mode_glow 5			//	rotates hue space
#define mode_glow_mod 6		//	rotates hue space and modulates brightness
#define mode_contrast 7		//	some leds off, some on
#define mode_flood 8		//	all leds on
#define mode_nolights 9		//	all leds off
#define mode_usr_def 10		//	common mode serial set / button set
#define mode_count 10		//			NOT A MODE! number of regular togglable modes ^
#define mode_power_on 11	//	fade into mode_usr_a
#define mode_power_off 13	//	fade from current mode to 0
#define mode_hdd_active 14	//	fade to green based on drive activity


  //globals
Adafruit_NeoPixel cases= Adafruit_NeoPixel(lednum, ledpin, NEO_GRB + NEO_KHZ800);
bool caseopen = false;    // is case open?
byte leds[lednum][4];     // array [module] [r,g,b, lum]
uint16_t presstime =0;    // time button was last pressed
uint16_t settime[4] = {0,0,0,0};      // time [r,g,b, lum] was last set
byte lastset;
byte mode;
bool pressresolve = false;



void setup()
{
	digitalWrite(resetpin,HIGH);

	Serial.begin(19200);
	Serial.print("Select a profile to edit [a-e]");
}



void reset()	//pull reset line low/high to reset mb
{
	digitalWrite(resetpin, LOW);
	delay(2);
	digitalWrite(resetpin, HIGH);
}


void advancemode()
{
	if(mode <= mode_count)
	{
		mode++;
	}
	else mode =0;
}


void setlevel()	//set leds to current colour after button released
{

}


void buttonrls()                // 0-50   mode switch
{                               // 50-6600 level set
  if((millis()-presstime) < 50 && presstime != 0) // >6600 reset mb
  {
    advancemode();
  }
  else if((millis()-presstime) < 6600 && presstime != 0)
  {
    setlevel();
  }
  else reset();
  pressresolve = false;
}

void buttonprs()
{
	pressresolve = true;

	if (digitalRead(resetbutton))
	{

	}
	else if (presstime < millis() && presstime != 0)
	{
		buttonrls();
		presstime = 0;
	}
}


void setleds()	//to figure out & set state of leds
{
  if(digitalRead(resetbutton))
  {
    presstime = millis();
    buttonprs();
  }
  uint16_t set[3];
  for(byte i=0; i < lednum; i++)
  {
    for(byte j=0; j<3; j++)
    {
      set[j] = leds[i][j]*leds[i][4];   //led = (brightness*colour) = (lum*col)/255
      set[j] = set[j] << 8;             // div by 255 shortcut div by 256 via bit shift
    }
    cases.setPixelColor(i, cases.Color(set[1],set[2],set[3]));
  }
}


void detect()	// polls each external sensor and updates vars
{				// reset button managed by setleds()
	caseopen = digitalRead(sidepanel);
	if(digitalRead(powerbutton))
	{
		mode = mode_power_on;
		digitalWrite(powerpin, HIGH);
		delay(2);
		digitalWrite(powerpin,LOW);
	}

	if (pressresolve) buttonprs();

}


void loop()
{
	detect();
	setleds();
	cases.show();

}
