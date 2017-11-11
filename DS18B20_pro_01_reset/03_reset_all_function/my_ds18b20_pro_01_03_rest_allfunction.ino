/**********************************************************************************
 - 功能描述：只作reset的測試，並將測試效果記錄下來
 - 詳細內容：不引用他人函數庫的情況下，reset DS18B20
 - 主機版本：Arduino UNO R3
***********************************************************************************/
#define g_wire_bus_pin 2      

void setup() {
	Serial.begin(9600);
	Serial.println("<<Star Test the DS18B20 >>");

}

void loop() {
	PrintInfo(IsResetOk()) ;
	delay(2000);
}

uint8_t IsResetOk()
{
	ResetPulse();
	WaitRead();
	//低電位代表reset成功
	if(!digitalRead(g_wire_bus_pin)) {
		return 1;
	} else {
		return 0;
	}
}

void ResetPulse() {
	pinMode(g_wire_bus_pin, OUTPUT);
	digitalWrite(g_wire_bus_pin, LOW);
	delayMicroseconds(250);
	pinMode(g_wire_bus_pin, INPUT);
}

uint8_t PresencePulse() {
	return digitalRead(g_wire_bus_pin);
}

void WaitRead() {
	delayMicroseconds(30);	
}

void PrintInfo(uint8_t isOk)
{
	if(isOk) {
		Serial.println("Reset OK");
	} else {
		Serial.println("Reset Fail");
	}

}

