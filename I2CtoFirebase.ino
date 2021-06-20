#include <Wire.h>
#include "Firebase_Arduino_WiFiNINA.h"

#define FIREBASE_HOST "arduinoconnect-7d092-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "7jw8bKcY48mddyN0cW42kX6hr7Vxry7Ks6ikA2Jz"
#define WIFI_SSID "CLAROJ83TW"
#define WIFI_PASSWORD "48575443259D839B"

FirebaseData firebaseData;

//Direcciones de los childs de la base de datos.
String path = "/Parametros/Data-";
String path_pH = "/pH";
String path_TEMP = "/TEMP";
String path_TDS = "/TDS";
String path_DO = "/DO";
String path_TURB = "/TURB";
String path_FECHA = "/FECHA";
String Fecha_cadena = "DEFAULT";
char mensaje_recibido[200];

int m; //Utilizado para identificar posición en mensaje_recibido.
int k = 1;
int k1 = 0;

//Declaración de índices de los contenedores de valores.
int indice1=0;
int indice2=0;
int indice3=0;
int indice4=0;
int indice5=0;
int indice6=0;
int indice7=0;
int indice8=0;

//Declaración de variables que almacenan valor recuperado de los parámetros.
float TEMP = 0;
float PH = 0;
float DO = 0;
float TURB = 0;
float TDS = 0;

void setup(){
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output
  delay(100);
  Serial.println();
  
  Serial.print("Conectando a Wi-Fi");
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED)
  {
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Conectado a Wi-Fi con la direccion IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  //Provide the autntication data
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, WIFI_SSID, WIFI_PASSWORD);
  Firebase.reconnectWiFi(true);
}

