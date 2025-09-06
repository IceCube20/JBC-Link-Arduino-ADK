/*
  // SPDX-License-Identifier: MIT OR GPL-2.0-only

  English:
  I'm not a programmer; this was created with the help of ChatGPT. It may contain errors. Tested with DDE and JTSE.
  --------------------------------------
  Passive JBC–USB bridge for CP210x (500000 baud, 8E1) with dual console,
  CLI mapping, and pretty print/decoder. Supports protocol P02 and P01 (auto).

  Key features
  ------------
  • STRICT PASSIVE: Do not transmit before handshake / first valid frame.
  • Protocol auto-detection:
      - P02: detected by BASE::M_HS handshake (fid=253) → reply HS-ACK {BASE::M_ACK}.
      - P01: fallback after ~1.2 s without HS OR when a NAK burst occurs
             (≥4 NAKs within 50 ms). Link is considered UP once a valid P01 frame arrives.
  • Unified TX: All JBC transmissions go through send_ctrl_by_proto(), which uses
                P02 (with FID) or P01 (without FID) depending on g_proto.
  • Keep-alive: BASE::M_SYN every 500 ms while link is UP (P01 & P02).
  • Firmware bootstrap (both protocols):
      After link-up, repeatedly request BASE::M_FIRMWARE until model string contains
      “DDE…”. Then set backend, read DEVICENAME, and request UID exactly once (no retry).
  • USB status (P02 only):
      Single delayed write (~5 s after FW) to set ':C' via family-specific
      M_W_USB_CONNECTSTATUS — only if not already ':C'.
  • Hotplug: Detect CP210x attach/detach → reinit lines, drain RX, reset state,
      wait passively for a new link.
  • RX silence watchdog: If no valid frames arrive after link-up, drop link,
      reinit, and wait passively again.
  • LED (pin 13): OFF=detached, BLINK=attached/no link, SOLID=link up,
      DOUBLE=link lost (short period).
  • Framing:
      P02: inner STX..ETX (SRC,DST,FID,CTRL,LEN,DATA,BCC==0), wrapped with DLE-STX/ETX.
      P01: inner STX..ETX (SRC,DST,    CTRL,LEN,DATA,BCC==0), wrapped with DLE-STX/ETX.

  Dual console (important)
  ------------------------
  • All prints go to USB (Serial) and Serial1 simultaneously.
  • Input is read from both ports and logged with a prefix: [USB UART] / [S1 UART].
  • Mega/ADK: TX1=18, RX1=19.
  • IMPORTANT: Only Arduino MEGA 2560 ADK or Arduino MEGA with USB Host Shield 2.0!

  CLI & logging
  -------------
  • Text CLI mapped via jbc_console_map.h to (ctrl,payload) per backend.
    Examples:  M_INF_PORT 0
               M_R_DEVICEIDORIGINAL
               M_R_SLEEPTEMP 0 2
  • Local commands: HELP, STATE, HEX ON/OFF, LOG ON/OFF (TXRX ON/OFF),
                    SYN ON/OFF, FID ON/OFF, USBCLI ON/OFF.
  • USBCLI: USB-originated JBC TX is read-only by default; enable with “USBCLI ON”.
  • CLI echo shows real FIDs and backend tag ([SOLD_CLI_SEND] …).
    For P01, FID=0 is logged as a placeholder.
  • Optional byte logging: CLI_DEBUG_RX (0/1).
  • Non-ASCII: CLI passes bytes >= 0x80 unchanged; supports Backspace/DEL, CR/LF.

  Tuning / defines
  ----------------
  • SERIAL_BAUD=250000 (console), CP210x fixed 500000 8E1.
  • KEEPALIVE_MS=500, RX_SILENCE_MS=3000, ENUM_SETTLE_MS=800, USB_SET_DELAY_MS=5000.
  • FW retry backoff: 250..2000 ms until model “DDE…” is detected.


  Compatibility & notes
  ---------------------
  • P01 has no HS/FID fields; hence the USB_CONNECTSTATUS sequence is P02-only.
  • Station address (stAddr) is learned from meaningful frames (e.g., FIRMWARE,
    DEVICEID, SYN/HS). For P01 the field layout is shifted accordingly.
  • Parser checks minimum inner-frame length per protocol (P01: ≥7, P02: ≥8)
    and requires XOR-BCC==0.

  Dependencies
  ------------
  • Usb.h, usbhub.h, CP210x.h
  • jbc_commands_full.h, jbc_cmd_names.h, jbc_payload_decode.h, jbc_console_map.h

  Deutsch:
  Ich bin keine Programmierer, wurde mit Hilfe von ChatGPT erstellt. Es können Fehler vorhanden sein. Getestet mit DDE und JTSE
  --------------------------------------
  Passive JBC-USB-Bridge für CP210x (500000 Baud, 8E1) mit Dual-Konsole,
  CLI-Mapping und Pretty-Print/Decoder. Unterstützt P02 und P01 (auto).

  Kerneigenschaften
  -----------------
  • STRICT PASSIVE: Vor Handshake/erstem gültigen Frame wird NICHT gesendet.
  • Protokoll-Autodetektion:
      - P02: erkannt am Handshake BASE::M_HS (fid=253) → HS-ACK mit {BASE::M_ACK}.
      - P01: Fallback nach ~1.2 s ohne HS ODER bei NAK-Burst (≥4 NAK in 50 ms).
              Link gilt als UP, sobald ein gültiges P01-Frame empfangen wurde.
  • Unified TX: Alle JBC-Sends laufen über send_ctrl_by_proto(), das je nach
                erkanntem Protokoll P02 (mit FID) oder P01 (ohne FID) verwendet.
  • Keep-Alive: BASE::M_SYN alle 500 ms solange Link UP (P01 & P02).
  • Firmware-Bootstrap (beide Protokolle):
      Nach Linkstart nur BASE::M_FIRMWARE anfragen, bis Model „DDE…“ erkannt ist.
      Dann Backend setzen, DEVICENAME lesen und UID EINMAL anfragen (ohne Retry).
  • USB-Status (nur P02): Einmalig, verzögert nach FW (~5 s), auf ':C' setzen
      (familienabhängiger M_W_USB_CONNECTSTATUS) — nur, wenn nicht bereits ':C'.
  • Hotplug: CP210x-Attach/Detach wird erkannt → Leitungen reinit, RX drain,
      State reset, passiv auf neuen Link warten.
  • RX-Silence-Watchdog: Wenn nach Link keine gültigen Frames mehr kommen,
      Link droppen, reinit, passiv warten.
  • LED-Status (Pin 13): OFF=detached, BLINK=attached/kein Link,
      SOLID=Link up, DOUBLE=Link verloren (kurzzeitig).
  • Framing:
      P02: inner STX..ETX (SRC,DST,FID,CTRL,LEN,DATA,BCC==0), in DLE-STX/ETX gekapselt.
      P01: inner STX..ETX (SRC,DST,    CTRL,LEN,DATA,BCC==0), in DLE-STX/ETX gekapselt.

  Dual-Konsole (wichtig)
  ----------------------
  • Alle Prints gehen gleichzeitig auf USB (Serial) und Serial1.
  • Eingaben werden von beiden Ports gelesen und mit Präfix geloggt:
      [USB UART] bzw. [S1 UART].
  • Mega/ADK: TX1=18, RX1=19.
  • WICHTIG: Nur mit Arduino MEGA 2560 ADK oder Arduino MEGA 2560 mit USB Host Shield 2.0 Kompatibel

  CLI & Logging
  -------------
  • Text-CLI mit Mapping (jbc_console_map.h) auf (ctrl,payload) pro Backend.
    Beispiele:  M_INF_PORT 0
                M_R_DEVICEIDORIGINAL
                M_R_SLEEPTEMP 0 2
  • Lokale Befehle: HELP, STATE, HEX ON/OFF, LOG ON/OFF (TXRX ON/OFF),
                    SYN ON/OFF, FID ON/OFF, USBCLI ON/OFF.
  • USBCLI: Standardmäßig ist Senden über USB gesperrt (read-only).
            Mit „USBCLI ON“ freigeben.
  • CLI-Echo zeigt echte FIDs (bei P01 wird FID=0 als Platzhalter geloggt) und
    Backend-Tag ([SOLD_CLI_SEND] …).
  • Optionales Byte-Logging CLI_DEBUG_RX (0/1).
  • Nicht-ASCII/Sonderzeichen: CLI lässt Bytes >= 0x80 passieren, Backspace/DEL,
    CR/LF unterstützt.

  Tuning/Defines
  --------------
  • SERIAL_BAUD=250000 (Konsole), CP210x fix 500000 8E1.
  • KEEPALIVE_MS=500, RX_SILENCE_MS=3000, ENUM_SETTLE_MS=800,
    USB_SET_DELAY_MS=5000.
  • FW-Retry Backoff: 250..2000 ms bis Model „DDE…“.


  Kompatibilität & Hinweise
  -------------------------
  • P01 hat keine HS/FID-Felder; USB_CONNECTSTATUS-Sequenz wird daher nur in P02
    verwendet (READ/WRITE-ACKs sind P02-only).
  • Adresse (stAddr) wird aus sinnvollen Frames gelernt (u. a. FIRMWARE,
    DEVICEID, SYN/HS). Bei P01 gilt die verschobene Feldstruktur.
  • Parser prüft Mindestlänge pro Protokoll (P01: ≥7, P02: ≥8) und XOR-BCC==0.

  Abhängigkeiten
  --------------
  • Usb.h, usbhub.h, CP210x.h
  • jbc_commands_full.h, jbc_cmd_names.h, jbc_payload_decode.h, jbc_console_map.h
*/



