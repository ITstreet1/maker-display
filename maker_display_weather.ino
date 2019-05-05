#include <Maker_LED_Matrix.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
Maker_LED_Matrix matrix(MAKER_LED_MATRIX_2);

const char* ssid     = "*******************";      // SSID lokalne mreze
const char* password = "*******************";   // lozinka
String APIKEY = "**************************"; // API kljuc
String CityID = "***************************"; //ID mesta za koji nam trebaju ocitavanja
WiFiClient client;
char servername[]="api.openweathermap.org";  //udaljeni server na koji se povezujemo
String result;
int  counter = 1;
String weatherDescription ="";
String weatherLocation = "";
String Country;
String Temperature;
String Humidity;
String Pressure;
String Wind;

void setup() {
  Serial.begin(9600);
  matrix.begin(&matrix);
  WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("Connected");
  matrix.brightness(20,0);
  matrix.message("Connected!", 200, 2, 1); //povezani smo
  delay(3000);
  

}

void loop() {
  if(counter == 1){ //prikupljanje podataka svakih deset minuta orig 60
      counter = 0;
      displayGettingData(); 
      delay(1000);
      getWeatherData();
  }else{
      counter++;
      displayWeather(weatherLocation,weatherDescription);//lokacija za gde prikazujemo podatke
      delay(5000);
      displayConditions(Temperature,Humidity,Pressure,Wind); // podaci o vremenu
      delay(5000);
  } 
}
void displayGettingData(){
  matrix.message("Getting data", 200, 2, 1);
  delay(4000);
}
void getWeatherData() //funkcija za slanje i primanje GET zahteva
{
  if (client.connect(servername, 80)) {  //proveravamo konekciju
    client.println("GET /data/2.5/weather?id="+CityID+"&units=metric&APPID="+APIKEY);
    client.println("Host: api.openweathermap.org");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
  } 
  else {
    Serial.println("connection failed"); //ako iste nema ide eror poruka
    Serial.println();
  }

  while(client.connected() && !client.available()) delay(1); //cekamo podatke
  while (client.connected() || client.available()) { //sve dok podataka ima
    char c = client.read();
      result = result+c;
    }

  client.stop();
  result.replace('[', ' ');
  result.replace(']', ' ');
  Serial.println(result);

char jsonArray [result.length()+1];
result.toCharArray(jsonArray,sizeof(jsonArray));
jsonArray[result.length() + 1] = '\0';
// pristupamo root-u
StaticJsonBuffer<1024> json_buf; // biblioteka 5.xx
JsonObject &root = json_buf.parseObject(jsonArray);

if (!root.success()){
  Serial.println("parseObject() failed");
}

//prikupljamo podatke koji nam trebaju. ovde se mogu dodati ostali zahtevi
String location = root["name"];
String country = root["sys"]["country"];
float temperature = root["main"]["temp"];
float humidity = root["main"]["humidity"];
String weather = root["weather"]["main"];
String description = root["weather"]["description"];
float pressure = root["main"]["pressure"];
float wind = root["wind"]["speed"];

weatherDescription = description;
weatherLocation = location;
Country = country;
Temperature = temperature;
Humidity = humidity;
Pressure = pressure;
Wind = wind;
}
void displayWeather(String location,String description){ //podaci o lokaciji
  matrix.deleteScroll();
  matrix.message((char*)location.c_str(),150, 2, 1);
  delay(5000);
  matrix.message((char*)Country.c_str(),150, 2, 1);
  delay(2000);
  matrix.message((char*)description.c_str(),150, 2, 1);
  delay(3000);
}
void displayConditions(String Temperature,String Humidity, String Pressure, String Wind){// podaci koje prikazujemo
  matrix.deleteScroll();
  //prikaz temperature
  matrix.message("Temperatura",150, 2, 1);
  delay(7000);
  matrix.message((char*)Temperature.c_str(),150, 2, 1);
  delay(4500);
  matrix.message("C",150, 2, 1);
  delay(2500);
  //prikaz vlaznosti vazduha
  matrix.message("Relativna vlaznost vazduha",150, 2, 1);
  delay(14000);
  matrix.message((char*)Humidity.c_str(),150, 2, 1);
  delay(3000);
  matrix.message("%",150, 2, 1);
  delay(2000);
  //prikaz vazdusnog pritiska
  matrix.message("Vazdusni pritisak",150, 2, 1);
  delay(10000);
  matrix.message((char*)Pressure.c_str(),150, 2, 1);
  delay(4000);
  matrix.message("mbar",150, 2, 1);
  delay(2500);
  //prikaz vazdusnog pritiska
  matrix.message("Brzina vetra",150, 2, 1);
  delay(8000);
  matrix.message((char*)Wind.c_str(),150, 2, 1);
  delay(3000);
  matrix.message("m/s",150, 2, 1);
  delay(2000);
  
}
