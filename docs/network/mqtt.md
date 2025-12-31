# MQTT

<img width="320" src="https://github.com/user-attachments/assets/77108478-e2bb-4892-928e-3f60fbda5d32" />

## Home Assistant setup

In Home Assistant

* create non admin user in home assistant e.g mqttuser
* install the Mosquitto MQTT Broker via Settings / Devices & Services, it will auto-discover the MoonLight devices
* Optionally enable TLS (Transport Layer Security) for secure connections in MQTT / Configure
 
In MoonLight / Network / MQTT:

* Enable MQTT
* URI: mqtt://homeassistant.local:1883 (non-secure)
    * replace homeassistant.local by its appropriate mDNS name or IP address
    * secure HA: use mqtts://homeassistant.local:8883
    * to test outside HA e.g. use mqtts://broker.hivemq.com:8883
* Username / password: mqttuser credentials
* client id: ml-xxxx.
* Apply Settings

MoonLight devices show up in HA as follows:

<img width="320" src="https://github.com/user-attachments/assets/9740c664-e8be-4ddc-9aea-4d46e995b10a" />

On QoS

Typical configurations:

* High reliability: Subscribe QoS 1, Publish QoS 1 (at-least-once delivery both ways)
* Performance: Subscribe QoS 1, Publish QoS 0 (reliable commands, fire-and-forget state)
* Low bandwidth: Subscribe QoS 0, Publish QoS 0 (best-effort, no guarantees)
* Your original (subscribe 2, publish 0) was unusual but valid — QoS 2 for commands ensures exactly-once, QoS 0 for state is fine if you don't need delivery guarantees.
