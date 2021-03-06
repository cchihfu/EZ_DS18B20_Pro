/**********************************************************************
 - 主機版本：Arduino UNO R3
 - 適用狀況：本程式只適用於一台Arduion連接一個DS18B20
 - 程式功能：確認DS18B20是否在線
 - 版本說明：依重構函式概念，將確認函式原子化
 - 注意事項：DS18B20的DQ腳需加上拉電阻4.7k
 *********************************************************************/
//Arduino數位腳位7接到DS18B20的第2腳(DQ)，DQ為Data input/output的縮寫
#define g_dq_pin 7

void setup() {
	Serial.begin(9600);
	Serial.println("<<Star Test the DS18B20 >>");
}

void loop() {
	ShowOkInfo(CommandReset()) ;
	delay(2000);
}

uint8_t CommandReset()
{
	if(!TestConnect()){return 0;} 
	TxReset();
	uint8_t f=RxResult();
	ThroughRx();
	return f;
}
uint8_t TestConnect()
{
	uint8_t retries = 60;
	//先拉高電位（轉為讀取狀態）
	noInterrupts();
	pinMode(g_dq_pin, INPUT);
	interrupts();
	//先觀察線路的狀態
	//若是正常就會處於高電位（DS18B20的一般狀態）
	//若不正常就代表接線出問題
	while(!digitalRead(g_dq_pin))
	{
		retries--;
		if(retries == 0) {
			Serial.println("DQ_pin no connect!");
			return 0;
		}
		delayMicroseconds(4);
	}
			return 1;

}

void TxReset() {
	//Tx階段：Step 1.主機發送重置脈沖
	pinMode(g_dq_pin, OUTPUT);
	//Tx階段：Step 2.主機拉低電位
	digitalWrite(g_dq_pin, LOW);
	//Tx階段：Step 3.主機維持於低電位
	delayMicroseconds(720);
	//Tx階段：Step 4.主機釋放電位控制，轉為輸入狀態
	pinMode(g_dq_pin, INPUT);
}
uint8_t RxResult() {
	//Rx階段：Step 5.延時並讀取DS18B20回應電位值
	delayMicroseconds(70);
	return !digitalRead(g_dq_pin);
}
void ThroughRx() {
	//Rx階段：Step 6.延時並讓其超過Rx的480us時間
	delayMicroseconds(410);
}

void ShowOkInfo(uint8_t isOk)
{
	if(isOk) {
		Serial.println("Reset OK");
	} else {
		Serial.println("Reset Fail");
	}

}

