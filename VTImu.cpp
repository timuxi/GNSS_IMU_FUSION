#include <iostream>
#include <unistd.h>
#include "uart.h"
#include "VTImu.h"
#define IMU_ADDR 0x50
static char byteSend[8] = {
    IMU_ADDR,
    0x03,
    0x00,
    0x34,
    0x00,
    0x0C,
    0X00,
    0X00};
static char accbyteSend[8] = {
    IMU_ADDR,
    0x03,
    0x00,
    0x34,
    0x00,
    0x03,
    0X49,
    0X84};

static char anglebyteSend[8] = {
    IMU_ADDR,
    0x03,
    0x00,
    0x3d,
    0x00,
    0x03,
    0X99,
    0X86};

static char palbyteSend[8] = {
    IMU_ADDR,
    0x03,
    0x00,
    0x37,
    0x00,
    0x03,
    0XB9,
    0X84};

double a[3], w[3], Angle[3];
// Imu数据处理
void getImuData(int fd, char *recv_buffer, int recv_length)
{
    int ret = Uart::writeData(fd, byteSend, 8);
    if (ret == -1)
    {
        printf("imu write failed!\n");
        return;
    }
    usleep(100);
    ret = Uart::readData(fd, recv_buffer, recv_length);
    if (ret == -1)
    {
        printf("imu read failed!\n");
        return;
    }
    for (int i = 0; i < ret; i++)
    {
        ParseData(recv_buffer[i]);
    }
}

void ParseData(char chr) //处理数据
{
    static char chrBuf[100];
    static unsigned char chrCnt = 0;
    signed short sData[12];
    int i;

    chrBuf[chrCnt++] = chr;
    if (chrCnt < 29) //满29字节进行下一步
    {
        return;
    }

    if ((chrBuf[0] != 0x50) || (chrBuf[1] != 0x03) || chrBuf[2] != 0x18)
    {
        printf("imudata Error !!\n");
        memcpy(&chrBuf[0], &chrBuf[1], 10);
        chrCnt--;
        return;
    }

    for (i = 0; i < 12; i++)
    {
        sData[i] = (short)((chrBuf[3 + i * 2] << 8) | chrBuf[4 + i * 2]);
    }

    for (i = 0; i < 3; i++)
    {
        a[i] = (float)(sData[i]) / 32768.0 * 16.0;          //加速度
        w[i] = (float)(sData[3 + i]) / 32768.0 * 2000.0;    //角速度
        Angle[i] = (float)(sData[9 + i]) / 32768.0 * 180.0; //角度
        // h[i] = (int)(sData[6+i])*1.0;//磁场
    }

    chrCnt = 0;
    return;
}
