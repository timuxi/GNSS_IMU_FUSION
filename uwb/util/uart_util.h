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

class UartUtil
{
public:
    static int usbInit(string port, int bord);
    static int writeData(int fd, char *send_buffer, int length);
    static int readData(int fd, char *recv_buffer, int length);
    static int uart_close(int fd);

private:
    static int uart_open(int fd, const char *pathname);
    static int uart_set(int fd, int nSpeed, int nBits, char nEvent, int nStop);
};