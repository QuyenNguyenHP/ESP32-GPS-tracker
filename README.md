# ESP32 GPS tracker — hardware selection and enclosure v0.1

Initial fit prototype prepared from `35b043d97132f16ca823.jpg`. Reporting choice: Wi-Fi/Bluetooth near a phone or hotspot. The case is ready for a trial print, but fit to the photographed parts is not verified. No physical assembly, firmware upload, radio test or print has been performed.

## Deliverables

- `enclosure/base.stl` and `enclosure/lid.stl`: separate print-oriented parts, millimetres.
- `enclosure/base.step`, `lid.step`, `assembly.step`: editable solid CAD; assembly includes simplified component envelopes, not manufacturer component models.
- `enclosure/build_enclosure.py`: parametric CadQuery source.
- `enclosure/layout.png` / `layout.svg`: dimensioned plan and schematic section.
- `enclosure/validation.json`: geometry checks, not a physical fit certification.
- `hardware/selection.md`: selected parts, connection plan and bring-up sequence.

## Mechanical assumptions

| Item | Envelope / allowance | Basis |
|---|---|---|
| XIAO ESP32-S3 PCB | 21 × 17.8 mm | Manufacturer; board identity legible in photo |
| GNSS board | 21 × 18 mm | L76K reference; photographed receiver model unconfirmed |
| Full board stack | z=5 to 18 mm above base exterior | Assumed; headers, solder and connectors must be measured |
| Patch plus backing | 32 × 32 × 8 mm | Conservative provisional allowance, not measured from photo |
| Optional battery | 27 × 30 × 8 mm maximum reserved envelope | Placeholder, no battery visible or selected |
| Case assembled | 104 × 50 × 24 mm | Designed; 2 mm wall/floor/lid |
| USB opening | 14 × 10 mm; bottom z=4.5 mm | Provisional port/cable access |

Coordinates use the lower left outside base corner as x=0,y=0,z=0. USB exits x=0. The ESP32 footprint is x=4..25,y=16.1..33.9; antenna is x=66..98,y=9..41. Low antenna guides have a cable opening toward the boards. Hold the patch backing on approximately 2 mm nonconductive foam tape (patch bottom z=4); do not press or glue onto the ceramic face. Keep the existing antenna ground/backing assembly intact.

Boards use a flat platform plus approximately 1 mm insulating foam tape; there are no guessed PCB screw holes. Secure the stack with removable nonconductive retention after checking clearance; tape alone is for bench fit trials. Headers must put the GNSS board clear of ESP32 components. Battery bay strips locate the reserved footprint without clamping a cell. Battery lead, connector and switch space must also fit; enlarge the model if needed. Keep the antenna face toward the lid and sky. Reserve clear plastic-wall space for the ESP32's separate 2.4 GHz antenna and test reception after assembly; it is not visible in the photo.

Four corners use 2.4 mm pilot bores, 18 mm deep from the rim. Start with four M2.5 × 8 mm pan-head plastic-compatible screws; engagement is about 6 mm through the lid. Pilot fit depends on filament and screw type: test gently, adjust pilot radius if necessary. Lid clearance holes are 2.8 mm. Alignment tabs have 0.4 mm nominal wall clearance. No gasket or waterproof rating; USB opening is exposed.

## Trial printing and fit

1. Import both STLs in millimetres at 100% scale. Base floor down, open side up; lid exterior down, tabs up (already exported in these orientations).
2. Start with PETG, 0.4 mm nozzle, 0.2 mm layers, 4 perimeters and 20–30% infill. PLA is suitable for a bench fit check. The USB roof requires a 14 mm bridge; use local support if the printer cannot bridge that span.
3. Deburr access and pilot holes. First test the empty lid and screws, then the unpowered boards and actual USB cable. Do not force connectors or compress a battery.
4. Confirm board stack including solder, underside connectors and coax height. Confirm antenna backing footprint/thickness, USB centre height and plug overmould reach. A USB extension may be needed if the plug shoulder cannot reach the recessed connector.
5. Verify the actual battery including protection board and wiring fits. Retest GNSS acquisition and Wi-Fi/BLE with the lid installed before committing to mounting.

To regenerate: install the pinned dependencies with `pip install -r enclosure/requirements-lock.txt` in a Python virtual environment, then run `python enclosure/build_enclosure.py`. If using another CadQuery version, validate its exported files again. `draw_layout.py` uses Matplotlib and is a schematic drawing; update it when changing CAD dimensions. Source parameters are intentionally exposed near the beginning of the CAD script.

## Sources

- [Seeed XIAO ESP32-S3 documentation](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/)
- [Seeed L76K GNSS documentation](https://wiki.seeedstudio.com/get_start_l76k_gnss/)
- [Seeed older L76-L GNSS documentation](https://wiki.seeedstudio.com/gnss_for_xiao/)

Consulted 18 November 2026. L76K is the selected reference for new GNSS hardware; verify the existing board before applying its pin map.
