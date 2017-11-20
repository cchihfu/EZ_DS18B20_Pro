#include <mlds18b20.h>

MLDs18b20 ds(7);

void setup() {
	Serial.begin(9600);
}

void loop() {
if(ds.reset())
	{
		Serial.println("So Great,Reset ok");
	} else {
		Serial.println("Reset error");
	}
	delay(3000);
}