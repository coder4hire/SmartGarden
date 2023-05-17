 ///////////////////////////////////////////////////////////////////
// Camera interaction code parts were taken from http://psenyukov.ru/%D0%B2%D0%B8%D0%B4%D0%B5%D0%BE%D0%BA%D0%B0%D0%BC%D0%B5%D1%80%D0%B0-%D0%BD%D0%B0-esp32/
// Thanks to author
//

#include "DHT.h"
#include "esp_camera.h"
#include "esp_wifi.h"
#include <WiFi.h>
#include "esp_timer.h"
#include "lwip/sockets.h"
#include "img_converters.h"
#include "Arduino.h"
#include "fb_gfx.h"
#include "soc/soc.h" 
#include "soc/rtc_cntl_reg.h"  
#include "esp_http_server.h"
#include <esp_task_wdt.h>
#include "PacketHeader.h"
#include <EEPROM.h>

//#define MANUAL_CONFIG_SWITCH 1

#define BTN_PIN 0
#define WDT_TIMEOUT 90

// Deep/Light sleep time in usec
//#define TIME_TO_SLEEP  (3580ul*1000000ul)
#define TIME_TO_SLEEP  (5ul*1000000ul)
#define CAMSERVER_CONNECTION_RETRY_INTERVAL (10000ul)

// WIFI reconnection time, in ms
#define WIFI_RECONNECT_TIME 30000

struct NetSettingsStruct
{
	const char* ssid;
	const char* password;

	IPAddress local_IP;
	IPAddress gateway;
	const char* serverHost;
};

#include "pwd.h"

uint8_t currentNetSetting=1; // Start looking from this settings preset
int netSettingsNum = sizeof(NetSettings) / sizeof(NetSettingsStruct);

#define PIXFORMAT_JPEG_CONVERTED ((pixformat_t)(PIXFORMAT_JPEG+0x10000))
unsigned long lastTimeWifiChecked=0;
bool isConnected=false;
bool isCameraOn=false;

DHT dht(13, DHT22);

EEPROMClass eeprom;

#if 1
#define dbgPrint(x) Serial.print(x)
#define dbgPrintln(x) Serial.println(x)
#define dbgPrintf Serial.printf
#else
void noprintf(char* str, ...) {}
#define dbgPrint(x)
#define dbgPrintln(x)
#define dbgPrintf noprintf
#endif

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


