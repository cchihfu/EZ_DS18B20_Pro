/**********************************************************************
 - 主機版本：Arduino UNO R3
 - 適用狀況：本程式只適用於一台Arduion連接一個DS18B20
 - 程式功能：確認DS18B20是否在線
 - 版本說明：找到最佳reset速度
 - 注意事項：DS18B20的DQ腳需加上拉電阻4.7k
 *********************************************************************/
//Arduino數位腳位2接到DS18B20的第2腳(DQ)，DQ為Data input/output的縮寫
const uint8_t g_dq_pin =2;

uint16_t tx_low_time=720;
uint16_t rx_wait_read_time=70;
uint16_t rx_pass_time=410;

void setup() {
	Serial.begin(9600);
	Serial.println("<<Star Test the DS18B20 >>");

}

void loop() {
	//先測Tx之時間min

	//Serial.println("<<Test the Speed : Tx Stay Low Time >>");
	uint8_t test_num=1;
	uint8_t tx_low_time_min=0;

	tx_low_time=1;
	for(int i = 0; i < 100; ++i)
	{
		while(!IsResetOk()) {
			tx_low_time++;
		}
		if(tx_low_time>tx_low_time_max)
		{
			tx_low_time_max=tx_low_time;
		}
		/*Serial.print("the ");
		Serial.print(test_num);
		Serial.print(" times test: ");
		PrintInfo(IsResetOk()) ;*/

		test_num++;
	}
	Serial.print("the 100 times test result:Tx stay in LOW time >>");
	Serial.println(tx_low_time_max);
	//PrintInfo(IsResetOk()) ;
	delay(1000);
}

uint8_t IsResetOk()
{
	TxReset();
	uint8_t f=RxReply();
	PassRx();
	return f;
}

void TxReset() {
	//Tx階段：Step 1.主機發送重置脈沖
	pinMode(g_dq_pin, OUTPUT);
	//Tx階段：Step 2.主機拉低電位
	digitalWrite(g_dq_pin, LOW);
	//Tx階段：Step 3.主機持續於低電位
	delayMicroseconds(tx_low_time);
	//Tx階段：Step 4.主機釋放電位控制，轉為輸入狀態
	pinMode(g_dq_pin, INPUT);
}
uint8_t RxReply() {
	//Rx階段：Step 5.延時並讀取DS18B20回應電位值
	delayMicroseconds(rx_wait_read_time);
	return !digitalRead(g_dq_pin);
}
void PassRx() {
	//Rx階段：Step 6.延時並讓其超過Rx的480us時間
	delayMicroseconds(rx_pass_time);
}

void PrintInfo(uint8_t isOk)
{
	if(isOk) {
		Serial.print("Reset OK");
		Serial.print(",");
		Serial.print(tx_low_time);
		Serial.print(",");
		Serial.print(rx_wait_read_time);
		Serial.print(",");
		Serial.println(rx_pass_time);
	} else {
		Serial.println("Reset Fail");
	}

}

