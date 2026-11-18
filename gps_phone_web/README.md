# Phone GPS web reader for XIAO ESP32-S3

The ESP32 creates a Wi-Fi network and serves a mobile GPS dashboard directly. No router, cloud account or internet is required to read the GPS. The phone does not need a GPS/Bluetooth application. It reads the external GNSS receiver via the ESP32, not the phone's own location.

## Upload and connect

1. Open `GPSPhoneWeb/GPSPhoneWeb.ino` in Arduino IDE. Keep `gps_state.h` and `web_page.h` in the same sketch folder.
2. In Library Manager install **TinyGPSPlus by Mikal Hart**. This project was compiled with TinyGPSPlus 1.0.3 and Espressif ESP32 board package 3.3.12.
3. Choose **XIAO_ESP32S3** and its USB port. Enable **USB CDC On Boot**. Upload the sketch. It replaces the previous raw serial test; serial output prints GPS/Wi-Fi status every two seconds at 115200 baud.
4. On the phone, join Wi-Fi **ESP32-GPS**, password **GPStracker24**. If the phone warns that this network has no internet, choose to stay connected. Disable automatic switching to another Wi-Fi network if necessary.
5. Type **http://192.168.4.1** directly into the phone browser's address bar. Use HTTP. Stay near the tracker.
6. Place the GNSS patch outdoors with the ceramic face toward the sky. The page automatically updates every second. Your previous zero-satellite output will appear as “GPS connected · no fix” until a valid position is acquired.

The XIAO needs its separate compatible 2.4 GHz antenna for reliable phone connectivity. Connect the ceramic GNSS patch to the GNSS receiver, not the ESP32 RF socket.

## Wiring

Use the same separate-wire configuration as the working serial test, with USB power to the XIAO:

| GNSS labeled pad | XIAO ESP32-S3 |
|---|---|
| 3V / 3V3 | 3V3 |
| GND | GND |
| TX | D7 / GPIO44 |
| RX | Disconnected (read only) |

Firmware listens at 9600 baud. GPS_TX_PIN is D6/GPIO43, but no wire is required there. This pin map assumes separate wires to labeled GNSS pads; it does not describe an older L76-L board connected through stack headers. If you already have working wiring on another pin, change `GPS_RX_PIN` in the sketch to match it. Disconnect USB before changing wiring. Do not feed 5 V into the GNSS 3V pad.

## Serial Monitor

Select 115200 baud and enable USB CDC On Boot for the XIAO ESP32-S3. Status repeats every two seconds so opening the monitor after startup still shows information. It includes the web address, connected phone count, received UART character count, checksum failures, satellite count and current fix/coordinates. Set `PRINT_RAW_NMEA=true` in the sketch to also print complete received NMEA sentences.

## Features and behavior

- Live latitude/longitude, satellites used, speed, altitude, HDOP and GPS UTC.
- Copy coordinates, including a fallback for browsers on an HTTP connection.
- Google Maps link for a valid fix. Maps requires internet and may not load while joined to the offline tracker. Copy the coordinates, switch to an internet connection, then paste them into your maps app if needed.
- Expandable raw NMEA diagnostic view, bounded to approximately 1.1 KB of recent text. Diagnostic lines are unfiltered received text, not a claim that every displayed line passed its checksum.
- The parser validates NMEA checksums. GGA/RMC fix status is tracked separately from retained last-known coordinates. No-fix and data older than five seconds produce null coordinates in the API and disable map/copy buttons.
- If the phone loses the ESP32 connection, the page clears readings and retries automatically. A request times out after 2.5 seconds.
- HDOP describes satellite geometry; it is not a distance error estimate.
- No position history is persisted and no automatic uploads occur. The map service receives coordinates only when you open the Maps link.

Change `AP_SSID` and `AP_PASSWORD` in the sketch to customize the network. Password must contain at least eight characters. Anyone joined to this Wi-Fi can read `/api/gps`. The local page uses HTTP inside the password-protected Wi-Fi network.

## Computer preview

Open `web/index.html` in a browser, then append `?demo=1` to its URL. This is explicitly labeled **DEMO PREVIEW**, with sample coordinates unrelated to the receiver. Use `?demo=1&state=nofix` or `?demo=1&state=disconnected` to inspect those states. Opening the HTML file without demo mode cannot access your ESP32's GPS; use the ESP32-served page for real readings.

Edit `web/index.html` and run `python3 embed_page.py` to update the firmware's embedded page before recompiling. No frontend package installation is required. The header is already generated for this release.

## Verification

- Compiled for `esp32:esp32:XIAO_ESP32S3`: 909672 bytes flash, 47040 bytes static RAM with the board defaults.
- Native GPS tests cover no-fix input matching the reported condition, valid position, fix loss, recovery, stale data, GN/GP talkers, hemispheres, bad checksum and timer wrap.
- JavaScript tests cover live readings, no-fix, disconnect, map/copy availability and demo labeling.
- Browser preview could not be inspected because browser access was unavailable. No firmware upload, physical GPS reception test or phone connection test was performed.

To rerun the included checks after installing TinyGPSPlus in the standard Arduino library directory:

```bash
g++ -std=c++17 -DARDUINO=100 -I tests/arduino_stub \
  -I "$HOME/Arduino/libraries/TinyGPSPlus/src" \
  tests/gps_state_test.cpp "$HOME/Arduino/libraries/TinyGPSPlus/src/TinyGPS++.cpp" \
  -o /tmp/gps-state-test
/tmp/gps-state-test
node tests/web_test.cjs
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32S3 GPSPhoneWeb
```

## References

- [Espressif Wi-Fi access point API](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html)
- [TinyGPSPlus parser](https://github.com/mikalhart/TinyGPSPlus)
- [Seeed XIAO ESP32-S3 setup](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/)