#include <Arduino.h>

#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__)
  // „Echte“ Ports benennen
  HardwareSerial& USBSER = ::Serial;
  HardwareSerial& AUXSER = ::Serial1;

  class DualSerial : public Stream {
  public:
    DualSerial(HardwareSerial& a, HardwareSerial& b) : _a(&a), _b(&b) {}

    void begin(unsigned long baud) { _a->begin(baud); _b->begin(baud); }
    void end()                     { _a->end();       _b->end();       }

    int available(void) override { return _a->available() + _b->available(); }
    int read(void) override {
      if (_a->available()) return _a->read();
      if (_b->available()) return _b->read();
      return -1;
    }
    int peek(void) override {
      if (_a->available()) return _a->peek();
      if (_b->available()) return _b->peek();
      return -1;
    }
    void flush(void) override { _a->flush(); _b->flush(); }

    size_t write(uint8_t c) override { _a->write(c); _b->write(c); return 1; }
    size_t write(const uint8_t* buf, size_t size) override {
      _a->write(buf, size); _b->write(buf, size); return size;
    }


    using Print::write;

    // verhindert Blockieren von while(!Serial){}
    operator bool() const { return true; }

  private:
    HardwareSerial* _a;
    HardwareSerial* _b;
  };

  // Ab hier wird *jede* Verwendung von Serial (auch in später inkludierten Headers)
  // auf die Dual-Konsole umgebogen:
  DualSerial Console(USBSER, AUXSER);
  #define Serial Console
#else
  #warning "Dual-Konsole braucht einen AVR mit Serial1 (z. B. ATmega2560/1280)."
  HardwareSerial& USBSER = ::Serial;
  HardwareSerial& AUXSER = ::Serial1;
#endif
// ============================================================================

#include <Usb.h>
#include <usbhub.h>
#include "CP210x.h"

#include "jbc_commands_full.h"   // nur Enums / IDs
#include "jbc_cmd_names.h"       // Namen + Backend-/Print-Mapper (JBC_PRINT_*)
#include "jbc_payload_decode.h"  // << NEU: Payload-Dekoder/Pretty-Printer
#include "jbc_console_map.h"     // << NEU: CLI-Map (Text -> (ctrl,payload))

using namespace jbc_cmd;

bool jbc_decode::g_log_show_fid = true;
int  jbc_decode::g_log_cur_fid  = -1;

#define SERIAL_BAUD     250000
#define KEEPALIVE_MS    500
#define ENUM_SETTLE_MS  800     // kleine Pause nach Attach
#define DRAIN_LOOPS     6       // RX-Drain Versuche
#define RX_SILENCE_MS   3000    // wie lange ohne *gültige* RX bis Link drop

// FW-Retry Tuning (bis Model "DDE..." erscheint)
#define FW_RETRY_MS_MIN     250
#define FW_RETRY_MS_MAX    2000

// ---------------- LED-Status ----------------
#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif
#define LED_BLINK_MS        500   // einfaches Blinken (attached, aber kein Link)
#define LED_DOUBLE_PERIOD   1000  // Gesamtdauer Doppel-Blink-Muster
#define LED_DOUBLE_PULSE    120   // Länge eines Blink-Impulses (ms)
#define LED_DOUBLE_GAP      120   // Abstand zwischen den zwei Impulsen (ms)

// --- USB-Status verzögert setzen ---
#define USB_SET_DELAY_MS 5000   // 5 s nach FW

// ---- Bridge-Info ----
#ifndef BRIDGE_FW
#define BRIDGE_FW    "V6.9.25beta"                 
#endif
#ifndef BRIDGE_HW
#define BRIDGE_HW    "Arduino MEGA ADK"     
#endif
#ifndef BRIDGE_AUTHOR
#define BRIDGE_AUTHOR "Icecube20"          
#endif

