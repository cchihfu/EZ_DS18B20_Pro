#include <SimpleDs18b20.h>
uint8_t dq_pin=7;
SimpleDs18b20 ds01(dq_pin);
SimpleDs18b20 ds02(8);

void setup() {
	Serial.begin(9600);
}

void loop() {
	Serial.print("7 Ds18B20's Temperature --> ");
	Serial.println(ds01.GetTemperature());
	Serial.print("8 Ds18B20's Temperature --> ");
	Serial.println(ds02.GetTemperature());
	delay(3000);
}