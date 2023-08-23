#include <TM1637.h>
#include <HX711.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiManager.h>



#define pin1DT 14
#define pin1SCK 12 

const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* mqttUser = "BalancaGSDE";
const char* mqttPassword = "";
WiFiClient espClient;
PubSubClient client(espClient);

const int CLK = 32;
const int DIO = 33;


HX711 balanca;
TM1637 tm(CLK, DIO);

float medida, pesagem = 0;
int a, b, c, d;



void setup() {


    Serial.begin(115200);
 
    tm.init();
    tm.set(BRIGHT_TYPICAL);

    WiFiManager wm;

    bool res;

    res = wm.autoConnect("Rede_pesagem","87654321"); // password protected ap

    if(!res) {
      Serial.println("Falha na conexão");

    } 
    else {  
      Serial.println("Conectado..)");
    }
    client.setServer(mqttServer, mqttPort);
    //client.setCallback(callback);
  

    while (!client.connected()) {
      Serial.println("Conectando com o Broker (MQTT)…");
      String clientId = "";
      clientId += String(random(0xffff), HEX);
      if (client.connect(clientId.c_str(), mqttUser, mqttPassword )) {
          Serial.println("Conectado com o Broker");
      } else {
          Serial.print("Falha na conexão. Erro: ");
          Serial.print(client.state());
          Serial.println("\n");
          delay(2000);
          
      }
    }
    client.publish("balanca", "1");

    Serial.println("Enviando status de conexão para o celular\n");
    Serial.println("Status de conexão: OK\n");

   balanca.begin(pin1DT, pin1SCK);
    balanca.set_scale(92431);
    delay(2000);
    balanca.tare();
}
 

void loop() {
  Serial.println("Balanca: ");
  medida = balanca.get_units(5);
  Serial.println(medida, 2);

  if(medida < 0){
    medida = medida * (-1);
  }

  pesagem = medida * 100;
  a = pesagem / 1000;
  b = (pesagem - (a*1000)) / 100;
  c = (pesagem - (a*1000) - (b*100)) / 10;
  d = (pesagem - (a*1000) - (b*100) - (c*10));

  tm.display(0, a);
  tm.display(1, b);
  tm.display(2, c);
  tm.display(3, d);

  char Peso[10];
  sprintf(Peso, "%f", medida);
  client.publish("BalancaGSDE/Balanca", Peso);


  delay(100);
  balanca.power_down();
  delay(1000);
  balanca.power_up();

}
