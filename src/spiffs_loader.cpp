#include "spiffs_loader.h"

void listFS() {
  Serial.println("📂 Listing Filesystem:");

#if defined(ESP32)
  File root = FS_SYSTEM.open("/");
  File file = root.openNextFile();
  while (file) {
    Serial.printf("  %s (%d bytes)\n", file.name(), file.size());
    file = root.openNextFile();
  }
#elif defined(ESP8266)
  Dir dir = FS_SYSTEM.openDir("/");
  while (dir.next()) {
    Serial.printf("  %s (%d bytes)\n", dir.fileName().c_str(), dir.fileSize());
  }
#endif
}

void writeFile(const char *path, const char *message) {
#if defined(ESP32)
  File file = FS_SYSTEM.open(path, FILE_WRITE);
#else
  File file = FS_SYSTEM.open(path, "w");
#endif
  if (!file) {
    Serial.println("❌ Gagal buka file untuk tulis");
    return;
  }
  file.print(message);
  file.close();
  Serial.printf("✅ File ditulis: %s\n", path);
}

String readFile(const char *path) {
#if defined(ESP32)
  File file = FS_SYSTEM.open(path, FILE_READ);
#else
  File file = FS_SYSTEM.open(path, "r");
#endif
  if (!file) {
    Serial.println("❌ Gagal buka file untuk baca");
    return "";
  }
  String content = file.readString();
  file.close();
  Serial.printf("📖 File dibaca: %s\n", path);
  return content;
}

void fsSetup() {
#if defined(ESP32)
  if (!FS_SYSTEM.begin(true)) { // auto-format kalau gagal
    Serial.println("❌ Gagal mount SPIFFS (ESP32)");
    return;
  }
#elif defined(ESP8266)
  if (!FS_SYSTEM.begin()) {
    Serial.println("❌ Gagal mount LittleFS, coba format...");
    if (FS_SYSTEM.format()) {
      Serial.println("✅ LittleFS diformat ulang.");
      if (!FS_SYSTEM.begin()) {
        Serial.println("❌ Tetap gagal mount setelah format!");
        return;
      }
    }
  }
#endif
  Serial.println("✅ Filesystem siap!");
}

void spiffsSetup() {
  fsSetup();
  listFS();
}