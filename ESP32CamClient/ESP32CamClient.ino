 ///////////////////////////////////////////////////////////////////
// Camera interaction code parts were taken from http://psenyukov.ru/%D0%B2%D0%B8%D0%B4%D0%B5%D0%BE%D0%BA%D0%B0%D0%BC%D0%B5%D1%80%D0%B0-%D0%BD%D0%B0-esp32/
// Thanks to author
//

#include "DHT.h"
#include "esp_camera.h"
#include "esp_wifi.h"
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
#include "WiFiClientExt.h"

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
	// Reading temperature or humidity takes about 250 milliseconds!
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
		deinitCamera();
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
	pinMode(BTN_PIN,INPUT);
	pinMode(BTN_PIN,INPUT_PULLUP);
	digitalWrite(BTN_PIN,1);	
    dbgPrintln(".1");
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
		dbgPrintln(".2");
		// Entered configuration mode
		blinkNTimes(10,50);		
		long millisStart = 0;
		while(true)
		{
			long latency = millis() - millisStart;

			if(!digitalRead(BTN_PIN))
			{
				if(!millisStart)
				{
					// Key pressed
					millisStart = millis();
				}
				dbgPrintln(".3");
				blinkNTimes(1,50);

				if(latency>10000) // something gone wrong, normally button should not be pressed more than 10 seconds
				{
					dbgPrintln("Something gone wrong, BTN PIN has been stuck. Resetting...");
					powerOffCamera();
					deinitCamera();
					ESP.restart();
				}

			}
			else if(millisStart)
			{
				dbgPrintln(".4");
				delay(100);

			 	//Key released
			 	millisStart = 0;
			 	if(latency>1000)
			 	{
			 		// Long press
			 		break;
			 	}
			// 	else if(latency>100)
			// 	{
			// 		// Short press
			// 		delay(500);
			// 		currentNetSetting = (currentNetSetting + 1) % netSettingsNum;
			// 		blinkNTimes(currentNetSetting+1,300);
			// 		dbgPrintf("WiFi config is switched manually,settings %d (%s)\n", currentNetSetting+1, NetSettings[currentNetSetting].ssid);
			// 		eeprom.write(0,currentNetSetting);
			// 		delay(500);
			//	}
			}
		}
	}
}

void setup()
{
	// Watchdog
	esp_task_wdt_init(WDT_TIMEOUT, true);
	esp_task_wdt_add(NULL); 

	Serial.begin(115200);
	Serial.setDebugOutput(false);
 	for(int i=0; i<20 && !Serial; i++)
	{
		delay(100);
	}
	dbgPrintln("----- Start ------");
	
	esp_wifi_start();
	isConnected=true;

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

	dbgPrintln("Init 1");
	startupConfig();
	dbgPrintln("Init 2");
	WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

	dbgPrintln("Init 3");
	pinMode(PWDN_GPIO_NUM,OUTPUT);
	dbgPrintln("Init 4");
	powerOnCamera();

	dbgPrintln("Init 5");
	lastTimeWifiChecked=0;

	dbgPrintln("Init 6");
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
		if(eeprom.read(0)!=currentNetSetting)
		{
			dbgPrintf("Storing new default connection (%d: %s) to EEPROM\n",currentNetSetting,NetSettings[currentNetSetting].ssid);
			eeprom.write(0,currentNetSetting);
		}
		isConnected=true;
	}
}

