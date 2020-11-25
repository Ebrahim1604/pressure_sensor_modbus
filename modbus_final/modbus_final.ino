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

#define MasterModbusAdd  0
#define SlaveModbusAdd  1
#define RS485Serial 2
Modbus_ ControllinoModbusMaster(MasterModbusAdd, RS485Serial, 0);
modbus_t ModbusQuery[2];
unsigned long WaitingTime;


//ModbusIP object
ModbusIP mb;

unsigned long samplerate = 9; // 600 results = 10 minutes, 9 is for testing

#define Meter 23456 // Meter number will change

float t_sp = 0.0;
float t_dp = 0.0;
float t_bc = 0.0;
float t_c = 0.0;
float t_fc = 0.0;
float t_sg = 0.0;
float t_f = 0.0;
float bc,c,fc,sg,f;

//inital values in first 10 mins:
float f_sp = 0.0;
float f_dp = 0.0;
float f_bc = 0.0;
float f_c = 0.0;
float f_fc = 0.0;
float f_sg = 0.0;
float f_f = 0.0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

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
}

void loop() {
  // put your main code here, to run repeatedly:
  
  update_registers();
  calculate_parameters();
  update_values();

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
}

void calculate_parameters()
{
  for(unsigned long i = 0; i<samplerate; i++)
  {
    t_sp = t_sp + (float)get_static_pressure();
    t_dp = t_dp + (float)get_diff_pressure();
    bc,c,fc,sg,f = AGA3Calc();
    t_bc = t_bc + bc;
    t_c = t_c + c;
    t_fc = t_fc + fc;
    t_sg = t_sg + sg;
    t_f = t_f + f;
    mb.task();
  }
   
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
}
 
float get_diff_pressure()
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

 
float get_static_pressure()
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

float AGA3Calc()
{
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
  
  double gasComps[GAS_COMPS_SIZE] = {0};
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
  
  double flowTemperatureInCelcius = flowTemperatureInCelcius_v;
  double pipeReferenceTemperatureInCelcius = pipeReferenceTemperatureInCelcius_v;
  double orificeReferenceTemperatureInCelcius = orificeReferenceTemperatureInCelcius_v;
  double baseTemperatureInCelcius = baseTemperatureInCelcius_v;
  double staticPressureKPA = 50.01; // get_static_pressure(); <<- I am calling this to get the value, you stated I shouldn't call this in the function. 50.01 is an example to test
  double baseStaticPressureKPA = baseStaticPressureKPA_v;
  double differentialPressureKPA = 31.6; // get_diff_pressure(); <<- I am calling this to get the value, you stated I shouldn't call this in the function. 31.6 is an example to test
  double orificeSizeMM = orificeSizeMM_v;
  double pipeSizeMM = pipeSizeMM_v;
  AgaConstants_MaterialType orificeMaterial = StainlessSteel;
  AgaConstants_MaterialType pipeMaterial = CarbonSteel;
  bool tapIsUpstream = true;

  Aga8Result calcZResult = Aga8_CalculateZ(gasComps, UnitConverter_CELCIUStoKELVIN(flowTemperatureInCelcius), 
    UnitConverter_KPAtoPSI(staticPressureKPA));
  double FlowZCalc = calcZResult.FlowCompressiblity;
  
  double flowCompressibility = FlowZCalc;
  double baseCompressibility = baseCompressibility_v;
  double specificGravity = specificGravity_v;

  Aga3Result result = Aga3_CalculateFlow(flowTemperatureInCelcius, pipeReferenceTemperatureInCelcius, orificeReferenceTemperatureInCelcius,
    baseTemperatureInCelcius, staticPressureKPA, baseStaticPressureKPA, differentialPressureKPA, orificeSizeMM, pipeSizeMM, orificeMaterial,
    pipeMaterial, tapIsUpstream, flowCompressibility, baseCompressibility, specificGravity);
    
  return (float)result.BaseCompressibility_Zb,(float)result.Compressibility_FPV,(float)result.FlowingCompressibility_Zf,(float)result.SpecificGravity,(float)result.Flow;
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
