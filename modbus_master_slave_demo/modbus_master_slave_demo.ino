#include <EtherCard.h>
#include "ModbusIP_ENC28J60.h"
#include "Modbusmasterrtu.h"

//Modbus Registers
const int add_1 = 30001;
const int add_2 = 30002;
const int add_3 = 30003;
const int add_4 = 30004;

#define MasterModbusAdd  0
#define SlaveModbusAdd  1
#define RS485Serial 2
Modbus_ ControllinoModbusMaster(MasterModbusAdd, RS485Serial, 0);
modbus_t ModbusQuery[2];

//ModbusIP object
ModbusIP mb;

#define time_interval 500.0 // interval in which queries will be checked in milliseconds

unsigned long WaitingTime;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  // The IP address for the shield
  byte ip[] = { 192, 168, 7, 120 };
  //Config Modbus IP
  mb.config(mac, ip);

  Serial.begin(9600);
  set_reg(60, 3);
  set_reg(61, 3);

  // Add resgisters
  mb.addIreg(add_1);
  mb.addIreg(add_2);
  mb.addIreg(add_3);
  mb.addIreg(add_4);

  set_timer(time_interval); // time in millisecs
}

void loop()
{
  float val_1 = display_diff_pressure();
  float val_2 = display_static_pressure();
  float val_3 = display_process_temp();
  float val_4 = display_sensor_temp();

  Serial.print(val_1);
  Serial.print(",");
  Serial.print(val_2);
  Serial.print(",");
  Serial.print(val_3);
  Serial.print(",");
  Serial.println(val_4);
  
  mb.Ireg(add_1, val_1);
  mb.Ireg(add_2, val_2);
  mb.Ireg(add_3, val_3);
  mb.Ireg(add_4, val_4);
 
}

float display_sensor_temp(void)
{
  union
    {
        double floatVal;
        uint16_t bytes[2];
    }floatConverter;
    
    floatConverter.bytes[0]= get_reg(400); 
    floatConverter.bytes[1]= get_reg(399);
    
    double fo = floatConverter.floatVal;
   
     return fo;

}

float display_diff_pressure()
{
  union
    {
        double floatVal;
        uint16_t bytes[2];
    }floatConverter;
    
    floatConverter.bytes[0]= get_reg(402); 
    floatConverter.bytes[1]= get_reg(401);
  
    double fo = floatConverter.floatVal;
  
    return fo;
  }

 
float display_static_pressure()
{
  union
    {
        double floatVal;
        uint16_t bytes[2];
    }floatConverter;
    
    floatConverter.bytes[0]= get_reg(404); 
    floatConverter.bytes[1]= get_reg(403);
  
    double fo = floatConverter.floatVal;
  
   return fo;
  }

float display_process_temp()
{
  union
    {
        double floatVal;
        uint16_t bytes[2];
    }floatConverter;
    
    floatConverter.bytes[0]= get_reg(406); 
    floatConverter.bytes[1]= get_reg(405);
  
    double fo = floatConverter.floatVal;
   
    return fo;
  }

void set_timer(float _time)
{ 
  noInterrupts();
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  // set compare match register for given time value
  unsigned long desired_time = (unsigned long)((_time*16000.0/1024.0) - 1.0);
  TCNT1  = 0; //initialize counter value to desred time interval
  OCR1A = desired_time;
  
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  interrupts();
  
}

ISR(TIMER1_COMPA_vect)
 {
  mb.task();
 }

int get_reg(uint16_t addr)
{
  uint16_t curr_reg_val[1];
  
  ModbusQuery[0].u8id = SlaveModbusAdd;
  ModbusQuery[0].u8fct = 3;
  ModbusQuery[0].u16RegAdd = addr - 1;
  ModbusQuery[0].u16CoilsNo = 1;
  ModbusQuery[0].au16reg = curr_reg_val;

  ControllinoModbusMaster.begin( 9600 );
  ControllinoModbusMaster.setTimeOut( 5000 );

  WaitingTime = millis() + 1000;

  while (millis() > WaitingTime);

  ControllinoModbusMaster.query( ModbusQuery[0] );

again1:
  ControllinoModbusMaster.poll();
  if (ControllinoModbusMaster.getState() == COM_IDLE)
  {
    Serial2.end();
    return (int)curr_reg_val[0];
  }
  else
  {
   goto again1;
  } 
    
}

void set_reg(uint16_t addr, int reg_val)
{
  uint16_t req_reg_val[1];
  
  ModbusQuery[1].u8id = SlaveModbusAdd;
  ModbusQuery[1].u8fct = 6;
  ModbusQuery[1].u16RegAdd = addr - 1;
  ModbusQuery[1].u16CoilsNo = 1;
  ModbusQuery[1].au16reg = req_reg_val;
  req_reg_val[0] = reg_val;

  ControllinoModbusMaster.begin( 9600 );
  ControllinoModbusMaster.setTimeOut( 5000 );

  WaitingTime = millis() + 1000;

  while (millis() > WaitingTime);

  ControllinoModbusMaster.query( ModbusQuery[1] );

again2:
  ControllinoModbusMaster.poll();
  if (ControllinoModbusMaster.getState() == COM_IDLE)
  {
    Serial2.end();
    //Serial.println("Speed changed!");
    return;
  }
  else
  {
   goto again2;
  } 
}
