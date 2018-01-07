# mbed_code
by Pieter Hap

This code will generate a tone on the mbed making use of MQTT.

You will have to use the mbed compiler to get the code on the mbed.

> https://os.mbed.com/compiler/

You will also have to install MQTTBox for this to work.

> https://chrome.google.com/webstore/detail/mqttbox/kaajoficamnjijhkeomgfljpicifbkaf

After you installed and opened MQTTBox, you will have to add a new MQTT Client. You will have to fill in an MQTT Client Name, change the protocol to mqtt/tcp en change the host to mqtt.labict.be:1883.

After you saved it you will have to fill in a Topic to publish (in this case it's pieter/mqtt). To test it you can fill in the Payload with a Hertz-value (e.g. 440) and click on the 'Publish'-button.