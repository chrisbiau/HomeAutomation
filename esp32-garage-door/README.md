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



https://docs.platformio.org/en/latest/projectconf/index.html
https://github.com/platformio/platform-espressif32/blob/master/examples/espidf-hello-world/src/hello_world_main.c


https://exploreembedded.com/wiki/Hello_World_with_ESP32_Explained


--- Available filters and text transformations: colorize, debug, default, direct, esp32_exception_decoder, hexlify, log2file, nocontrol, printable, send_on_enter, time
--- More details at https://bit.ly/pio-monitor-filters
--- Miniterm on COM4  115200,8,N,1 ---
--- Quit: Ctrl+C | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
green...
blue...
red...
ESP-ROM:esp32s2-rc4-20191025
Build:Oct 25 2019
rst:0x1 (POWERON),boot:0x8 (SPI_FAST_FLASH_BOOT)
SPIWP:0xee
mode:DIO, clock div:2
load:0x3ffe8100,len:0x4
load:0x3ffe8104,len:0x18f0
load:0x40050000,len:0x1678
load:0x40054000,len:0x2100
entry 0x40050334
␛[0;32mI (49) boot: ESP-IDF v4.2-dev-461-g6fd4904-dirty 2nd stage bootloader␛[0m
␛[0;32mI (49) boot: compile time 17:09:57␛[0m
␛[0;32mI (49) boot: chip revision: 0␛[0m
␛[0;32mI (52) boot.esp32s2: SPI Speed      : 40MHz␛[0m
␛[0;32mI (57) boot.esp32s2: SPI Mode       : DIO␛[0m
␛[0;32mI (62) boot.esp32s2: SPI Flash Size : 2MB␛[0m
␛[0;32mI (66) boot: Enabling RNG early entropy source...␛[0m
␛[0;32mI (72) boot: Partition Table:␛[0m
␛[0;32mI (75) boot: ## Label            Usage          Type ST Offset   Length␛[0m
␛[0;32mI (83) boot:  0 nvs              WiFi data        01 02 00009000 00006000␛[0m
␛[0;32mI (90) boot:  1 phy_init         RF data          01 01 0000f000 00001000␛[0m
␛[0;32mI (98) boot:  2 factory          factory app      00 00 00010000 00100000␛[0m
␛[0;32mI (105) boot: End of partition table␛[0m
␛[0;32mI (109) esp_image: segment 0: paddr=0x00010020 vaddr=0x3f000020 size=0x05454 ( 21588) map␛[0m
␛[0;32mI (124) esp_image: segment 1: paddr=0x0001547c vaddr=0x3ffbd740 size=0x01c2c (  7212) load␛[0m
␛[0;32mI (129) esp_image: segment 2: paddr=0x000170b0 vaddr=0x40024000 size=0x00404 (  1028) load␛[0m
␛[0;32mI (136) esp_image: segment 3: paddr=0x000174bc vaddr=0x40024404 size=0x08b5c ( 35676) load␛[0m
␛[0;32mI (157) esp_image: segment 4: paddr=0x00020020 vaddr=0x40080020 size=0x139a8 ( 80296) map␛[0m
␛[0;32mI (179) esp_image: segment 5: paddr=0x000339d0 vaddr=0x4002cf60 size=0x007d4 (  2004) load␛[0m
␛[0;32mI (186) boot: Loaded app from partition at offset 0x10000␛[0m
␛[0;32mI (186) boot: Disabling RNG early entropy source...␛[0m
␛[0;32mI (188) cache: Instruction cache         : size 8KB, 4Ways, cache line size 32Byte␛[0m
␛[0;32mI (196) cpu_start: Pro cpu up.␛[0m
␛[0;32mI (199) cpu_start: Single core mode␛[0m
␛[0;32mI (204) heap_init: Initializing. RAM available for dynamic allocation:␛[0m
␛[0;32mI (211) heap_init: At 3FFBD734 len 0000000C (0 KiB): D/IRAM␛[0m
␛[0;32mI (217) heap_init: At 3FFBFB28 len 0003C4D8 (241 KiB): D/IRAM␛[0m
␛[0;32mI (223) heap_init: At 3FFFC000 len 00003A10 (14 KiB): DRAM␛[0m
␛[0;32mI (230) cpu_start: Pro cpu start user code␛[0m
␛[0;32mI (287) spi_flash: detected chip: generic␛[0m
␛[0;32mI (288) spi_flash: flash io: dio␛[0m
␛[0;33mW (288) spi_flash: Detected size(4096k) larger than the size in the binary image header(2048k). Using the size in the binary image header.␛[0m
␛[0;32mI (298) cpu_start: Starting scheduler on PRO CPU.␛[0m
␛[0;32mI (304) example: esp32s2 IDF_test version: 1.01␛[0m
␛[0;32mI (304) example: esp_idf version:8.2.0␛[0m
␛[0;32mI (314) example: LED Rainbow Chase  Start in GPIO 18␛[0m
red...
green...
blue...
red...
green...
