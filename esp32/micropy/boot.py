# This file is executed on every boot (including wake-boot from deepsleep)
#import esp
#esp.osdebug(None)
import webrepl
import network
webrepl.start()
sta_if = network.WLAN(network.STA_IF); sta_if.active(True)
sta_if.connect("lukn23","12345678")
#sta_if.connect("CLARO5GDCC6B8","azUmycWqc7")

#uart = machine.UART(1, 9600, rx=12, tx=13)
#uart.init(9600, bits=8, parity=None, stop=1)


#test test