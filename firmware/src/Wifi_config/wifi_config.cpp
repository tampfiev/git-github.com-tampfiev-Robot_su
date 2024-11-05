#include "wifi_config.h"


AsyncWebServer server(80);
const char* robot_ssid = "ESP32-Access-Point";
const char* robot_password = "12345678";

const char* websockets_server_host = "tofutest.stepup.edu.vn";
const uint16_t websockets_server_port = 8763;

WebsocketsClient client;


String ssid_robot;
String password_robot;

bool internet_connect;



// Trang HTML với CSS nhúng, lưu vào bộ nhớ flash với từ khóa PROGMEM
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Step Up Robot - Wi-Fi Connect</title>
    <style>
        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
        }

        body {
            font-family: Arial, sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background: linear-gradient(to bottom right, #9c27b0, #e91e63, #f44336);
        }

        .container {
            background: rgba(255, 255, 255, 0.2);
            backdrop-filter: blur(10px);
            border-radius: 20px;
            padding: 20px;
            width: 100%;
            max-width: 400px;
            box-shadow: 0 4px 10px rgba(0, 0, 0, 0.3);
            text-align: center;
        }

        h1 {
            color: white;
            font-size: 24px;
            margin-bottom: 20px;
        }

        label {
            color: white;
            font-size: 14px;
            display: block;
            text-align: left;
            margin-bottom: 5px;
        }

        input {
            width: 100%;
            padding: 10px;
            margin-bottom: 15px;
            border: none;
            border-radius: 5px;
            background: rgba(255, 255, 255, 0.7);
            font-size: 14px;
        }

        button {
            width: 100%;
            padding: 10px;
            background: linear-gradient(to right, #9c27b0, #e91e63);
            border: none;
            border-radius: 5px;
            color: white;
            font-size: 16px;
            cursor: pointer;
            transition: background 0.3s;
        }

        button:hover {
            background: linear-gradient(to right, #7b1fa2, #c2185b);
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Connect to Wi-Fi</h1>
        <form id="wifiForm">
            <label for="ssid">Wi-Fi Name (SSID):</label>
            <input type="text" id="ssid" name="ssid" placeholder="Enter Wi-Fi name" required>
            <label for="password">Password:</label>
            <input type="password" id="password" name="password" placeholder="Enter Wi-Fi password" required>
            <button type="submit">Connect</button>
        </form>
    </div>

    <script>
        document.getElementById('wifiForm').addEventListener('submit', function(e) {
            e.preventDefault();
            const ssid = document.getElementById('ssid').value;
            const password = document.getElementById('password').value;
            fetch('/connect', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/x-www-form-urlencoded'
                },
                body: 'ssid=' + encodeURIComponent(ssid) + '&password=' + encodeURIComponent(password)
            }).then(response => response.json())
              .then(data => alert(data.message))
              .catch(error => console.error('Error:', error));
        });
    </script>
</body>
</html>
)rawliteral";



String readStringFromNVS(const char* key) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        Serial.println("Error opening NVS handle");
        return "";
    }

    size_t required_size;
    err = nvs_get_str(nvs_handle, key, NULL, &required_size);
    if (err != ESP_OK) {
        Serial.printf("Failed to get %s from NVS\n", key);
        nvs_close(nvs_handle);
        return "";
    }

    char* value = new char[required_size];
    err = nvs_get_str(nvs_handle, key, value, &required_size);
    if (err == ESP_OK) {
        Serial.printf("Read %s: %s\n", key, value);
    } else {
        Serial.printf("Failed to get %s\n", key);
        delete[] value;
        nvs_close(nvs_handle);
        return "";
    }

    String result = String(value);
    delete[] value;
    nvs_close(nvs_handle);
    return result;
}


bool nvs_config(void)
{
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      // NVS partition was truncated and needs to be erased
      // Retry nvs_flash_init
      nvs_flash_erase();
      err = nvs_flash_init();
  }

  if (err != ESP_OK) {
      Serial.println("Failed to initialize NVS");
      return false;
  }

  ssid_robot = readStringFromNVS("ssid");
  password_robot = readStringFromNVS("password");

  if (ssid_robot.length() > 0 && password_robot.length() > 0) {
      Serial.println("Connecting to WiFi with stored credentials...");
      if(connectToWiFi(ssid_robot.c_str(), password_robot.c_str()))
      {
        return true;
      }
      else {
        return false;
      }
  } else {
      Serial.println("No WiFi credentials found in NVS.");
      return false;
  }
}

