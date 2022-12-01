# This file is executed on every boot (including wake-boot from deepsleep)
#import esp
#esp.osdebug(None)
import machine
import webrepl
import network
import time
from machine import Pin
webrepl.start()

Pin(4, Pin.OUT).value(1)
time.sleep(0.1)
Pin(4, Pin.OUT).value(0)
time.sleep(0.1)

#try:
sta_if = network.WLAN(network.STA_IF); sta_if.active(True)
try:
  sta_if.connect("INTELBRAS","LESC12345")
except Exception as e:
  print("conn", e)
#except Exception as e:
#    print("-> ", e)
#sta_if.connect("lukn23","12345678")
#sta_if.connect("CLARO5GDCC6B8","azUmycWqc7")
#sta_if.connect("BIOINFO-IC","b10cp3g1")
time.sleep(1)
print("end boot")
