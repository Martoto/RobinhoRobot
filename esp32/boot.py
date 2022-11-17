# This file is executed on every boot (including wake-boot from deepsleep)
#import esp
#esp.osdebug(None)
import webrepl
import network
import time
webrepl.start()

sta_if = network.WLAN(network.STA_IF)
sta_if.active(True)

if not sta_if.isconnected():
    print("connecting")
    #sta_if.connect("INTELBRAS","LESC12345")
    sta_if.connect("BIOINFO-IC","b10cp3g1")
    

while not sta_if.isconnected():
    print("connecting wwauit")
    time.sleep(0.5)

#time.sleep(5)
#print("connected", sta_if.ifconfig())
