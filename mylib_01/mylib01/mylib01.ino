#include <Mlds18b20.h>
uint8_t pin=7;
MLDS18B20 ds(pin);
void setup() {
	Serial.begin(9600);
}
void loop() {
if(ds.CommandReset())
	{
		Serial.println(" so Great,Reset ok");
	} else {
		Serial.println("Reset error");
	}
	delay(3000);
}