void loop(){
  delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany){
  
  m = 0;
  while(1 < Wire.available()){
    mensaje_recibido[m] = Wire.read(); //receive byte as a character
    m = m + 1;
  }
  //Convierte el mensaje recibido a String.
  String mensaje_parametros = String(mensaje_recibido);
  if(mensaje_parametros[0] != '='){
    //Identifica la posición de los indicadores de parámetros en el mensaje recibido.
    indice1 = mensaje_parametros.indexOf('*');
    indice2 = mensaje_parametros.indexOf(';');
    indice3 = mensaje_parametros.indexOf('?');
    indice4 = mensaje_parametros.indexOf('/');
    indice5 = mensaje_parametros.indexOf('+');
    indice6 = mensaje_parametros.indexOf(')');
    
    //Recupera la cadena que contiene el valor de de los parámetros.
    String TEMP_cadena = mensaje_parametros.substring(indice1+1,indice2);
    String PH_cadena = mensaje_parametros.substring(indice2+1,indice3);
    String TDS_cadena = mensaje_parametros.substring(indice3+1,indice4);
    String TURB_cadena = mensaje_parametros.substring(indice4+1,indice5);
    String DO_cadena = mensaje_parametros.substring(indice5+1,indice6);
  
    //Conversión de parámetros a valores flotantes.
    TEMP = TEMP_cadena.toFloat();
    PH = PH_cadena.toFloat();
    TDS = TDS_cadena.toFloat();
    DO = DO_cadena.toFloat();
    TURB = TURB_cadena.toFloat();
  
    //Acesso a Firebase.
    //Sube informacion de Temperatura a base de datos.
    if (Firebase.setFloat(firebaseData, path + k1 + k + path_TEMP, TEMP)){
    //Success, then read the payload value return from server
    //This confirmed that your data was set to database as float number
    if (firebaseData.dataType() == "float")
      Serial.println("Informacion de Temperatura subida adecuadamente a base de datos.");
      Serial.print("Informacion subida fue: ");
      Serial.println(firebaseData.floatData());
    }else {
    //Failed, then print out the error detail
      Serial.print("Error subiendo Temperatura: : ");
      Serial.println(firebaseData.errorReason());
    }
    
    //Sube informacion de pH a base de datos.
    if (Firebase.setFloat(firebaseData, path + k1 + k + path_pH, PH)){
    //Success, then read the payload value return from server
    //This confirmed that your data was set to database as float number
    if (firebaseData.dataType() == "float")
      Serial.println("Informacion de pH subida adecuadamente a base de datos.");
      Serial.print("Informacion subida fue: ");
      Serial.println(firebaseData.floatData());
    }else {
    //Failed, then print out the error detail
      Serial.print("Error subiendo pH: : ");
      Serial.println(firebaseData.errorReason());
    }
    
    //Sube informacion de Sólidos Disueltos Totales (TDS) a base de datos.
    if (Firebase.setFloat(firebaseData, path + k1 + k + path_TDS, TDS)){
    //Success, then read the payload value return from server
    //This confirmed that your data was set to database as float number
    if (firebaseData.dataType() == "float")
      Serial.println("Informacion de Sólidos Disueltos Totales (TDS) subida adecuadamente a base de datos.");
      Serial.print("Informacion subida fue: ");
      Serial.println(firebaseData.floatData());
    }else {
    //Failed, then print out the error detail
      Serial.print("Error subiendo TDS: : ");
      Serial.println(firebaseData.errorReason());
    }
    
    //Sube informacion de Oxígeno Disueltos Totales (D.O) a base de datos.
    if (Firebase.setFloat(firebaseData, path + k1 + k + path_DO, DO)){
    //Success, then read the payload value return from server
    //This confirmed that your data was set to database as float number
    if (firebaseData.dataType() == "float")
      Serial.println("Informacion de Oxígeno Disueltos Totales (D.O) subida adecuadamente a base de datos.");
      Serial.print("Informacion subida fue: ");
      Serial.println(firebaseData.floatData());
    }else {
    //Failed, then print out the error detail
      Serial.print("Error subiendo Oxigeno disuelto: : ");
      Serial.println(firebaseData.errorReason());
    }
    
    //Sube informacion de Turbidez a base de datos.
    if (Firebase.setFloat(firebaseData, path + k1 + k + path_TURB, TURB)){
    //Success, then read the payload value return from server
    //This confirmed that your data was set to database as float number
    if (firebaseData.dataType() == "float")
      Serial.println("Informacion de Turbidez subida adecuadamente a base de datos.");
      Serial.print("Informacion subida fue: ");
      Serial.println(firebaseData.floatData());
    }else {
    //Failed, then print out the error detail
      Serial.print("Error subiendo Turbidez: ");
      Serial.println(firebaseData.errorReason());
    }
    k = k + 1;
    if(k == 10){
      k = 0;
      k1 = k1 + 1;
      }
    //Información subida a Firebase.
    Serial.println("Mensaje recibido fue:");
    Serial.print("Temperatura: ");
    Serial.println(TEMP);
    Serial.print("pH: ");
    Serial.println(PH);
    Serial.print("TDS: ");
    Serial.println(TDS);
    Serial.print("Oxigeno Disuelto: ");
    Serial.println(DO);
    Serial.print("Turbidez: ");
    Serial.println(TURB);

    //Borrando toda la información del segundo mensaje recibido po I2C.
    for( int w = 0; w < sizeof(mensaje_recibido);  ++w ){
      mensaje_recibido[w] = (char)0;
    }
  }else{
    indice7 = mensaje_parametros.indexOf('=');
    indice8 = mensaje_parametros.indexOf('!');
    String Fecha_cadena = mensaje_parametros.substring(indice7+1,indice8);
    //Sube informacion de Temperatura a base de datos.
    if (Firebase.setString(firebaseData, path + k1 + k + path_FECHA, Fecha_cadena)){
    //Success, then read the payload value return from server
    //This confirmed that your data was set to database as float number
    if (firebaseData.dataType() == "string")
      Serial.println("Informacion de Fecha subida adecuadamente a base de datos.");
      Serial.print("Informacion subida fue: ");
      Serial.println(firebaseData.floatData());
    }else {
    //Failed, then print out the error detail
      Serial.print("Error subiendo Fecha: : ");
      Serial.println(firebaseData.errorReason());
    }
    Serial.println();
    Serial.print("Fecha: ");
    Serial.println(Fecha_cadena);
    }
}
