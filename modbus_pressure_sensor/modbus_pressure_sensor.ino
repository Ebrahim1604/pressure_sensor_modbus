#include "Modbusrtu.h"

#define MasterModbusAdd  0
#define SlaveModbusAdd  1
#define RS485Serial 2
Modbus ControllinoModbusMaster(MasterModbusAdd, RS485Serial, 0);
modbus_t ModbusQuery[2];

unsigned long WaitingTime;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println("_____________Sensor values are displayed below:___________");
 /*
  Serial.println("Debugging connection: ");
  
  int u = get_reg(63);
  Serial.print("Test1: Holding reg 63 current value = ");
  Serial.println(u);
  Serial.println("Test2: writing value 20 to Holding reg 63");
  set_reg(63,20);
  u = get_reg(63);
  Serial.print("Holding reg 63 changed value =");
  Serial.println(u);
  
  int e = ControllinoModbusMaster.getLastError();

  if (e==255){Serial.println("Error: Time-out, no response from slave");}
  else if (e==1){Serial.println("Error: Function code not available");}
  else if (e==2){Serial.println("Error: Address beyond available space for Modbus registers ");}
  else if (e==3){Serial.println("Error: Coils or registers number beyond the available space");}
  else {Serial.println("No error in communication");}
  */
  delay(4000);
}

void loop() {
  // put your main code here, to run repeatedly:
  display_diff_pressure();
  display_static_pressure();
  display_process_temp();
  display_sensor_temp();
}


void display_sensor_temp(void)
{
  union
    {
        double floatVal;
        uint16_t bytes[2];
    }floatConverter;
    
    floatConverter.bytes[0]= get_reg(400); 
    floatConverter.bytes[1]= get_reg(399);
  
    /*
    Serial.print("1st word = ");
    Serial.print(floatConverter.bytes[0]);
    Serial.print(" 2nd word = ");
    Serial.println(floatConverter.bytes[1]);
    */
    
    double fo = floatConverter.floatVal;
    Serial.print("Sensor Module temp = ");
    Serial.print(fo);
    
    int u = get_reg(63);

    if (u == 20)
    {
      Serial.println(" degree C");
    }
    
    else if (u == 21)
    {
      Serial.println(" degree F");
    }

    else
    {
      Serial.println("Unknown units");
     }

}

void display_diff_pressure()
{
  union
    {
        double floatVal;
        uint16_t bytes[2];
    }floatConverter;
    
    floatConverter.bytes[0]= get_reg(402); 
    floatConverter.bytes[1]= get_reg(401);
  
    /*
    Serial.print("1st word = ");
    Serial.print(floatConverter.bytes[0]);
    Serial.print(" 2nd word = ");
    Serial.println(floatConverter.bytes[1]);
    */
    
    double fo = floatConverter.floatVal;
    Serial.print("diff pressure = ");
    Serial.print(fo);
    
    int u = get_reg(60);

    if (u == 1)
    {Serial.println(" inh20 @60F"); }

    else if (u == 2)
    {Serial.println(" Pa"); }

    else if(u == 3)
    {Serial.println(" KPa"); }

    else if(u == 4)
    {Serial.println(" MPa"); }

    else if(u == 5)
    {Serial.println(" psi"); }

    else if(u == 6)
    {Serial.println(" inh20 @68F"); }

    else if(u == 7)
    {Serial.println(" bar"); }

    else if(u == 8)
    {Serial.println(" mbar"); }

    else if(u == 9)
    {Serial.println(" g/cm2"); }

    else if (u == 10)
    {Serial.println(" kg/cm2"); }

    else if (u == 11)
    {Serial.println(" inHg"); }

    else if(u == 12)
    {Serial.println(" fth20"); }

    else if(u == 13)
    {Serial.println(" torr"); }

    else if(u == 14)
    {Serial.println(" atm"); }

    else if(u == 15)
    {Serial.println(" mmh20"); }

    else if (u == 16)
    {Serial.println(" mmHg"); }

    else if (u == 238)
    {Serial.println(" inh20@4C"); }

    else if (u == 239)
    {Serial.println(" mmh20@4C"); }

    else
    {Serial.println(" Unknown units"); }
  }

void display_static_pressure()
{
  union
    {
        double floatVal;
        uint16_t bytes[2];
    }floatConverter;
    
    floatConverter.bytes[0]= get_reg(404); 
    floatConverter.bytes[1]= get_reg(403);
  
    /*
    Serial.print("1st word = ");
    Serial.print(floatConverter.bytes[0]);
    Serial.print(" 2nd word = ");
    Serial.println(floatConverter.bytes[1]);
    */
    
    double fo = floatConverter.floatVal;
    Serial.print("static pressure = ");
    Serial.print(fo);
    
    int u = get_reg(61);

    if (u == 1)
    {Serial.println(" inh20 @60F"); }

    else if (u == 2)
    {Serial.println(" Pa"); }

    else if(u == 3)
    {Serial.println(" KPa"); }

    else if(u == 4)
    {Serial.println(" MPa"); }

    else if(u == 5)
    {Serial.println(" psi"); }

    else if(u == 6)
    {Serial.println(" inh20 @68F"); }

    else if(u == 7)
    {Serial.println(" bar"); }

    else if(u == 8)
    {Serial.println(" mbar"); }

    else if(u == 9)
    {Serial.println(" g/cm2"); }

    else if (u == 10)
    {Serial.println(" kg/cm2"); }

    else if (u == 11)
    {Serial.println(" inHg"); }

    else if(u == 12)
    {Serial.println(" fth20"); }

    else if(u == 13)
    {Serial.println(" torr"); }

    else if(u == 14)
    {Serial.println(" atm"); }

    else if(u == 15)
    {Serial.println(" mmh20"); }

    else if (u == 16)
    {Serial.println(" mmHg"); }

    else if (u == 238)
    {Serial.println(" inh20@4C"); }

    else if (u == 239)
    {Serial.println(" mmh20@4C"); }

    else
    {Serial.println(" Unknown units"); }

  }

void display_process_temp()
{
  union
    {
        double floatVal;
        uint16_t bytes[2];
    }floatConverter;
    
    floatConverter.bytes[0]= get_reg(406); 
    floatConverter.bytes[1]= get_reg(405);
  
    /*
    Serial.print("1st word = ");
    Serial.print(floatConverter.bytes[0]);
    Serial.print(" 2nd word = ");
    Serial.println(floatConverter.bytes[1]);
    */
    
    double fo = floatConverter.floatVal;
    Serial.print("Process temp = ");
    Serial.print(fo);
    
    int u = get_reg(62);

    if (u == 20)
    {
      Serial.println(" degree C");
    }
    
    else if (u == 21)
    {
      Serial.println(" degree F");
    }

    else
    {
      Serial.println("Unknown units");
     }

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
