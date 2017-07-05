EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:teensy
LIBS:switches
LIBS:ControllerCarrier-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Teensy3.6 U1
U 1 1 595D3D6B
P 5650 3500
F 0 "U1" H 5650 5800 60  0000 C CNN
F 1 "Teensy3.6" H 5650 1200 60  0000 C CNN
F 2 "teensy:Teensy35_36" H 5650 3500 60  0001 C CNN
F 3 "" H 5650 3500 60  0000 C CNN
	1    5650 3500
	1    0    0    -1  
$EndComp
$Comp
L SW_Push SW1
U 1 1 595D40C4
P 7400 2700
F 0 "SW1" H 7450 2800 50  0000 L CNN
F 1 "SW_Push" H 7400 2640 50  0000 C CNN
F 2 "w_switch:PCB_PUSH" H 7400 2900 50  0001 C CNN
F 3 "" H 7400 2900 50  0001 C CNN
	1    7400 2700
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X06 J1
U 1 1 595D41AC
P 7850 1550
F 0 "J1" H 7850 1900 50  0000 C CNN
F 1 "OLED_128x32" V 7950 1550 50  0000 C CNN
F 2 "oled:OLED_128x32_i2c" H 7850 1550 50  0001 C CNN
F 3 "" H 7850 1550 50  0001 C CNN
	1    7850 1550
	0    -1   -1   0   
$EndComp
Text GLabel 4650 2600 0    31   Input ~ 0
LED_DAT
Text GLabel 4650 5300 0    31   Input ~ 0
LED_CLK
Text GLabel 4650 1400 0    31   Input ~ 0
GND
Text GLabel 6650 4700 2    31   Input ~ 0
VIN
Text GLabel 4650 1500 0    31   Input ~ 0
BTN_A
Text GLabel 4650 1600 0    31   Input ~ 0
BTN_B
Text GLabel 4650 1700 0    31   Input ~ 0
BTN_C
Text GLabel 7600 1750 3    31   Input ~ 0
OLED_SDA
Text GLabel 7700 1750 3    31   Input ~ 0
OLED_SCL
Text GLabel 7800 1750 3    31   Input ~ 0
OLED_RST
Text GLabel 7900 1750 3    31   Input ~ 0
GND
Text GLabel 8100 1750 3    31   Input ~ 0
OLED_33V
NoConn ~ 8000 1750
Text GLabel 6650 4900 2    31   Input ~ 0
OLED_33V
Text GLabel 6650 5500 2    31   Input ~ 0
OLED_SDA
Text GLabel 6650 5400 2    31   Input ~ 0
OLED_SCL
Text GLabel 6650 5300 2    31   Input ~ 0
OLED_RST
Text GLabel 7200 2700 0    31   Input ~ 0
BTN_A
Text GLabel 7600 2700 2    31   Input ~ 0
GND
$Comp
L SW_Push SW2
U 1 1 595D53C1
P 7400 3000
F 0 "SW2" H 7450 3100 50  0000 L CNN
F 1 "SW_Push" H 7400 2940 50  0000 C CNN
F 2 "w_switch:PCB_PUSH" H 7400 3200 50  0001 C CNN
F 3 "" H 7400 3200 50  0001 C CNN
	1    7400 3000
	1    0    0    -1  
$EndComp
Text GLabel 7200 3000 0    31   Input ~ 6
BTN_B
Text GLabel 7600 3000 2    31   Input ~ 0
GND
$Comp
L SW_Push SW3
U 1 1 595D53EB
P 7400 3300
F 0 "SW3" H 7450 3400 50  0000 L CNN
F 1 "SW_Push" H 7400 3240 50  0000 C CNN
F 2 "w_switch:PCB_PUSH" H 7400 3500 50  0001 C CNN
F 3 "" H 7400 3500 50  0001 C CNN
	1    7400 3300
	1    0    0    -1  
$EndComp
Text GLabel 7200 3300 0    31   Input ~ 0
BTN_C
Text GLabel 7600 3300 2    31   Input ~ 0
GND
$Comp
L CONN_01X04 J3
U 1 1 595D552C
P 3450 3500
F 0 "J3" H 3450 3750 50  0000 C CNN
F 1 "LED_OUT" V 3550 3500 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x04_Pitch2.54mm" H 3450 3500 50  0001 C CNN
F 3 "" H 3450 3500 50  0001 C CNN
	1    3450 3500
	0    -1   -1   0   
$EndComp
Text GLabel 3400 3700 3    31   Input ~ 0
LED_DAT
Text GLabel 3500 3700 3    31   Input ~ 0
LED_CLK
Text GLabel 3600 3700 3    31   Input ~ 0
GND
Text GLabel 3300 3700 3    31   Input ~ 0
VIN
$Comp
L CONN_01X02 J2
U 1 1 595D56BD
P 3450 2750
F 0 "J2" H 3450 2900 50  0000 C CNN
F 1 "BATT_IN" V 3550 2750 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 3450 2750 50  0001 C CNN
F 3 "" H 3450 2750 50  0001 C CNN
	1    3450 2750
	0    -1   -1   0   
$EndComp
Text GLabel 3400 2950 3    31   Input ~ 0
VIN
Text GLabel 3500 2950 3    31   Input ~ 0
GND
$Comp
L +BATT #PWR01
U 1 1 595D5B29
P 3250 2850
F 0 "#PWR01" H 3250 2700 50  0001 C CNN
F 1 "+BATT" H 3250 2990 50  0000 C CNN
F 2 "" H 3250 2850 50  0001 C CNN
F 3 "" H 3250 2850 50  0001 C CNN
	1    3250 2850
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR02
U 1 1 595D5B4B
P 3650 2850
F 0 "#PWR02" H 3650 2600 50  0001 C CNN
F 1 "GND" H 3650 2700 50  0000 C CNN
F 2 "" H 3650 2850 50  0001 C CNN
F 3 "" H 3650 2850 50  0001 C CNN
	1    3650 2850
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3250 2850 3250 2950
Wire Wire Line
	3250 2950 3400 2950
Wire Wire Line
	3650 2850 3650 2950
Wire Wire Line
	3650 2950 3500 2950
Text GLabel 4650 5200 0    31   Input ~ 0
GND
$EndSCHEMATC
