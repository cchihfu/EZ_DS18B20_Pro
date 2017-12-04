#include <SimpleDs18b20.h>
uint8_t dq_pin=7;
SimpleDs18b20 ds01(dq_pin);
void setup() {
	Serial.begin(9600);
}
void loop() {
	Serial.print("Ds18B20's Temperature --> ");
	Serial.println(ds01.GetTemperature());
	delay(1000);
}
