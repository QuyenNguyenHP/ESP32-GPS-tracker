# Selected tracker hardware

Use the photographed **Seeed Studio XIAO ESP32-S3**, plus the existing XIAO GNSS add-on and patch antenna **once its receiver identity is confirmed**. There is no need to replace a working ESP32. If buying a receiver, select **Seeed L76K GNSS Module for XIAO, SKU 109100021**, with its matching active antenna. Selection is based on physical compatibility and reuse, not a comparison of all available receivers.

The XIAO ESP32-S3 provides Wi-Fi and BLE in a 21 × 17.8 mm footprint. It has battery charge management. Follow its manufacturer instructions for battery pads and antenna connection. [Manufacturer documentation](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/).

The L76K reference board measures 18 × 21 mm, supports multiple constellations and uses default 9600-baud NMEA UART. Its supplied active patch uses a U.FL connection. It is a suitable compact pairing for the XIAO. [Manufacturer documentation](https://wiki.seeedstudio.com/get_start_l76k_gnss/).

The photographed add-on is shown from its back, so the chip model is not established. The older L76-L uses a different XIAO pin assignment and is marked on hold by Seeed. Check the front marking before wiring. [Older module documentation](https://wiki.seeedstudio.com/gnss_for_xiao/).

## Parts to use or obtain

| Part | Decision |
|---|---|
| ESP32 | Reuse photographed XIAO ESP32-S3 |
| GNSS | Reuse after identification; L76K selected for a new purchase |
| GNSS patch | Reuse only with confirmed matching receiver/antenna requirements |
| ESP32 Wi-Fi/BLE antenna | Matching XIAO 2.4 GHz U.FL antenna; separate from GNSS patch |
| Power for initial build | USB-C cable and stable USB supply |
| Battery later | Protected single-cell 3.7 V nominal LiPo; actual size and allowed charge current to be verified before selection |
| Assembly | Compatible low-profile headers, insulating foam tape, four M2.5 × 8 mm screws |

No cellular modem is needed for the chosen nearby Wi-Fi/Bluetooth use. GNSS receives position; ESP32 sends it. Wi-Fi reporting requires hotspot credentials and an endpoint. BLE reporting requires a phone application or GATT client; owning a phone alone does not provide a reporting integration. These services and firmware are a subsequent build step.

## Connections: verify board version first

For a correctly oriented XIAO-compatible socket stack, use its matching headers and alignment marks. For separate wires, connect 3.3 V to the breakout's documented 3V input, common GND, receiver TX to ESP32 RX and receiver RX to ESP32 TX. Use 3.3 V UART logic. Do not apply battery voltage to the 3V3 pin. Keep the GNSS patch connected to the GNSS RF socket; the ESP32 RF socket takes its own Wi-Fi/BLE antenna.

| Confirmed module | ESP32 receives GNSS TX on | ESP32 transmits to GNSS RX on |
|---|---|---|
| L76K XIAO add-on | D7 / GPIO44 | D6 / GPIO43 |
| Older L76-L XIAO add-on | D3 / GPIO4 | D2 / GPIO3 |

These assignments follow the two module guides above; connector stacking orientation still needs inspection. ESP32 hardware UART can be routed to these pins. Set 9600 baud for initial testing unless the receiver was reconfigured.

## Bring-up sequence

1. Start on USB power. Confirm the XIAO enumerates and can run a serial example.
2. Identify the GNSS receiver, inspect soldering and connect the matching antennas. Confirm pin orientation before power.
3. Read raw NMEA on its documented pins; then parse position, fix validity, fix age, UTC and satellite count. Never send stale coordinates as a fresh fix.
4. Test outdoors with the ceramic face skyward. Compare open-case and closed-case acquisition and reception.
5. Implement BLE position notifications or Wi-Fi upload for the intended phone/hotspot, then measure actual current and select battery capacity from required runtime. No runtime estimate is validated yet.
6. Check battery polarity, protection and allowed charging current against the board's charging specification before connection. Provide removable lead disconnection or a suitably rated switch if desired; the present case has no dedicated switch opening.

Only hardware selection and enclosure drafting are delivered here. Physical build and firmware have not been tested.
