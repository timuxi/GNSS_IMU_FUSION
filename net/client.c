#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <assert.h>
#include <termios.h>
#include <sys/time.h>
#include <time.h>

#define PORT "25329" // the port client will be connecting to
#define IP "103.46.128.53"
#define MAXDATASIZE 20 // max number of bytes we can get at once

int sockfd, numbytes;
char buf[MAXDATASIZE];
char r_buf[MAXDATASIZE];
static int ret;
static int fd_net;

#define BORD_NET 115200

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

int uart_set(int fd_imu, int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio, oldtio;
    /*保存测试现有串口参数设置，在这里如果串口号等出错，会有相关的出错信息*/
    if (tcgetattr(fd_imu, &oldtio) != 0)
    {
        perror("SetupSerial 1");
        printf("tcgetattr(fd_imu,&oldtio) -> %d\n", tcgetattr(fd_imu, &oldtio));
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
    tcflush(fd_imu, TCIFLUSH);
    /*激活新配置*/
    if ((tcsetattr(fd_imu, TCSANOW, &newtio)) != 0)
    {
        perror("com set error");
        return -1;
    }
    printf("set done!\n");
    return 0;
}

int uart_close(int fd_imu)
{
    assert(fd_imu);
    close(fd_imu);
    /*可以在这里做些清理工作*/
    return 0;
}

int writeData(int fd_imu, char *send_buffer, int length)
{
    length = write(fd_imu, send_buffer, length * sizeof(unsigned char));
    return length;
}

int readData(int fd, char *recv_buffer, int length)
{
    length = read(fd, recv_buffer, length);
    return length;
}

// get sockaddr, IPv4 or IPv6
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

// uart初始化
void uartInit()
{
    fd_net = uart_open(fd_net, "/dev/ttyUSBnet");
    if (fd_net == -1)
    {
        fprintf(stderr, "uart2net error\n");
        exit(EXIT_FAILURE);
    }

    //初始化串口（串口名、波特率、数据位、奇偶校验位、停止位）
    if (uart_set(fd_net, BORD_NET, 8, 'N', 1) == -1)
    {
        fprintf(stderr, "uart set failed!\n");
        exit(EXIT_FAILURE);
    }
}

void *recvMag()
{
    while (1)
    {
        bzero(buf, MAXDATASIZE);
        if ((numbytes = recv(sockfd, buf, sizeof buf, 0)) == -1)
        {
            perror("recv");
            exit(1);
        }
        ret = writeData(fd_net, buf, numbytes);
        if (ret == -1)
        {
            printf("imu write failed!\n");
            exit(EXIT_FAILURE);
        }
        //如果收到的数据不为0,则将其打印
        if (ret != 0)
        {
            printf("recveMsg :");
            for (int i = 0; i < ret; i++)
            {
                printf("%c", buf[i]);
            }
            printf("\n");
        }
    }
}

int main(int argc, char *argv[])
{
    bzero(r_buf, MAXDATASIZE);

    uartInit();

    // int sockfd, numbytes;
    // char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    pthread_t t1;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if ((rv = getaddrinfo(IP, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo:%s\n", gai_strerror(rv));
        return 1;
    }
    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("client:socket");
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("client:connect");
            continue;
        }
        break;
    }
    if (p == NULL)
    {
        fprintf(stderr, "client:failed to connect\n");
        return 2;
    }
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    printf("client:connecting to %s\n", s);
    freeaddrinfo(servinfo); // all done with this structure

    int err = pthread_create(&t1, NULL, recvMag, NULL);
    if (err != 0)
    {
        printf("receive failed");
        exit(1);
    }

    while (1)
    {
        bzero(r_buf, MAXDATASIZE);
        ret = readData(fd_net, r_buf, MAXDATASIZE);
        if (ret == -1)
        {
            fprintf(stderr, "uart read failed!\n");
            exit(EXIT_FAILURE);
        }

        //如果发送成功,则将其打印
        if (ret != 0)
        {
            if (send(sockfd, r_buf, ret, 0) == -1)
            {
                printf("send failed!\n");
            }
            else
            {
                printf("sendMsg :");
                for (int i = 0; i < ret; i++)
                {
                    printf("%c", buf[i]);
                }
                printf("\n");
            }
        }
    }

    return 0;
}
