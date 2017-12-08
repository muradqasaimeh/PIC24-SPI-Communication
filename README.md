# PIC24 to PIC24 SPI Communication

This project demonstrates the Serial Peripheral Interface (SPI) communication between two explorer 16 development board. The first PIC-24 (Master SPI) reads the Potentiometer value (range from 0-1023) and converts it into four characters and send over the SPI link. The second PIC-24 receives the four characters and parses them to control the LEDs flashing rate based on the received value.

Demo video: https://www.youtube.com/watch?v=nH9QJKTZFVc



![Alt text](./PIC_SPI.png?raw=true "PIC24 SPI Communication")