void saveWifiCredentials(const char* ssid, const char* password) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err == ESP_OK) {
        // Write SSID to NVS
        err = nvs_set_str(nvs_handle, "ssid", ssid);
        if (err == ESP_OK) {
            Serial.println("SSID saved");
        }

        // Write Password to NVS
        err = nvs_set_str(nvs_handle, "password", password);
        if (err == ESP_OK) {
            Serial.println("Password saved");
        }

        // Commit changes to NVS
        err = nvs_commit(nvs_handle);
        if (err != ESP_OK) {
            Serial.println("Failed to commit to NVS");
        }

        // Close NVS handle
        nvs_close(nvs_handle);
    } else {
        Serial.println("Error opening NVS handle!");
    }
}

// Hàm kết nối Wi-Fi
bool connectToWiFi(const char* ssid, const char* password) {
    int timeout_wf = 0;
    WiFi.begin(ssid, password);
    Serial.println("Connecting to Wi-Fi...");
    
    while ((WiFi.status() != WL_CONNECTED) && (timeout_wf < 5)){
        delay(1000);
        Serial.print(".");
        timeout_wf++;
    }
    if(timeout_wf >= 5)
    {
        Serial.println();
        Serial.println("Can't connect to Wi-Fi");
        return false;
    }
    Serial.println();
    Serial.println("Connected to Wi-Fi");
    return true;
}

bool check_Wifi_connect(const char* ssid, const char* password)
{
    static int cnt_time = 0;
    WiFi.begin(ssid, password);
    
    if ((WiFi.status() != WL_CONNECTED) && (cnt_time < TIME_OUT_3S))
    {
        Serial.print(".");
        cnt_time++;
    }

    if(cnt_time >= TIME_OUT_3S)
    {
        return false;
    }
    return true;
}

// bool checkInternet() {
//     static int cnt_time_connect = 0;

//     if(Ping.ping("8.8.8.8"))  // Check by connecting to google.com
//     {
//         cnt_time_connect++;
//         if(cnt_time_connect > TIME_OUT_PING)
//         {
//             Serial.println("Connect succesfully");
//             cnt_time_connect = 0;
//             return true;
//         }
//     }
//     else
//     {
//         cnt_time_connect--;
//         if(cnt_time_connect < -TIME_OUT_PING)
//         {
//             Serial.println("Connect failed");
//             cnt_time_connect = 0;
//             return false;
//         }
//     }
//     Serial.printf("cnt_time_connect = %d\r\n", cnt_time_connect);
//     return true;
// }


bool checkInternet() {
    static int cnt_time_connect = 0;

    if(internet_connect)  // Check by connecting to google.com
    {
        cnt_time_connect++;
        if(cnt_time_connect > TIME_OUT_PING)
        {
            Serial.println("Connect succesfully");
            cnt_time_connect = 0;
            return true;
        }
    }
    else
    {
        cnt_time_connect--;
        if(cnt_time_connect < -TIME_OUT_PING)
        {
            Serial.println("Connect failed");
            cnt_time_connect = 0;
            return false;
        }
    }
    // Serial.printf("cnt_time_connect = %d\r\n", cnt_time_connect);
    return true;
}

// Xử lý yêu cầu POST từ form kết nối Wi-Fi
void handleConnect(AsyncWebServerRequest *request) {
  if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
    String ssid = request->getParam("ssid", true)->value();
    String password = request->getParam("password", true)->value();
    
    // Gửi thông tin Wi-Fi đến cổng Serial
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("Password: ");
    Serial.println(password);

    saveWifiCredentials(ssid.c_str(), password.c_str());

    // Kết nối Wi-Fi
    connectToWiFi(ssid.c_str(), password.c_str());
    
    // Gửi phản hồi JSON
    request->send(200, "application/json", "{\"message\":\"Connecting to Wi-Fi...\"}");
  } else {
    request->send(400, "application/json", "{\"message\":\"Invalid Request\"}");
  }
}


void connect_wifi_server()
{
    // Tạo điểm truy cập Wi-Fi
  WiFi.softAP(robot_ssid, robot_password);
  Serial.println("Access Point Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Khởi động mDNS để ánh xạ tên miền cục bộ "stepup.local"
  if (!MDNS.begin("stepup")) {  // Tên miền sẽ là "http://stepup.local"
    Serial.println("Error starting mDNS");
  } else {
    Serial.println("mDNS responder started at stepup.local");
  }

  // Xử lý các yêu cầu đến
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", htmlPage);
  });

  server.on("/connect", HTTP_POST, handleConnect);

  // Khởi động máy chủ web
  server.begin();
}


void disconnect_wifi_server() 
{
    // Stop the web server
    server.end();
    Serial.println("Web server stopped");

    // Disconnect the WiFi access point
    WiFi.softAPdisconnect(true);
    Serial.println("WiFi Access Point disconnected");

    // Optionally, stop the mDNS responder
    MDNS.end();
    Serial.println("mDNS responder stopped");
}