#include <Bounce2.h>

//setup macros for time
#define SECOND	1000L
#define MINUTE	SECOND * 60L
#define HOUR	MINUTE * 60L

//how long the relay should be turned on for
const unsigned long runTime = 2 * HOUR;

//setup the I/O pins
const int relayPin = 4;
const int buttonPin = 0;

//setup the debouncer for the button
Bounce buttonDebounce = Bounce();

void setup(){

	//setup the relay pin as an output
	pinMode(relayPin, OUTPUT);	
	digitalWrite(relayPin, LOW);

	//setup the button pin and attach it to the debouncer
	pinMode(buttonPin, INPUT);
	buttonDebounce.attach(buttonPin);
	buttonDebounce.interval(50);

}

void loop(){
	buttonDebounce.update();

	//if the user presses the button, trigger the relay
	if(buttonDebounce.rose()){

		//turn the relay on
		digitalWrite(relayPin, HIGH);

		//wait for runTime amount of time
		delay(runTime);

		//turn the relay off
		digitalWrite(relayPin, LOW);
	}

}
