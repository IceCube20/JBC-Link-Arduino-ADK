# JBC-Link-Arduino-ADK
JBC Link (CP210x USB ↔ Arduino) – P02/P01 Auto

Passive JBC–USB bridge for CP210x-based JBC stations (fixed 500000 baud, 8E1 on the CP210x side).
Dual console output (USB Serial + Serial1), text CLI with command mapping, pretty-printed logs/decoder, hotplug & watchdog.
Supports Protocol P02 and Protocol P01 with auto-detection.

I'm not a programmer; this was created with the help of ChatGPT. It may contain errors. Tested with DDE and JTSE.

Features

Strict passive: no TX before handshake/first valid frame.

Protocol auto-detect

P02: detected via BASE::M_HS (handshake, fid=253) → replies HS-ACK {BASE::M_ACK}.

P01: fallback after ~1.2 s without HS or on NAK bursts (≥4 NAK in 50 ms).
Link becomes UP once a valid P01 frame is received.

Unified TX: all sends go through send_ctrl_by_proto() → uses FID for P02, no FID for P01.

Keep-Alive: BASE::M_SYN every 500 ms while link is UP.

Firmware bootstrap (both protocols): request BASE::M_FIRMWARE until the model contains DDE…, then set backend, read DEVICENAME, request UID once.

USB connect status (P02-only): one delayed write (~5 s after FW) to set ':C' (*_M_W_USB_CONNECTSTATUS) if not already ':C'.

Hotplug: CP210x attach/detach detection, line re-init, RX drain, state reset.

RX silence watchdog: drops & re-inits the link if no valid frames arrive for a while.

LED (pin 13): OFF = detached, BLINK = attached/no link, SOLID = link up, DOUBLE = link lost (brief).

Dual console: everything prints to USB (Serial) and Serial1; both accept CLI input.

Hardware

Arduino MEGA 2560/ADK (requires Serial1).

MEGA ADK: built-in USB Host works.

MEGA 2560: use a USB Host Shield compatible with CP210x.

Pinout (MEGA/ADK): TX1=18, RX1=19 (for your secondary UART / ESPHome, optional).

Station USB plugs into the Arduino’s USB Host (not the programming port).

Serial & Timing

Console (USB Serial): SERIAL_BAUD = 250000 (8N1).
(500k caused issues with some ESPHome setups; 250k is the default here.)

CP210x side: fixed 500000 baud, 8E1.

Dependencies

Usb.h, usbhub.h, CP210x.h (USB Host library with CP210x support).

Project headers: jbc_commands_full.h, jbc_cmd_names.h, jbc_payload_decode.h, jbc_console_map.h.

Install an Arduino USB Host library that includes CP210x support (often part of/alongside “USB Host Shield Library 2.0”). Make sure CP210x.h is available.

Build & Upload

Board: Arduino Mega 2560 or Arduino Mega ADK.

Open the sketch and ensure SERIAL_BAUD matches your monitor (default 250000).

Compile & upload.

Open Serial Monitor @ 250000 (8N1).

Plug the JBC station USB into the Arduino’s USB Host.

CLI (examples)

Local toggles & info:

HELP
STATE
HEX ON | HEX OFF
LOG ON | LOG OFF        (same as TXRX ON/OFF)
SYN ON | SYN OFF        (show/hide M_SYN logs)
FID ON | FID OFF        (show/hide FID in pretty-prints)
USBCLI ON | USBCLI OFF  (allow/deny JBC TX from USB console)


Mapped JBC commands (backends handled via map):

M_INF_PORT 0
M_R_DEVICEIDORIGINAL
M_R_SLEEPTEMP 0 2


Note: USB-originated TX is read-only by default; enable with USBCLI ON.

Protocol notes

P01: no HS/FID fields; therefore *_USB_CONNECTSTATUS read/write is P02-only.

Station address (stAddr) is learned from meaningful frames (FW, DEVICEID, SYN/HS).

Parser checks minimum inner length per protocol (P01 ≥ 7, P02 ≥ 8) and XOR-BCC==0.

Troubleshooting

No link? Watch the console for [HS] … (P02) or [PROTO] … P01 messages.

If your PC serial stack is flaky, keep console at 250000 and avoid 500k on the console.

MEGA 2560 + USB Host Shield: confirm shield power and CP210x support.

Disclaimer

I'm not a programmer; this was created with the help of ChatGPT. It may contain errors.
Tested with DDE and JTSE.
