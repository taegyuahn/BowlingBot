// RC 카
#include "nRF24L01.h" // 설치 필요
#include "RF24.h" // 설치 필요
#include <SPI.h>


/************ 전역 변수 ************/
RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001"; // 수신기 주소(기본)


setup() 
{
    // 통신모듈 초기 설정
	radio.begin();
	radio.openReadingPipe(0, address);
	radio.setPALevel(RF24_PA_MIN);
	radio.startListening();

	// 아두이노 관련 핀 설정 코드
	pinMode(11, OUTPUT); // 앞 바퀴
	pinMode(12, OUTPUT); // 뒤 바퀴
	pinMode(13, OUTPUT); // 왼쪽 바퀴
	pinMode(14, OUTPUT); // 오른쪽 바퀴
}


loop() 
{
	int front = 0; // 전후 축 값
	int side = 0; // 좌우 축 값
	int msg[4] = { 0, };

	while (1) {
		radio.read(&msg, sizeof(msg)); // 수신기로부터 데이터 받음
		// 받은 데이터가 잘 되었는지 검사 (시작과 끝 데이터)
		if (msg[0] == -456 && msg[3] == -789) break;
	}

	// 추출한 데이터로부터 전후좌우 축 값 추출
	front = msg[1];
	side = msg[2];

	// 전후 축 값으로 전후 담당 바퀴 2개 제어
	analogWrite(11, front);
	analogWrite(12, front);

	// 좌우 축 값으로 좌우 담당 바퀴 2개 제어
	analogWrite(13, side);
	analogWrite(14, side);

}
