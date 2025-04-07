
- The Bluetooth KISS TNC implementation schematic and board designs for quick access

## Schematic 

![](res/Pasted%20image%2020250407184734.png)

## Program instructions

- Firmware found [here](https://raw.githubusercontent.com/mobilinkd/tnc1/arduino/images/mobilinkd-473-arduino.hex)
- Programming using the com port: `avrdude -c arduino -p m328p -P /dev/ttyUSB1 -b 57600 -U mobilinkd-473-arduino.hex`
- Programming using a USBasp: `avrdude -p m328p -c usbasp -U flash:w:<filename>.hex`