bool SendPhoto()
{
	const char dummy[32]={};
	PacketHeader header = { 0xCA3217AD, HOST_PWD };

	if(isConnected)
	{
		WiFiClientExt client;
		if (!client.connect(NetSettings[currentNetSetting].serverHost, port)) 
		{
			dbgPrintf("Connection to CamServer failed (%s)\n",NetSettings[currentNetSetting].ssid);
			//esp_sleep_enable_timer_wakeup(CAMSERVER_CONNECTION_RETRY_INTERVAL*1000ul);
			//esp_light_sleep_start();
			delay(CAMSERVER_CONNECTION_RETRY_INTERVAL);
			return false;
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
					dbgPrintf("(Retry %d) Captured frame, size:%d\n", retries, frame->len);
					header.PayloadLength = frame->len;
					size_t written=0;
//					if(client.write((char*)&header, sizeof(header))!=sizeof(header))
					if(client.writeBlocking((uint8_t*)&header, sizeof(header))!=sizeof(header))
					{
						dbgPrintf("Can't write to socket (1), isConn:%d",(int)client.connected());
						break;
					}

					if(client.writeBlocking(frame->buf, frame->len)<(size_t)frame->len)
					{
						dbgPrintf("Can't write to socket (2), isConn:%d",(int)client.connected());
						break;
					}
					// for(unsigned int k=0; k<frame->len; k+=written)
					// {
					// 	unsigned int len = frame->len-k < 512 ? frame->len-k : 512;
					// 	if((written = client.write(frame->buf+k, len))!=len)
					// 	{
					// 		if(!client.connected())
					// 		{
					// 			dbgPrintf("Can't write to socket (2), isConn:%d",(int)client.connected());
					// 			break;
					// 		}
					// 		dbgPrintf("p%03d",written);
					// 	}
					// 	else {
					// 		dbgPrintf("s");
					// 	}
					// }
					dbgPrintf("\n");

					// Workaround - sending some junk to be sure that real data was delivered
					if(client.write(dummy,sizeof(dummy))!=sizeof(dummy))
					{
						dbgPrintf("Can't write to socket (3), isConn:%d",(int)client.connected());
						break;
					}
					client.flush();
					int counter = 45;
					for(; counter>=0; counter--)
					{
						esp_task_wdt_reset();
						unsigned short buf=0;
						dbgPrint("-");
						if(client.available()>=2)
						{
							int bytesRead = client.readBytes((uint8_t*)&buf,2);
							dbgPrintf("Bytes read: %d, data:%x\n",bytesRead,buf);
							if(buf==0x4B4F || bytesRead==-1)
							{
								blinkNTimes(2,150);
								dbgPrintln("Acknowledged (OK)");
								break;
							}
						}
						// if(client.write(dummy,sizeof(dummy))!=sizeof(dummy))
						// {
						// 	dbgPrintf("Can't write to socket (4), isConn:%d",(int)client.connected());
						// 	break;
						// }
						
						client.flush();
						delay(300);
					}

					if(counter==-1)
					{
						freeFrame(frame);
						powerOffCamera();
						deinitCamera();
						break;
					}

					client.flush();
					shutdown(client.fd(),SHUT_WR);
					dbgPrintln("Packet is sent");
				}
				close(client.fd());
				client.stop();

				freeFrame(frame);
				powerOffCamera();
				deinitCamera();
				return true;
			}
			else
			{
				deinitCamera();
			}
		}
		
		close(client.fd());		
		client.stop();
	}
	return false;
}


void loop()
{
	dbgPrintln("L 1");
	esp_wifi_set_ps(WIFI_PS_NONE);

	dbgPrintln("L 2");
	reconnectIfNeeded();
	dbgPrintln("L 3");
	esp_task_wdt_reset();

	dbgPrintln("L 4");
	//--- Sending image to server
	if(SendPhoto())
	{
		WiFi.disconnect();

		esp_wifi_stop();
		esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP);

		// Go to sleep after successfull transaction
		dbgPrintln("Go to sleep...");
		Serial.flush();

		//esp_deep_sleep_start();
		int ret = esp_light_sleep_start();
		//dbgPrintf("Light sleep: %d\n",ret);
		dbgPrintln("----- Wakeup, Neo ------");

		// Configure button
		pinMode(BTN_PIN,INPUT_PULLUP);
		esp_wifi_start();
		isConnected=true;		
		lastTimeWifiChecked = 0;
		return;						
	}

	delay(5000);
}
