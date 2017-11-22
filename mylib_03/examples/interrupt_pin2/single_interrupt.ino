#include <SimpleDs18b20.h>

uint8_t dq_pin=7;
volatile int state = LOW;

SimpleDs18b20 ds01(dq_pin);

void setup() {
	Serial.begin(9600);
	//當int.0（pin2）改變時，觸發中斷函式，改變狀態
	attachInterrupt(0, GetTemp, CHANGE);
	pinMode(2, INPUT);
}

void loop() {
	digitalWrite(13, state);
	//依讀取與否的狀態來讀取溫度值
	if(state) {
		Serial.print("Ds18B20's Temperature --> ");
		Serial.println(ds01.GetTemperature());
		state = !state;
	}
	
	delay(1000);

}
void GetTemp()
{

	state = !state;
}