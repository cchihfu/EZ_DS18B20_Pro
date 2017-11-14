/**********************************************************************
 - 主機版本：Arduino UNO R3
 - 適用狀況：本程式只適用於一台Arduion連接一個DS18B20
 - 程式功能：確認DS18B20是否在線
 - 版本說明：找到最佳reset速度
 - 注意事項：DS18B20的DQ腳需加上拉電阻4.7k
 *********************************************************************/
//Arduino數位腳位2接到DS18B20的第2腳(DQ)，DQ為Data input/output的縮寫
const uint8_t g_dq_pin =2;

uint16_t g_time_keep_low=720;
uint16_t g_time_wait_read=70;
uint16_t g_time_through=410;
uint16_t g_time_keep_low_min=0;
uint16_t g_time_wait_read_min=0;


void setup() {
	Serial.begin(9600);
	Serial.println("<<Star Test the DS18B20 >>");
}

void loop() {
	//先測Tx之時間min
	TestTxTime();

	//先測Tx之時間min
	TestRxTime();

	//使用min值再加一些作測試10次
	TestMinTime();

	Serial.println("");
	delay(3000);
}

void TestTxTime()
{
	g_time_keep_low=1;
	for(int i = 0; i < 100; ++i)
	{
		while(!CommandReset()) {
			g_time_keep_low++;
		}
		if(g_time_keep_low>g_time_keep_low_min)
		{
			g_time_keep_low_min=g_time_keep_low;
		}

	}
	Serial.print("the 100 times test result:Tx stay in LOW time >>");
	Serial.println(g_time_keep_low_min);

}
void TestRxTime()
{
	//將Tx維持低電位的基礎設定值取回來
	g_time_keep_low=720;
	g_time_wait_read=1;
	for(int i = 0; i < 100; ++i)
	{
		while(!CommandReset()) {
			//無法reset成功就增加等待取時間
			g_time_wait_read++;

			//改變Rx穿越等待時段
			g_time_through=480-g_time_wait_read;
		}
		//成功時就將最小值放入其中
		if(g_time_wait_read>g_time_wait_read_min)
		{
			g_time_wait_read_min=g_time_wait_read;
		}
	}
	Serial.print("the 100 times test result:Rx wait time >>");
	Serial.println(g_time_wait_read_min);
}

void TestMinTime()
{
	//Tx維持低電位的時間以最小值加30
	g_time_keep_low=g_time_keep_low_min+30;

	//Rx讀取時間以最小值加3
	g_time_wait_read=g_time_wait_read_min+3;

	//調整Rx穿越時間
	g_time_through=480-g_time_wait_read;

	//測試十次
	for(int i = 0; i < 10; ++i)
	{
		ShowOkInfo(CommandReset()) ;
	}
}


uint8_t CommandReset()
{
	TxReset();
	uint8_t f=RxResult();
	ThroughRx();
	return f;
}

void TxReset() {
	//Tx階段：Step 1.主機發送重置脈沖
	pinMode(g_dq_pin, OUTPUT);
	//Tx階段：Step 2.主機拉低電位
	digitalWrite(g_dq_pin, LOW);
	//Tx階段：Step 3.主機持續於低電位
	delayMicroseconds(g_time_keep_low);
	//Tx階段：Step 4.主機釋放電位控制，轉為輸入狀態
	pinMode(g_dq_pin, INPUT);
}
uint8_t RxResult() {
	//Rx階段：Step 5.延時並讀取DS18B20回應電位值
	delayMicroseconds(g_time_wait_read);
	return !digitalRead(g_dq_pin);
}
void ThroughRx() {
	//Rx階段：Step 6.延時並讓其超過Rx的480us時間
	delayMicroseconds(g_time_through);
}

void ShowOkInfo(uint8_t isOk)
{
	if(isOk) {
		Serial.print("Reset OK");
		Serial.print(",");
		Serial.print(g_time_keep_low);
		Serial.print(",");
		Serial.print(g_time_wait_read);
		Serial.print(",");
		Serial.println(g_time_through);
	} else {
		Serial.println("Reset Fail");
	}

}

