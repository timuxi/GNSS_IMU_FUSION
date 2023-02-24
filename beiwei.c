#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <termios.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

static char accbyteSend[5] = {0x77, 0x04, 0x00, 0x59, 0x5D}; // 每次读取数据前发送的命令
const int imu_beiwei_read_size = 23;                         // 读取的数据为23位，每位是16进制表示的两位数，例如0x12
double a[3];                                                 // 存取加速度数据
double w[3];                                                 // 存取角速度数据

static int ret;
static int fd_imu;
#define BORD_IMU 9600 // 波特率
#define SLEEP_TIME_1_MS (10 * 1000)

int hexToten(int num)
{
    return num / 16 * 10 + num % 16;
}

void ParseData(char *chrBuf) // 处理数据
{
    int acc_interval = 4;
    int w_interval = 9;

    if ((chrBuf[0] != 0x77) || (chrBuf[1] != 0x16) || (chrBuf[2] != 0x00) || (chrBuf[3] != 0x84))
    {
        printf("imudata Error !!\n");
        return;
    }

    int j = acc_interval;
    for (int i = 0; i < 3; i++)
    {
        double sign = 1;
        if (chrBuf[j] >= 0x10)
        {
            sign = -1;
            chrBuf[j] -= 0x10;
        }
        w[i] = chrBuf[j++];
        w[i] = w[i] * 100 + hexToten(chrBuf[j++]);
        w[i] += 0.01 * hexToten(chrBuf[j++]);
        w[i] *= sign;
    }

    for (int i = 0; i < 3; i++)
    {
        double sign = 1;
        if (chrBuf[j] >= 0x10)
        {
            sign = -1;
            chrBuf[j] -= 0x10;
        }
        a[i] = chrBuf[j++];
        a[i] += 0.01 * hexToten(chrBuf[j++]);
        a[i] += 0.0001 * hexToten(chrBuf[j++]);
        a[i] *= sign;
    }
}

int uart_open(int fd_imu, const char *pathname)
{
    fd_imu = open(pathname, O_RDWR | O_NOCTTY);
    if (-1 == fd_imu)
    {
        perror("Can't Open Serial Port");
        return (-1);
    }
    else
    {
        printf("open %s success!\n", pathname);
    }
    /*测试是否为终端设备*/
    if (isatty(STDIN_FILENO) == 0)
        printf("standard input is not a terminal device\n");
    else
        printf("isatty success!\n");
    return fd_imu;
}

// 串口名、波特率、数据位、奇偶校验位、停止位
int uart_set(int fd, int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio, oldtio;
    /*保存测试现有串口参数设置，在这里如果串口号等出错，会有相关的出错信息*/
    if (tcgetattr(fd, &oldtio) != 0)
    {
        perror("SetupSerial 1");
        printf("tcgetattr(fd,&oldtio) -> %d\n", tcgetattr(fd, &oldtio));
        return -1;
    }
    bzero(&newtio, sizeof(newtio));
    /*步骤一，设置字符大小*/
    newtio.c_cflag |= CLOCAL | CREAD;
    newtio.c_cflag &= ~CSIZE;
    /*设置数据位*/
    switch (nBits)
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    }
    /*设置奇偶校验位*/
    switch (nEvent)
    {
    case 'o':
    case 'O': // 奇数
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'e':
    case 'E': // 偶数
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'n':
    case 'N': // 无奇偶校验位
        newtio.c_cflag &= ~PARENB;
        break;
    default:
        break;
    }
    /*设置波特率*/
    switch (nSpeed)
    {
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
        break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
        break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    case 460800:
        cfsetispeed(&newtio, B460800);
        cfsetospeed(&newtio, B460800);
        break;
    case 921600:
        cfsetispeed(&newtio, B921600);
        cfsetospeed(&newtio, B921600);
        break;
    default:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    }
    /*设置停止位*/
    if (nStop == 1)
        newtio.c_cflag &= ~CSTOPB;
    else if (nStop == 2)
        newtio.c_cflag |= CSTOPB;
    /*设置等待时间和最小接收字符*/
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;
    /*处理未接收字符*/
    tcflush(fd, TCIFLUSH);
    /*激活新配置*/
    if ((tcsetattr(fd, TCSANOW, &newtio)) != 0)
    {
        perror("com set error");
        return -1;
    }
    printf("set done!\n");
    return 0;
}

int writeData(int fd, char *send_buffer, int length)
{
    length = write(fd, send_buffer, length * sizeof(unsigned char));
    if (length == -1)
    {
        printf("imu write failed!\n");
        exit(EXIT_FAILURE);
    }
    return length;
}

int readData(int fd, char *recv_buffer, int length)
{
    length = read(fd, recv_buffer, length);
    if (ret == -1)
    {
        printf("imu read failed!\n");
        exit(EXIT_FAILURE);
    }
    return length;
}

void usbInit()
{
    // 树莓派USB口号/dev/ttyUSBn
    fd_imu = uart_open(fd_imu, "/dev/ttyUSB1");
    if (fd_imu == -1)
    {
        fprintf(stderr, "imu_open error\n");
        exit(EXIT_FAILURE);
    }

    // 初始化串口（串口名、波特率、数据位、奇偶校验位、停止位）
    if (uart_set(fd_imu, BORD_IMU, 8, 'N', 1) == -1)
    {
        fprintf(stderr, "imu set failed!\n");
        exit(EXIT_FAILURE);
    }
}

string Time2Str()

{

    time_t tm;

    time(&tm); // 获取time_t类型的当前时间

    char tmp[64];

    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&tm));

    return tmp;
}

int main(void)
{
    char r_buf[imu_beiwei_read_size];
    bzero(r_buf, imu_beiwei_read_size);
    ofstream outFile;
    usbInit();
    outFile.open("file_test.txt"); // 打开文件
    while (1)
    {
        ret = writeData(fd_imu, accbyteSend, 5);
        usleep(50 * SLEEP_TIME_1_MS);
        bzero(r_buf, imu_beiwei_read_size);
        ret = readData(fd_imu, r_buf, imu_beiwei_read_size) != 0;
        ParseData(r_buf);
        string time = Time2Str();
        outFile << time.c_str()
                << "a0=" << a[0]
                << " a1=" << a[1]
                << " a2=" << a[2]
                << " w0=" << w[0]
                << " w1=" << w[1]
                << " w2=" << w[2]
                << endl;

        cout << time.c_str()
             << "a0=" << a[0]
             << " a1=" << a[1]
             << " a2=" << a[2]
             << " w0=" << w[0]
             << " w1=" << w[1]
             << " w2=" << w[2]
             << endl;
        usleep(50 * SLEEP_TIME_1_MS);
    }
    outFile.close();
}