static const __FlashStringHelper* mcu_name(){
  #if defined(__AVR_ATmega2560__)
    return F("ATmega2560");
  #elif defined(__AVR_ATmega1280__)
    return F("ATmega1280");
  #elif defined(__AVR_ATmega32U4__)
    return F("ATmega32U4");
  #elif defined(ARDUINO_ARCH_ESP32)
    return F("ESP32");
  #elif defined(ARDUINO_ARCH_ESP8266)
    return F("ESP8266");
  #else
    return F("unknown");
  #endif
}

static uint32_t usb_set_due_at       = 0;   // Zeitpunkt für evtl. Write
static bool     usb_set_done         = false; // nur einmal je Link
static bool     usb_status_known     = false;
static bool     usb_status_is_C      = false;
static bool     usb_status_read_sent = false;
// Post-Firmware-Bootstrap nur einmal pro Link
static bool fw_bootstrap_done = false;

enum LedMode { LED_OFF, LED_BLINK, LED_SOLID, LED_DOUBLE };
static LedMode  g_led_mode = LED_OFF;
static uint32_t g_led_mode_since = 0;
static uint32_t g_led_mode_until = 0;     // 0 = unbegrenzt

static inline bool dec_print_with_fid(uint8_t fid, Backend be, uint8_t ctrl, const uint8_t* d, uint8_t len){
  jbc_decode::set_current_fid(fid);
  bool handled = jbc_decode::print(be, ctrl, d, len);
  jbc_decode::set_current_fid(-1);
  return handled;
}

static inline void led_set_mode(LedMode m, uint32_t duration_ms=0){
  g_led_mode = m;
  g_led_mode_since = millis();
  g_led_mode_until = duration_ms ? (g_led_mode_since + duration_ms) : 0;
}

static void print_bridge_banner(){
  Serial.println(F("\n[JBC Link CP210x to USB by IceCube20]"));

  Serial.print(F("[BRIDGE] fw="));   Serial.print(BRIDGE_FW);
  Serial.print(F(" hw="));           Serial.print(BRIDGE_HW);
  Serial.print(F(" mcu="));          Serial.print(mcu_name());
  Serial.print(F(" @"));             Serial.print(F_CPU/1000000); Serial.print(F("MHz"));
  Serial.print(F(" uart="));         Serial.print(SERIAL_BAUD);
  Serial.print(F(" author="));       Serial.print(BRIDGE_AUTHOR);   // <<< NEU
  Serial.print(F(" build="));        Serial.print(__DATE__); Serial.print(' '); Serial.print(__TIME__);
  Serial.println();
}

static void led_status_tick(){
  // zeitbegrenzte Modi auslaufen lassen → Zustand anhand attach/link wählen
  if (g_led_mode_until && (int32_t)(millis() - g_led_mode_until) >= 0){
    g_led_mode_until = 0;
    // g_attached & link_up sind weiter unten deklariert, hier nur genutzt
    extern bool g_attached;
    extern bool link_up;
    if (!g_attached)      g_led_mode = LED_OFF;
    else if (link_up)     g_led_mode = LED_SOLID;
    else                  g_led_mode = LED_BLINK;
    g_led_mode_since = millis();
  }

  uint32_t now = millis();
  switch (g_led_mode){
    case LED_OFF:
      digitalWrite(LED_BUILTIN, LOW);
      break;

    case LED_SOLID:
      digitalWrite(LED_BUILTIN, HIGH);
      break;

    case LED_BLINK: {
      static uint32_t t_last=0;
      static bool st=false;
      if (now - t_last >= LED_BLINK_MS){ t_last = now; st = !st; }
      digitalWrite(LED_BUILTIN, st ? HIGH : LOW);
      break;
    }

    case LED_DOUBLE: {
      // Muster: ON … OFF … ON … längere Pause
      uint32_t phase = (now - g_led_mode_since) % LED_DOUBLE_PERIOD;
      bool on = (phase < LED_DOUBLE_PULSE) ||
                (phase >= (LED_DOUBLE_PULSE + LED_DOUBLE_GAP) &&
                 phase <  (LED_DOUBLE_PULSE*2 + LED_DOUBLE_GAP));
      digitalWrite(LED_BUILTIN, on ? HIGH : LOW);
      break;
    }
  }
}
// ---------------------------------------------------------

static inline bool is_usb_connectstatus_read(uint8_t ctrl){
  using namespace jbc_cmd;
  return ctrl == SOLD_02::M_R_USB_CONNECTSTATUS
      || ctrl == HA_02::M_R_USB_CONNECTSTATUS
      || ctrl == PH_02::M_R_USB_CONNECTSTATUS
      || ctrl == FE_02::M_R_USB_CONNECTSTATUS
      || ctrl == SF_02::M_R_USB_CONNECTSTATUS;
}

// (a) Unified Write-ACK (optional, nur fürs hübsche Log)
static inline bool is_usb_connectstatus_write(uint8_t ctrl){
  using namespace jbc_cmd;
  return ctrl == SOLD_02::M_W_USB_CONNECTSTATUS
      || ctrl == HA_02::M_W_USB_CONNECTSTATUS
      || ctrl == PH_02::M_W_USB_CONNECTSTATUS
      || ctrl == FE_02::M_W_USB_CONNECTSTATUS
      || ctrl == SF_02::M_W_USB_CONNECTSTATUS;
}

// Debug toggles
static bool DBG_LOG=false;
static bool DBG_HEX=false;
bool jbc_decode::g_log_show_syn = false;   // default: M_SYN stumm

static bool g_log_show_txrx = false;   // [TX]/[RX]-Zeilen anzeigen?
static String g_tx_ctx_pending;       // nächste CLI-Zeile, die gesendet wird



// ===== CLI: EINMALIGE GLOBALS & PRÄFIX =====

static bool g_usb_jbc_send_enabled = false; // USB darf JBC senden? Default: nein (read-only)
static bool g_cli_from_usb         = false; // Quelle der aktuellen CLI-Zeile: true=USB, false=Serial1

static inline const __FlashStringHelper* cli_src_prefix(){
  return g_cli_from_usb ? F("[USB UART]") : F("[S1 UART]");
}

static inline void print_cli_cmd_with_fid(uint8_t fid, const String& s, Backend be){
  String s_norm = s; 
  s_norm.trim();
  while (s_norm.indexOf("  ") != -1) s_norm.replace("  ", " ");

  Serial.print(cli_src_prefix()); Serial.print(' ');
  Serial.print('[');
  Serial.print(jbc_decode::fam_tag(be));
  Serial.print(F("_CLI_SEND]"));
  Serial.print(F(" <fid=")); Serial.print(fid); Serial.print(F(">  cmd=\""));
  Serial.print(s_norm);
  Serial.println('"');
}




