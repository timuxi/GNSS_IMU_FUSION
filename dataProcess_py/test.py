import serial  # 导入串口包
import time  # 导入时间包

ser = serial.Serial("COM3", 115200, timeout=5)  # 开启com3口，波特率115200，超时5
ser.flushInput()  # 清空缓冲区


def main():
    while True:
        count = ser.inWaiting()  # 获取串口缓冲区数据
        if count != 0:
            recv = ser.read(ser.in_waiting).decode("gbk")  # 读出串口数据，数据采用gbk编码
            print(time.time(), " --- recv --> ", recv)  # 打印一下子
        time.sleep(0.1)  # 延时0.1秒，免得CPU出问题


if __name__ == '__main__':
    main()