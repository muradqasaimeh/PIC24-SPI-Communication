# PIC24 to PIC24 SPI Communication

This project demonstrates communication between two explorer 16 development board using Serial Peripheral Interface (SPI). The first PIC-24 (Master SPI) reads the Potentiometer value (range from 0-1023) and converts it into four characters and send over the SPI link. The second PIC-24 receives the four characters and parses them to control the LEDs flashing rate based on the received value.



![Alt text](./PIC_SPI.png?raw=true "PIC24 SPI Communication")
