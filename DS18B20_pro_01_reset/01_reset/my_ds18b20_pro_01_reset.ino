/**********************************************************************

            【DS18B20控制專案】

 - 檔案名稱：my_ds18b20_pro_01_reset
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
	boolean HaveDs18b20 = true;  //接收有無DS18B20的布林值變數
}
void loop() {
	Serial.println("Start Reset DS18B20 in the Wire Bus...");

	pinMode(DQ_Pin, OUTPUT);    //將DQ_Pin調整為輸出狀態
	digitalWrite(DQ_Pin, HIGH); //將電位先拉高
	digitalWrite(DQ_Pin, LOW);  //主機拉低電位（開始主機reset脈波）

	delayMicroseconds(720);     //等待720us，平均值的計算((480+960)/2=720）

	digitalWrite(DQ_Pin, HIGH); //將主機拉高電位

	delayMicroseconds(70);
	//此值要跳過電阻上拉動作時間約15~60us，再加10us保證讀到的狀態

	pinMode(DQ_Pin, INPUT);
	//將DQ_Pin設定作數位輸入動作

	boolean HaveDs18b20 = !digitalRead(DQ_Pin);
	//將讀取的電位值反轉，所以是低電位為有，高電位代表無喲！

	if (HaveDs18b20) {
		Serial.println("Yes,the Wire BUS have DS18B20");
	} else {
		Serial.println("No DS18B20 in the wire bus");
	}
	Serial.println();
	delay(2000);
}