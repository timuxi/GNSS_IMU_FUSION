import os
import datetime
import time
import RPi.GPIO as GPIO

from FileUtil import FileUtil

GPIO.setwarnings(False)
SPEED_PUL_PIN = 13  # 9
DIRECTION_PIN = 15  # 8

pulse = 1000
dx = 2 / pulse  # 每一次脉冲移动的距离（cm）
move_distance = 10  # cm
current_location = 0   # 表示当前在0刻度线位置
direction = True  # True 20-> 0移动

move_speed_1_s = 0.0018873 # 表示移动1cm需要1s
move_speed_100_ms = 0.00011105 # 表示移动1cm需要0.1s

current_path = os.path.dirname(os.path.abspath(__file__)).join("data")
file_name = "GT" + datetime.datetime.now().strftime('%Y_%m_%d_%H_%M_%S') + ".csv"
fileutil = FileUtil(path=current_path, filename=file_name)

def move(distance, sleepTime=0.001):
    global current_location
    pulse_num = int(distance / dx)
    d_str = "20->0" if direction else "0->20"
    flag = -1 if direction==True else 1
    current_location = current_location + (pulse_num * dx) * flag
    if current_location > 20:
        print(current_location)
        exit()
    elif current_location<0:
        print(current_location)
        exit()

    tt = time.time_ns()
    for i in range(int(pulse_num)):
        turn_on_gpio(SPEED_PUL_PIN)
        time.sleep(sleepTime)
        turn_off_gpio(SPEED_PUL_PIN)
    ttt = (time.time_ns() - tt)/(1000 * 1000 * 1000)
    print("移动距离: ",distance,"cm，移动方向：", d_str,"time: (s)",ttt)


def turn_on_gpio(PIN):
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(PIN, GPIO.OUT)
    GPIO.output(PIN, GPIO.HIGH)


def turn_off_gpio(PIN):
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(PIN, GPIO.OUT)
    GPIO.output(PIN, GPIO.LOW)


def setDirection(start, end):
    global direction
    if start < end:
        direction = False
    elif start > end:
        direction = True
    else:
        print("ERR! start = :",start,"end = :",end)
        exit()
    if direction:
        turn_off_gpio(DIRECTION_PIN)
    else:
        turn_on_gpio(DIRECTION_PIN)

def changeDirection():
    global direction
    if direction:
        direction = False
    else:
        direction = True
    if direction:
        turn_off_gpio(DIRECTION_PIN)
    else:
        turn_on_gpio(DIRECTION_PIN)


def Time2Str():
    now = datetime.datetime.utcnow()
    current_time = int(now.timestamp() * 10)
    today_start = datetime.datetime(now.year, now.month, now.day)
    today_start_timestamp = int(today_start.timestamp() * 10)
    duration_sec = (current_time - today_start_timestamp) / 10
    return "{:.1f}".format(duration_sec)



def one_circle():
    for i in range(20):
        tt = datetime.now()
        move(distance=1, sleepTime=move_speed_1_s)
        print("start: ", tt, "end: ", datetime.now())

def init():
    global current_location
    current_location = int(input("请输入当前位置（cm）以数字表示："))
    start = current_location
    end = int(input("end= "))
    setDirection(start=start,end=end)
    time.sleep(0.1)

if __name__ == "__main__":
    init()
    for k in range(60):
        for i in range(20):
            speed = move_speed_1_s if i%2==0 else move_speed_100_ms
            move(distance=1, sleepTime=move_speed_100_ms)
            fileutil.write(Time2Str()+ "," +str(current_location)+"\n")
        changeDirection()