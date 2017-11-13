/**********************************************************************
 - 主機版本：Arduino UNO R3
 - 適用狀況：本程式只適用於一台Arduion連接一個DS18B20
 - 程式功能：確認DS18B20是否在線
 - 版本說明：初版
 - 注意事項：DS18B20的DQ腳需加上拉電阻4.7k
 *********************************************************************/
//Arduino數位腳位2接到DS18B20的第2腳(DQ)，DQ為Data input/output的縮寫
#define DQ_Pin 2            

void setup() {
	Serial.begin(9600);
	char is_exist = 0;  //有無DS18B20之變數，先預設為不存在(0)
	Serial.println("Start Reset DS18B20 in the Wire Bus...");
}
void loop() {
	//Tx階段：Step 1.主機發送重置脈沖
	pinMode(DQ_Pin, OUTPUT);    //將DQ_Pin調整為輸出狀態，自動會回歸HIGH電位

	//Tx階段：Step 2.主機拉低電位
	digitalWrite(DQ_Pin, LOW);  

	//Tx階段：Step 3.主機持續於低電位
	delayMicroseconds(720);     //等待720us，平均值的計算((480+960)/2=720）

	//Tx階段：Step 4.主機釋放電位控制，轉為輸入狀態
	pinMode(DQ_Pin, INPUT);		//將DQ_Pin設定作數位輸入動作

	//Rx階段：Step 5.讀取DS18B20電位值
	delayMicroseconds(70);		//此值為DS18B20 max回應反應時間60us再加10us
	
	//將讀取的電位值反轉，所以是低電位為有DS18B20
	char is_exist = !digitalRead(DQ_Pin);

	//Rx階段：Step 6.延時至超過主機接收訊息時段
	delayMicroseconds(410);

	//結果輸出
	if(is_exist) {
		Serial.println("DS18B20 Exist");
	} else {
		Serial.println("No DS18B20 right here");
	}
	
	delay(2000);
}