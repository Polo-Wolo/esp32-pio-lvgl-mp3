# pio-lvgl
PlatformIo ESP32 display with LVGL


##### Sources
###### Micro-controller link
https://docs.waveshare.com/ESP32-S3-AMOLED-1.91?variant=ESP32-S3-Touch-AMOLED-1.91-M

### Windows unpacking issue
I had an issue unpacking `esp32-core`, this is due to windows path character limited.

Extend windows paths :

```sh
reg add "HKLM\SYSTEM\CurrentControlSet\Control\FileSystem" /v LongPathsEnabled /t REG_DWORD /d 1 /f
```

Delete `C:/Users/YOU/.platformio/.cache`

Restart you PC and try again.