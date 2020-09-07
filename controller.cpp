// 컨트롤러
#include <Wire.h>
#include "BNO055_support.h"  // 설치 필요

#include "nRF24L01.h" // 설치 필요
#include "RF24.h" // 설치 필요
#include <SPI.h>

/************ 전역 변수 ************/
RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001"; // 수신기 주소(기본)

int init_pitch, init_roll;

struct bno055_t my_bno; // 센서 정보를 담은 구조체
// h(heading = yaw), pitch, roll를 signed short 형으로 담은 구조체
struct bno055_euler euler_data;


setup()
{
    // 자이로센서 초기 설정 코드
    Wire.begin(); // I2C 통신 시작
    BNO_Init(&my_bno); // 센서와 통신 시작, 정보를 my_bno 인스턴스에 담음
    bno055_set_operation_mode(OPERATION_MODE_NDOF); // 센서 모드 설정
    bno055_read_euler_hrp(&euler_data); // 센서로부터 값을 받아서 구조체에 복사

    // 지금의 pitch roll 값을 기억 (초기값)
    init_pitch = euler_data.p;
    init_roll = euler_data.r;

    // 통신모듈 관련 설정 코드
    radio.begin();
    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_MIN); // 최소거리
    radio.stopListening();

    // 아두이노 관련 핀 설정 코드
    pinMode(SWITCH_FORWARD, INPUT); // 전진 스위치
    pinMode(SWITCH_BACKWARD, INPUT); // 후진 스위치
}


loop()
{
    int front = 0; // 전후 축 값
    int side = 0; // 좌우 축 값

    // 전후 축 값 (front) 값 정하기
    if (digitalRead(SWITCH_FORWARD)) { // 전진 스위치 눌림
        front = MAX;
    }
    else if (digitalRead(SWITCH_BACKWARD)) { // 후진 스위치 눌림
        front = 0 - MAX;
    }
    else { // 기울이는 정도에 따라 앞 뒤로 이동
        bno055_read_euler_hrp(&euler_data);
        signed short pitch = euler_data.p - init_pitch;
        front = (int)((float)pitch * 1.2); // 변환식은 하드웨어 굴려보고 식 찾아야함 (임시)
    }

    // roll 각도 -> 좌우 축 값 (side)
    bno055_read_euler_hrp(&euler_data);
    signed short roll = euler_data.r - init_roll;
    side = (int)((float)roll * 1.2); // 변환식은 하드웨어 굴려보고 식 찾아야함 (임시)

    // 전후 축 값, 좌우 축 값을 프로토콜에 맞추어 변환
    int msg[4] = { -456, 0, 0, -789 }; // 시작, pitch, roll, 끝 (프로토콜에 관해 논의 필요)
    msg[1] = pitch;
    msg[2] = roll;

    // 변환한 값을 전송
    radio.write(&msg, sizeof(msg));
}
