# This file is executed on every boot (including wake-boot from deepsleep)
#import esp
#esp.osdebug(None)
import webrepl
import network
import time
import utime
import machine

webrepl.start()

def disconnect():
    try:
     #  disconnects STA, even if it is not connected
        sta_if.disconnect()
    except:
        pass
    sta_if.active(False)        
    utime.sleep(1)
    

sta_if = network.WLAN(network.STA_IF) # create station interface


def connect():
    count = 0
    utime.sleep(1)
    if not sta_if.isconnected():
        print('connecting to hotspot...')
        sta_if.active(True)
        #sta_if.ifconfig((config.WiFi_device, '255.255.255.0', config.gateway, '8.8.8.8'))
        sta_if.connect("BIOINFO-IC","b10cp3g1")

        while (count < 5):
            count += 1

            if (sta_if.isconnected()):
                count = 0
                print (' network config:', sta_if.ifconfig())
                break

            print ('.', end = '')
            time.sleep(1)


    if (count == 5):
        try:
            print('failed to connect' + '\n')
        except OSError:
            pass

     #  disconnect or you get errors
        disconnect()

    count = 0 #  reset count

    utime.sleep(1)


def disconnect():
    try:
     #  disconnects STA, even if it is not connected
        sta_if.disconnect()
    except:
        pass
    sta_if.active(False)        
    utime.sleep(1)
    
    
sta_if = network.WLAN(network.STA_IF)   #  create station interface

while True:
    print("start")
    disconnect()
    print("s1")
    time.sleep(1)
    print("s2")
    connect()
    print("s3")
    time.sleep(5)
    print("s4")
    disconnect()
    print ('disconnected')
    time.sleep(5)
    print("s5")
    
#time.sleep(5)
#print("connected", sta_if.ifconfig())