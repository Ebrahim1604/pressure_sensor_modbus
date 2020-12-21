#include <EtherCard.h>

#include "Modbusmasterrtu.h"

#include "AgaHelper_Arduino.h"
#include "Aga3.h"
#include "Aga8.h"

#include "ModbusIP_ENC28J60.h"

//Modbus Registers
const int add_1 = 30001;
const int add_2 = 30002;
const int add_3 = 30003;
const int add_4 = 30004;
const int add_5 = 30005;
const int add_6 = 30006;
const int add_7 = 30007;
const int add_8 = 30008;

unsigned long samplerate = 10; // counter

#define MasterModbusAdd  0
#define SlaveModbusAdd  1
#define RS485Serial 2
Modbus_ ControllinoModbusMaster(MasterModbusAdd, RS485Serial, 0);
modbus_t ModbusQuery[2];
unsigned long WaitingTime;


//ModbusIP object
ModbusIP mb;

//unsigned long samplerate = 5; // 10 min counter
#define Meter 23456 // Meter number will change

double t_sp = 0.0;
double t_dp = 0.0;
double t_bc = 0.0;
double t_c = 0.0;
double t_fc = 0.0;
double t_sg = 0.0;
double t_f = 0.0;
double* arr_v;
double bc,c,fc,sg,f;

//inital values in first 10 mins:
double f_sp = 0.0;
double f_dp = 0.0;
double f_bc = 0.0;
double f_c = 0.0;
double f_fc = 0.0;
double f_sg = 0.0;
double f_f = 0.0;

double a,b;

// SETUP VARIABLES
  double Helium_He_v = 0;
  double Nitrogen_N2_v = 0.0348 * 0.01;
  double CarbonDioxide_CO2_v = 2.9718 * 0.01;
  double Methane_C1_v = 96.8113 * 0.01;
  double Ethane_C2_v = 0.1753 * 0.01;
  double Propane_C3_v = 0.0068 * 0.01;
  double iButane_iC4_v = 0;
  double nButane_nC4_v = 0;
  double nPentane_nC5_v = 0;
  double iPentane_iC5_v = 0;
  double nHexane_C6_v = 0;
  double nHeptane_C7_v = 0;

  double flowTemperatureInCelcius_v = 15.56; // 60F
  double pipeReferenceTemperatureInCelcius_v = 15.56; // 60F
  double orificeReferenceTemperatureInCelcius_v = 15.56; // 60F
  double baseTemperatureInCelcius_v = 15.56; // 60F
  double baseStaticPressureKPA_v = 101.325;
  double orificeSizeMM_v = 31.75; // 1.250
  double pipeSizeMM_v = 77.9272; //3.068
  double baseCompressibility_v = 1.0021; // COMPRESSIBLITY FACTOR 
  double specificGravity_v = 0.5846; // REAL SPECIFIC GRAVITY FROM SHEET
  
  double flowTemperatureInCelcius = flowTemperatureInCelcius_v;
  double pipeReferenceTemperatureInCelcius = pipeReferenceTemperatureInCelcius_v;
  double orificeReferenceTemperatureInCelcius = orificeReferenceTemperatureInCelcius_v;
  double baseTemperatureInCelcius = baseTemperatureInCelcius_v; 
  double baseStaticPressureKPA = baseStaticPressureKPA_v;

  double orificeSizeMM = orificeSizeMM_v;
  double pipeSizeMM = pipeSizeMM_v;
  AgaConstants_MaterialType orificeMaterial = StainlessSteel;
  AgaConstants_MaterialType pipeMaterial = CarbonSteel;
  bool tapIsUpstream = true;

  double gasComps[GAS_COMPS_SIZE] = {0};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);

  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  // The IP address for the shield
  byte ip[] = { 192, 168, 7, 120 };
  //Config Modbus IP
  mb.config(mac, ip);

  set_reg(60, 3);
  set_reg(61, 3);

  mb.addIreg(add_1);
  mb.addIreg(add_2);
  mb.addIreg(add_3);
  mb.addIreg(add_4);
  mb.addIreg(add_5);
  mb.addIreg(add_6);
  mb.addIreg(add_7);
  mb.addIreg(add_8);
  
  mb.Ireg(add_1, f_sp);
  mb.Ireg(add_2, f_dp);
  mb.Ireg(add_3, f_bc);
  mb.Ireg(add_4, f_c);
  mb.Ireg(add_5, f_fc);
  mb.Ireg(add_6, f_sg);
  mb.Ireg(add_7, f_f);
  mb.Ireg(add_8, Meter);
  

  gasComps[Helium_He] = Helium_He_v;
  gasComps[Nitrogen_N2] = Nitrogen_N2_v;
  gasComps[CarbonDioxide_CO2] = CarbonDioxide_CO2_v;
  gasComps[Methane_C1] = Methane_C1_v;
  gasComps[Ethane_C2] = Ethane_C2_v;
  gasComps[Propane_C3] = Propane_C3_v;
  gasComps[iButane_iC4] = iButane_iC4_v;
  gasComps[nButane_nC4] = nButane_nC4_v;
  gasComps[nPentane_nC5] = nPentane_nC5_v;
  gasComps[iPentane_iC5] = iPentane_iC5_v;
  gasComps[nHexane_C6] = nHexane_C6_v;
  gasComps[nHeptane_C7] = nHeptane_C7_v;

