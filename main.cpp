/*******************************************************************************
 * Copyright (c) 2014, 2015 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Ian Craggs - make sure QoS2 processing works, and add device headers
 *******************************************************************************/

 /**
  This is a sample program to illustrate the use of the MQTT Client library
  on the mbed platform.  The Client class requires two classes which mediate
  access to system interfaces for networking and timing.  As long as these two
  classes provide the required public programming interfaces, it does not matter
  what facilities they use underneath. In this program, they use the mbed
  system libraries.

 */

 // change this to 1 to output messages to LCD instead of serial
 #define USE_LCD 0
 
 #if USE_LCD
 #include "C12832.h"
 
 // the actual pins are defined in mbed_app.json and can be overridden per target
 C12832 lcd(LCD_MOSI, LCD_SCK, LCD_MISO, LCD_A0, LCD_NCS);
 
 #define logMessage lcd.cls();lcd.printf
 
 #else
 
 #define logMessage printf
 
 #endif
 
 #define MQTTCLIENT_QOS2 1
 
 #include "easy-connect.h"
 #include "MQTTNetwork.h"
 #include "MQTTmbed.h"
 #include "MQTTClient.h"
 #include <stdlib.h>
 #include "mbed.h"
 
 PwmOut speaker(p26);
 
 int arrivedcount = 0;
 
 /* Mail */
 typedef struct {
 float sound;
 } mail_t;
 
 
 Mail<mail_t, 16> mail_box;
 
 
 void messageArrived(MQTT::MessageData& md)
 {
     MQTT::Message &message = md.message;
     logMessage("Message arrived: qos %d, retained %d, dup %d, packetid %d\r\n", message.qos, message.retained, message.dup, message.id);
     logMessage("Payload %.*s\r\n", message.payloadlen, (char*)message.payload);
     ++arrivedcount;
     
     mail_t *mail = mail_box.alloc();
     
     float message_to_float = atof((char*)message.payload);
     
     float speaker_period = (1.0 / message_to_float);
     
     mail->sound = speaker_period;
     mail_box.put(mail);
 }
 
 
 void send_thread () {
     while (true) {
         osEvent evt = mail_box.get();
         if (evt.status == osEventMail) {
             mail_t *mail = (mail_t*)evt.value.p;
             speaker.period(mail->sound);
             speaker =0.5; //50% duty cycle - max volume
             wait(3);
             speaker=0.0; // turn off audio
             wait(0.5);
             mail_box.free(mail);
         }
     }
 }
 
 
 int main(int argc, char* argv[])
 {
     
     float version = 0.6;
     char* topic = "pieter/mqtt";
 
     logMessage("HelloMQTT: version is %.2f\r\n", version);
 
     NetworkInterface* network = easy_connect(true);
     if (!network) {
         return -1;
     }
 
     MQTTNetwork mqttNetwork(network);
 
     MQTT::Client<MQTTNetwork, Countdown> client(mqttNetwork);
 
     const char* hostname = "labict.be";
     int port = 1883;
     logMessage("Connecting to %s:%d\r\n", hostname, port);
     int rc = mqttNetwork.connect(hostname, port);
     if (rc != 0)
         logMessage("rc from TCP connect is %d\r\n", rc);
 
     MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
     data.MQTTVersion = 3;
     data.clientID.cstring = "pieter";
     if ((rc = client.connect(data)) != 0)
         logMessage("rc from MQTT connect is %d\r\n", rc);
 
     if ((rc = client.subscribe(topic, MQTT::QOS2, messageArrived)) != 0)
         logMessage("rc from MQTT subscribe is %d\r\n", rc);
     
     Thread thread(send_thread);
 
     while(true){
         client.yield(100);
     }
 
     return 0;
     
 }
 