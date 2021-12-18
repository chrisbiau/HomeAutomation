# ESP32 GARDEN WATERING TIMER

# Specifications:
* Get the status of a valve in jeedom.
* Control the valve from jeedom.
* If connection is lost, set up a safety timer to close all valve.

# Get status
Send state **open** or **close** valve id to mqtt server by topic: *"garden/valve/id/state"*

# Publish command
Send command **open** or **close** valve id to esp32 by topic: *"garden/valve/id/command"*

# Hardware
+ Power
+ ESP32
+ Relay
  + 12 ways
+ Sensor
+ Valve

# 