static void dump_hex(const char* tag,const uint8_t* b,size_t n){
  if(!DBG_HEX) return;
  Serial.print(tag); Serial.print(" ["); Serial.print(n); Serial.println("]");
  for(size_t i=0;i<n;i++){ if(b[i]<16) Serial.print('0'); Serial.print(b[i],HEX); Serial.print(' '); }
  Serial.println();
}
// --- Protokollerkennung ---
enum Proto { PROTO_AUTO, PROTO_P02, PROTO_P01 };
static Proto g_proto = PROTO_AUTO;
static uint32_t t_proto_probe_due = 0;  // Fallback-Timer für P01

// NAK-Burst-Erkenner (für P01-Handshake)
static uint8_t  g_nak_burst = 0;
static uint32_t g_last_nak_ts = 0;
static inline void nak_seen(){
  uint32_t now = millis();
  if (now - g_last_nak_ts <= 50) g_nak_burst++;
  else                           g_nak_burst = 1;
  g_last_nak_ts = now;
}

// ---- P02 framing (inner STX..ETX, XOR-BCC==0; outer DLE STX ... DLE ETX) ----
static const uint8_t DLE=0x10, STX=0x02, ETX=0x03;
static inline uint8_t xor_bcc(const uint8_t* d,size_t n){ uint8_t x=0; while(n--) x^=*d++; return x; }

// USB
USB    Usb;
USBHub Hub(&Usb);

class CPAsync : public CP210xAsyncOper {
public:
  uint8_t OnInit(CP210x *p) override {
    p->IFCEnable();
    p->SetBaudRate(500000);
    p->SetDataBits(8);
    p->SetParity(CP210X_PARITY_EVEN);
    p->SetStopBits(CP210X_STOP_BITS_1);
    p->SetFlowControl(CP210X_FLOW_CONTROL_OFF);
    delay(5);
    return 0;
  }
} cp_async;

CP210x CP(&Usb, &cp_async);

// Addresses / state
static const uint8_t pcAddr = 0x1D;   // host (7-bit)
static uint8_t stAddr = 0x00;         // learned
bool link_up=false;
static bool hs_seen=false;
static uint32_t t_last_syn=0;
static uint32_t last_hs_ts=0;

// Backend (nach *valider* FW Zeile)
static Backend g_backend = BK_UNKNOWN;

// Hotplug & Silence
bool g_attached = false;
static uint32_t t_last_rx_valid = 0;   // Zeitstempel *gültiger* Frames (BCC==0)

// Einmaliges Setzen des USB-Connect-Modes pro Link (altes Flag, nicht mehr benutzt)
static bool usb_mode_set = false;

// FW-Retry state
static bool     fw_ok        = false;   // erst true, wenn Model "DDE..." erkannt
static uint32_t t_fw_next    = 0;       // nächster Zeitpunkt für FW-Request
static uint32_t fw_retry_gap = FW_RETRY_MS_MIN;

// Einmalige UID-Anfrage (ohne Retry)
static bool uid_requested = false;

// FID-Sequenz (resetbar)
static uint8_t g_fid_seq = 1;
static inline uint8_t next_fid(){ if(g_fid_seq==0 || g_fid_seq>239) g_fid_seq=1; return g_fid_seq++; }
static inline void reset_fid_seq(){ g_fid_seq = 1; }

// Kleine Helfer
static inline uint8_t dst_current(){ return stAddr ? stAddr : 0x00; }



static void build_p02(uint8_t src,uint8_t dst,uint8_t fid,uint8_t ctrl,
                      const uint8_t* data,uint8_t len,
                      uint8_t* inner,size_t &in_len){
  size_t i=0;
  inner[i++]=STX;
  inner[i++]=(src & 0x7F);
  inner[i++]=(dst & 0x7F);
  inner[i++]=fid;
  inner[i++]=ctrl;
  inner[i++]=len;
  for(uint8_t k=0;k<len;k++) inner[i++]=data[k];
  inner[i++]=0x00; inner[i++]=ETX;
  inner[i-2]=xor_bcc(inner,i);
  in_len=i;
}

static void send_frame_p02(const uint8_t* inner,size_t in_len,const char* tag=nullptr){
  uint8_t tx[520]; size_t j=0;
  tx[j++]=DLE; tx[j++]=STX;
  for(size_t i=1;i+1<in_len;i++){ uint8_t b=inner[i]; if(b==DLE) tx[j++]=DLE; tx[j++]=b; }
  tx[j++]=DLE; tx[j++]=ETX;
  dump_hex(tag?tag:"[TX]", tx, j);
  CP.SndData((uint16_t)j, tx);
}

static void send_ctrl(uint8_t dst,uint8_t ctrl,const uint8_t* data=nullptr,uint8_t len=0,uint8_t fid=0xFF){
  uint8_t inner[300]; size_t n=0;
  uint8_t usefid = (fid==0xFF)? next_fid() : fid;
  build_p02(pcAddr, dst, usefid, ctrl, data?data:nullptr, len, inner, n);
  if (jbc_decode::g_log_show_syn || !jbc_decode::is_syn_ctrl(ctrl)) {
    if (g_log_show_txrx) {
      JBC_PRINT_TX(g_backend, ctrl, usefid, dst);
    }
    if (g_tx_ctx_pending.length()) {
      print_cli_cmd_with_fid(usefid, g_tx_ctx_pending, g_backend);
      g_tx_ctx_pending = "";
    }
  }
  send_frame_p02(inner,n,"TX P02");
}

// HS-ACK (ctrl=BASE::M_HS, payload={BASE::M_ACK}, fid=253)
static void send_hs_ack(uint8_t dst){
  uint8_t payload[1] = { BASE::M_ACK };
  uint8_t inner[64]; size_t n=0;
  build_p02(pcAddr, dst, 253, BASE::M_HS, payload, 1, inner, n);
  Serial.print(F("[TX] HS-ACK fid=253 dst=0x")); Serial.println(dst,HEX);
  send_frame_p02(inner, n, "[TX HS-ACK]");
}

// ---- P01 (ohne FID) ----
static void build_p01(uint8_t src,uint8_t dst,uint8_t ctrl,
                      const uint8_t* data,uint8_t len,
                      uint8_t* inner,size_t &in_len){
  size_t i=0;
  inner[i++]=STX;
  inner[i++]=(src & 0x7F);
  inner[i++]=(dst & 0x7F);
  inner[i++]=ctrl;
  inner[i++]=len;
  for(uint8_t k=0;k<len;k++) inner[i++]=data[k];
  inner[i++]=0x00; inner[i++]=ETX;
  inner[i-2]=xor_bcc(inner,i);
}

