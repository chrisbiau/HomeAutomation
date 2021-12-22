# ESP32 FLOOR HEATING

*WARING*

With this dev board --> Keep the **boot** bouton pressed to establish the connection when flash esp.

# Specifications:
* Get the temperatures of differents floor zone.
* Send all sensor values to mqtt server.

# Send sensor temprature (MQTT PUBLISH):
Send "MQTT_EVENT_CONNECTED" when mqtt client is connected to mqtt server by topic: *"/floor/heating/event"*
Send JSON every 10sec to mqtt server by topic: *"/floor/heating/sensor"*

# Hardware
+ Power
  + AZDelivery LM2596S Adaptateur d'alimentation DC-DC Step Down Module [[datasheet][linkId0]]
+ Plateform
  + AZDelivery EESP-WROOM-32, 211-161007, FCC ID: 2AC7Z-ESPWROOM3  [[datasheet][linkId1]]
+ Sensor
  + One Wire Waterproof Temperature Sensor DS18B20 [[datasheet][linkId2]]
  

# 


[linkId0]: https://cdn.shopify.com/s/files/1/1509/1638/files/LM2596S_DC-DC_Netzteil_Adapter_Step_down_Modul_Datenblatt_AZ-Delivery_Vertriebs_GmbH.pdf?v=1609154001 "LM2596S_DC-DC_Netzteil_Adapter_Step_down_Modul_Datenblatt_AZ-Delivery_Vertriebs_GmbH.pdf"

[linkId1]: https://codedocu.de/Daten/Files/3/Files_3091/ESP32_NodeMCU_Developmentboard_az_Delivery.pdf "ESP32_NodeMCU_Developmentboard_az_Delivery"

[linkId2]: http://dlnmh9ip6v2uc.cloudfront.net/datasheets/Sensors/Temp/DS18B20.pdf "Datasheet DS18B20"

