#include "reyette.h"
#include "led.h"
#include <ArduinoJson.h>

String wifi_ssid;
String wifi_password;
IPAddress local_IP, gateway, subnet, primaryDNS, secondaryDNS;

bool saveConfig(const char* path = "/config/config.json") {
  StaticJsonDocument<512> doc;
  doc["wifi_ssid"]     = wifi_ssid;
  doc["wifi_password"] = wifi_password;
  doc["static_ip"]     = local_IP.toString();
  doc["gateway"]       = gateway.toString();
  doc["subnet"]        = subnet.toString();
  doc["primary_dns"]   = primaryDNS.toString();
  doc["secondary_dns"] = secondaryDNS.toString();

  File file = SD.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("⚠️ Gagal menulis config.json");
    return false;
  }
  serializeJsonPretty(doc, file);
  file.close();
  Serial.println("✅ Config berhasil disimpan ke SD: " + String(path));
  return true;
}

bool loadConfig() {
  File file = SD.open("/config/config.json");
  if (!file) {
    Serial.println("⚠️ config.json tidak ada, gunakan default & buat baru");

    wifi_ssid     = wifi_ssid_default;
    wifi_password = wifi_password_default;
    local_IP      = static_ip_default;
    gateway       = gateway_ip_default;
    subnet        = subnet_ip_default;
    primaryDNS    = primary_dns_default;
    secondaryDNS  = secondary_dns_default;

    SD.mkdir("/config");
    saveConfig();
    return true;
  }

  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.print("⚠️ Gagal parse config.json: ");
    Serial.println(error.c_str());

    wifi_ssid     = wifi_ssid_default;
    wifi_password = wifi_password_default;
    local_IP      = static_ip_default;
    gateway       = gateway_ip_default;
    subnet        = subnet_ip_default;
    primaryDNS    = primary_dns_default;
    secondaryDNS  = secondary_dns_default;

    saveConfig();
    return true;
  }

  wifi_ssid     = doc["wifi_ssid"]     | wifi_ssid_default;
  wifi_password = doc["wifi_password"] | wifi_password_default;
  local_IP.fromString(doc["static_ip"] | static_ip_default.toString());
  gateway.fromString(doc["gateway"]    | gateway_ip_default.toString());
  subnet.fromString(doc["subnet"]      | subnet_ip_default.toString());
  primaryDNS.fromString(doc["primary_dns"] | primary_dns_default.toString());
  secondaryDNS.fromString(doc["secondary_dns"] | secondary_dns_default.toString());

  Serial.println("✅ Config berhasil dimuat dari SD");
  Serial.println("SSID: " + wifi_ssid);
  Serial.println("Password: " + wifi_password);
  Serial.print("Static IP: "); Serial.println(local_IP);
  return true;
}

void networkSetup(const char* ssid, const char* password) {
  WiFi.mode(WIFI_STA);  // pastikan mode station

  // Atur IP statis sebelum koneksi
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("⚠️ Gagal mengatur IP statis!");
  } else {
    Serial.println("ℹ️ IP statis berhasil disetel.");
  }

  WiFi.begin(ssid, password);
  Serial.print("🔌 Connecting to WiFi");

  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 30) {
    delay(500);
    Serial.print(".");
    ledControl(PIN, 1, 100);
    retry++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Connected to WiFi");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    delay(1000);
    ledControl(PIN, 1000, 1);
  } else {
    Serial.println("\n❌ Gagal konek WiFi");
    ledControl(PIN, 1000, 5);
  }
}

void networking() {
  if (!loadConfig()) {
    Serial.println("⚠️ Config gagal dimuat, gunakan default.");
    wifi_ssid     = wifi_ssid_default;
    wifi_password = wifi_password_default;
    local_IP      = static_ip_default;
    gateway       = gateway_ip_default;
    subnet        = subnet_ip_default;
    primaryDNS    = primary_dns_default;
    secondaryDNS  = secondary_dns_default;
  }

#if defined(ESP32) || defined(ESP8266)
  networkSetup(wifi_ssid.c_str(), wifi_password.c_str());
#endif
}