static void send_ctrl_p01(uint8_t dst,uint8_t ctrl,const uint8_t* data=nullptr,uint8_t len=0){
  uint8_t inner[300]; size_t n=0;
  build_p01(pcAddr, dst, ctrl, data?data:nullptr, len, inner, n);
  if (g_log_show_txrx && (jbc_decode::g_log_show_syn || !jbc_decode::is_syn_ctrl(ctrl))) {
    JBC_PRINT_TX(g_backend, ctrl, /*fid*/0, dst); // FID=0 als Platzhalter
  }
  send_frame_p02(inner,n,"TX P01");
}

// --- vereinheitlichter Sender: nutzt P01 oder P02 je nach g_proto ---
static inline void send_ctrl_by_proto(uint8_t dst,uint8_t ctrl,
                                      const uint8_t* data=nullptr,uint8_t len=0,
                                      uint8_t fid=0xFF){
  if (g_proto == PROTO_P01) send_ctrl_p01(dst, ctrl, data, len);
  else                      send_ctrl(dst,   ctrl, data, len, fid);
}


// --- Requests ---
static void request_fw_now(){
  send_ctrl_by_proto(dst_current(), BASE::M_FIRMWARE);
}

static void request_name_by_backend(){
  if (!fw_ok) return;
  uint8_t dst = dst_current();
  switch(g_backend){
    case BK_SOLD:  send_ctrl_by_proto(dst, SOLD_02::M_R_DEVICENAME); break;
    case BK_HA:    send_ctrl_by_proto(dst, HA_02::M_R_DEVICENAME);   break;
    case BK_FE:    send_ctrl_by_proto(dst, FE_02::M_R_DEVICENAME);   break;
    case BK_PH:    send_ctrl_by_proto(dst, PH_02::M_R_DEVICENAME);   break;
    case BK_SF:    send_ctrl_by_proto(dst, SF_02::M_R_DEVICENAME);   break;
    case BK_SOLD1: send_ctrl_by_proto(dst, SOLD_01::M_R_DEVICENAME); break;
    default:
      send_ctrl_by_proto(dst, SOLD_02::M_R_DEVICENAME);
      send_ctrl_by_proto(dst, SOLD_01::M_R_DEVICENAME);
      break;
  }
}

static void request_uid_once(){
  if (!fw_ok || uid_requested) return;
  uid_requested = true;
  uint8_t dst = dst_current();
  switch (g_backend){
    case BK_HA:    send_ctrl_by_proto(dst, HA_02::M_R_DEVICEID);   break;
    case BK_FE:    send_ctrl_by_proto(dst, FE_02::M_R_DEVICEID);   break;
    case BK_PH:    send_ctrl_by_proto(dst, PH_02::M_R_DEVICEID);   break;
    case BK_SF:    send_ctrl_by_proto(dst, SF_02::M_R_DEVICEID);   break;
    case BK_SOLD:  // Fallthrough
    case BK_SOLD1:
    default:       send_ctrl_by_proto(dst, SOLD_02::M_R_DEVICEID); break;
  }
}


// --- Hotplug & Silence helpers ---
static void reset_link_state(){
  stAddr = 0x00;
  link_up = false;
  hs_seen = false;
  g_backend = BK_UNKNOWN;
  t_last_syn = 0;
  last_hs_ts = 0;
  fw_ok = false; t_fw_next = 0; fw_retry_gap = FW_RETRY_MS_MIN;
  uid_requested = false;

  // USB-Delay-State zurücksetzen
  usb_mode_set = false;
  usb_set_due_at = 0;
  usb_set_done = false;
  usb_status_known = false;
  usb_status_is_C = false;
  usb_status_read_sent = false;
  fw_bootstrap_done = false;   // Bootstrap beim neuen Link wieder erlauben
  reset_fid_seq();
}

static void cp_reinit_lines(){
  CP.IFCEnable();
  CP.SetBaudRate(500000);
  CP.SetDataBits(8);
  CP.SetParity(CP210X_PARITY_EVEN);
  CP.SetStopBits(CP210X_STOP_BITS_1);
  CP.SetFlowControl(CP210X_FLOW_CONTROL_OFF);
  delay(5);
  Serial.println(F("[CP210x] Reinit lines: 500000 8E1, FC=OFF"));
}

static void drain_rx(){
  uint8_t tmp[128]; uint16_t m;
  for (int i=0;i<DRAIN_LOOPS;i++){
    m = sizeof(tmp);
    if (CP.RcvData(&m, tmp)!=0) break;
    if (m==0) delay(10);
  }
}

static void on_attach_init(){
  Serial.print(F("[ATTACH] CP210x @addr ")); Serial.println(CP.GetAddress());
  cp_reinit_lines();
  delay(ENUM_SETTLE_MS);
  drain_rx();
  reset_link_state();       // passiv auf HS warten
  t_last_rx_valid = millis();
  led_set_mode(LED_BLINK);  // attached, noch kein Link
  g_proto = PROTO_AUTO;
  t_proto_probe_due = millis() + 1200;  // ~1.2 s auf HS warten, dann P01 probieren
  g_nak_burst = 0; g_last_nak_ts = 0;
}

static void on_detach_cleanup(){
  Serial.println(F("[DETACH] CP210x removed"));
  reset_link_state();       // Keep-Alive aus, alles zurücksetzen
  t_last_rx_valid = 0;
  led_set_mode(LED_OFF);    // komplett aus
}

// --- Firmware Retry Tick (bis Model "DDE..." erkannt) ---
static void fw_retry_tick(){
  if (!link_up || fw_ok) return;
  uint32_t now = millis();
  if ((int32_t)(now - t_fw_next) >= 0){
    request_fw_now();
    // exponentieller Backoff capped
    if (fw_retry_gap < FW_RETRY_MS_MAX) {
      fw_retry_gap <<= 1;
      if (fw_retry_gap > FW_RETRY_MS_MAX) fw_retry_gap = FW_RETRY_MS_MAX;
    }
    t_fw_next = now + fw_retry_gap;
  }
}

