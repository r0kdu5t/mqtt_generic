# **mqtt_generic**

_Forked from Arduino_mqtt_temperature by J Archer._

Arduino sketch to retrieve current temperature from a Dallas ds18b20 and push to MQTT

This is essentially a base sketch for all Arduino based home automation boards for my house. It does several things,

1) uses the DS18B20 sensors unique ID to generate a mac address for use with an ethernet shield and starts above network using generated mac and DHCP, this allows the same piece of code to be copied to multiple boards without massive effort to configure the networking.

2) As we are using the DS18B20 for mac generation we may as well use the data so the sensor is actually probed for temperatures at a regular interval.

3) the data is then farmed off to a data logger or end point using MQTT.

You must copy the CONFIG.h.sample file to CONFIG.h and edit your own variables for this to work.

* MQTT_BROKER - set this to the hostname or IP of your MQTT Broker
* TOPIC_BASE - set the the base topic you want to be published on here
* MAC - Possible fall back mac address.
* ONE_WIRE_BUS 2 - the pin a 1-Wire sensor connects to. If used.

4) #### Libraries in use.
* Blah
* Blah2
* PubSubClient
    - *Check on the value of **MQTT_MAX_PACKET_SIZE** in 'PubSubClient.h'*

