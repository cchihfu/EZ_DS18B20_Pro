#include "mlds18b20.h"

MLDs18b20::MLDs18b20(uint8_t DQ_PIN) {
	//���M�~�����s�B�A�A���i�ऺ�����t�~���s���p����001
	//�άO�����HA��F���N���@�T���ǹF�A�n�󦹻����A�q�`�Τp�g
	//�e���A�["_"���u�A�H�ϧO�A�N���D�����~����������ƭ�
	//�غc���G�~���s���������]�w
	//�N���ܼ��x�s�� private �ܼƥH�Ѩ�L���O�����禡�ϥ�
	_g_dq_pin = DQ_PIN;

	//�N�� pin �}��l�Ʀ���X���A
	pinMode(_g_dq_pin, INPUT);
}

//:: �Ÿ��C�o�N���禡�O�ݩ�MLDs18b20���O�������禡

// �禡�\�໡��
// 1.�˴�pin�}�O�_�s�����`�]��
// 2.�˴�DS18B20�O�_�ॿ�`�^��
// �H�W���p�A�Yok�h�Ǧ^1�A���`�N�Ǧ^0
uint8_t MLDs18b20::reset(void)
{
	//�˴�01�G���u���A
	if(!TestConnect()) {return 0;}

	//�˴�02:DS18B20���A
	noInterrupts();
	//Tx���q�GStep 1.�D���o�e���m�ߨR
	pinMode(_g_dq_pin, OUTPUT);
	//Tx���q�GStep 2.�D���ԧC�q��
	digitalWrite(_g_dq_pin, LOW);
	interrupts();

	//Tx���q�GStep 3.�D�������C�q��
	delayMicroseconds(480);

	//Tx���q�GStep 4.�D������q�챱��A�ର��J���A
	noInterrupts();
	pinMode(_g_dq_pin, INPUT);

	//Rx���q�GStep 5.���ɨ�Ū��DS18B20�^���q���
	delayMicroseconds(70);
	uint8_t f=!digitalRead(_g_dq_pin);
	interrupts();

	//Rx���q�GStep 6.���ɨ�����W�LRx��480us�ɶ�
	delayMicroseconds(410);
	return f;
}

uint8_t MLDs18b20::TestConnect(void)
{
	//�������`���}�Garduino����DQ_pin���u�����`
	uint8_t retries = 60;
	//���԰��q��]�ରŪ�����A�^
	noInterrupts();
	pinMode(_g_dq_pin, INPUT);
	interrupts();
	//���[��u�������A�A�ɶ���240us�C�j4��d�@��
	//�Y�O���`�u�����B�󰪹q��]idle state�^
	while(!digitalRead(_g_dq_pin))
	{
		retries--;
		if(retries == 0) {return 0;}
		delayMicroseconds(4);
	}
	return 1;

}

