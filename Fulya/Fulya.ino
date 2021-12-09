#include <WiFi.h>
#include <PubSubClient.h>
//#-----------------------------------
/*
  Cihaz 0:Berk
  Cihaz 1:Enes
  Cihaz 2:Fulya
*/
const char* DeviceName = "Fulya";     //cihazın uniqe adı
const char* ssid = "";               //wifi adın
const char* password = "";          //wifi şifresi
const char* mqtt_server = "broker.hivemq.com";   //Public mqtt brokerin ipsi
int mqtt_port = 1883;
//#-----------------------------------
String Subscribe1 = "BEFESP32/out_02";          //Berkantden gelen özel mesajlar
String Subscribe2 = "BEFESP32/out_12";          //Enesden gelen özel mesajlar
String Subscribe3 = "BEFESP32/out_1";           //Berkantden gelen mesajlar
String Subscribe4 = "BEFESP32/out_0";           //Enesden gelen mesajlar
//#-----------------------------------
//"1:merhaba " /sadece Berkant e gider
String Publish1 = "BEFESP32/out_20";           //Berkant e özel mesaj gönderilen adres
String WhisperTo1 = "berk:";                   //Özel mesaj göndermek için kullanılan özel kelime(?)

//"2:merhaba " /sadece Enes ye gider
String Publish2 = "BEFESP32/out_21";           //Enes ye özel mesaj gönderilen adres
String WhisperTo2 = "enes:";                   //Özel mesaj göndermek için kullanılan özel kelime(?)

//"merhaba " /herkese mesaj gider
String Publish3 = "BEFESP32/out_2";            //Herkese Mesaj gönderme adresi




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
    Serial.println("Berk şunu fısıldadı: " + messageTemp);
  }
  else if (String(topic) == Subscribe2 ) {
    Serial.println("Enes şunu fısıldadı: " + messageTemp);
  }
  else if (String(topic) == Subscribe3 ) {
    Serial.println("Berk : " + messageTemp);
  }
  else if (String(topic) == Subscribe4) {
    Serial.println("Enes : " + messageTemp);
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

String TempMessage;
bool Ready;
void loop() {

  if (!client.connected()) {//mqtt ye bağlanıldı mı?
    reconnect();            //mqttye bağlan
  }
  client.loop();            //mqtt den mesaj geldi mi kontrol et


  if (Serial.available() > 0) { // seriala yazı yazıldı mı
    char state;
    state = Serial.read(); // serialdaki yazıyı oku
    
    if (state != 10)      //eğer okunan harfin ASCII değeri 10 değilse mesaja ekle
      TempMessage += String(state);

    if (state == 10) {  //eğer okunan harfin ASCII değeri 10(enter tuşu) ise mesajı
      //mqtt ye gönder, tempmesajı sıfırla
      Ready = true;

      state = 0;
    }
  }
  if (Ready) {
    Ready = false;
    SendMessage();
    TempMessage = "";
  }
}
//mesaj gönderme fonksiyonu
void SendMessage() {
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