void blinkNTimes(int n, int interval=300)
{
	digitalWrite(33, 1);
	for (int i = 0; i < n; i++)
	{
		digitalWrite(33, 0);
		delay(interval);
		digitalWrite(33, 1);
		delay(interval);
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
		dbgPrintln("Camera capture failed");
		return NULL;
	}

	size_t out_len, out_width, out_height;
	if (fb->format == PIXFORMAT_JPEG)
	{
		dbgPrintln("HW_JPG");
		retVal = fb;
	}
	else
	{
		dbgPrintln("SW_JPG");
		retVal = (camera_fb_t*)malloc(sizeof(camera_fb_t));
		retVal->width = fb->width;
		retVal->height = fb->height;
		bool res = frame2jpg(fb, 90, &retVal->buf, &retVal->len);
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

void readDHTValues(PacketHeader& header)
{
	// Reading temperature or humidity takes about 2           50 milliseconds!
	float h = NAN;
	float t = NAN;
	int retries = 10;
	while ((isnan(h) || isnan(t)) && retries--)
	{
		h = dht.readHumidity();
		t = dht.readTemperature();
		delay(500);
	}

	dbgPrintf("Humidity: %.1f%%  Temperature: %.1f C\n", h, t);
	header.Temperature = t;
	header.Humidity = h;
}


void initCamera()
{
	digitalWrite(PWDN_GPIO_NUM,0);

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
	config.jpeg_quality = 5;
	config.fb_count = 2;

	// Camera init
	esp_err_t err = esp_camera_init(&config);

	if (err != ESP_OK)
	{
		dbgPrintf("Camera init failed with error 0x%x", err);
		blinkNTimes(2);
		delay(2000);
		ESP.restart();
		return;
	}
	
}

void deinitCamera()
{
	esp_camera_deinit();
	gpio_uninstall_isr_service();
}

void powerOnCamera()
{
	if(!isCameraOn)
	{
		digitalWrite(PWDN_GPIO_NUM, LOW);
		isCameraOn=true;
		delay(1000);
	}
}

void powerOffCamera()
{
	digitalWrite(PWDN_GPIO_NUM, HIGH);
	isCameraOn=false;
}

void startupConfig()
{
	// Configure button
	pinMode(BTN_PIN,INPUT_PULLUP);

	// Switching net config
	blinkNTimes(1,1000);
	blinkNTimes(1,300);
	delay(700);
	blinkNTimes(1,1000);
	blinkNTimes(1,300);
	delay(700);

	// Changing net setting if needed
	if(!digitalRead(BTN_PIN))
	{
		// Entered configuration mode
		blinkNTimes(10,50);		
		long millisStart = 0;
		while(true)
		{
			if(!digitalRead(BTN_PIN))
			{
				if(!millisStart)
				{
					// Key pressed
					millisStart = millis();
				}
				blinkNTimes(1,50);
			}
			else
			{
				delay(100);
			}			

			if(millisStart && digitalRead(BTN_PIN))
			{
				//Key released
				long latency = millis() - millisStart;
				millisStart = 0;
				if(latency>1000)
				{
					// Long press
					break;
				}
				else if(latency>100)
				{
					// Short press
					delay(500);
					currentNetSetting = (currentNetSetting + 1) % netSettingsNum;
					blinkNTimes(currentNetSetting+1,300);
					dbgPrintf("WiFi config is switched manually,settings %d (%s)\n", currentNetSetting+1, NetSettings[currentNetSetting].ssid);
					eeprom.write(0,currentNetSetting);
					delay(500);
				}
			}
		}
	}
}

void setup()
{
	// Watchdog
	esp_task_wdt_init(WDT_TIMEOUT, true);
	esp_task_wdt_add(NULL); 

	// Set flash off
	pinMode(4,INPUT_PULLDOWN);

	// Configure status LED
	pinMode(33, OUTPUT);

	// Reading current net setting
	currentNetSetting = eeprom.read(0);
	dbgPrintf("NetSettingsNum read from EEPROM:%d\n",netSettingsNum);
	if(currentNetSetting>=netSettingsNum)
	{
		dbgPrintln("Invalid netSettingsNum is read from EEPROM, resetting to 0");
		currentNetSetting = 0;
	}

	Serial.begin(115200);
	Serial.setDebugOutput(false);

	startupConfig();

	WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

	pinMode(PWDN_GPIO_NUM,OUTPUT);
	powerOnCamera();

	lastTimeWifiChecked=0;

	digitalWrite(33, 1);

	// Initializing DHT Sensor
	dht.begin();
}

void reconnectIfNeeded()
{
	// if wifi is down, try reconnecting every 30 seconds
	if(WiFi.status() != WL_CONNECTED)
	{
		unsigned long now = millis();
		if (!lastTimeWifiChecked || 
			now > (lastTimeWifiChecked + WIFI_RECONNECT_TIME) || now < lastTimeWifiChecked)
		{
			#ifndef MANUAL_CONFIG_SWITCH
			if(!isConnected)
			{
				// If we successfully connected to some wifi before, let's retry same configuration again first
				currentNetSetting = (currentNetSetting + 1) % netSettingsNum;
			}
			#endif
			dbgPrintf("\nReconnecting, settings %d (%s)\n", currentNetSetting+1, NetSettings[currentNetSetting].ssid);
			
			WiFi.disconnect();
			// Configures static IP address
			if (!WiFi.config(NetSettings[currentNetSetting].local_IP, NetSettings[currentNetSetting].gateway, subnet, primaryDNS, secondaryDNS))
			{
				dbgPrintln("STA Failed to configure");
				blinkNTimes(3);
				delay(2000);
				ESP.restart();
				return;
			}

			digitalWrite(33, 0);
			isConnected=false;
			WiFi.begin(NetSettings[currentNetSetting].ssid, NetSettings[currentNetSetting].password);
			lastTimeWifiChecked = millis();
		}
	}
	else if (!isConnected)
	{
		dbgPrintf("\nWiFi connected,settings %d (%s)\n",currentNetSetting+1, NetSettings[currentNetSetting].ssid);
		blinkNTimes(currentNetSetting+1,100);
		isConnected=true;
	}
}

void loop()
{
	esp_wifi_set_ps(WIFI_PS_NONE);
	const char dummy[32]={};
	PacketHeader header = { 0xCA3217AD, HOST_PWD };

	reconnectIfNeeded();
	esp_task_wdt_reset();

	//--- Sending image to server
	if(isConnected)
	{
		WiFiClient client;
		client.
		if (!client.connect(NetSettings[currentNetSetting].serverHost, port)) 
		{
			dbgPrintf("Connection to CamServer failed (%s)\n",NetSettings[currentNetSetting].ssid);
			//esp_sleep_enable_timer_wakeup(CAMSERVER_CONNECTION_RETRY_INTERVAL*1000ul);
			//esp_light_sleep_start();
			delay(CAMSERVER_CONNECTION_RETRY_INTERVAL);
			return;
		}
		dbgPrintf("Connected to CamServer (%s)\n",NetSettings[currentNetSetting].ssid);

 		//client.setNoDelay(true);
 		//client.setTimeout(WIFI_SOCKET_TIMEOUT);

		readDHTValues(header);

		for(int retries=0; retries<5; retries++)
		{
			initCamera();	
			powerOnCamera();

			camera_fb_t* frame = NULL;
			if ((frame = captureFrame()) != NULL)
			{
				if (frame->len > 0)
				{
					dbgPrintf("Captured frame, size:%d\n", frame->len);
					header.PayloadLength = frame->len;
					client.write((char*)&header, sizeof(header));
					client.write(frame->buf, frame->len);
					// Workaround - sending some junk to be sure that real data was delivered
					client.write(dummy,sizeof(dummy));
					client.flush();
					
					for(int counter = 30; counter>=0; counter--)
					{
						esp_task_wdt_reset();
						unsigned short buf=0;
						dbgPrint(".");
						if(client.available()>=2)
						{
							int bytesRead = client.read((uint8_t*)&buf,2);
							dbgPrintf("Bytes read: %d, data:%x\n",bytesRead,buf);
							if(buf==0x4B4F || bytesRead==-1)
							{
								blinkNTimes(2,150);
								dbgPrintln("Acknowledged (OK)");
								break;
							}
						}
						delay(1000);
					}
					shutdown(client.fd(),SHUT_WR);
					dbgPrintln("Packet is sent");
				}
				close(client.fd());
				client.stop();

				freeFrame(frame);
				powerOffCamera();
				deinitCamera();
					
				// Go to sleep after successfull transaction
				dbgPrintln("Go to sleep...");
				Serial.flush();

				esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP);
				//esp_deep_sleep_start();
				int ret = esp_light_sleep_start();
				dbgPrintf("Light sleep: %d\n",ret);
				return;
			}
			else
			{
				deinitCamera();
			}
		}
		
		close(client.fd());		
		client.stop();
	}
	delay(5000);
}
