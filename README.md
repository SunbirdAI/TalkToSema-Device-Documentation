# IoT Voice Firmare
This code collects voice feedback from devices deployed by SEMA.
# Required libraries 
- TinyGSM
- SPI
- SD
- TMRPCM
- EEPROM
- ARDUINO-TIMER
- RTClib
- LowPower
# Device operation details Gen 1 - Nov 2022
- wake up on button press and record file
- record count of 5 files
- upload 5 files, reset counter to 1
- no sleep mode
# Device operation details Gen 2 - May 2023
- wake up on button press and record file
- record files continually during the day
- Go to sleep mode
- upload files at given time at night, reset counter to 1
- Go to sleep mode
# Device operation details Gen 3 - Nov 2023
- wake up on button press and record file
- record files while saving their corresponding timestamp of acquisition
- Go to sleep
- wake up device to upload at defined time interval preferrably in evening base on RTC interrupt
- Go to sleep
# Authored by:
- HW Team(JTM)
