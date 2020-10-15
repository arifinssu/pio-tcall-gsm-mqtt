char msg[50];
int math;
const char apn[] = "internet";
const char user[] = "";
const char pass[] = "";

// TTGO T-Call pin definitions
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23 //  bentrok
#define MODEM_TX             27
#define MODEM_RX             26

// Wire I2C
#define I2C_SDA              21
#define I2C_SCL              22

#define GSM Serial1
#define TINY_GSM_MODEM_SIM800
#define TINY_GSM_RX_BUFFER 1024

#include <Arduino.h>
#include <PubSubClient.h>
#include <TinyGsmClient.h>

TinyGsm modem(GSM);
TinyGsmClient client(modem);
PubSubClient mqtt(client);

const char *broker = "147.139.139.55";

void msetup() {
  Serial.println("Connecting modem...");
  String modemInfo = modem.getModemInfo();
  Serial.print("Modem: ");
  Serial.println(modemInfo);
  Serial.print("Waiting for network...");
  if (!modem.waitForNetwork(240000L)) {
    Serial.println(" fail");
    delay(10000);
    return;
  }
  Serial.println(" OK");
  if (modem.isNetworkConnected()) {
    Serial.println("Network ");
  }
  Serial.print(F("Connecting to APN: "));
  Serial.print(apn);
  if (!modem.gprsConnect(apn, user, pass)) {
    Serial.println(" fail");
    delay(10000);
    return;
  }
  Serial.println(" OK");
}

void reconnect() {
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    // String idnya = "cobacoba#Device1#";
    // String username = "macuser";
    // String password = "mac57588";
    if (mqtt.connect("coba", "macuser", "mac57588")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);
  GSM.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);
  msetup();
  mqtt.setServer(broker, 1883);
  reconnect();
}

void loop() {
  if (!mqtt.connected()) {
    msetup();
    reconnect();
  }
  mqtt.loop();
  delay(1000);
  math = random(60);
  sprintf(msg, "%i", math);
  mqtt.publish("cobacoba/Device1/direct/Tag1", msg);
  Serial.println(msg);
  memset(msg,0,50);
  delay(1000);
}