#include "reyette.h"

uint32_t clockList[] = {40000000, 25000000, 20000000, 15000000, 10000000, 400000};
int bestClock = -1;

void ledControl(int OPIN, int state, int delayTime);

// =============================
// Utility MIME
// =============================
String getContentType(String filename) {
  if (filename.endsWith(".htm") || filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js"))  return "application/javascript";
  else if (filename.endsWith(".json")) return "application/json";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".jpeg")) return "image/jpeg";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".mp3")) return "audio/mpeg";
  else if (filename.endsWith(".wav")) return "audio/wav";
  else if (filename.endsWith(".ogg")) return "audio/ogg";
  else if (filename.endsWith(".flac")) return "audio/flac";
  else if (filename.endsWith(".aac")) return "audio/aac";
  else if (filename.endsWith(".m4a")) return "audio/mp4";
  else if (filename.endsWith(".wma")) return "audio/x-ms-wma";
  else if (filename.endsWith(".mp4")) return "video/mp4";
  else if (filename.endsWith(".mov")) return "video/quicktime";
  else if (filename.endsWith(".avi")) return "video/x-msvideo";
  else if (filename.endsWith(".wmv")) return "video/x-ms-wmv";
  else if (filename.endsWith(".flv")) return "video/x-flv";
  else if (filename.endsWith(".mkv")) return "video/x-matroska";
  else if (filename.endsWith(".webm")) return "video/webm";
  else if (filename.endsWith(".mpeg") || filename.endsWith(".mpg")) return "video/mpeg";
  else if (filename.endsWith(".txt")) return "text/plain";
  return "application/octet-stream";
}

// =============================
// Proteksi file
// =============================
bool isProtectedFile(String path) {
  if (path.endsWith("config.json")) return true;
  if (path.startsWith("/config")) return true;
  return false;
}

// =============================
// Helper sendFile (beda ESP32/ESP8266)
// =============================
bool sendFileResponse(AsyncWebServerRequest *request, String path) {
#if defined(ESP32)
  if (SD.exists(path)) {
    request->send(SD, path, getContentType(path));
    return true;
  }
#elif defined(ESP8266)
  if (SD.exists(path)) {
    File file = SD.open(path, FILE_READ);
    if (file) {
      request->send(file, path, getContentType(path));
      file.close();
      return true;
    }
  }
#endif
  return false;
}

// =============================
// Handler file
// =============================
bool handleFileRead(AsyncWebServerRequest *request, String path) {
  if (isProtectedFile(path)) {
    request->send(403, "text/plain", "Access denied!");
    return false;
  }

  File dir = SD.open(path);
  if (dir && dir.isDirectory()) {
    String indexPath = path;
    if (!indexPath.endsWith("/")) indexPath += "/";
    indexPath += "index.html";

    if (sendFileResponse(request, indexPath)) {
      return true;
    }

    // Generate listing
    String html = "<!DOCTYPE html><html><head><meta charset='utf-8'>";
    html += "<title>Index of " + path + "</title>";
    html += "<style>";
    html += "body{background-color:#000;color:#fff;font-family:Arial}";
    html += "ul{list-style:none;padding:0}";
    html += "li{margin:5px 0}";
    html += "a{color:#fff;text-decoration:none}";
    html += "a:hover{color:#4caf50;}";
    html += "</style>";
    html += "</head><body>";
    html += "<h2>Index of " + path + "</h2><ul>";

    File file = dir.openNextFile();
    while (file) {
      String filename = String(file.name());

      if (filename.startsWith(path)) {
        filename = filename.substring(path.length());
      }

      if (filename.length() > 0) {
        if (file.isDirectory()) {
          html += "<li>📁 <a href=\"" + path;
          if (!path.endsWith("/")) html += "/";
          html += filename + "\">" + filename + "/</a></li>";
        } else {
          html += "<li>📄 <a href=\"" + path;
          if (!path.endsWith("/")) html += "/";
          html += filename + "\">" + filename + "</a></li>";
        }
      }
      file = dir.openNextFile();
    }

    html += "</ul></body></html>";
    request->send(200, "text/html", html);
    return true;
  }

  if (sendFileResponse(request, path)) {
    return true;
  }

  request->send(404, "text/plain", "File tidak ditemukan: " + path);
  return false;
}

// =============================
// Tes tulis-baca
// =============================
bool testWriteRead() {
  if (SD.exists("/reyette.txt")) {
    SD.remove("/reyette.txt");
  }

  File file = SD.open("/reyette.txt", FILE_WRITE);
  if (!file) return false;

  file.print("ESP test data");
  file.close();

  file = SD.open("/reyette.txt", FILE_READ);
  if (!file) return false;

  String content = file.readString();
  file.close();

  SD.remove("/reyette.txt");

  return (content == "ESP test data");
}

// =============================
// Init SD card
// =============================
void initSDCard() {
  bestClock = -1;

#if defined(ESP32)
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  for (int i = 0; i < sizeof(clockList) / sizeof(clockList[0]); i++) {
    uint32_t freq = clockList[i];
    Serial.printf("Coba SPI clock %d Hz...\n", freq);

    if (SD.begin(SD_CS, SPI, freq)) {
      if (testWriteRead()) {
        bestClock = freq;
        Serial.printf("✅ Berhasil stabil di %d Hz\n", freq);
        break;
      }
      SD.end();
    } else {
      Serial.println("❌ Mount gagal");
    }
  }

#elif defined(ESP8266)
  if (SD.begin(SD_CS)) {
    if (testWriteRead()) {
      bestClock = 0;
      Serial.println("✅ SD stabil di default clock");
    } else {
      Serial.println("⚠️ Mount oke tapi tulis/baca gagal");
    }
  } else {
    Serial.println("❌ Mount gagal di ESP8266");
  }
#endif

  if (bestClock >= 0) {
#if defined(ESP32)
    if (bestClock > 0) {
      SD.begin(SD_CS, SPI, bestClock);
    }
#endif
    Serial.println("📂 SD siap dipakai.");
    ledControl(PIN, 1, 100);
    delay(1000);
  } else {
    Serial.println("\n⛔ Tidak ada clock yang cocok, cek wiring/SD card!");
  }
}
