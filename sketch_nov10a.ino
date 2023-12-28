#include <WiFi.h>
#include <PubSubClient.h>

#define LED_BUILTIN 2
// Wifi
 const char* ssid = "AndroidAP4949";
 const char* password = "smqpk73692";
//const char* ssid = "mAedee";
//const char* password = "1234567890";
// MQTT Server
const char* mqtt_server = "broker.emqx.io";

int flag_OK, flag_Cold;
int flag_speed_ok, flag_speed_hight;
long msg[2];
unsigned long lastMsg = 0;
int value = 0;
char Start[1] = {1};

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
    if (strcmp(topic, "Location/Temp") == 0)
    {
        for (int i = 0; i < length; i++)
            msg_received[i] = (char)payload[i];
    }

    if (strcmp(msg_received, "Cold") == 0)
        flag_Cold = 1;

    else if (strcmp(msg_received, " OK ") == 0)
        flag_OK = 1;

    Serial.println("HERE WE ARE 2");
    Serial.println();
} //end callback

//----------------------------------------------------------------------
/*void callback(char* topic, byte* speed, unsigned int length)
{
    char msg_received[9];
    // Recieving data from MQTT server
    Serial.println("HERE WE ARE");
    if (strcmp(topic, "location/speed") == 0)
    {
        for (int i = 0; i < length; i++)
            msg_received[i] = (char)speed[i];
    }

    if (strcmp(msg_received, " speed ok ") == 0)
        flag_speed_ok = 1;

    else if (strcmp(msg_received, "speed high") == 0)
        flag_speed_hight= 1;

    Serial.println("HERE WE ARE 3");
    Serial.println();
}*/

//----------------------------------------------------------------------
void reconnect()
{
    // Loop until we're reconnected
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "ESP8266Client-";
        clientId += String(
          
          
          random(0xffff), HEX);
        // Attempt to connect
        // if your MQTT broker has clientID,username and password
        if (client.connect(clientId.c_str()))
        {
            Serial.println("connected");
            // once connected to MQTT broker, subscribe command if any
            client.publish("Location/LatLong", Start);
            client.subscribe("Location/Temp", 1);
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 6 seconds before retrying
            delay(5000);
        }
    }
} //end reconnect()

//----------------------------------------------------------------------
void setup()
{
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    client.subscribe("Location/Temp", 1);
    //client.subscribe("Location/Temp");
    client.publish("Location/LatLong", Start);
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
        

    String Lat_lon = "";
    for (int i = 0; i < 2; i++)
    {
        Lat_lon += String(msg[i]);
        if (i < 1)
        {
            Lat_lon += ",";
        }
    }
    long now = millis();
    if (now - lastMsg > 5000)
    {
        lastMsg = now;
        ++value;
        client.publish("Location/LatLong", Lat_lon.c_str());
        Serial.println("Publish!");
    }

    if (flag_Cold == 1)
    {
        Serial.println("weather is Cold!");
        flag_Cold = 0;
    }
    if (flag_OK == 1)
    {
        Serial.println("weather is OK!");
        flag_OK = 0;
    }
 
    if (flag_speed_ok  == 1)
    {
        Serial.println("speed is ok!");
        flag_speed_ok = 0;
    }
    if (flag_speed_hight == 1)
    {
        Serial.println("speed is hight!");
        flag_speed_hight = 0;
    }
    delay(10000);


}
