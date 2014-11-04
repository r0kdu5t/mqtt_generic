mqtt_generic
========================

Arduino sketch to retrieve current temperature from a Dallas DS18B20 and push to MQTT. With thanks and appreciation of work others in the community have done to provide the ground work.


This is essentially a base sketch. It does several things,

1) Uses either the DS18B20 sensors unique ID to generate a MAC address or for boards so equiped can retrieve the MAC address from a Microchip 24AA125E48 I2C ROM. Starts above network using generated MAC and DHCP, this allows the same piece of code to be copied to multiple boards without massive effort to configure the networking.

2) If using the DS18B20 for MAC generation we may as well use the data so the sensor is actually probed for temperatures at a regular interval.

3) The data is then farmed off to a data logger or end point using MQTT.

You must copy the config.h.sample file to config.h and edit your own variables for this to work.

MQTT_SERVER - set this to the hostname or IP of your MQTT Broker

TEMP_TOPIC set the topic you wish the temperature to be published to here

CLIENT_ID - This can be anything but should be unique to this sketch - this will probably be changed in future
