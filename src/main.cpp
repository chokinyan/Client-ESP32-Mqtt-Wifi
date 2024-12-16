#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "Ultrasonic.h"

/////////////////////////////////////////////////
//Parametres MOSQUITTO  broker local à renseigner
/////////////////////////////////////////////////
// Paramètres réseau:
#define BROKER "192.168.247.134"        //adresse ip Broker
#define MQTTPORT 1883        //port broker
#define MYHOSTNAME  "Esp32"        //nom client mqtt
#define QoS 0        //Qualité de service
#define USER "Flappy"       //Compte Broker
#define MDP "Step"       //Mot de passe compte
#define TOPIC_OUT1 "stepper/etat"       //topic(s) de publication:

#define INTERVAL  10                 //Intervale de répétition en ms d'envoi mqtt
//////////////////////////////////////////////
//////////////////////////////////////////////


/////////////////////////////////////////////
//Parametre AP WIFI /////////////////////////
#define SECRET_SSID   "S23Adem_"         		       // replace with your WiFi network name
#define SECRET_PASS   "mdp12345"                     // replace with your WiFi password
/////////////////////////////////////////////
/////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////
// definition des broches des capteurs /////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

Ultrasonic ultrasonicSensor(13);

/////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////// Ne pas toucher cette zone ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
char ssid[] = SECRET_SSID;   // SSID
char pass[] = SECRET_PASS;   // Clé WIFI
int keyIndex = 0;            // your network key Index number (needed only for WEP)

const char *mqtt_server = BROKER;
uint16_t mqtt_PORT = MQTTPORT;     // Port TCP sur lequel le broker écoute
const uint8_t qos = QoS;           // Qualité de service
char myhostname[32]= MYHOSTNAME;   // Nom donné au client MQTT

const char *outTopic1 = TOPIC_OUT1;
char message1[1];
long valeurCapteur;
////////////////////////////////////////////////////////////////////////////////////////////
// Déclaration d'un Objet pour la connexion WIFI/////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
WiFiClient  client;
/////////////////////////////////////////////////////////////////////////////////////////////
// Déclaration d'un Objet pour la connexion au broker MQTT local (Publisher/Subscriber)//////
/////////////////////////////////////////////////////////////////////////////////////////////
PubSubClient mqttClient(client);
/////////////////////////////////////////////////////////////////////////////////////////////
// Intervalle de temps séparant la publication du (des) topic()s ////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
const long interval = INTERVAL;     
unsigned long previousMillis = 0;
// Permet de calculer l'intervalle de temps depuis la dernière 
// publication du (des) topic(s)
/////////////////////////fin zone à ne pas toucher ///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////
//Fonction InitWifi:  Initialisation liaison WIFI       ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
void InitWIFI()
{
  WiFi.begin(ssid, pass);

	Serial.println("Etat WIFI:");
	Serial.print("Tentative de connexion au point acces Wifi ...");
	
	while(WiFi.status() != WL_CONNECTED)
	{
		Serial.println("......................  tentative connexion Wifi en cours  ......................................");
    delay(1000);
	}
	Serial.println("Connexion etablie au point d'acces Wifi!");
	Serial.print("Adresse IP obtenue du DHCP: ");
	Serial.println(WiFi.localIP());
  Serial.println("\n");
}
/////////////////////////////////////////////////////////////////////////////////////////////
//Fin fonction  InitWifi() //////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////
// Fonction reconnect() : Maintien de la connexion au broker MQTT et abonnement au(x) topic(s)
//////////////////////////////////////////////////////////////////////////////////////////////
void reconnect()
{
        // Si Non connecté, alors on tente de se connecter
        if (!mqttClient.connected())
        {
            Serial.print("Pas connected, erreur avec code rc= ");
            Serial.println(mqttClient.state());
            Serial.println("\n");
            Serial.println("Tentative de connection au broker mosquitto local...");
            mqttClient.connect(myhostname);          
            delay(200); 
        } 
        else 
        {
            mqttClient.publish(outTopic1,message1); // publication du topic
        }
 }
//////////////////////////////////////////////////////////////////////////////////////////////
/////////       Fin fonction reconnect()      ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////


void setup() 
{

  Serial.begin(115200);        // init liaison moniteur série (port COM PC/ESP32) 
	delay(1000);                 // attente 1s
	Serial.println("\n");        // retour ligne
	InitWIFI();                  //init liaison wifi
  // création liaison vers broker MQTT local
  mqttClient.setServer(mqtt_server, mqtt_PORT);
} 



void loop()
{   

    // Temporisation "non blocante" pas comme un delay(), avant l'envoi d'un ou plusieurs topics
    long currentMillis = millis();
    if (currentMillis - previousMillis > interval) // si temporisation > interval en ms
    {
      
          //lecture des valeurs capteurs///////////////////////
    valeurCapteur = ultrasonicSensor.MeasureInCentimeters();
    
    if(valeurCapteur <= 2)
    {
      valeurCapteur = 2;
    }

    if(valeurCapteur >= 11)
    {
      valeurCapteur = 11;
    }


    sprintf(message1, "%ld", valeurCapteur);  //pour convertir valeur numérique en texte
      
                                                   // alors on publie le(s) message(s)
        // On mémorise la "date" à laquelle le ou les messages ont été publiés
        previousMillis = currentMillis;
      
        Serial.print("Valeur capteur: ");
        Serial.println(valeurCapteur);    //Affichage dans terminal des valeurs

        reconnect(); 
    }
 
 }
