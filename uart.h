#include <iostream>

class Uart
{
public:
    static bool uartInit(int &fd, const char *pathname, int bord);
    static int writeData(int fd, char *send_buffer, int length);
    static int readData(int fd, char *recv_buffer, int length);
    static void uart_close(int fd);

private:
    int uart_set();
};
