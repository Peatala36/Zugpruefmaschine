import serial
import time

arduino = serial.Serial(port="/dev/ttyUSB0", baudrate=9600, timeout=.1)

def write_read(x):
    arduino.write(bytes(x, 'utf-8'))
    time.sleep(0.05)
    data = arduino.readline()
    return data

while True:
    num = input("Befehl an die Zugprüfmaschine: ")
    arduino.write(bytes(num, 'utf-8'))
    time.sleep(0.05)
    #value = write_read(num)
    #print(arduino.readline())
