#include <WiFi.h>
#include <PubSubClient.h>
//#-----------------------------------
/*
  Cihaz 0:Berk
  Cihaz 1:Enes
  Cihaz 2:Cihaz 2
*/
const char* DeviceName = "Berkant";     //cihazın uniqe adı
const char* ssid = "Berkany";               //wifi adın
const char* password = "Adalet34";          //wifi şifresi
const char* mqtt_server = "broker.hivemq.com";   //Public mqtt brokerin ipsi
int mqtt_port = 1883;
//#-----------------------------------
String Subscribe1 = "BEESP32/out_10";          //Kullanıcı 1den gelen özel mesajlar
String Subscribe2 = "BEESP32/out_20";          //Kullanıcı 2den gelen özel mesajlar
String Subscribe3 = "BEESP32/out_1";           //Kullanıcı 1den gelen mesajlar
String Subscribe4 = "BEESP32/out_2";           //Kullanıcı 2den gelen mesajlar
//#-----------------------------------
//"1:merhaba " /sadece kullanıcı 1 e gider
String Publish1 = "BEESP32/out_01";           //Kullanıcı 1 e özel mesaj gönderilen adres
String WhisperTo1 = "enes:";                   //Özel mesaj göndermek için kullanılan özel kelime(?)

//"2:merhaba " /sadece kullanıcı 2 ye gider
String Publish2 = "BEESP32/out_02";           //Kullanıcı 2 ye özel mesaj gönderilen adres
String WhisperTo2 = "2:";                   //Özel mesaj göndermek için kullanılan özel kelime(?)

//"merhaba " /herkese mesaj gider
String Publish3 = "BEESP32/out_0";            //Herkese Mesaj gönderme adresi




WiFiClient espClient;                       //Wifi classını oluşturma
PubSubClient client(espClient);             //Mqtt classını yi wifiya bağlama

void setup() {
  Serial.begin(115200);                     //Seri haberleşmeyi 115200 baud rate inde başlat
   while(!Serial) {}                        //Kullanıcı serial paneli açtı mı

  delay(10);                                //10ms bekle
  // We start by connecting to a WiFi network

  Serial.print("Connecting to ");
  WiFi.begin(ssid, password);               //wifiye bağlanmayı dene

  while (WiFi.status() != WL_CONNECTED) {   //wifiye bağlandı mı?
    delay(500);                             //bağlanmadıysa 500 ms bekle ve tekrar dene
    Serial.print(".");
  }

  Serial.println("");                       //wifiye başarılı şekilde bağlanıldı
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());           //esp32nin lokal ipsi

  client.setServer(mqtt_server, mqtt_port);     //mqtt brokerın ipsi ve portu
  client.setCallback(callback);             //Mesaj geldiğinde halgi fonksiyon çağırılacak

}


//Mesaj okuma ve işleme fonksiyonu
void callback(char* topic, byte* message, unsigned int length) {
  String messageTemp;

  for (int i = 0; i < length; i++) {

    messageTemp += (char)message[i];              //subscribe adresinden gelen mesaj
  }
  

  if (String(topic) == Subscribe1 ) {
    Serial.println("Kullanıcı 1 şunu fısıldadı: " + messageTemp);
  }
  else if (String(topic) == Subscribe2 ) {
    Serial.println("Kullanıcı 2 şunu fısıldadı: " + messageTemp);
  }
  else if (String(topic) == Subscribe3 ) {
    Serial.println("Kullanıcı 1 : " + messageTemp);
  }
  else if (String(topic) == Subscribe4) {
    Serial.println("Kullanıcı 2 : " + messageTemp);
  }


}


//Mqtt ye bağlanma fonksiyonu
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(DeviceName)) {
      Serial.println("connected");

      //subscribe edilen adresler
      client.subscribe(Subscribe1.c_str());
      client.subscribe(Subscribe2.c_str());
      client.subscribe(Subscribe3.c_str());
      client.subscribe(Subscribe4.c_str());

    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

long lastMsg = 0;
void loop() {

  if (!client.connected()) {//mqtt ye bağlanıldı mı?
    reconnect();            //mqttye bağlan
  }
  client.loop();            //mqtt den mesaj geldi mi kontrol et
  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    SendMessage("merhaba enes");
    SendMessage("enes: fısıldadım");
    }
}
//mesaj gönderme fonksiyonu
void SendMessage(String TempMessage) {
  String Mes = TempMessage;
  if (Mes != "") {
    if (Mes.startsWith(WhisperTo1)) {
      Mes = Mes.substring(WhisperTo1.length());                 //mesajdaki ilk 2 harfi sil
      client.publish(Publish1.c_str(), Mes.c_str());

      Serial.println(WhisperTo1.substring(0, WhisperTo1.length() - 1) + "'a fısıldadın:");

    }
    else if (Mes.startsWith(WhisperTo2)) {
      Mes = Mes.substring(WhisperTo2.length());                 //mesajdaki ilk 2 harfi sil
      client.publish(Publish2.c_str(), Mes.c_str());

      Serial.println(WhisperTo2.substring(0, WhisperTo2.length() - 1) + "'a fısıldadın:");

    }
    else {
      client.publish(Publish3.c_str(), Mes.c_str());
      Serial.println(String (DeviceName) + "(sen):");
    }
  }

}
