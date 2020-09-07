// ��Ʈ�ѷ�
#include <Wire.h>
#include "BNO055_support.h"  // ��ġ �ʿ�

#include "nRF24L01.h" // ��ġ �ʿ�
#include "RF24.h" // ��ġ �ʿ�
#include <SPI.h>

/************ ���� ���� ************/
RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001"; // ���ű� �ּ�(�⺻)

int init_pitch, init_roll;

struct bno055_t my_bno; // ���� ������ ���� ����ü
// h(heading = yaw), pitch, roll�� signed short ������ ���� ����ü
struct bno055_euler euler_data;


setup()
{
    // ���̷μ��� �ʱ� ���� �ڵ�
    Wire.begin(); // I2C ��� ����
    BNO_Init(&my_bno); // ������ ��� ����, ������ my_bno �ν��Ͻ��� ����
    bno055_set_operation_mode(OPERATION_MODE_NDOF); // ���� ��� ����
    bno055_read_euler_hrp(&euler_data); // �����κ��� ���� �޾Ƽ� ����ü�� ����

    // ������ pitch roll ���� ��� (�ʱⰪ)
    init_pitch = euler_data.p;
    init_roll = euler_data.r;

    // ��Ÿ�� ���� ���� �ڵ�
    radio.begin();
    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_MIN); // �ּҰŸ�
    radio.stopListening();

    // �Ƶ��̳� ���� �� ���� �ڵ�
    pinMode(SWITCH_FORWARD, INPUT); // ���� ����ġ
    pinMode(SWITCH_BACKWARD, INPUT); // ���� ����ġ
}


loop()
{
    int front = 0; // ���� �� ��
    int side = 0; // �¿� �� ��

    // ���� �� �� (front) �� ���ϱ�
    if (digitalRead(SWITCH_FORWARD)) { // ���� ����ġ ����
        front = MAX;
    }
    else if (digitalRead(SWITCH_BACKWARD)) { // ���� ����ġ ����
        front = 0 - MAX;
    }
    else { // ����̴� ������ ���� �� �ڷ� �̵�
        bno055_read_euler_hrp(&euler_data);
        signed short pitch = euler_data.p - init_pitch;
        front = (int)((float)pitch * 1.2); // ��ȯ���� �ϵ���� �������� �� ã�ƾ��� (�ӽ�)
    }

    // roll ���� -> �¿� �� �� (side)
    bno055_read_euler_hrp(&euler_data);
    signed short roll = euler_data.r - init_roll;
    side = (int)((float)roll * 1.2); // ��ȯ���� �ϵ���� �������� �� ã�ƾ��� (�ӽ�)

    // ���� �� ��, �¿� �� ���� �������ݿ� ���߾� ��ȯ
    int msg[4] = { -456, 0, 0, -789 }; // ����, pitch, roll, �� (�������ݿ� ���� ���� �ʿ�)
    msg[1] = pitch;
    msg[2] = roll;

    // ��ȯ�� ���� ����
    radio.write(&msg, sizeof(msg));
}
