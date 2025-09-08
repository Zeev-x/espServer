#include "reyette.h"
#include "webHandler.h"
#include "spiffs_loader.h"

const char uploadPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <link rel="icon" href="/favicon.ico" type="image/x-icon">
  <title>Reyette Project</title>
  <link rel="stylesheet" href="/upload.css">
</head>
<body>
  <div class="content">
    <div class="container">
      <h2>📂 Upload File</h2>
      <form id="uploadForm">
        <input type="file" id="fileInput" name="upload" multiple><br>
        <input type="submit" value="Upload">
      </form>
      <div id="uploadList"></div>
    </div>
  </div>

  <script src="/upload.js"></script>
</body>
</html>
)rawliteral";

const char upload_style[] PROGMEM = R"rawliteral(
:root {
    --main-color: #6200ff;
    --main-color-2: #6200ff79;
    --main-color-3: #004c70;
    --text-color: #0084ff;
    --text-color-hover: #004c70;
}
* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
  /*font-family: Arial, sans-serif;*/
}
body {
  display: flex;
  min-height: 100vh;
  background: linear-gradient(120deg,#000000fb,#0000005b,#000000fb),
                url(/upload.jpg);
    background-attachment: fixed;
    background-position: center;
    background-position-y: 50%;
    background-position-x: 50%;
    background-size: cover;
    background-repeat: no-repeat;
    color: white;
}
.content {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  width: 100%;
}

.container {
  background: #0000001f;
  padding: 30px;
  border-radius: 15px;
  backdrop-filter: blur(10px);
  -moz-backdrop-filter: blur(10px);
  box-shadow: 0 8px 20px rgba(0,0,0,0.15);
  max-width: 600px;
  width: 90%;
  text-align: center;
  }

    .container h2 {
      margin-bottom: 25px;
      color: #ffffff;
    }

    input[type="file"] {
      padding: 10px;
      border-radius: 10px;
      border: 1px solid #ffffff;
      color: #ffffff;
      cursor: pointer;
      width: 80%;
      transition: 0.3s;
    }

    input[type="file"]::-webkit-file-upload-button {
      background: var(--main-color);
      color: white;
      border: none;
      padding: 10px 15px;
      border-radius: 5px;
      cursor: pointer;
      transition: 0.3s;
    }

    input[type="file"]::-webkit-file-upload-button:hover {
      background: var(--main-color-2);
    }

    input[type="file"]:hover {
      border-color: var(--main-color-2);
    }

    input[type="submit"] {
      margin-top: 15px;
      padding: 12px 25px;
      border: none;
      border-radius: 10px;
      background: var(--main-color);
      color: #fff;
      font-size: 16px;
      cursor: pointer;
      transition: 0.3s;
    }

    input[type="submit"]:hover {
      background: var(--main-color-2);
    }

    .upload-card {
      color: #ffffff;
      background: #0000009f;
      padding: 15px 20px;
      border-radius: 12px;
      margin-top: 15px;
      text-align: left;
      box-shadow: 0 4px 10px rgba(0,0,0,0.08);
    }

    .progress-container {
      width: 100%;
      background: #dddddd3b;
      border-radius: 10px;
      margin-top: 8px;
      height: 20px;
      overflow: hidden;
    }

    .progress-bar {
      width: 0%;
      height: 100%;
      background: linear-gradient(90deg, var(--main-color-3), var(--main-color-2), var(--main-color));
      border-radius: 10px;
      transition: width 0.3s ease;
    }

    .file-status {
      margin-top: 5px;
      font-size: 14px;
      color: #ffffffff;
    }
)rawliteral";

const char upload_script[] PROGMEM = R"rawliteral(
const form = document.getElementById('uploadForm');
const uploadList = document.getElementById('uploadList');

    form.onsubmit = (e) => {
      e.preventDefault();
      const files = document.getElementById('fileInput').files;
      if (!files.length) { alert("Pilih file dulu!"); return; }

      uploadList.innerHTML = "";

      for (let file of files) {
        const card = document.createElement('div');
        card.className = "upload-card";

        const title = document.createElement('div');
        title.textContent = "📄 " + file.name;
        card.appendChild(title);

        const progressContainer = document.createElement('div');
        progressContainer.className = "progress-container";
        const progressBar = document.createElement('div');
        progressBar.className = "progress-bar";
        progressContainer.appendChild(progressBar);
        card.appendChild(progressContainer);

        const status = document.createElement('p');
        status.className = "file-status";
        card.appendChild(status);

        uploadList.appendChild(card);

        const formData = new FormData();
        formData.append("upload", file);

        const xhr = new XMLHttpRequest();
        xhr.open("POST", "/upload", true);

        xhr.upload.onprogress = (event) => {
          if (event.lengthComputable) {
            const percent = (event.loaded / event.total) * 100;
            progressBar.style.width = percent + "%";
            status.textContent = `Progress: ${Math.round(percent)}%`;
          }
        };

        xhr.onload = () => {
          if (xhr.status == 200) {
            status.textContent = "✅ Upload selesai!";
          } else {
            status.textContent = "❌ Upload gagal!";
          }
        };

        xhr.send(formData);
      }
    };
)rawliteral";

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  String folder = String(upload_folder);

  // Pastikan folder ada
  if (!SD.exists(folder)) {
    SD.mkdir(folder);
  }

  // Path file yang konsisten
  String filepath = folder + "/" + filename;

  // Chunk pertama → hapus file lama, lalu buat file baru
  if (!index) {
    Serial.printf("📂 Mulai upload: %s\n", filepath.c_str());
    SD.remove(filepath);
    File file = SD.open(filepath, FILE_WRITE);
    if (!file) {
      Serial.println("❌ Gagal membuat file di SD");
      return;
    }
    file.close();
  }

  // Tambah data ke file
  #if defined(ESP32)
    File file = SD.open(filepath, FILE_APPEND);
  #elif defined(ESP8266)
    File file = SD.open(filepath, FILE_WRITE);
    file.seek(file.size());
  #endif

  if (file) {
    file.write(data, len);
    file.close();
  }

  // Selesai upload
  if (final) {
    Serial.printf("✅ Upload selesai: %s (%u bytes)\n", filepath.c_str(), (unsigned int)(index + len));
    request->send(200, "text/plain", "Upload OK: " + filename);
  }
}

void uploadFile(){
    server.on("/upload.css", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/css", upload_style);
    });

    server.on("/upload.js", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/javascript", upload_script);
    });

    server.on("/upload.jpg", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(FS_SYSTEM, "/upload.jpg", "image/jpeg");
    });

    server.on("/upload", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/html", uploadPage);
    });

  server.on(
      "/upload", HTTP_POST,
      [](AsyncWebServerRequest *request) {
      request->send(200);
    },
    handleUpload
  );

}
