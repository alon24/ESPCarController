#include <user_config.h>
#include <SmingCore.h>
#include <wiinunchuck.h>
#include <WebsocketClient.h>

#ifndef WIFI_SSID
	#define WIFI_SSID "PleaseEnterSSID" // Put you SSID and Password here
	#define WIFI_PWD "PleaseEnterPass"
#endif

#define sclPin 13
#define sdaPin 14

WiiNunchucks nunchucks;
Timer pollTimer;

WebsocketClient wsClient;
Timer msgTimer;
Timer restartTimer;

int msg_cnt =0;

String ws_Url =  "ws://echo.websocket.org"; //"ws://192.168.1.2:8080/";
void wsDisconnected(bool success);
void wsMessageSent();
void wsConnected(wsMode Mode)
{
	if (Mode == ws_Connected)
	{
		msgTimer.initializeMs(1 * 1000, wsMessageSent);
		msgTimer.start();
	}
	else
	{
		Serial.println("Connection with server not successful. Reconnecting..");
		wsClient.connect(ws_Url);
	}
}
void wsMessageReceived(String message)
{
    Serial.printf("WebSocket message received:\r\n%s\r\n", message.c_str());
}

void restart()
{
	msg_cnt = 0;
	wsClient.connect(ws_Url);

	 msgTimer.setCallback(wsMessageSent);
	 msgTimer.setIntervalMs(1*1000);
	 msgTimer.start();
}
void wsDisconnected(bool success)
{
	if (success == true)
	{
		Serial.println("Websocket Client Disconnected Normally. End of program ..");
		return;
	}
	else
	{
		Serial.println("Websocket Client Disconnected. Reconnecting ..");

	}
	 msgTimer.setCallback(restart);
	 msgTimer.setIntervalMs(5*1000);
	 msgTimer.startOnce();
}


void wsMessageSent()
{
   if(WifiStation.isConnected() == true)
   { // Check if Esp8266 is connected to router
	   	if (msg_cnt >25)
		{
			Serial.println("End Websocket client session");
			wsClient.disconnect(); // clean disconnect.
			msgTimer.stop();
		}
    	else
    	{
			String message = "Hello " + String(msg_cnt++);
			Serial.print("Message to WS Server : ");
			Serial.println(message);
			wsClient.sendMessage(message);
    	}

   }

}

void handleNunchuckPolling() {
	String direction = nunchucks.getDirection();

	debugf("direction = %s", direction.c_str());
}

void init()
{
	Serial.begin(74880); // 74880
	Serial.systemDebugOutput(true); // Debug output to serial

	//setup i2c pins
	Wire.pins(sclPin, sdaPin);

	//Change CPU freq. to 160MHZ
	System.setCpuFrequency(eCF_160MHz);
	wifi_set_sleep_type(NONE_SLEEP_T);

	nunchucks.init();
	pollTimer.initializeMs(200, handleNunchuckPolling);
	pollTimer.start();

//	WifiAccessPoint.enable(false);
//
//	WifiStation.config("AndroidAP", "");
//	WifiStation.enable(true);
//
//
//	WifiStation.waitConnection(connectOk, 30, connectFail);
}

