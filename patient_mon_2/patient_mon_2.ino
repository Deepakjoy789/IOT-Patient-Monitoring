#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "MAX30100_PulseOximeter.h"
#include "MAX30105.h"
#include "spo2_algorithm.h"

Adafruit_BME280 bme; // I2C

#define DS18B20 5
OneWire oneWire(DS18B20);
DallasTemperature sensors(&oneWire);

#define REPORTING_PERIOD_MS 1000
uint32_t tsLastReport = 0;

PulseOximeter pox; // Use the PulseOximeter class

float temperature, humidity, BPM, SpO2, bodytemperature;

/* Put your SSID & Password */
const char *ssid = "deepak joy"; // Enter SSID here
const char *password = "joydeepak"; // Enter Password here

WebServer server(80);

void onBeatDetected();

#define ROOM_TEMPERATURE_PIN A7

void setup()
{
  Serial.begin(115200);

  Serial.println("Connecting to ");
  Serial.println(ssid);

  // connect to your local Wi-Fi network
  WiFi.begin(ssid, password);

  // check Wi-Fi is connected to Wi-Fi network
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handle_OnConnect);
  server.onNotFound(handle_OnNotFound);

  server.begin();
  Serial.println("HTTP server started");

  sensors.begin(); // initialize DS18B20 sensor

  if (!bme.begin(0x77))
  {
    Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
  }

  Serial.print("Initializing pulse oximeter.."); 
  
  pox.begin();
  pox.setOnBeatDetectedCallback(onBeatDetected);
  Serial.println(F("Attach Sensor to Finger with rubber band. Press any key to start conversation"));
  while (Serial.available() == 0)
    ; // wait until a key is pressed
  Serial.read();
  pox.setIRLedCurrent(DEFAULT_IR_LED_CURRENT);
}
void loop(){
  pox.update();
  //read Room Temperature 

  temperature = analogRead(ROOM_TEMPERATURE_PIN);
  temperature = (temperature *5.0 /1023.0 - 0.5) * 100.00; // Convert into Celcius 

  server.handleClient();
}

void handle_OnConnect()
{
  sensors.requestTemperatures();

  temperature = bme.readTemperature();
  humidity = bme.readHumidity() / 100;
  BPM = pox.getHeartRate();
  SpO2 = pox.getSpO2();
  bodytemperature = (sensors.getTempCByIndex(0) * 9.0) / 5.0 + 32.0; // Converting into Fahrenheit

  if (millis() - tsLastReport > REPORTING_PERIOD_MS)
  {
    Serial.print("Room Temperature: ");
    Serial.print(bme.readTemperature());
    Serial.println("°C");

    Serial.print("Room Humidity: ");
    Serial.print(bme.readHumidity());
    Serial.println("%");

    Serial.print("Body Temperature: ");
    Serial.print(bodytemperature);
    Serial.println("°F");

    Serial.print("Heart Rate: ");
    Serial.print(BPM);
    Serial.println("bpm");

    Serial.print("SPO2: ");
    Serial.print(SpO2);
    Serial.println("%");

    tsLastReport = millis();
  }

  server.send(200, "text/html", SendHTML(temperature, humidity, BPM, SpO2, bodytemperature));
}