// Decoder
static void on_inner_frame(const uint8_t* f,size_t n){
  if ( (n < (g_proto == PROTO_P01 ? 7 : 8)) || xor_bcc(f, n) != 0 ) return;
  t_last_rx_valid = millis();   // gültiges Frame gesehen

  uint8_t src=f[1]&0x7F, dst=f[2]&0x7F, fid=f[3], ctrl=f[4], len=f[5];
  const uint8_t* d=&f[6];
  // P01: kein FID; Felder 1 Byte früher
  if (g_proto == PROTO_P01){
    fid  = 0;
    ctrl = f[3];
    len  = f[4];
    d    = &f[5];
  }

  // Bei P01: sobald ein gültiges Frame ankommt, Link als UP markieren
  if (g_proto == PROTO_P01 && !link_up){
    link_up = true;
    led_set_mode(LED_SOLID);
  }


  if ((jbc_decode::g_log_show_syn || !jbc_decode::is_syn_ctrl(ctrl)) && g_log_show_txrx) {
    JBC_PRINT_RX(g_backend, src, ctrl, fid, len);
  }

  // Adresse lernen aus sinnvollen Frames
  if(src && src!=stAddr){
    switch(ctrl){
      case BASE::M_FIRMWARE:
      case SOLD_02::M_R_DEVICEID:
      case SOLD_01::M_R_DEVICEID:
      case BASE::M_SYN:
      case BASE::M_HS:
        stAddr=src;
        Serial.print(F("[ADDR] learned 0x")); Serial.println(stAddr,HEX);
        break;
      default: break;
    }
  }

  // Handshake announce -> HS-ACK; duplicate HS dämpfen
  if(fid==253 && ctrl==BASE::M_HS){
    g_proto = PROTO_P02;  // eindeutig P02
    uint32_t now = millis();
    if (now - last_hs_ts < 300) return; // dämpfen
    last_hs_ts = now;

    hs_seen=true; link_up=true;
    if(src) stAddr=src;
    Serial.print(F("[HS] from 0x")); Serial.println(src,HEX);
    send_hs_ack(dst_current());

    // Link steht → Dauerlicht
    
    led_set_mode(LED_SOLID);
    

    // Ab hier NUR Firmware anfragen, bis Model "DDE..." kommt
    fw_ok = false; fw_retry_gap = FW_RETRY_MS_MIN;
    t_fw_next = millis(); // sofortiger erster Versuch
    return;
  }

  // Parse FW string (ohne eigene Prints; Ausgabe über Decoder)
  if (ctrl==BASE::M_FIRMWARE && len>0){
    String s; s.reserve(len);
    for(uint8_t i=0;i<len;i++) s += (char)d[i];

    // Model extrahieren
    int a = s.indexOf(':'), b = s.indexOf(':', a+1);
    String model = (a>0 && b>a) ? s.substring(a+1, b) : String();

    // Spezialfall: PDE -> nicht akzeptieren, weiter FW anfragen
    if (model.startsWith("PDE")){
      fw_ok = false;
      return;
    }

    // Backend setzen und FW akzeptieren
    g_backend = jbc_name::backend_from_fwline(s);
    fw_ok = true;

    // Hübsch ausgeben
    dec_print_with_fid(fid, g_backend, ctrl, d, len);

    // Post-FW-Sequenz nur einmal pro Link
    if (!fw_bootstrap_done) {
      request_name_by_backend();
      request_uid_once();

      if (g_proto != PROTO_P01) {  // P02-Only
      

        // USB-Status jetzt einmal lesen und Write frühestens in 5 s erlauben
        usb_set_done         = false;
        usb_status_known     = false;
        usb_status_is_C      = false;
        usb_status_read_sent = false;
        usb_set_due_at       = millis() + USB_SET_DELAY_MS;

        switch (g_backend) {
          case BK_HA:
            send_ctrl(dst_current(), HA_02::M_R_USB_CONNECTSTATUS);
            usb_status_read_sent = true;
            Serial.println(F("[AUTO] USB_CONNECTSTATUS read (HA) requested"));
            break;
          case BK_SOLD:
            send_ctrl(dst_current(), SOLD_02::M_R_USB_CONNECTSTATUS);
            usb_status_read_sent = true;
            Serial.println(F("[AUTO] USB_CONNECTSTATUS read (SOLD) requested"));
            break;
          case BK_PH:
            send_ctrl(dst_current(), PH_02::M_R_USB_CONNECTSTATUS);
            usb_status_read_sent = true;
            Serial.println(F("[AUTO] USB_CONNECTSTATUS read (PH) requested"));
            break;
          case BK_FE:
            send_ctrl(dst_current(), FE_02::M_R_USB_CONNECTSTATUS);
            usb_status_read_sent = true;
            Serial.println(F("[AUTO] USB_CONNECTSTATUS read (FE) requested"));
            break;
          case BK_SF:
            send_ctrl(dst_current(), SF_02::M_R_USB_CONNECTSTATUS);
            usb_status_read_sent = true;
            Serial.println(F("[AUTO] USB_CONNECTSTATUS read (SF) requested"));
            break;
          default:
            break;
        }
      }
       fw_bootstrap_done = true;
    }
    return;
  }

  // DEVICEID (nur informieren; KEIN Retry, egal ob len==0)
  if (ctrl==SOLD_02::M_R_DEVICEID || ctrl==HA_02::M_R_DEVICEID ||
      ctrl==FE_02::M_R_DEVICEID   || ctrl==PH_02::M_R_DEVICEID ||
      ctrl==SF_02::M_R_DEVICEID) {
    dec_print_with_fid(fid, g_backend, ctrl, d, len);
    return;
  }

  // --- USB-Status Hooks (READ, nur Familien mit USB-Status) ---
  if (is_usb_connectstatus_read(ctrl)) {
    usb_status_known = true;
    usb_status_is_C  = (len>=2 && d[0]==':' && (d[1]=='C' || d[1]=='c'));
  }

  // --- USB-Status Write-ACK (familienübergreifend, ohne SOLD_01) ---
  if (is_usb_connectstatus_write(ctrl) && len>=1 && d[0]==0x06) {
    Serial.println(F("[AUTO] USB_CONNECTSTATUS write ACK"));
  }

  // -> generischer Pretty-Print
  if (dec_print_with_fid(fid, g_backend, ctrl, d, len)) return;
}

static void feed_rx(uint8_t b){
  enum { S_WAIT_DLE, S_WAIT_STX, S_IN, S_ESC };
  static uint8_t st=S_WAIT_DLE;
  static uint8_t buf[520]; static size_t ln=0;
  static uint8_t inner[522];
  switch(st){
    case S_WAIT_DLE:
      if(b==DLE){ st=S_WAIT_STX; break; }
      if(b==0x15){  // NAK puls -> P01-Stationsstil
        nak_seen();
        if (g_proto==PROTO_AUTO && g_nak_burst>=4){
          g_proto = PROTO_P01;
          Serial.println(F("[PROTO] NAK burst -> switch to P01"));
          send_ctrl_p01(dst_current(), BASE::M_SYN, nullptr, 0);
          request_fw_now();
        }
      }
      break;

    case S_WAIT_STX: if(b==STX){ ln=0; st=S_IN; } else st=S_WAIT_DLE; break;
    case S_IN: if(b==DLE) st=S_ESC; else { if(ln<sizeof(buf)) buf[ln++]=b; } break;
    case S_ESC:
      if(b==DLE){ if(ln<sizeof(buf)) buf[ln++]=DLE; st=S_IN; }
      else if(b==ETX){
        size_t n=0; inner[n++]=STX; memcpy(&inner[n],buf,ln); n+=ln; inner[n++]=ETX;
        dump_hex("[RAW]", inner, n);
        if(xor_bcc(inner,n)==0) on_inner_frame(inner,n);
        ln=0; st=S_WAIT_DLE;
      } else if(b==STX){ ln=0; st=S_IN; }
      else { if(ln<sizeof(buf)) buf[ln++]=b; st=S_IN; }
      break;
  }
}

