# Install and test mqtt server

## Reference:

[Elilo.net: Mqtt sur jeedom][linkId1]

## Install: 

```
root@jeedom:~#apt-get update
root@jeedom:~#apt-get install mosquitto mosquitto-clients
```
- mosquitto is server (broker)
- mosquitto-clients is MQTT clients : mosquitto_sub, mosquitto_pub, mosquitto_passwd


Centos install:
```
yum install mosquitto
```

## Configuration:

```
/etc/mosquitto/mosquitto.conf
```

## Commands:

Status mosquitto service:
```
/etc/init.d/mosquitto status
```

Stop mosquitto service:
```
/etc/init.d/mosquitto stop
```

Start mosquitto service:
```
/etc/init.d/mosquitto start
```

## Test service:

Listening topic *sensor/temperature*
```
root@jeedom:~#mosquitto_sub -h localhost -t "sensor/temperature"
```

Publish value *25* in topic : *sensor/temperature*
```
root@jeedom:~#mosquitto_pub -h localhost -t "sensor/temperature" -m 25
```

# Secure:

__TODO: __ Ref [Secure the Mosquitto MQTT Messaging Broker][linkId2] 




[linkId1]: https://www.elilo.net/2017/03/mqtt-sur-jeedom.html "lilo.net: Mqtt sur jeedom"

[linkId2]: https://www.digitalocean.com/community/tutorials/how-to-install-and-secure-the-mosquitto-mqtt-messaging-broker-on-centos-7 "Secure the Mosquitto MQTT Messaging Broker"