# MQTT

<img width="320" src="https://github.com/user-attachments/assets/51e336fe-4935-4135-a9f8-bf469c67de8e" />

## Home Assistant setup

In Home Assistant

* create non admin user in home assistant e.g mqttuser
* install the Mosquitto MQTT Broker via Settings / Devices & Services, it will auto discover the MoonLight devices
 
In MoonLight / Network / MQTT:

* Enable MQTT
* URI: mqtt://homeassistant.local:1883
    * replace homeassistant.local by it appropriate mdns name or IP address
    * to test outsde HA e.g. use mqtts://broker.hivemq.com:8883\
* Username / password: mqttuser credentials
* client id: ml-xxxx. to do: use hostname
* Apply Settings