Serial.println("wait for sometime");
  int time_test = 10;
  unsigned long time_start = micros();
  for(int i = 0; i<time_test; i++)
  {
    mb.task(); // Keep
    a = get_static_pressure();
    b = get_diff_pressure();
    t_sp = t_sp + a;
    mb.task(); // Keep
    t_dp = t_dp + b;
    mb.task(); // Keep
    arr_v = AGA3Calc(a,b);
    bc = arr_v[0];
    mb.task();
    c = arr_v[1];
    mb.task();
    fc = arr_v[2];
    mb.task();
    sg = arr_v[3];
    mb.task();
    f = arr_v[4];
    mb.task();
    t_bc = t_bc + bc;
    t_c = t_c + c;
    t_fc = t_fc + fc;
    t_sg = t_sg + sg;
    t_f = t_f + f;
    //mb.task();
  }
 unsigned long time_end = micros();
  double dur = time_end-time_start;
  Serial.print("Time = ");
  Serial.println(dur/time_test);
  
}

void loop() {
  // put your main code here, to run repeatedly:
 /* 
  update_registers();
  calculate_parameters();
  update_values();
  */
}

void update_registers()
{
  mb.Ireg(add_1, f_sp);
  mb.Ireg(add_2, f_dp);
  mb.Ireg(add_3, f_bc);
  mb.Ireg(add_4, f_c);
  mb.Ireg(add_5, f_fc);
  mb.Ireg(add_6, f_sg);
  mb.Ireg(add_7, f_f);
  mb.Ireg(add_8, Meter);
  //mb.task();
}

void update_values()
{
  f_sp = t_sp/samplerate;
  f_dp = t_dp/samplerate;
  f_bc = t_bc/samplerate;
  f_c = t_c/samplerate;
  f_fc = t_fc/samplerate;
  f_sg = t_sg/samplerate;
  f_f = t_f/samplerate;
  mb.task();
  
  t_sp = 0.0;
  t_dp = 0.0;
  t_bc = 0.0;
  t_c = 0.0;
  t_fc = 0.0;
  t_sg = 0.0;
  t_f = 0.0;
  //mb.task();
}
 
double get_diff_pressure()
{
  union
    {
        float doubleVal;
        uint16_t bytes[2];
    }doubleConverter;
    
    doubleConverter.bytes[0]= get_reg(402); 
    doubleConverter.bytes[1]= get_reg(401);
  
    double fo = (double)doubleConverter.doubleVal;
  
    return fo;
  }

 
double get_static_pressure()
{
  union
    {
        float doubleVal;
        uint16_t bytes[2];
    }doubleConverter;
    
    doubleConverter.bytes[0]= get_reg(404); 
    doubleConverter.bytes[1]= get_reg(403);
  
    double fo = (double)doubleConverter.doubleVal;
  
   return fo;
  }

double* AGA3Calc(double a, double b)
{ 
  double staticPressureKPA = a;
  //mb.task();
  double differentialPressureKPA = b;
  //mb.task();
  Aga8Result calcZResult = Aga8_CalculateZ(gasComps, UnitConverter_CELCIUStoKELVIN(flowTemperatureInCelcius), 
    UnitConverter_KPAtoPSI(staticPressureKPA));
  double FlowZCalc = calcZResult.FlowCompressiblity;
  //mb.task();
  double flowCompressibility = FlowZCalc;
  double baseCompressibility = baseCompressibility_v;
  double specificGravity = specificGravity_v;
  //mb.task();
  Aga3Result result = Aga3_CalculateFlow(flowTemperatureInCelcius, pipeReferenceTemperatureInCelcius, orificeReferenceTemperatureInCelcius,
    baseTemperatureInCelcius, staticPressureKPA, baseStaticPressureKPA, differentialPressureKPA, orificeSizeMM, pipeSizeMM, orificeMaterial,
    pipeMaterial, tapIsUpstream, flowCompressibility, baseCompressibility, specificGravity);
   //mb.task(); 

   static double arr[5];
   arr[0] = result.BaseCompressibility_Zb;
   arr[1] = result.Compressibility_FPV;
   arr[2] = result.FlowingCompressibility_Zf;
   arr[3] = result.SpecificGravity;
   arr[4] = result.Flow;
  Serial.println("math");

   return arr;
}

int get_reg(uint16_t addr)
{
  uint16_t curr_reg_val[1];
  
  ModbusQuery[0].u8id = SlaveModbusAdd;
  ModbusQuery[0].u8fct = 3;
  ModbusQuery[0].u16RegAdd = addr - 1;
  ModbusQuery[0].u16CoilsNo = 1;
  ModbusQuery[0].au16reg = curr_reg_val;

  ControllinoModbusMaster.begin( 57600 );
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

  ControllinoModbusMaster.begin( 57600 );
  ControllinoModbusMaster.setTimeOut( 5000 );

  WaitingTime = millis() + 1000;

  while (millis() > WaitingTime);

  ControllinoModbusMaster.query( ModbusQuery[1] );

again2:
  ControllinoModbusMaster.poll();
  if (ControllinoModbusMaster.getState() == COM_IDLE)
  {
    Serial2.end();
    return;
  }
  else
  {
   goto again2;
  } 
}
