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
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

static int ret;
static int data_size = 1024;
static int fd;
#define BORD 115200

//串口名、波特率、数据位、奇偶校验位、停止位
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
    case 'O': //奇数
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'e':
    case 'E': //偶数
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'n':
    case 'N': //无奇偶校验位
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

int uart_close(int fd)
{
    assert(fd);
    close(fd);
    /*可以在这里做些清理工作*/
    return 0;
}

int writeData(int fd, char *send_buffer, int length)
{
    length = write(fd, send_buffer, length * sizeof(unsigned char));
    return length;
}

int readData(int fd, char *recv_buffer, int length)
{
    length = read(fd, recv_buffer, length);
    return length;
}

string getTime()
{
    time_t now1 = time(NULL);
    tm *tm_t = localtime(&now1);
    stringstream ss;
    ss << tm_t->tm_year + 1900 << ((tm_t->tm_mon + 1) > 9 ? "-" : "-0") << tm_t->tm_mon + 1 << ((tm_t->tm_mday) > 9 ? "-" : "-0") << tm_t->tm_mday
       << ((tm_t->tm_hour) > 9 ? " " : " 0") << tm_t->tm_hour << ((tm_t->tm_min) > 9 ? ":" : ":0") << tm_t->tm_min << ((tm_t->tm_sec) > 9 ? ":" : ":0") << tm_t->tm_sec;
    return ss.str();
}

int uart_open(int fd, const char *pathname)

{
    fd = open(pathname, O_RDWR | O_NOCTTY);
    if (-1 == fd)
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
    return fd;
}
void usbInit()
{
    // USB口号/dev/ttyUSBn
    fd = uart_open(fd, "/dev/ttyUSBnet");
    if (fd == -1)
    {
        fprintf(stderr, "gps_open error\n");
        exit(EXIT_FAILURE);
    }

    //初始化串口（串口名、波特率、数据位、奇偶校验位、停止位）
    if (uart_set(fd, BORD, 8, 'N', 1) == -1)
    {
        fprintf(stderr, "gps set failed!\n");
        exit(EXIT_FAILURE);
    }
}

int main(void)
{
    char r_buf[data_size];
    // usb初始化
    usbInit();

    ofstream OutFile(getTime() + ".log");

    while (1)
    {
        bzero(r_buf, data_size);
        ret = readData(fd, r_buf, data_size);
        string str = r_buf;
        OutFile << str.substr(0, ret);
    }

    OutFile.close();
    ret = uart_close(fd);
    if (ret == -1)
    {
        printf("uart close error\n");
    }
    exit(EXIT_SUCCESS);
}
