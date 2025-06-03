# PCB Design updates 


## V1.1 

![400](res/Pasted%20image%2020250128110349.png)

---
## Build guide 

- Follow the component list below and populate the PCB with components 
- Note 2 changes:
	- the 2N2222 needs to be replaced by a BS170 MOSFET
	- Bridge the Pins **D2** and **D3** on the Arduino
- Once you have every thing soldered in connect power via the USB C connector
	- You should see the power led light up on the arduino
	- Power led on the GPS module
- Once you have confirmed power to all the components follow the programming guide 

---
## Component list

| **Symbol** | **Value**    |
| ---------- | ------------ |
| C1         | 100n         |
| C2,C3      | 4.7u         |
| C3,C4      | pol smd      |
| D1         | 1n4001       |
| Q1         | 2N2222/BS170 |
| R1         | 6.2k         |
| R2         | 2k           |
| R3         | 270          |
| R4         | 3.9k         |
| R5         | 1.2k         |
| R6-R8      | 1k           |
| R10        | 2.7k         |
| R11,R12    | 330          |

---
## Program instructions

- Open up the Arduino code provided
- to install the Aprs library go to: Tools > Manage Libraries....
	- Search for `LibAPRS` and install the library by Baris DINC
- Once you have the library installed compile the program and it should compile without errors
- Change `line 45` to add your call
### Basic radio interface testing

- Once the Arduino is properly programmed on power on the Radio will be keyed up 2 times as a test. 
	- If this step fails: check your connectors and check your MOSFET.
- To test the system indoors without GPS lock uncomment `line 30`, this will enable a simulate mode where GPS data is being. artificially provided 
- Once the second count reaches the value set in `APRS_TX_offset_utc_sec` your radio will transmit an APRS packet.
	- Radio should key down automatically
	- It has been repoted with some radios this may not happen and this was often observed when there was an extended length of cable involved
		- adding a ferrite bead would potentially solve the issue
		- if not using a separate external antenna should help
- NOTE: change the `APRS_TX_offset_utc_sec` to some random value. This is the time your specific APRS unit will transmit. Since its on simplex we don't want every one to TX at the same time. 
- once you have completed testing comment out `line 30` congratulations now you have a working APRS beacon 

if you have questions please reach out to KE8TJE