#include <EtherCard.h>
#include "ModbusIP_ENC28J60.h"
#include "Modbus.h"

//Modbus Registers
const int add_1 = 100;
const int add_2 = 101;
const int add_3 = 102;
const int add_4 = 103;

//random values for testing
#define val_1 12.2
#define val_2 14.5
#define val_3 16.8
#define val_4 17.3

//ModbusIP object
ModbusIP mb;


void setup() {
    // The media access control (ethernet hardware) address for the shield
    byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    // The IP address for the shield
    byte ip[] = { 192, 168, 1, 120 };
    //Config Modbus IP
    mb.config(mac, ip);

    // Add resgisters
    mb.addIreg(add_1);
    mb.addIreg(add_2);
    mb.addIreg(add_3);
    mb.addIreg(add_4);

    //assignong random values
    mb.Ireg(add_1, val_1);
    mb.Ireg(add_2, val_2);
    mb.Ireg(add_3, val_3);
    mb.Ireg(add_4, val_4);
}

void loop() {
   //Call once inside loop() - all magic here
   mb.task();
}