// --- Silence-Watchdog: wenn Link oben, aber keine gültigen RX mehr ---
static void link_watchdog_tick(){
  if (!link_up) return;
  uint32_t now = millis();
  if (t_last_rx_valid==0) return;
  if (now - t_last_rx_valid > RX_SILENCE_MS){
    Serial.println(F("[LINK] RX silence → drop link, reinit, wait for HS"));
    led_set_mode(LED_DOUBLE, 5000);   // 5s Doppel-Blink als „Link abgebrochen“
    reset_link_state();
    cp_reinit_lines();
    drain_rx();
  }
}

static void proto_probe_tick(){
  if (!g_attached || link_up) return;
  if (g_proto != PROTO_AUTO)  return;
  if ((int32_t)(millis() - t_proto_probe_due) >= 0){
    g_proto = PROTO_P01;
    Serial.println(F("[PROTO] No HS -> fallback to P01"));
    // anklopfen: SYN + FIRMWARE im P01-Format
    send_ctrl_p01(dst_current(), BASE::M_SYN, nullptr, 0);
    request_fw_now(); // nutzt jetzt P01
  }
}


// --- CLI-Callback, den die Map nutzt ---
static void jbc_send_from_cli(uint8_t ctrl, const uint8_t* payload, uint8_t len){
  send_ctrl_by_proto(dst_current(), ctrl, payload, len);
}


// CLI
static void print_cli_help(){
  Serial.println(F("[HELP] Lokale Kommandos:"));
  Serial.println(F("  STATE"));
  Serial.println(F("  HEX ON | HEX OFF"));
  Serial.println(F("  LOG ON | LOG OFF   (zeigt/verbirgt [TX]/[RX])"));
  Serial.println(F("  SYN ON | SYN OFF   (M_SYN Logs an/aus)"));
  Serial.println(F("  FID ON | FID OFF   (FID in Pretty-Prints an/aus)"));
  Serial.println(F("  TXRX ON | TXRX OFF (Alias für LOG ON/OFF)"));
  Serial.println(F("  USBCLI ON | USBCLI OFF  (JBC-Senden über USB erlauben/verbieten)"));
  Serial.println(F("  Prefixe in Ausgaben: [USB] / [S1]"));
  Serial.println(F("JBC Kommandos:"));
  Serial.println(F("  z.B.  M_INF_PORT 0"));
  Serial.println(F("        M_R_DEVICEIDORIGINAL"));
  Serial.println(F("        M_R_SLEEPTEMP 0 2"));
}



static void cli_process(const String &line_in){
  // Normalisieren
  String u = line_in, clean; clean.reserve(u.length());
  for (size_t i=0;i<u.length();++i){
    uint8_t uc = (uint8_t)u[i];
    if (uc == '\t') uc = ' ';
    if (uc >= 32 && uc != 127) clean += (char)uc;  // ≥128 zulassen
  }
  clean.trim();
  while (clean.indexOf("  ") != -1) clean.replace("  ", " ");
  while (clean.startsWith(">") || clean.startsWith("#") || clean.startsWith("$")) { clean.remove(0,1); clean.trim(); }
  if (!clean.length()) return;

  String up = clean; up.toUpperCase();
  int posM = up.indexOf("M_"); if (posM > 0) { clean = clean.substring(posM); up = up.substring(posM); }

  // Lokale Kommandos
  if (up == "HELP") { print_cli_help(); return; }

  if (up == "STATE"){
    Serial.print(cli_src_prefix()); Serial.print(' ');
    Serial.print(F("[ST] link=")); Serial.print(link_up?F("UP"):F("DOWN"));
    Serial.print(F(" stAddr=0x")); Serial.print(stAddr,HEX);
    Serial.print(F(" backend="));
    switch(g_backend){
      case BK_SOLD:  Serial.println(F("SOLD")); break;
      case BK_SOLD1: Serial.println(F("SOLD_01")); break;
      case BK_HA:    Serial.println(F("HA")); break;
      case BK_FE:    Serial.println(F("FE")); break;
      case BK_PH:    Serial.println(F("PH")); break;
      case BK_SF:    Serial.println(F("SF")); break;
      default:       Serial.println(F("?")); break;
    }
    Serial.print(cli_src_prefix()); Serial.print(' ');
    Serial.print(F("[FW] ok=")); Serial.print(fw_ok?F("yes"):F("no"));
    Serial.print(F(" nextFW=")); Serial.println((int32_t)(t_fw_next - millis()));
    return;
  }

  if (up == "HEX ON")  { DBG_HEX = true;  Serial.print(cli_src_prefix()); Serial.println(F(" [DBG] HEX=ON"));  return; }
  if (up == "HEX OFF") { DBG_HEX = false; Serial.print(cli_src_prefix()); Serial.println(F(" [DBG] HEX=OFF")); return; }

  if (up == "LOG ON"  || up == "TXRX ON")  { g_log_show_txrx = true;  Serial.print(cli_src_prefix()); Serial.println(F(" [DBG] LOG=ON (TX/RX sichtbar)"));    return; }
  if (up == "LOG OFF" || up == "TXRX OFF") { g_log_show_txrx = false; Serial.print(cli_src_prefix()); Serial.println(F(" [DBG] LOG=OFF (TX/RX ausgeblendet)")); return; }

  if (up == "SYN ON")  { jbc_decode::g_log_show_syn = true;  Serial.print(cli_src_prefix()); Serial.println(F(" [DBG] SYN=ON"));  return; }
  if (up == "SYN OFF") { jbc_decode::g_log_show_syn = false; Serial.print(cli_src_prefix()); Serial.println(F(" [DBG] SYN=OFF")); return; }

  if (up == "FID ON")  { jbc_decode::g_log_show_fid = true;  Serial.print(cli_src_prefix()); Serial.println(F(" [DBG] FID=ON"));  return; }
  if (up == "FID OFF") { jbc_decode::g_log_show_fid = false; Serial.print(cli_src_prefix()); Serial.println(F(" [DBG] FID=OFF")); return; }

  if (up == "USBCLI ON")  { g_usb_jbc_send_enabled = true;  Serial.print(cli_src_prefix()); Serial.println(F(" [CLI] USB JBC-Senden: ON"));  return; }
  if (up == "USBCLI OFF") { g_usb_jbc_send_enabled = false; Serial.print(cli_src_prefix()); Serial.println(F(" [CLI] USB JBC-Senden: OFF")); return; }

  // JBC-Kommandos
  if (up.startsWith("M_")){
    if (g_cli_from_usb && !g_usb_jbc_send_enabled) {
      Serial.print(cli_src_prefix()); Serial.println(F(" [CLI] JBC-Senden über USB ist deaktiviert. (USBCLI ON zum Aktivieren)"));
      return;
    }
    g_tx_ctx_pending = clean; // Echo mit echter FID
    if (jbc_cli::handle(clean, g_backend, dst_current(), jbc_send_from_cli)) {
      return;
    }
    g_tx_ctx_pending = ""; // kein Match
  }

  Serial.print(cli_src_prefix()); Serial.print(F(" [CLI] Unbekannter Befehl: \""));
  Serial.print(clean);
  Serial.println(F("\"  — 'HELP' eingeben."));
}




