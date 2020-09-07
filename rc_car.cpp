// RC ī
#include "nRF24L01.h" // ��ġ �ʿ�
#include "RF24.h" // ��ġ �ʿ�
#include <SPI.h>


/************ ���� ���� ************/
RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001"; // ���ű� �ּ�(�⺻)


setup() 
{
    // ��Ÿ�� �ʱ� ����
	radio.begin();
	radio.openReadingPipe(0, address);
	radio.setPALevel(RF24_PA_MIN);
	radio.startListening();

	// �Ƶ��̳� ���� �� ���� �ڵ�
	pinMode(11, OUTPUT); // �� ����
	pinMode(12, OUTPUT); // �� ����
	pinMode(13, OUTPUT); // ���� ����
	pinMode(14, OUTPUT); // ������ ����
}


loop() 
{
	int front = 0; // ���� �� ��
	int side = 0; // �¿� �� ��
	int msg[4] = { 0, };

	while (1) {
		radio.read(&msg, sizeof(msg)); // ���ű�κ��� ������ ����
		// ���� �����Ͱ� �� �Ǿ����� �˻� (���۰� �� ������)
		if (msg[0] == -456 && msg[3] == -789) break;
	}

	// ������ �����ͷκ��� �����¿� �� �� ����
	front = msg[1];
	side = msg[2];

	// ���� �� ������ ���� ��� ���� 2�� ����
	analogWrite(11, front);
	analogWrite(12, front);

	// �¿� �� ������ �¿� ��� ���� 2�� ����
	analogWrite(13, side);
	analogWrite(14, side);

}
