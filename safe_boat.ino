#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Wire.h>
#define THRESHOLD   300
TinyGPSPlus gps;  // The TinyGPS++ object
SoftwareSerial ss(D6, D5); // The serial connection to the GPS device
const char* ssid = "Rwanda-ltd";
const char* password = "VRT0931VRT";
float latitude , longitude;
int year , month , date, hour , minute , second;
String date_str , time_str , lat_str , lng_str,speed_str;
int pm;
int buttonPin=D0;
int buzzer =  D1; 
#define SIGNAL_PIN A0
//int numreadings = 10;
float income;
int buttonState = 0; 
String latarray[20];
String lngarray[20];
//String current_latarray[10], current_lngarray[10], previous_latarray[10], previous_lngarray[10] ;
unsigned int i = 0;
const unsigned long Interval = 13000;
unsigned long previousTime = 0;
const int pingPin = D7; // Trigger Pin of Ultrasonic Sensor
const int echoPin = D8; // Echo Pin of Ultrasonic Sensor
int value,water_level,emergance =0;
float wter_presence;
WiFiServer server(80);
WiFiClient wifiClient;
void setup()
{pinMode(buzzer, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  pinMode(SIGNAL_PIN, OUTPUT);
  Serial.begin(115200);
  ss.begin(9600);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  server.begin();
  Serial.println("Server started");
  // Print the IP address
  Serial.println(WiFi.localIP());

}

void loop()
{  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
    digitalWrite(buzzer, HIGH);
    emergance=1;
  } else {
    // turn LED off:
    digitalWrite(buzzer, LOW);
    emergance=0;
  }
   value = analogRead(A0); // read the analog value from sensor
   Serial.print("Sensor value: ");
  Serial.println(value);
  
  if (value > THRESHOLD) 
  {
    Serial.print("The Boat linkage detected");
    wter_presence=1;
    digitalWrite(buzzer, HIGH);
  }
  else 
  {
  Serial.println("The Boat is not linking");
  wter_presence=0;
  digitalWrite(buzzer, LOW);
  }
  sonicSensor();
  unsigned long currentTime = millis();
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
    { 
     if (currentTime - previousTime >= Interval) 
     { 

      if (gps.location.isValid())
      {
        //Serial.print("Getting Data");
        latitude = gps.location.lat();
        lat_str = String(latitude , 6);
        longitude = gps.location.lng();
        lng_str = String(longitude , 6);
        speed_str = String(gps.speed.kmph());
        latarray[i] = lat_str;
        lngarray[i]= lng_str;
        i++;
        Serial.print(i);
        if (i>=20)
       // {
          i=0; //reset to beginning of array, so you don't try to save readings outside of the bounds of the array
        // } 
        Serial.println("Latitude:");
        Serial.println(latarray[i]);
        Serial.println("Longitude:");
        Serial.println(lngarray[i]);
        Serial.println();
       }
       }            
       previousTime = currentTime;
      //}
      if (gps.date.isValid())
      {
        //Serial.print("Getting Time");
        date_str = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();
        if (date < 10)
          date_str = '0';
        date_str += String(date);
        date_str += " / ";
        if (month < 10)
          date_str += '0';
        date_str += String(month);
        date_str += " / ";
        if (year < 10)
          date_str += '0';
        date_str += String(year);
      }
      if (gps.time.isValid())
      {
        time_str = "";
        hour = gps.time.hour();
        minute = gps.time.minute();
        second = gps.time.second();
        minute = (minute + 30);
        if (minute > 59)
        {
          minute = minute - 60;
          hour = hour + 1;
        }
        hour = (hour + 5) ;
        if (hour > 23)
          hour = hour - 24;
        if (hour >= 12)
          pm = 1;
        else
          pm = 0;
        hour = hour % 12;
        if (hour < 10)
          time_str = '0';
        time_str += String(hour);
        time_str += " : ";
        if (minute < 10)
          time_str += '0';
        time_str += String(minute);
        time_str += " : ";
        if (second < 10)
          time_str += '0';
        time_str += String(second);
        if (pm == 1)
          time_str += " PM ";
        else
          time_str += " AM ";
      }
    }
        Serial.println("SPEED-POST DETECTOR");
        Serial.println("LATITUDE:"); 
        if(lat_str=="")
        {Serial.println("........");}else{Serial.println(lat_str);}
        Serial.println("LONGITUDE:");
          if(lng_str=="")
        {Serial.println("........");}else{Serial.println(lng_str);}
        Serial.println(speed_str);
        Serial.println(income);
      transport();
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }
  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n <!DOCTYPE html> <html> <head> <title>Location Tracking with NodeMCU</title> <style>";
  s += "a:link {background-color: YELLOW;text-decoration: none;}";
  s += "table, th, td {border: 5px solid black;} </style> </head> <body> <h1  style=";
  s += "font-size:300%;";
  s += " ALIGN=CENTER> IOT Speed Limit Sign  Tracking </h1>";
  s += "<p ALIGN=CENTER style=""font-size:150%;""";
  s += "> <b>Location Details</b></p> <table ALIGN=CENTER style=";
  s += "width:50%";
  s += "> <tr> <th>Latitude</th>";
  s += "<td ALIGN=CENTER >";
  s += lat_str;
  s += "</td> </tr> <tr> <th>Longitude</th> <td ALIGN=CENTER >";
  s += lng_str;
  s += "</td> </tr> <tr>  <th>Date</th> <td ALIGN=CENTER >";
  s += date_str;
  s += "</td></tr> <tr> <th>Time</th> <td ALIGN=CENTER >";
  s += time_str;
  s += "</td></tr> <tr> <th>Speed</th> <td ALIGN=CENTER >";
  s += speed_str;
  s += "</td>  </tr> </table> ";
  if (gps.location.isValid())
  {
    // s += "<p align=center><a style=""color:RED;font-size:125%;"" href=""http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=";
     s += "<p align=center><a style=""color:RED;font-size:125%;"" href=""https://www.google.com/maps/dir/";
 //https://www.google.com/maps/dir/26.8172985,75.8286322/26.8181889,75.830…;         
    for (int j=0; j<20; j++)
    {
    s += latarray[j];                           
    s += ",";
    s += lngarray[j];
    if (j<10)
    s += "/"; 
    }
    s += """ target=""_top"">Click here!</a> To check the location in Google maps.</p>";
  }
  s += "</body> </html> \n";
  client.print(s);
 delay(20);  
transport();
  delay(20); 
 }

 void transport()
{
  
//   randomNumber=random(50,87);
// if(WiFi.status() == WL_CONNECTED) 
// {
    HTTPClient http;
    // String link="http://speedsigns.iot.rw/in.php?sn=SPT001&car_speed=&lat_str=-1.982725&lng_str=30.101233"; 
     String link = "http://safeboat.vrt.rw/ins.php?sn=SPT001&emergance="+ String(emergance)+"&lat_str="+String(lat_str)+"&lng_str="+String(lng_str)+"&wter_presence="+String(wter_presence)+"&water_level="+String(water_level);
    http.begin(wifiClient,link);
    int httpCode = http.GET();
    String payload = http.getString();
    income=payload.toInt();
      Serial.println(link);
    Serial.print("payload::");   
    Serial.println(payload);
    Serial.print("speed::");     
    Serial.println(income);

}


void sonicSensor()
{
   long duration, inches, cm;
   pinMode(pingPin, OUTPUT);
   digitalWrite(pingPin, LOW);
   delayMicroseconds(2);
   digitalWrite(pingPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(pingPin, LOW);
   pinMode(echoPin, INPUT);
   duration = pulseIn(echoPin, HIGH);
   inches = microsecondsToInches(duration);
   cm = microsecondsToCentimeters(duration);
   Serial.print(inches);
   Serial.print("in, ");
   Serial.print(cm);
   water_level=cm;
   Serial.print("cm");
   Serial.println();
   delay(100);
}
    long microsecondsToInches(long microseconds) {
   return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
   return microseconds / 29 / 2;
}
