/**********************************************************************

            【DS18B20控制專案】

 - 檔案名稱：my_ds18b20_pro_02_reset_function
 - 功能描述：DS18B20重置（Reset）
 - 詳細內容：不引用他人函數庫的情況下，讓Arduino與DS18B20於1對1的連接下，
            能做直接復位（reset）動作，並偵測線上是否存有DS18B20並回饋訊息
 - 主機版本：Arduino UNO R3
 - 作者名稱：Billy
 - 參數說明：無
 - 檔案日期：2017/11/02
 *********************************************************************/
#define DQ_Pin 2            //Arduino數位腳位2接到DS18B20，DQ為Data input/output的縮寫

void setup() {
	Serial.begin(9600);
	Serial.println("<<Look for DS18B20 By Reset_function>>");
}

/**********************************************************************
 - 功能描述：DS18B20重置（Reset）
 - 隸屬模塊：DS18B20控制模塊
 - 函數屬性：外部，用戶可調用
 - 參數說明：無
 - 返回說明：返回值為0時，代表reset成功，否則失敗（代表線上找不到DS18B20）
 *********************************************************************/
unsigned char DS18B20_Reset()
{

	pinMode(DQ_Pin, OUTPUT);    //將DQ_Pin調整為輸出狀態
	digitalWrite(DQ_Pin, HIGH); //將電位先拉高
	digitalWrite(DQ_Pin, LOW);  //主機拉低電位（開始主機reset脈波）

	delayMicroseconds(720);     //等待720us，平均值的計算((480+960)/2=720）

	digitalWrite(DQ_Pin, HIGH); //將主機拉高電位

	delayMicroseconds(70);
	//此值要跳過電阻上拉動作時間約15~60us，再加10us保證讀到的狀態

	pinMode(DQ_Pin, INPUT);
	//將DQ_Pin設定作數位輸入動作


	return digitalRead(DQ_Pin);

}

void loop() {
	Serial.println("Reset DS18B20 ...");
	//啟動reset DS18B20副程式，當其傳回值是1時，代表無DS18B20
	//傳回值是0代表reset成功
	//Serial.println(DS18B20_Reset());

	if(DS18B20_Reset()) {
		Serial.println("No DS18B20 in the wire bus");
	}
	else {
		Serial.println("Yes,DS18B20 On Line");
	}

	delay(2000);

}
