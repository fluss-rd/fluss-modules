/* Descripción general del código:
 * En esta tercera versión de SMS_Rx, se transmitirá la información de los parámetros al Arduino encargado de subir la información a la base de datos.
 * La idea principal es utilizar el protocolo de comunicación I2C, para no interferir con las operaciones del módulo SIM808, ya que utiliza los pines
 * Tx y Rx del Arduino UNO R3. Además, se recupera el tiempo de envío del mensaje, con esto se pretende no sobreescribir información ya recolectada en
 * la base de datos Firebase. 
 */
#include <DFRobot_sim808.h>
#include <Wire.h>

#define MESSAGE_LENGTH 160
char message[MESSAGE_LENGTH];
int messageIndex = 0;

char phone[11];
char datetime[24];
char numero_autorizado[11] = "8097207044";
String year = "20";

//Declaración de índices de los contenedores de valores.
int indice11=0,indice12=0;
int indice21=0,indice22=0;
int indice31=0,indice32=0;
int indice41=0,indice42=0;
int indice51=0,indice52=0;

//Declaración de variables que almacenan valor recuperado de los parámetros.
float TEMP = 0;
float PH = 0;
float DO = 0;
float TURB = 0;
float TDS = 0;

DFRobot_SIM808 sim808(&Serial);

void setup() {
   Wire.begin();        // join i2c bus (address optional for master)
  //mySerial.begin(9600);
  Serial.begin(9600);
  //******** Initialize sim808 module *************
  while(!sim808.init()) {
      Serial.println("Error de inicializacion");
      delay(1000);
  }
  delay(3000);
  Serial.println("Inicia enviando mensajes SMS.");
}

void loop() {
  //*********** Detecting unread SMS ************************
   messageIndex = sim808.isSMSunread();
    Serial.print("ID: ");
    Serial.println(messageIndex);

   //*********** At least, there is one UNREAD SMS ***********
   if (messageIndex > 0) {
      sim808.readSMS(messageIndex, message, MESSAGE_LENGTH, phone, datetime);
      
      //***********In order not to full SIM Memory, is better to delete it**********
      sim808.deleteSMS(messageIndex);

      //Identifica si el mensaje proviene de un número autorizado.
      if(phone[1] == numero_autorizado[1] && phone[2] == numero_autorizado[2] && phone[3] == numero_autorizado[3] && phone[4] == numero_autorizado[4] && phone[5] == numero_autorizado[5] && phone[6] == numero_autorizado[6] && phone[7] == numero_autorizado[7] && phone[8] == numero_autorizado[8] && phone[9] == numero_autorizado[9] && phone[10] == numero_autorizado[10]){
        
        //Ajustar fecha a formato adecuado para Firebase.
        String fecha = year + datetime;
        String anio = fecha.substring(0,4);
        String mes = fecha.substring(5,7);
        String dia = fecha.substring(8,10);
        String hora = fecha.substring(11,13);
        String minuto = fecha.substring(14,16);
        String segundo = fecha.substring(17,19);
   
        fecha.replace("/","-");
        fecha.replace(",",".");
        
        //Convierte el mensaje a String, para ser manipulado de mejor forma.
        String message_sms = message;
        
        //Identifica la posición de los indicadores de parámetros en el mensaje recibido.
        indice11 = message_sms.indexOf('*');
        indice12 = message_sms.indexOf(';');
        indice21 = message_sms.indexOf('?');
        indice22 = message_sms.indexOf('/');
        indice31 = message_sms.indexOf('+');
        indice32 = message_sms.indexOf(')');
        indice41 = message_sms.indexOf('!');
        indice42 = message_sms.indexOf('(');
        indice51 = message_sms.indexOf('%');
        indice52 = message_sms.indexOf('=');
        
        //Recupera la cadena que contiene el valor de de los parámetros.
        String TEMP_cadena = message_sms.substring(indice11+1,indice12);
        String PH_cadena = message_sms.substring(indice21+1,indice22);
        String TDS_cadena = message_sms.substring(indice31+1,indice32);
        String DO_cadena = message_sms.substring(indice41+1,indice42);
        String TURB_cadena = message_sms.substring(indice51+1,indice52);
        
        //Conversión de parámetros a valores flotantes.
        TEMP = TEMP_cadena.toFloat();
        PH = PH_cadena.toFloat();
        TDS = TDS_cadena.toFloat();
        DO = DO_cadena.toFloat();
        TURB = TURB_cadena.toFloat();

        //Inicia una conversación para enviar la información de fecha de recepción de mensaje.
        Wire.beginTransmission(4);
        Wire.write("=");
        Wire.write(anio.c_str());
        Wire.write("-");
        Wire.write(mes.c_str());
        Wire.write("-");
        Wire.write(dia.c_str());
        Wire.write(" ");
        Wire.write(hora.c_str());
        Wire.write(":");
        Wire.write(minuto.c_str());
        Wire.write(":");
        Wire.write(segundo.c_str());
        Wire.write("!");
        Wire.endTransmission();    //Finaliza transmisión de parámetros.
        
        //Preparar conexión I2C con Arduino MKR WiFi 1010, para subir los parámetros a Realtime Database Firebase.
        Wire.beginTransmission(4); //Transmitir información a dispositivo identificado como 4.
        Wire.write("*");
        Wire.write(TEMP_cadena.c_str());
        Wire.write(";");
        Wire.write(PH_cadena.c_str());
        Wire.write("?");
        Wire.write(TDS_cadena.c_str());
        Wire.write("/");
        Wire.write(DO_cadena.c_str());
        Wire.write("+");
        Wire.write(TURB_cadena.c_str());
        Wire.write(")");
        Wire.endTransmission();

        //Representación de mensaje que se envía al Arduino MKR WiFi 1010.
        Serial.println();
        Serial.println("Informacion recuperada de mensaje.");
        Serial.print("Fecha: ");Serial.println(fecha);
        Serial.print("Temperatura: ");Serial.println(TEMP);
        Serial.print("pH: ");Serial.println(PH);
        Serial.print("Oxigeno Disuelto: ");Serial.println(DO);
        Serial.print("Opacidad: ");Serial.println(TURB);
        Serial.print("TDS: ");Serial.println(TDS);
      }
      else{
        Serial.println();
        Serial.println("Numero no autorizado.");
        }
   }
}
