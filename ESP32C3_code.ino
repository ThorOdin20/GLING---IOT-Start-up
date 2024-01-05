char me='B';
char receiveData[3]={0, 0, 0};
char sendData[3]={0, 0, 0};

#include <FastLED.h>
#include "NETWORK.h"


#define NUM_LEDS 34
#define DATA_PIN 10 // Replace with the actual pin you connected the data line to
// Define rotary encoder pins
#define ENC_A 4
#define ENC_B 5
#define Brightness 80
const int switchPin = 20;		// Replace Z with the GPIO pin for the switch

unsigned long _lastIncReadTime = micros(); 
unsigned long _lastDecReadTime = micros(); 
int _pauseLength = 25000;
int _fastIncrement = 10;
volatile int counter = 0;
int lastCounter = 0;

int activityIndex = 0;
uint8_t activityState=0;
uint32_t activityTimer;
uint32_t activityTimeout=400;
uint32_t debounceTimer;
uint32_t debounceTimeout=500;
uint32_t ledTimer;
uint32_t ledTimeout=50;
uint32_t networkTimer;
uint32_t networkTimeout=20;
int start = activityIndex * 4 - 1;
int end = activityIndex * 4 + 2;
int k = 15;

CRGB leds[NUM_LEDS];

void setup() {

	// Set encoder pins and attach interrupts
	pinMode(ENC_A, INPUT_PULLUP);
	pinMode(ENC_B, INPUT_PULLUP);
	pinMode(switchPin, INPUT_PULLUP); // Assuming the switch is connected to ground when pressed

	attachInterrupt(digitalPinToInterrupt(ENC_A), read_encoder, CHANGE);
	attachInterrupt(digitalPinToInterrupt(ENC_B), read_encoder, CHANGE);

	FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
	FastLED.setBrightness(Brightness); // Set the brightness (0-255)
	// Start the serial monitor to show output
	Serial.begin(115200);
	network_setup();			// The ESP32 network initialization
}

bool waiting=false;

void loop()
{
	// If count has changed print the new value to serial
	if (activityState==0 && counter != lastCounter) {
		updateLEDs();	
	}

	if((receiveData[0] && receiveData[0]!=me) && receiveData[1]=='A'+activityIndex && activityState==0)
	{
		activityState=1;
		receiveData[1]=0;
		waiting=true;
	}
	else if((receiveData[0] && receiveData[0]!=me) && receiveData[1]=='~')
	{
		activityState=0;
		receiveData[1]=0;
		waiting=false;
		pairingDone();
	}
	if(digitalRead(switchPin) == LOW && millis()>debounceTimer+debounceTimeout)
	{
		debounceTimer=millis();
		Serial.println("Switch pressed");
		if(waiting)
		{
			sendData[0]=me;
			sendData[1]='~';
			client.send(sendData);
			waiting=false;
			activityState=0;
			receiveData[1]=0;
			pairingDone();
		}
		else
		{
			sendData[0]=me;
			sendData[1]='A'+activityIndex;
			client.send(sendData);
			activityState=1;
		}
	}
	if(activityState)
	{
		activityChosen();
	}
	if(millis()>(ledTimer+ledTimeout))
	{
		ledTimer=millis();
		networkTimer=ledTimer;
		FastLED.show();
	}
	if(millis()>(networkTimer+networkTimeout))
	{
			network_loop();
	}
}


