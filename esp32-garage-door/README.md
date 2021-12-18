# ESP32 GARAGE DOOR

# Specifications:
* Get the status of the door in jeedom.
* Raise an alert if the door is not open or not close while X seconds.

# Get status:
Send state **open** or **close** door to mqtt server by topic: *"garage/door/state"*


# Hardware
+ Power
  + AZDelivery LM2596S Adaptateur d'alimentation DC-DC Step Down Module [[datasheet][linkId0]]
+ Plateform
  + ESP32-S2 Saola 1R Dev Kit featuring ESP32-S2 WROVER  [[datasheet][linkId1]]
+ Sensor
  + omptoncn NJK18 5002C2 NPN NC Hall magnetic sensor Detection distance 0-10mm   
+ Other
  + adafruit 8-channel Bi-directional Logic Level Converter - TXB0108 [[datasheet][linkId2]]
  

# Status door


[linkId0]: https://cdn.shopify.com/s/files/1/1509/1638/files/LM2596S_DC-DC_Netzteil_Adapter_Step_down_Modul_Datenblatt_AZ-Delivery_Vertriebs_GmbH.pdf?v=1609154001 "LM2596S_DC-DC_Netzteil_Adapter_Step_down_Modul_Datenblatt_AZ-Delivery_Vertriebs_GmbH.pdf"

[linkId1]: https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/get-started/index.html#introduction "Get Started - ESP32-S2 - — ESP-IDF Programming Guide latest documentation"


[linkId2]: https://cdn-shop.adafruit.com/datasheets/txb0108.pdf "TXB0108 8-Bit Bidirectional Voltage-Level Translator With Auto-Direction Sensing (Rev. B)"