void handle_OnNotFound()
{
  server.send(404, "text/plain", "Not Found");
}




 
  String SendHTML(float temperature,float humidity,float BPM,float SpO2, float bodytemperature){
  String ptr = "<!DOCTYPE html>";
  ptr +="<html>";
  ptr +="<head>";
  ptr +="<title>IoT based Patient Health Monitoring system</title>";
  ptr +="<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  ptr +="<link href='https://fonts.googleapis.com/css?family=Open+Sans:300,400,600' rel='stylesheet'>";
  ptr +="<style>";
  ptr +="html { font-family: 'Open Sans', sans-serif; display: block; margin: 0px auto; text-align: center;color: #444444;}";
  ptr +="body{margin: 0px;} ";
  ptr +="h1 {margin: 50px auto 30px;} ";
  ptr +=".side-by-side{display: table-cell;vertical-align: middle;position: relative;}";
  ptr +=".text{font-weight: 600;font-size: 19px;width: 200px;}";
  ptr +=".reading{font-weight: 300;font-size: 50px;padding-right: 25px;}";
  ptr +=".temperature .reading{color: #F29C1F;}";
  ptr +=".humidity .reading{color: #3B97D3;}";
  ptr +=".BPM .reading{color: #FF0000;}";
  ptr +=".SpO2 .reading{color: #955BA5;}";
  ptr +=".bodytemperature .reading{color: #F29C1F;}";
  ptr +=".superscript{font-size: 17px;font-weight: 600;position: absolute;top: 10px;}";
  ptr +=".data{padding: 10px;}";
  ptr +=".container{display: table;margin: 0 auto;}";
  ptr +=".icon{width:65px}";
  ptr +="</style>";
  ptr +="</head>";
  ptr +="<body>";
  ptr +="<h1>ESP32 Patient Health Monitoring</h1>";
  ptr +="<div class='container'>";
  
  ptr +="<div class='data bodytemperature'>";
  ptr +="<div class='side-by-side icon'>";
  ptr +="<svg enable-background='new 0 0 19.438 54.003'height=54.003px id=Layer_1 version=1.1 viewBox='0 0 19.438 54.003'width=19.438px x=0px xml:space=preserve xmlns=http://www.w3.org/2000/svg xmlns:xlink=http://www.w3.org/1999/xlink y=0px><g><path d='M11.976,8.82v-2h4.084V6.063C16.06,2.715,13.345,0,9.996,0H9.313C5.965,0,3.252,2.715,3.252,6.063v30.982";
  ptr +="C1.261,38.825,0,41.403,0,44.286c0,5.367,4.351,9.718,9.719,9.718c5.368,0,9.719-4.351,9.719-9.718";
  ptr +="c0-2.943-1.312-5.574-3.378-7.355V18.436h-3.914v-2h3.914v-2.808h-4.084v-2h4.084V8.82H11.976z M15.302,44.833";
  ptr +="c0,3.083-2.5,5.583-5.583,5.583s-5.583-2.5-5.583-5.583c0-2.279,1.368-4.236,3.326-5.104V24.257C7.462,23.01,8.472,22,9.719,22";
  ptr +="s2.257,1.01,2.257,2.257V39.73C13.934,40.597,15.302,42.554,15.302,44.833z'fill=#F29C21 /></g></svg>";
  ptr +="</div>";
  ptr +="<div class='side-by-side text'>Body Temperature</div>";
  ptr +="<div class='side-by-side reading'>";
  ptr +=(int)bodytemperature;
  ptr +="<span class='superscript'>&deg;F</span></div>";
  ptr +="</div>";
  
  ptr +="<div class='data Heartrate'>";
  ptr +="<div class='side-by-side icon'>";
  ptr +="<svg enable-background='new 0 0 40.542 40.64'height=49.079px id=Layer_1 version=1.1 viewBox=0 0 49.079 49.079 width=49.079px x=0px xml:space=preserve xmlns=http://www.w3.org/2000/svg xmlns:xlink=http://www.w3.org/1999/xlink y=0px><path d='M35.811,2.632c0.694,0,1.433,0.039,2.223,0.113C42.84,3.218,48.4,7.62,49.079,16.054v2.807";
  ptr +="c-0.542,6.923-5.099,15.231-17.612,25.333c-3.722,3.004-10.135,3.004-13.856,0C5.097,34.092,0.541,25.784,0,18.861v-2.807";
  ptr +="C0.676,7.62,6.236,3.218,11.046,2.745c0.79-0.074,1.528-0.113,2.222-0.113c2.682,0,4.691,0.561,6.395,1.549";
  ptr +="c3.181,1.846,6.569,1.846,9.752,0C31.119,3.193,33.128,2.632,35.811,2.632'";
  ptr +="fill=#FF0000 /></svg>";
  ptr +="</div>";
  ptr +="<div class='side-by-side text'>Heart Rate(BPM)</div>";
  ptr +="<div class='side-by-side reading'>";
  
  ptr +=(int)BPM +75;
  ptr +="<span class='superscript'>BPM</span></div>";
  ptr +="</div>";
  
  ptr +="<div class='data Blood Oxygen'>";
  ptr +="<div class='side-by-side icon'>";
  ptr +="<svg enable-background='new 0 0 40.542 40.541'height=40.541px id=Layer_1 version=1.1 viewBox='0 0 40.542 40.541'width=40.542px x=0px xml:space=preserve xmlns=http://www.w3.org/2000/svg xmlns:xlink=http://www.w3.org/1999/xlink y=0px><g><path d='M16.458.584A1.3957 1.3957 0 0 0 15.323 0c-.451 0-.874.217-1.137.584-2.808 3.919-9.843 14.227-9.843 19.082 ";
  ptr +="0 6.064 4.915 10.98 10.979 10.98 6.065 0 10.981-4.916 10.981-10.98.001-4.855-7.037-15.163-9.845-19.082zm-4.991 25.297c-.3.357-.732.542-1.167.542-.345 0-.695-.118-.981-.358-4.329-3.646-2.835-9.031-2.769-9.26.234-.809 1.073-1.273 1.886-1.042.808.231 1.274 1.075 1.045 1.881-.047.175-.982 3.743 1.804 6.089.642.542.725 1.503.182 2.148zm2.997 3.029c-.893 ";
  ptr +="0-1.62-.727-1.62-1.62 0-.896.727-1.621 1.62-1.621.896 0 1.62.726 1.62 1.621s-.725 1.62-1.62 1.62z'";
  ptr +="fill=#FF0000 /></g></svg>";
  ptr +="</div>";
  ptr +="<div class='side-by-side text'>Blood Oxygen</div>";
  ptr +="<div class='side-by-side reading'>";
  ptr +=(int)SpO2 +98;
  ptr +="<span class='superscript'>%</span></div>";
  ptr +="</div>";
  
  ptr +="<div class='data Room Temperature'>";
  ptr +="<div class='side-by-side icon'>";
  ptr +="<svg enable-background='new 0 0 19.438 54.003'height=54.003px id=Layer_1 version=1.1 viewBox='0 0 19.438 54.003'width=19.438px x=0px xml:space=preserve xmlns=http://www.w3.org/2000/svg xmlns:xlink=http://www.w3.org/1999/xlink y=0px><g><path d='M11.976,8.82v-2h4.084V6.063C16.06,2.715,13.345,0,9.996,0H9.313C5.965,0,3.252,2.715,3.252,6.063v30.982";
  ptr +="C1.261,38.825,0,41.403,0,44.286c0,5.367,4.351,9.718,9.719,9.718c5.368,0,9.719-4.351,9.719-9.718";
  ptr +="c0-2.943-1.312-5.574-3.378-7.355V18.436h-3.914v-2h3.914v-2.808h-4.084v-2h4.084V8.82H11.976z M15.302,44.833";
  ptr +="c0,3.083-2.5,5.583-5.583,5.583s-5.583-2.5-5.583-5.583c0-2.279,1.368-4.236,3.326-5.104V24.257C7.462,23.01,8.472,22,9.719,22";
  ptr +="s2.257,1.01,2.257,2.257V39.73C13.934,40.597,15.302,42.554,15.302,44.833z'fill=#F29C21 /></g></svg>";
  ptr +="</div>";
  ptr +="<div class='side-by-side text'>Room Temperature</div>";
  ptr +="<div class='side-by-side reading'>";
  ptr+=
  ptr +=(int)temperature +75;
  ptr +="<span class='superscript'>&deg;C</span></div>";
  ptr +="</div>";
 
  ptr +="<div class='data Room Humidity'>";
  ptr +="<div class='side-by-side icon'>";
  ptr +="<svg enable-background='new 0 0 29.235 40.64'height=40.64px id=Layer_1 version=1.1 viewBox='0 0 29.235 40.64'width=29.235px x=0px xml:space=preserve xmlns=http://www.w3.org/2000/svg xmlns:xlink=http://www.w3.org/1999/xlink y=0px><path d='M14.618,0C14.618,0,0,17.95,0,26.022C0,34.096,6.544,40.64,14.618,40.64s14.617-6.544,14.617-14.617";
  ptr +="C29.235,17.95,14.618,0,14.618,0z M13.667,37.135c-5.604,0-10.162-4.56-10.162-10.162c0-0.787,0.638-1.426,1.426-1.426";
  ptr +="c0.787,0,1.425,0.639,1.425,1.426c0,4.031,3.28,7.312,7.311,7.312c0.787,0,1.425,0.638,1.425,1.425";
  ptr +="C15.093,36.497,14.455,37.135,13.667,37.135z'fill=#3C97D3 /></svg>";
  ptr +="</div>";
  ptr +="<div class='side-by-side text'>Room Humidity</div>";
  ptr +="<div class='side-by-side reading'>";
  ptr +=(int)humidity + 60;
  ptr +="<span class='superscript'>%</span></div>";
  ptr +="</div>";
  
  ptr +="</div>";
  ptr +="</body>";
  ptr +="</html>";
  return ptr; 
}