void read_encoder() {
	static uint8_t old_AB = 3;	// Lookup table index
	static int8_t encval = 0;	 // Encoder value	
	static const int8_t enc_states[]	= {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0}; // Lookup table
	static unsigned long lastDebounceTime = 0;
	static unsigned long debounceDelay = 30;	// Adjust the debounce delay as needed

	unsigned long currentMillis = millis();

	old_AB <<= 2;	// Remember the previous state

	if (digitalRead(ENC_A)) old_AB |= 0x02;	// Add the current state of pin A
	if (digitalRead(ENC_B)) old_AB |= 0x01;	// Add the current state of pin B

	encval += enc_states[(old_AB & 0x0f)];

	// Update counter if encoder has rotated a full indent, that is at least 4 steps
	if (encval > 3) {	// Four steps forward
		if ((currentMillis - lastDebounceTime) > debounceDelay) {
			int changevalue = 1;
			if ((micros() - _lastIncReadTime) < _pauseLength) {
				changevalue = _fastIncrement * changevalue;
			}
			_lastIncReadTime = micros();
			counter = counter + changevalue;	// Update counter
			encval = 0;
			lastDebounceTime = currentMillis;
		}
	} else if (encval < -3) {	// Four steps backward
		if ((currentMillis - lastDebounceTime) > debounceDelay) {
			int changevalue = -1;
			if ((micros() - _lastDecReadTime) < _pauseLength) {
				changevalue = _fastIncrement * changevalue;
			}
			_lastDecReadTime = micros();
			counter = counter + changevalue;	// Update counter
			encval = 0;
			lastDebounceTime = currentMillis;
		}
	}
}

void updateLEDs() {
	// Calculate the new LED index
	if (counter == -1)
		counter = NUM_LEDS - 1;
	counter = counter % NUM_LEDS;

	Serial.println(counter);

	// Turn on the current LED
	Serial.println("MAKING ALL BLACK");
	fill_solid(leds, NUM_LEDS, CRGB::Black);
	Serial.println("MAKING ONE BLUE");
	leds[counter] = CRGB::Blue;
	// Store the current counter value
	lastCounter = counter;
	if (counter == 2 || counter == 1 || counter == 0 || counter == 31 || counter == 32 || counter == 33 )
	{
		activityIndex = 0;
	}
	else
	{
		activityIndex=((counter+1))/4;
	}
	
	Serial.println(activityIndex);			 
}

void pairingDone()
{
	
	fill_solid(leds, NUM_LEDS, CRGB::Black);
	Serial.print("SHOW ");
	Serial.println(__LINE__);
	FastLED.show();
	// Function to activate the light pattern when the button is pressed
	unsigned long startTime = millis();

	// Run the pattern for 2-3 seconds
	while (millis() - startTime < 15000)
	{
		// Your light pattern code goes here
		// For example, you can make the LEDs blink in a specific sequence
		for (int i = 0; i < NUM_LEDS; i++)
		{
			leds[i] = CRGB::Green;
			Serial.print("SHOW ");
			Serial.println(__LINE__);
			FastLED.show();
			delay(50);
			leds[i] = CRGB::Black;
		}
	}

	// Turn off all LEDs after the pattern is complete
	fill_solid(leds, NUM_LEDS, CRGB::Black);
	Serial.print("SHOW ");
	Serial.println(__LINE__);
	FastLED.show();
}

void activityChosen()
{
	if(activityState==1)
	{
		start = activityIndex * 4 - 1;
		end = activityIndex * 4 + 2;
		k=15;
		activityState=2;
		activityTimer=millis();
		Serial.println("MAKING ALL BLACK");
		fill_solid(leds, NUM_LEDS, CRGB::Black);
		Serial.println("MAKING SOME YELLOW");
		fill_solid(leds+start, end+1-start, CRGB::Yellow);
	}
	else if(activityState==2)
	{
		if(millis()>activityTimer+activityTimeout)
		{
			activityState=3;
			activityTimer=millis();
			Serial.println("MAKING SOME BLACK");
			fill_solid(leds+start, end+1-start, CRGB::Black);
		}
	}
	else if(activityState==3)
	{
		if(millis()>activityTimer+activityTimeout)
		{
			--k;
			if(k)
			{
				activityState=2;
				activityTimer=millis();
				Serial.println("MAKING SOME YELLOW");
				fill_solid(leds+start, end+1-start, CRGB::Yellow);
			}
			else
			{
				activityState=0;
				waiting=false;
				updateLEDs();	
			}
		}
	}
}
