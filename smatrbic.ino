#include <WiFi.h>
#include <PubSubClient.h>

#define LED_BUILTIN 2
//Wifi
const char* ssid = "Amir Mohammad";
const char* password = "13821382";
// const char* ssid = "Parsa.GH";
// const char* password = "Parsa1382";
//MQTT Server
const char* mqtt_server = "broker.emqx.io";

int flag_OK, flag_Cold; 
int flag_SpeedOk, flag_SpeedHight ; 
long msg[2] ; 
unsigned long lastMsg = 0;
int value = 0;
char Start[1]={1};

WiFiClient espClient;
PubSubClient client(espClient);

//----------------------------------------------------------------------
void setup_wifi() 
{
   delay(100);
  // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
//----------------------------------------------------------------------
void callback(char* topic, byte* payload, unsigned int length) 
{
  char msg_received[3];
  // Recieving data from MQTT server
  Serial.println("HERE WE ARE");
  if (strcmp(topic,"Location/Temp") == 0)
  {
    for(int i=0;i<=(length-1);i++)
      msg_received[i]=(char)payload[i];
  }

  if(strcmp(msg_received,"Cold") == 0)
    flag_Cold = 1;

  else if(strcmp(msg_received," OK ") == 0)
    flag_OK = 1;
  Serial.println("HERE WE ARE 2");
  //====
    
  //for speed----------------------------------------
    char msg_receivedS[9];
  // Recieving data from MQTT server
  Serial.println("HERE WE ARE 3");
  if (strcmp(topic,"Location/speed") == 0)
  {
    for(int i=0;i<=(length-1);i++)
      msg_receivedS[i]=(char)payload[i];
  }

  if(strcmp(msg_receivedS,"speed high") == 0)
    flag_SpeedHight = 1;

  else if(strcmp(msg_receivedS," speed ok ") == 0)
    flag_SpeedOk = 1;
  Serial.println(flag_SpeedHight);


  Serial.println();
} //end callback

//----------------------------------------------------------------------
void reconnect() 
{
  // Loop until we're reconnected
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if you MQTT broker has clientID,username and password
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
     //once connected to MQTT broker, subscribe command if any
      client.publish("Location/LatLong",Start);
      client.subscribe("Location/Temp",1);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 6 seconds before retrying
      delay(6000);
    }
  }
} //end reconnect()

//----------------------------------------------------------------------
void setup() 
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN,OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  client.subscribe("Location/Temp",1);
  client.subscribe("Location/speed",1); 
  client.publish("Location/LatLong",Start);
}


void loop() 
{
  if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();

      msg[0] = random(-90,90);
      msg[1] = random(-180,180); 

  //for(int i = 0 ; i < 2 ; i++)
  //  msg[i] = random(20,30);
  
  String Lat_lon = "";
  for (int i = 0; i < 2; i++) 
  {
      Lat_lon += String(msg[i]);
      if (i < 1) //2 - 1
      {
        Lat_lon += ",";
      }
  }
  long now = millis();
  if (now - lastMsg > 5000)
  {
    lastMsg = now;
    ++value;
    client.publish("Location/LatLong",Lat_lon.c_str()); 
    Serial.println("Publish!");
  }
  // for temp------------------------------
  if(flag_Cold == 1)
  {
    Serial.println("weather is Cold!");
    flag_Cold =0;
  }
  if(flag_OK == 1)
  {
    Serial.println("weather is OK!");
    flag_OK =0;
  } 
  //for speed-----------------------------
    if(flag_SpeedHight == 1)
  {
    Serial.println("speed is hight!");
    flag_SpeedHight =0;
  }
  if(flag_SpeedOk == 1)
  {
    Serial.println("speed is OK!");
    flag_SpeedOk =0;
  } 
  delay(10000);
}
