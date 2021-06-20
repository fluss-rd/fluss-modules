//Este es el código principal del proyecto final, en este se incluirán todo el código de los demás sensores.

//INCLUSIÓN DE LIBRERÍAS.
//Para oxígeno disuelto total
#include <Arduino.h>

//Sim808 DFROBOT: Librería para utilizar el módulo SIM808, con funcionalidades de SMS y TCP get.
#include <DFRobot_sim808.h>

//Librerías para sensor de temperatura.
#include <OneWire.h>
#include <DallasTemperature.h>

//Librerías para sensor de pH.
#include "DFRobot_PH.h"
#include <EEPROM.h>

//Librerías para sensor de TDS.
#include "GravityTDS.h"

//Definición de número telefónico.
#define PHONE_NUMBER "8097207044"

//SECCIÓN DE SENSOR DE TEMPERATURA------------------------------------------------------------------------------------------------------------------------------------------------------------
const int pin_de_datos = 9;                         //Conexión de pin de datos en el Arduino UNO.
//Declaración de objetos.
OneWire oneWireObjeto(pin_de_datos);
DallasTemperature sensorDS18B20(&oneWireObjeto);
//Declaración de variables del sensor de temperatura.
float temperatura_valor = 0;
int verificar_temperatura = 0;

//SECCIÓN DE SENSOR DE TDS--------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define PIN_SENSOR_TDS A0             //Pin del sensor de TDS conectado al Arduino UNO.
GravityTDS gravityTds;
float TDS_valor = 0;

//SECCIÓN DE SENSOR DE pH---------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define PIN_SENSOR_PH A1              //Pin del sensor de pH conectado al Arduino UNO.
float Medicion_Sensor_pH = 0;
float pH_valor= 0;
DFRobot_PH ph;

//SECCIÓN DE SENSOR DE TURBIDEZ---------------------------------------------------------------------------------------------------------------------------------------------------------------
#define PIN_SENSOR_TURBIDEZ A2        //Pin del sensor de turbidez conectado al Arduino UNO. 
int informacion_turbidez = 0;         //Valor de 0-1023 del valor de opacidad del sensor.
float turbidez_voltaje = 0;           //Voltaje que se genera en el sensor.
float turbidez_valor = 0;             //Valor de opacidad en 0-100%.
float voltaje_turbidez_limpio = 4.53; //Valor de voltaje para agua limpia.

//SECCIÓN DE SENSOR DE TURBIDEZ---------------------------------------------------------------------------------------------------------------------------------------------------------------
#define DO_PIN A3
#define VREF 5000    //VREF (mv)
#define ADC_RES 1024 //ADC Resolution
//Single-point calibration Mode=0
//Two-point calibration Mode=1
#define TWO_POINT_CALIBRATION 1
float READ_TEMP = 0;//Current water temperature ℃, Or temperature sensor function
//Single point calibration needs to be filled CAL1_V and CAL1_T
#define CAL1_V (786) //mv
#define CAL1_T (27)   //℃
//Two-point calibration needs to be filled CAL2_V and CAL2_T
//CAL1 High temperature point, CAL2 Low temperature point
#define CAL2_V (932) //mv
#define CAL2_T (10)   //℃
const uint16_t DO_Table[41] = {
    14460, 14220, 13820, 13440, 13090, 12740, 12420, 12110, 11810, 11530,
    11260, 11010, 10770, 10530, 10300, 10080, 9860, 9660, 9460, 9270,
    9080, 8900, 8730, 8570, 8410, 8250, 8110, 7960, 7820, 7690,
    7560, 7430, 7300, 7180, 7070, 6950, 6840, 6730, 6630, 6530, 6410};

uint8_t Temperaturet;
uint16_t ADC_Raw;
uint16_t ADC_Voltage;
uint16_t DO;

int16_t readDO(uint32_t voltage_mv, uint8_t temperature_c)
{
#if TWO_POINT_CALIBRATION == 0
  uint16_t V_saturation = (uint32_t)CAL1_V + (uint32_t)35 * temperature_c - (uint32_t)CAL1_T * 35;
  return (voltage_mv * DO_Table[temperature_c] / V_saturation);
#else
  uint16_t V_saturation = (int16_t)((int8_t)temperature_c - CAL2_T) * ((uint16_t)CAL1_V - CAL2_V) / ((uint8_t)CAL1_T - CAL2_T) + CAL2_V;
  return (voltage_mv * DO_Table[temperature_c] / V_saturation);
#endif
}

/* Conexión de sensores en placa Arduino UNO.
 * Sensor de temperatura -------> PIN 9
 * Sensor de TDS ---------------> PIN A0
 * Sensor de pH ----------------> PIN A1
 * Sensor de Turbidez ----------> PIN A2
 * Sensor de Oxígeno Disuelto --> PIN A3
 */

//CONTENIDOS DEL MENSAJE.
String INDICADOR_SISTEMA = "SISTEMA#1:";
String LOCALIZACION_MENSAJE = "UB ";
String TEMPERATURA_MENSAJE = "TEMP*";
String PH_MENSAJE = ";pH?";
String TDS_MENSAJE = "/TDS+";
String TURBIDEZ_MENSAJE = ")TURB!";
String DO_MENSAJE = "(D.O%";
String FINAL_MENSAJE = "=";
char FINAL_MESSAGE_CHAR[200];

//Definición de tiempos.
unsigned long time_limit = 300000;   //Time expressed in milliseconds.
unsigned long time_passed = 0;       //Time passed in the system.
unsigned long actual_time;           //Actual time in the system.

