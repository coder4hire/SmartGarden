///////////////////////////////////////////////////////////////////
// Camera interaction code parts were taken from http://psenyukov.ru/%D0%B2%D0%B8%D0%B4%D0%B5%D0%BE%D0%BA%D0%B0%D0%BC%D0%B5%D1%80%D0%B0-%D0%BD%D0%B0-esp32/
// Thanks to author
//

#include "esp_camera.h"
#include <WiFi.h>
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "fb_gfx.h"
#include "soc/soc.h" 
#include "soc/rtc_cntl_reg.h"  
#include "esp_http_server.h"
#include "pwd.h"

#pragma pack(push, 1)
struct PacketHeader
{
	uint32_t Preamble;
	char Pwd[16];
	int32_t Temperature;
	int32_t Humidity;
	uint32_t PayloadLength;
};
#pragma pack(pop)

#define PIXFORMAT_JPEG_CONVERTED ((pixformat_t)(PIXFORMAT_JPEG+0x10000))
time_t lastTimeWifiChecked;

//const char* ssid = "***";
//const char* password = "***";
//IPAddress local_IP(0,0,0,0);
//IPAddress gateway(0,0,0,0);
//IPAddress subnet(0,0,0,0);

IPAddress primaryDNS(0, 0, 0, 0);   //optional
IPAddress secondaryDNS(0, 0, 0, 0); //optional

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22


void BlinkNTimes(int n)
{
	digitalWrite(33, 1);
	for (int i = 0; i < n; i++)
	{
		digitalWrite(33, 0);
		delay(300);
		digitalWrite(33, 1);
		delay(300);
	}
}

static camera_fb_t* captureFrame()
{
	camera_fb_t* fb = NULL;
	camera_fb_t* retVal = NULL;
	int64_t fr_start = esp_timer_get_time();

	fb = esp_camera_fb_get();
	if (!fb)
	{
		Serial.println("Camera capture failed");
		return NULL;
	}

	size_t out_len, out_width, out_height;
	if (fb->format == PIXFORMAT_JPEG)
	{
		retVal = fb;
	}
	else
	{
		retVal = (camera_fb_t*)malloc(sizeof(camera_fb_t));
		retVal->width = fb->width;
		retVal->height = fb->height;
		bool res = frame2jpg(fb, 80, &retVal->buf, &retVal->len);
		retVal->format = PIXFORMAT_JPEG_CONVERTED;
		esp_camera_fb_return(fb);
		if (!res)
		{
			free(retVal);
			return NULL;
		}
	}
	return retVal;
}

static void freeFrame(camera_fb_t* fb)
{
	if (fb->format == PIXFORMAT_JPEG_CONVERTED)
	{
		free(fb->buf);
		free(fb);
	}
	else
	{
		esp_camera_fb_return(fb);
	}
}

void setup()
{
	WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

	Serial.begin(115200);
	Serial.setDebugOutput(false);

	camera_config_t config;
	config.ledc_channel = LEDC_CHANNEL_0;
	config.ledc_timer = LEDC_TIMER_0;
	config.pin_d0 = Y2_GPIO_NUM;
	config.pin_d1 = Y3_GPIO_NUM;
	config.pin_d2 = Y4_GPIO_NUM;
	config.pin_d3 = Y5_GPIO_NUM;
	config.pin_d4 = Y6_GPIO_NUM;
	config.pin_d5 = Y7_GPIO_NUM;
	config.pin_d6 = Y8_GPIO_NUM;
	config.pin_d7 = Y9_GPIO_NUM;
	config.pin_xclk = XCLK_GPIO_NUM;
	config.pin_pclk = PCLK_GPIO_NUM;
	config.pin_vsync = VSYNC_GPIO_NUM;
	config.pin_href = HREF_GPIO_NUM;
	config.pin_sscb_sda = SIOD_GPIO_NUM;
	config.pin_sscb_scl = SIOC_GPIO_NUM;
	config.pin_pwdn = PWDN_GPIO_NUM;
	config.pin_reset = RESET_GPIO_NUM;
	config.xclk_freq_hz = 20000000;
	config.pixel_format = PIXFORMAT_JPEG;
	config.frame_size = FRAMESIZE_SVGA;
	config.jpeg_quality = 10;
	config.fb_count = 2;

	// Camera init
	esp_err_t err = esp_camera_init(&config);

	pinMode(33, OUTPUT);
	if (err != ESP_OK)
	{
		Serial.printf("Camera init failed with error 0x%x", err);
		BlinkNTimes(2);
		delay(2000);
		ESP.restart();
		return;
	}

	// Configures static IP address
	if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
	{
		Serial.println("STA Failed to configure");
		BlinkNTimes(3);
		delay(2000);
		ESP.restart();
		return;
	}

	// Wi-Fi connection
	digitalWrite(33, 0);
	WiFi.begin(ssid, password);
	for (int i = 0; WiFi.status() != WL_CONNECTED; i++)
	{
		delay(500);
		Serial.print(".");
		if (i == 60)
		{
			WiFi.disconnect();
			digitalWrite(33, 1);
			delay(1000);
			digitalWrite(33, 0);
			Serial.println("\nReconnecting");
		}
	}
	digitalWrite(33, 1);

	Serial.println("");
	Serial.println("WiFi connected");
	digitalWrite(33, 1);

}

void loop()
{
	// if wifi is down, try reconnecting every 30 seconds
	if (WiFi.status() != WL_CONNECTED && millis() > (lastTimeWifiChecked + 30000))
	{
		Serial.println("Reconnecting to WiFi...");
		WiFi.disconnect();
		WiFi.begin(ssid, password);
		lastTimeWifiChecked = millis();
	}

	if (WiFi.status() == WL_CONNECTED)
	{
		digitalWrite(33, 0);
		delay(10);
		digitalWrite(33, 1);
	}

	delay(5000);

	//--- Sending image to server
	WiFiClient client;
	if (client.connect(serverHost, port)) {
		Serial.println("Connection failed.");
		return;
	}

	camera_fb_t* frame = NULL;
	if ((frame = captureFrame()) != NULL)
	{
		PacketHeader header = {0xCA3217AD, HOST_PWD};
		header.PayloadLength = frame->len;
		client.write((char*)&header,sizeof(header));
		client.write(frame->buf, frame->len);
		client.flush();
	}

	client.stop();
}
