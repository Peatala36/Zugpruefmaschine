import serial
import time
import sys, signal
from tkinter import *

import tkinter.font as tkf

mywin = Tk()

#arduino = serial.Serial(port="/dev/ttyUSB0", baudrate=9600, timeout=.1)

def write_read(x):
    #arduino.write(bytes(x, 'utf-8'))
    time.sleep(0.05)
    #data = arduino.readline()
    return data

#while True:
    #num = input("Befehl an die Zugprüfmaschine: ")
    #arduino.write(bytes(num, 'utf-8'))
    #time.sleep(0.05)
    #value = write_read(num)
    #print(arduino.readline())

def btn_toStart_click():
    try:
        arduino.write(bytes(0, 'utf-8'))
    except:
        pass

def btn_messung_ZV_click():
    try:
        arduino.write(bytes(3, 'utf-8'))
    except:
        pass
    
def btn_messung_AZ_click():
    try:
        arduino.write(bytes(4, 'utf-8'))
    except:
        pass

def win_close():
    mywin.quit()

def strg_c(signal, frame):
    mywin.quit()

def do_nothing():
    mywin.after(200, do_nothing)

def main():
    mywin.protocol("WM_DELETE_WINDOW", win_close)
    mywin.wm_title('Zugprüfung')
    myfont = tkf.Font(family='DejaVuSans', size=20)
    btn_toStart = Button(mywin, text="Fahrt zur Nullposition", command=btn_toStart_click)
    btn_messung_ZV = Button(mywin, text="Start Messung Z-L-Diagramm", command=btn_messung_ZV_click)
    btn_messung_AZ = Button(mywin, text="Start Messung Anfangszugkraft", command=btn_messung_AZ_click)
    
    btn_toStart.pack()
    btn_messung_ZV.pack()
    btn_messung_AZ.pack()

    signal.signal(signal.SIGINT, strg_c)
    mywin.after(200, do_nothing)
    
    mywin.mainloop()

main()