DFRobot_SIM808 sim808(&Serial);
  
void setup() {
 
  Serial.begin(9600);
  //GPS
    //************* Turn on the GPS power************
    if( sim808.attachGPS())
        Serial.println("Open the GPS power success");
    else 
        Serial.println("Open the GPS power failure");
      
  //SENSOR DE TEMPERATURA.--------------------------------------------------------------------------------------------------------------------------------------------------------------------
    sensorDS18B20.begin();                //Iniciación de la comunicación con el sensor.
    sensorDS18B20.setResolution(12);      //Resolución de la medición de la temperatura con 12 bits.

  //SENSOR DE TDS-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    gravityTds.setPin(PIN_SENSOR_TDS);
    gravityTds.setAref(5.0);              //Voltaje de referencia del ACD del arudino Uno.
    gravityTds.setAdcRange(1024);         //1024 for 10bit ADC;4096 for 12bit ADC
    gravityTds.begin();                   //Inicialización del sensor TDS.

  //SENSOR DE pH------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    ph.begin();                           //Inicialización del sensor pH.
}

void loop() {
  
  //Verifica si el módulo SIM808 no está funcionando adecuadamente.
  while(!sim808.init()) {
      delay(1000);
      Serial.print("Error en SIM808. Verifique conexiones.");
      Serial.print(actual_time);
  }
  
  //Verifica si el módulo SIM808 está funcionando adecuadamente.
  while(sim808.init()){
      Serial.println("SIM808 Habilitado. Listo para enviar informacion.");
      
      //Verificar voltaje del panel solar.
      //Si voltaje del panel solar es menor que X, ajustar time_limit = 600000 (10 minutos como prueba en este caso).
      
      actual_time = millis();
      Serial.print(actual_time);

      //Si se cumple el tiempo establecido, se inicia la toma de información de los parámetros.
      if ((actual_time - time_passed) > time_limit) {

        //ETAPA DE LOCALIZACIÓN.---------------------------------------------------------------------------------------------------------------------------------------------------------------
        
        
        //ETAPA DE SENSOR DE TEMPERATURA.--------------------------------------------------------------------------------------------------------------------------------------------------------------------     
        sensorDS18B20.requestTemperatures();                  //Se hace request del valor de la temperatura de la sustancia.
        temperatura_valor = sensorDS18B20.getTempCByIndex(0); //Representar la temperatura en grados centígrados.
        String temperatura_String = String(temperatura_valor,2);  //Convertir temperatura numérica a string con dos puntos decimales.
        delay(10);

        //ETAPA DE SENSOR DE TDS-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        gravityTds.setTemperature(temperatura_valor);       //Realiza la compensación por temperatura.
        gravityTds.update();                                //Toma una muestra y determina el valor. 
        TDS_valor = gravityTds.getTdsValue();               //Solicitud de valor de TDS de la muestra.
        String TDS_String = String(TDS_valor,2);            //Convertir TDS numérica a string con dos puntos decimales.
        delay(10);
        
        //ETAPA DE SENSOR DE PH------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        Medicion_Sensor_pH = analogRead(PIN_SENSOR_PH)/1024.0*5000;  //Se efectua una lectura del voltaje que genera el sensor en el pin.
        pH_valor = ph.readPH(Medicion_Sensor_pH,temperatura_valor);  //Convierte el voltaje leído a pH, efectuando una compensación por temperatura. 
        String pH_String = String(pH_valor,2);                       //Convertir pH numérica a string con dos puntos decimales.
        ph.calibration(Medicion_Sensor_pH,temperatura_valor);        //Utilizada para calibrar el sensor pH.
        delay(10);
          
        //ETAPA DE SENSOR DE TURBIDEZ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        informacion_turbidez = analogRead(PIN_SENSOR_TURBIDEZ);                        //Se lee el voltaje en un rango de 0-1023.
        turbidez_voltaje = informacion_turbidez * (5.0 / 1024.0);                      //Se efectua una lectura del voltaje que genera el sensor en el pin.
        turbidez_valor = abs(100 - (turbidez_voltaje/voltaje_turbidez_limpio)*100);    //Se hace una conversión del voltaje medido a un rango de opacidad de 0-100%.
        String turbidez_String = String(turbidez_valor,2);                             //Convertir Opacidad numérica a string con dos puntos decimales.
        
        //ETAPA DE SENSOR DE OXÍGENO DISUELTO-------------------------------------------------------------------------------------------------------------------------------------------------
        Temperaturet = (uint8_t)temperatura_valor;
        ADC_Raw = analogRead(DO_PIN);
        ADC_Voltage = uint32_t(VREF) * ADC_Raw / ADC_RES;
        String DO_String = String(readDO(ADC_Voltage, Temperaturet));


        
        //ETAPA DE PREPARACIÓN DE MENSAJE SMS-------------------------------------------------------------------------------------------------------------------------------------------------
        String FINAL_MESSAGE = TEMPERATURA_MENSAJE + temperatura_String + PH_MENSAJE + pH_String + TDS_MENSAJE + TDS_String + TURBIDEZ_MENSAJE + turbidez_String + DO_MENSAJE + DO_String + FINAL_MENSAJE;
        FINAL_MESSAGE.toCharArray(FINAL_MESSAGE_CHAR,200);
        sim808.sendSMS(PHONE_NUMBER,FINAL_MESSAGE_CHAR); //Envía el mensaje SMS al número establecido.
        
        Serial.println("Mensaje enviado.");             //El mensaje fue enviado adecuadamente.
        time_passed = actual_time;                      //Ajustando un nuevo timer.
     }
  }
}
