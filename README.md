mqtt_generic
========================

Arduino sketch to create MQTT beacon.This is essentially a base sketch. With many thanks and appreciation of others in the community whom have done much to provide the ground work.


It can can be configured to use either;

1) the DS18B20 sensors unique ID to generate a MAC address. 
2) can retrieve the MAC address from a Microchip 24AA125E48 I2C ROM.
Starts above network using generated MAC and DHCP, this allows the same piece of code to be copied to multiple boards without massive effort to configure the networking.

If using the DS18B20 for MAC generation we may as well use the data so the sensor is actually probed for temperatures at a regular interval.

The data is then farmed off to a data logger or end point using MQTT.

You must copy the config.h.sample file to config.h and edit your own variables for this to work.

ONE_WIRE_BUS - Set to the Ardunio pin used for 1-Wire bus

MQTT_SERVER - Set this to the hostname or IP of your MQTT Broker