// ======= CLI-Reader: nur CR/LF, kein Idle-Flush =======
// Optionales Byte-Logging: 1 = an, 0 = aus
#define CLI_DEBUG_RX 0

static String cli_buf_usb, cli_buf_s1;

static void handle_line_from(bool from_usb, String &buf) {
  if (!buf.length()) return;
  g_cli_from_usb = from_usb;
  #if CLI_DEBUG_RX
    Serial.print(from_usb ? F("[USB]") : F("[S1]"));
    Serial.print(F(" [CLI_RX line] \"")); Serial.print(buf); Serial.println('"');
  #endif
  cli_process(buf);
  buf = "";
}

static void feed_from(Stream& port, String &buf, bool from_usb)
{
  while (port.available() > 0) {
    int c = port.read();
    if (c < 0) break;

    #if CLI_DEBUG_RX
      Serial.print(from_usb ? F("[USB]") : F("[S1]"));
      Serial.print(F(" [CLI_RX byte] "));
      if (c >= 32 && c <= 126) { Serial.print('\''); Serial.write((char)c); Serial.print('\''); }
      else { Serial.print(F("0x")); Serial.print(c, HEX); }
      Serial.println();
    #endif

    // Backspace / DEL
    if (c == 0x08 || c == 0x7F) { if (buf.length()) buf.remove(buf.length()-1); continue; }

    // CR oder LF => Zeile abschließen
    if (c == '\r' || c == '\n') { handle_line_from(from_usb, buf); continue; }

    // Tab -> Space (optional)
    if (c == '\t') c = ' ';

    uint8_t uc = (uint8_t)c;           // wichtig auf AVR: char ist signed
    if (uc >= 32 && uc != 127) {       // alles >= 32 außer DEL erlauben (inkl. ≥128)
      buf += (char)uc;
      if (buf.length() > 256) { handle_line_from(from_usb, buf); }
    }
    // andere Steuerzeichen ignorieren (CR/LF/BS werden oben schon behandelt)

  }
}

static void cli_tick_dual()
{
  feed_from(USBSER, cli_buf_usb, /*from_usb=*/true);
  feed_from(AUXSER, cli_buf_s1,  /*from_usb=*/false);
}





void setup(){



  Serial.begin(SERIAL_BAUD);
  while(!Serial){}  // blockiert dank DualSerial nicht

  pinMode(LED_BUILTIN, OUTPUT);
  led_set_mode(LED_OFF);



  print_bridge_banner(); 

  if(Usb.Init()==-1){ Serial.println(F("[USB] Init failed")); while(1){} }
  Serial.println(F("[USB] OK"));
}




void loop(){
  Usb.Task();

  bool attached = (CP.GetAddress()!=0);
  if (attached && !g_attached){ g_attached=true; on_attach_init(); }
  if (!attached && g_attached){ g_attached=false; on_detach_cleanup(); }

  if(attached){
    // RX
    uint8_t buf[128]; uint16_t n=sizeof(buf);
    if(CP.RcvData(&n, buf)==0 && n>0){ for(uint16_t i=0;i<n;i++) feed_rx(buf[i]); }

    // Keepalive nur nach Link
    if(link_up){
      uint32_t now=millis();
      if(now - t_last_syn >= KEEPALIVE_MS){
        t_last_syn = now;
        send_ctrl_by_proto(dst_current(), BASE::M_SYN, nullptr, 0);
      }
    }
  }


  // USB ':C' delayed one-shot nach 5 s
  if (link_up && fw_ok && !usb_set_done && usb_set_due_at &&
      (int32_t)(millis() - usb_set_due_at) >= 0)
  {
    if (!usb_status_known || !usb_status_is_C){
      uint8_t payload[2] = { ':', 'C' };
      switch (g_backend){
        case BK_HA:
          send_ctrl(dst_current(), HA_02::M_W_USB_CONNECTSTATUS, payload, 2);
          Serial.println(F("[AUTO] USB_CONNECTSTATUS -> ':C' (HA)"));
          break;
        case BK_SOLD:
          send_ctrl(dst_current(), SOLD_02::M_W_USB_CONNECTSTATUS, payload, 2);
          Serial.println(F("[AUTO] USB_CONNECTSTATUS -> ':C' (SOLD)"));
          break;
        case BK_PH:
          send_ctrl(dst_current(), PH_02::M_W_USB_CONNECTSTATUS, payload, 2);
          Serial.println(F("[AUTO] USB_CONNECTSTATUS -> ':C' (PH)"));
          break;
        case BK_FE:
          send_ctrl(dst_current(), FE_02::M_W_USB_CONNECTSTATUS, payload, 2);
          Serial.println(F("[AUTO] USB_CONNECTSTATUS -> ':C' (FE)"));
          break;
        case BK_SF:
          send_ctrl(dst_current(), SF_02::M_W_USB_CONNECTSTATUS, payload, 2);
          Serial.println(F("[AUTO] USB_CONNECTSTATUS -> ':C' (SF)"));
          break;
        default:
          Serial.println(F("[AUTO] USB_CONNECTSTATUS not supported on backend"));
          break;
      }
    } else {
      Serial.println(F("[AUTO] USB_CONNECTSTATUS already ':C' — no write"));
    }
    usb_set_done = true;
  }

  // LED-Status immer aktualisieren
  led_status_tick();
  
  
  cli_tick_dual();

  // Silence/Hotplug Watchdog
  link_watchdog_tick();
    
  proto_probe_tick();

  // FW-Retry (bis DDE kommt)
  fw_retry_tick();

}
