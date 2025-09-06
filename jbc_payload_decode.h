// SPDX-License-Identifier: MIT OR GPL-2.0-only

#pragma once
#include <Arduino.h>
#include "jbc_commands_full.h"
#include "jbc_cmd_names.h"   // Backend enum + pretty print helpers

using namespace jbc_cmd;

namespace jbc_decode {

  // Globals (defined in the .ino)
  extern bool g_log_show_fid;
  extern int  g_log_cur_fid;
  extern bool g_log_show_syn;        // <-- moved inside the namespace

  static inline void set_current_fid(int fid){ g_log_cur_fid = fid; }

// Vorab-Deklaration: Definition kommt weiter unten im File
static bool decode_datetime_payload(Backend be, const uint8_t* d, uint8_t len);


// --- SYN-Logging Toggle (Definition kommt im .ino) ---
extern bool g_log_show_syn;  // false = M_SYN stumm, true = anzeigen

// erkennt M_SYN (V02 = 0x16) für alle Familien
static inline bool is_syn_ctrl(uint8_t ctrl){
  using namespace jbc_cmd;
  return ctrl == 0x16
      || ctrl == BASE::M_SYN
      || ctrl == SOLD_01::M_SYN || ctrl == SOLD_02::M_SYN
      || ctrl == HA_02::M_SYN   || ctrl == PH_02::M_SYN
      || ctrl == FE_02::M_SYN   || ctrl == SF_02::M_SYN;
}

// ACK-Erkennung (analog zu is_nack_ctrl)
static inline bool is_ack_ctrl(uint8_t ctrl){
  using namespace jbc_cmd;
  return ctrl == 0x06
      || ctrl == BASE::M_ACK
      || ctrl == SOLD_01::M_ACK || ctrl == SOLD_02::M_ACK
      || ctrl == HA_02::M_ACK   || ctrl == PH_02::M_ACK
      || ctrl == FE_02::M_ACK   || ctrl == SF_02::M_ACK;
}


// NACK-Erkennung (analog zu is_syn_ctrl)
static inline bool is_nack_ctrl(uint8_t ctrl){
  using namespace jbc_cmd;
  return ctrl == 0x15
      || ctrl == BASE::M_NACK
      || ctrl == SOLD_01::M_NACK || ctrl == SOLD_02::M_NACK
      || ctrl == HA_02::M_NACK   || ctrl == PH_02::M_NACK
      || ctrl == FE_02::M_NACK   || ctrl == SF_02::M_NACK;
}


// =========================
// KV-Helpers (einzeilig)
// =========================
static inline const __FlashStringHelper* fam_tag(Backend be){
  switch(be){
    case BK_SOLD:  return F("SOLD");
    case BK_SOLD1: return F("SOLD1");
    case BK_HA:    return F("HA");
    case BK_PH:    return F("PH");
    case BK_FE:    return F("FE");
    case BK_SF:    return F("SF");
    default:       return F("BASE");
  }
}
// Kopf nur mit Tag: [TAG] (inkl. optionalem <fid=...>)
static inline void print_hdr_tag(const __FlashStringHelper* tag){
  Serial.print('['); Serial.print(tag); Serial.print(']');
  if (jbc_decode::g_log_show_fid && jbc_decode::g_log_cur_fid >= 0){
    Serial.print(F(" <fid=")); Serial.print(jbc_decode::g_log_cur_fid); Serial.print('>');
  }
  Serial.print(' ');
}

// Kopf: [FAM_CTRL]
static inline void print_hdr_line(Backend be, const __FlashStringHelper* ctrl){
  Serial.print('['); Serial.print(fam_tag(be)); Serial.print('_'); Serial.print(ctrl); Serial.print(']');
  if (jbc_decode::g_log_show_fid && jbc_decode::g_log_cur_fid >= 0){
    Serial.print(F(" <fid=")); Serial.print(jbc_decode::g_log_cur_fid); Serial.print('>');
  }
  Serial.print(' ');
}

// key="string"
static inline void kv_s(const __FlashStringHelper* k, const String& v){
  Serial.print(' '); Serial.print(k); Serial.print('=');
  Serial.print('"');
  for (size_t i=0;i<v.length();i++){ char c=v[i]; if(c=='"') Serial.print(F("\\\"")); else Serial.print(c); }
  Serial.print('"');
}
// key="FS"
static inline void kv_fs(const __FlashStringHelper* k, const __FlashStringHelper* v){
  Serial.print(' '); Serial.print(k); Serial.print('=');
  Serial.print('"'); Serial.print(v); Serial.print('"');
}
// key=123
static inline void kv_u(const __FlashStringHelper* k, uint32_t v){
  Serial.print(' '); Serial.print(k); Serial.print('='); Serial.print(v);
}
// key=12.3
static inline void kv_c(const __FlashStringHelper* k, float c, uint8_t digits=1){
  Serial.print(' '); Serial.print(k); Serial.print('='); Serial.print(c, digits);
}
// key=0xAB / 0x12345678
static inline void kv_hex(const __FlashStringHelper* k, uint32_t v, uint8_t width=2){
  Serial.print(' '); Serial.print(k); Serial.print(F("=0x"));
  for (int i=(width-1)*4; i>=0; i-=4){
    uint8_t nib = (v >> i) & 0xF;
    Serial.print(nib < 10 ? char('0'+nib) : char('A'+nib-10));
  }
}


// --- Helpers: LE-Reader ---
static inline uint16_t u16le(const uint8_t* p){ return (uint16_t)p[0] | ((uint16_t)p[1] << 8); }
static inline int16_t  s16le(const uint8_t* p){ return (int16_t)u16le(p); }
static inline uint32_t u24le(const uint8_t* p){ return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16); }
static inline uint32_t u32le(const uint8_t* p){
  return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

// UTI → °C
static inline float    uti_to_c(uint16_t uti){ return (float)uti / 9.0f; } // JBC UTI ≈ 1/9 °C

// BCD & Zeitdrucker
static inline uint8_t  bcd2(uint8_t b){ return (uint8_t)((b>>4)*10 + (b&0x0F)); }
static inline bool     is_bcd(uint8_t b){ return ((b>>4) <= 9) && ((b&0x0F) <= 9); }
static inline uint8_t  bcd_swap(uint8_t b){
  uint8_t hi = (uint8_t)((b >> 4) & 0x0F);
  uint8_t lo = (uint8_t)(b & 0x0F);
  if (hi <= 9 && lo <= 9) return (uint8_t)(lo*10 + hi);
  return 0xFF; // ungültig
}
// Einheitliche mm:ss(.t) Formatierung für Deci-Sekunden
static inline String fmt_mmss_tenths(uint16_t ds){
  uint16_t total_s = ds / 10;
  uint8_t  t       = ds % 10;
  char buf[8];
  snprintf(buf, sizeof(buf), "%02u:%02u",
           (unsigned)(total_s/60), (unsigned)(total_s%60));
  String s(buf);
  if (t) { s += '.'; s += char('0' + t); }
  return s;
}


static void print_dur_hms(uint32_t sec){
  uint32_t h = sec / 3600UL;
  uint8_t  m = (uint8_t)((sec % 3600UL) / 60UL);
  uint8_t  s = (uint8_t)(sec % 60UL);
  Serial.print(h); Serial.print(':');
  if (m<10) Serial.print('0'); Serial.print(m); Serial.print(':');
  if (s<10) Serial.print('0'); Serial.print(s);
}

// Minuten hübsch als H:MM
static inline void print_hm_from_minutes(uint32_t minutes){
  uint32_t h = minutes / 60UL;
  uint8_t  m = (uint8_t)(minutes % 60UL);
  Serial.print(h); Serial.print(':');
  if (m<10) Serial.print('0'); Serial.print(m);
}

// Sekunden-seit-Mitternacht hübsch als HH:MM:SS
static void print_hms(uint32_t sod){
  sod %= 86400UL;
  uint8_t h = (uint8_t)(sod / 3600UL);
  uint8_t m = (uint8_t)((sod % 3600UL) / 60UL);
  uint8_t s = (uint8_t)(sod % 60UL);
  if (h<10) Serial.print('0'); Serial.print(h); Serial.print(':');
  if (m<10) Serial.print('0'); Serial.print(m); Serial.print(':');
  if (s<10) Serial.print('0'); Serial.print(s);
}

// String/Hex/IPv4
static String sanitize_ascii(const uint8_t* d, uint8_t len){
  String s; s.reserve(len);
  for(uint8_t i=0;i<len;i++){ char c=(char)d[i]; if(c>=0x20 && c!=0x7F) s+=c; }
  return s;
}
static void print_hex(const uint8_t* d, uint8_t len){
  for(uint8_t i=0;i<len;i++){
    if(d[i]<16) Serial.print('0');
    Serial.print(d[i], HEX);
    if(i+1<len) Serial.print(' ');
  }
}
static String hex_string(const uint8_t* d, uint8_t len, bool spaced=true, bool uppercase=true){
  String s; s.reserve(len * (spaced ? 3 : 2));
  for(uint8_t i=0;i<len;i++){
    if(i && spaced) s += ' ';
    if(d[i] < 16) s += '0';
    s += String(d[i], HEX);
  }
  if (uppercase) s.toUpperCase();
  return s;
}

static void print_ip4(const uint8_t* p){
  Serial.print((uint8_t)p[0]); Serial.print('.');
  Serial.print((uint8_t)p[1]); Serial.print('.');
  Serial.print((uint8_t)p[2]); Serial.print('.');
  Serial.print((uint8_t)p[3]);
}

// --- kleine Pretty-Printer ---
static inline void print_pct_from_ppm(const __FlashStringHelper* tag, uint16_t ppm){
  if(ppm>1000) ppm=1000; // clamp
  print_hdr_tag(tag);
  Serial.print(ppm/10.0f, 1); Serial.print(F(" % (raw=")); Serial.print(ppm); Serial.println(')');
}
static inline void print_temp_c_from_uti(const __FlashStringHelper* tag, uint16_t uti){
  print_hdr_tag(tag);
  Serial.print(uti_to_c(uti),1); Serial.println(F(" °C"));
}
static inline void print_u16_raw(const __FlashStringHelper* tag, uint16_t v){
  print_hdr_tag(tag);
  Serial.println(v);
}
static inline void print_onoff(const __FlashStringHelper* tag, uint8_t v){
  print_hdr_tag(tag);
  if(v==0) Serial.println(F("OFF"));
  else if(v==1) Serial.println(F("ON"));
  else { Serial.print(F("code=")); Serial.println(v); }
}
static inline void print_mmss_tenths(const __FlashStringHelper* tag, uint16_t deciSeconds){
  const uint32_t total_s = deciSeconds / 10;
  const uint8_t  tenths  = deciSeconds % 10;
  const uint16_t mm = total_s / 60;
  const uint16_t ss = total_s % 60;
  print_hdr_tag(tag);
  if (mm < 10) Serial.print('0'); Serial.print(mm);
  Serial.print(':');
  if (ss < 10) Serial.print('0'); Serial.print(ss);
  if (tenths) { Serial.print('.'); Serial.print(tenths); }
  Serial.println();
}

// Tool-Error → Name, pro Familie 0..N
static const __FlashStringHelper* tool_error_name_fam(Backend be, uint8_t code){
  switch (be){
    case BK_SOLD:
    case BK_SOLD1:
      switch (code){
        case 0: return F("OK");
        case 1: return F("SHORTCIRCUIT");
        case 2: return F("SHORTCIRCUIT_NR");
        case 3: return F("OPENCIRCUIT");
        case 4: return F("NOTOOL");
        case 5: return F("WRONGTOOL");
        case 6: return F("DETECTIONTOOL");
        case 7: return F("MAXPOWER");
        case 8: return F("STOPOVERLOAD_MOS");
        default: return nullptr;
      }

    case BK_HA:
      switch (code){
        case 0:  return F("OK");
        case 1:  return F("AIR_PUMP_ERROR");
        case 2:  return F("PROTECION_TC_HIGH");   
        case 3:  return F("REGULATION_TC_HIGH");
        case 4:  return F("EXTERNAL_TC_MISSING");
        case 5:  return F("SELECTED_TEMP_NOT_REACHED");
        case 6:  return F("HIGH_HEATER_INTENSITY");
        case 7:  return F("LOW_HEATER_RESISTANCE");
        case 8:  return F("WRONG_HEATER");
        case 9:  return F("NOTOOL_HA");
        case 10: return F("DETECTIONTOOL_HA");
        default: return nullptr;
      }

    case BK_PH:
      switch (code){
        case 0: return F("OK");
        case 1: return F("SELECTED_TEMP_NOT_REACHED_PH");
        case 2: return F("LOW_HEATER_INTENSITY");
        case 3: return F("TC1_NOT_CONNECTED");
        case 4: return F("TC1_LIMIT_REACHED");
        case 5: return F("TC2_NOT_CONNECTED");
        case 6: return F("TC2_LIMIT_REACHED");
        case 7: return F("TC1_TC2_NOT_CONNECTED");
        default: return nullptr;
      }

    // FE/SF: keine bestätigte Tool-Error-Tabelle bekannt
    case BK_FE:
    case BK_SF:
    default:
      return nullptr;
  }
}


static const __FlashStringHelper* station_error_name(uint8_t c){
  switch (c){
    case 0: return F("OK");
    case 1: return F("STOPOVERLOAD_TRAFO");
    case 2: return F("WRONGSENSOR_TRAFO");
    case 3: return F("MEMORY");
    case 4: return F("MAINSFREQUENCY");
    case 5: return F("STATION_MODEL");
    case 6: return F("NOT_MCU_TOOLS");
    default: return nullptr;
  }
}

static const __FlashStringHelper* typeofground_name(uint8_t v){
  switch (v){
    case 0: return F("SOFTGROUND");
    case 1: return F("HARDGROUND");
    default: return nullptr;
  }
}


// ---------- Tool-Code → Name ----------
static const __FlashStringHelper* tool_name_from_code(uint8_t code){
  switch(code){
    case 0:  return F("NOTOOL");
    case 1:  return F("T210");
    case 2:  return F("T245");
    case 3:  return F("PA");
    case 4:  return F("HT");
    case 5:  return F("DS");
    case 6:  return F("DR");
    case 7:  return F("NT105");
    case 8:  return F("NP105");
    case 9:  return F("T470");
    default: return nullptr;
  }
}

// HA-spezifisch
static const __FlashStringHelper* ha_tool_name(uint8_t code){
  switch(code){
    case 0: return F("NOTOOL");
    case 1: return F("JT");
    case 2: return F("TE");
    default: return nullptr;
  }
}

//--- kleine Namenstabellen ---
static const char* tempunit_name(uint8_t v){
  switch(v){ case 0: return "CELSIUS"; case 1: return "FAHRENHEIT"; default: return nullptr; }
}
static const char* language_from_ascii(const uint8_t* d, uint8_t len){
  if(len < 2) return nullptr;
  char a = (char)d[0], b = (char)d[1];
  if((a=='E'||a=='e') && (b=='N'||b=='n')) return "ENGLISH";
  if((a=='E'||a=='e') && (b=='S'||b=='s')) return "SPANISH";
  if((a=='D'||a=='d') && (b=='E'||b=='e')) return "GERMAN";
  if((a=='F'||a=='f') && (b=='R'||b=='r')) return "FRENCH";
  if((a=='I'||a=='i') && (b=='T'||b=='t')) return "ITALIAN";
  if((a=='P'||a=='p') && (b=='T'||b=='t')) return "PORTUGUESE";
  return nullptr;
}
static const char* language_from_code(uint8_t v){
  switch(v){
    case 0: return "ENGLISH"; case 1: return "SPANISH"; case 2: return "GERMAN";
    case 3: return "FRENCH";  case 4: return "ITALIAN"; case 5: return "PORTUGUESE";
    default: return nullptr;
  }
}

// THEME Namen (heuristisch)
static const char* theme_name(uint8_t v){
  switch(v){
    case 0: return "DARK";
    case 1: return "LIGHT";
    case 2: return "AUTO";
    default: return nullptr;
  }
}

// NACK-Reason-Name (heuristisch)
static const char* nack_reason_name(uint8_t r){
  switch(r){
    case 0x01: return "FORMAT/LENGTH";
    case 0x02: return "UNSUPPORTED_CMD";
    case 0x03: return "ILLEGAL_STATE/LOCKED";
    case 0x04: return "BAD_ARGUMENTS";
    case 0x05: return "NOT_ALLOWED/REMOTE_OFF";
    default:   return nullptr;
  }
}



// --------------------
// Familienübergreifende READ-Decodes → jetzt KV mit Familien-Prefix
// inkl. USB_CONNECTSTATUS (SOLD/HA/PH/FE/SF) und REMOTEMODE (SOLD/SOLD1/HA/PH)
// --------------------
static bool decode_crossfamily_reads(Backend be, uint8_t ctrl, const uint8_t* d, uint8_t len){
  using namespace jbc_cmd;

  if (be == BK_UNKNOWN) return false;

  // --- kleine Helper ---
  auto print_usb_status = [&](){
    print_hdr_line(be, F("M_R_USB_CONNECTSTATUS"));
    if (!len){ kv_fs(F("text"), F("")); Serial.println(); return true; }
    String s = sanitize_ascii(d,len);
    if (!s.length()){ Serial.print(F(" raw=")); print_hex(d,len); Serial.println(); return true; }
    kv_s(F("text"), s);
    char mode = 0;
    for (int i=(int)s.length()-1; i>=0; --i){
      char c = s[i];
      if ((c>='A'&&c<='Z') || (c>='a'&&c<='z')){ mode = (c>='a'&&c<='z') ? (char)(c-32) : c; break; }
    }
    if (mode=='C') kv_fs(F("mode"), F("PC_CONTROL"));
    else if (mode=='M') kv_fs(F("mode"), F("MONITOR"));
    Serial.println();
    return true;
  };

  auto print_discover = [&](){
    print_hdr_line(be, F("M_R_DISCOVER"));
    if (len == 1){
      const uint8_t v = d[0];
      if (v == 0x06){                      // ACK → „vorhanden“
        kv_u (F("present"), 1);
        kv_hex(F("ack"), v, 2);
        Serial.println();
        return true;
      }
      if (v == 0x15){                      // NACK → „nicht vorhanden“
        kv_u (F("present"), 0);
        kv_hex(F("nack"), v, 2);
        Serial.println();
        return true;
      }
      if (v == '0' || v == '1'){           // manche FW geben ASCII ‚0‘/‚1‘ zurück
        kv_u (F("present"), (v=='1') ? 1 : 0);
        kv_hex(F("raw"), v, 2);
        Serial.println();
        return true;
      }
      // Unbekannter 1-Byte-Code
      kv_hex(F("code"), v, 2);
      Serial.println();
      return true;
    }

    // Unerwartete Länge → Rohdump für Diagnose
    kv_s(F("raw"), hex_string(d, len));
    Serial.println();
    return true;
  };


  auto print_pin = [&](){
    print_hdr_line(be, F("M_R_PIN"));
    if (len==4 && d[0]>='0'&&d[0]<='9' && d[1]>='0'&&d[1]<='9' &&
                 d[2]>='0'&&d[2]<='9' && d[3]>='0'&&d[3]<='9'){
      String p; p.reserve(4); for(uint8_t i=0;i<4;i++) p += char(d[i]);
      kv_s(F("pin"), p);
    } else {
      Serial.print(F(" raw=")); print_hex(d,len);
    }
    Serial.println();
    return true;
  }; 

  auto print_remotemode = [&](){
    if (len<1) return false;
    print_hdr_line(be, F("M_R_REMOTEMODE"));
    uint8_t v = d[0];
    if (v==0 || v==1) kv_u(F("on"), v);
    else              kv_u(F("code"), v);
    Serial.println();
    return true;
  };

  // ----- TEMPUNIT -----
  if ((be==BK_SOLD || be==BK_SOLD1) &&
      (ctrl==SOLD_02::M_R_TEMPUNIT || ctrl==SOLD_01::M_R_TEMPUNIT)){
    if (len>=1){
      uint8_t u=d[0];
      const char* n = (u==0||u==1) ? tempunit_name(u)
                     : (u=='C'||u=='c') ? "CELSIUS"
                     : (u=='F'||u=='f') ? "FAHRENHEIT" : nullptr;
      print_hdr_line(be, F("M_R_TEMPUNIT"));
      if(n) kv_s(F("unit"), String(n));
      kv_u(F("code"), u);
      Serial.println();
      return true;
    }
  }
  if (be==BK_HA && ctrl==HA_02::M_R_TEMPUNIT){
    if (len>=1){
      uint8_t u=d[0];
      const char* n = (u==0||u==1) ? tempunit_name(u)
                     : (u=='C'||u=='c') ? "CELSIUS"
                     : (u=='F'||u=='f') ? "FAHRENHEIT" : nullptr;
      print_hdr_line(be, F("M_R_TEMPUNIT"));
      if(n) kv_s(F("unit"), String(n));
      kv_u(F("code"), u);
      Serial.println();
      return true;
    } 
  }

  // ----- LANGUAGE -----
  if ((be==BK_SOLD || be==BK_SOLD1) &&
      (ctrl==SOLD_02::M_R_LANGUAGE || ctrl==SOLD_01::M_R_LANGUAGE)){
    print_hdr_line(be, F("M_R_LANGUAGE"));
    if (len>=2){ if(const char* n=language_from_ascii(d,len)){ kv_s(F("name"), String(n)); kv_fs(F("src"), F("ASCII")); Serial.println(); return true; } }
    if (len>=1){ if(const char* n=language_from_code(d[0]))   { kv_s(F("name"), String(n)); kv_fs(F("src"), F("CODE"));  Serial.println(); return true; } }
    Serial.print(F(" raw=")); print_hex(d,len); Serial.println(); return true; 
  }
  if (be==BK_HA && ctrl==HA_02::M_R_LANGUAGE){
    print_hdr_line(be, F("M_R_LANGUAGE"));
    if (len>=2){ if(const char* n=language_from_ascii(d,len)){ kv_s(F("name"), String(n)); kv_fs(F("src"), F("ASCII")); Serial.println(); return true; } }
    if (len>=1){ if(const char* n=language_from_code(d[0]))   { kv_s(F("name"), String(n)); kv_fs(F("src"), F("CODE"));  Serial.println(); return true; } }
    Serial.print(F(" raw=")); print_hex(d,len); Serial.println(); return true;
  }

  // ----- USB_CONNECTSTATUS (alle Familien) -----
  if ((be==BK_SOLD || be==BK_SOLD1) && ctrl==SOLD_02::M_R_USB_CONNECTSTATUS) return print_usb_status();
  if (be==BK_HA && ctrl==HA_02::M_R_USB_CONNECTSTATUS) return print_usb_status();
  if (be==BK_PH && ctrl==PH_02::M_R_USB_CONNECTSTATUS) return print_usb_status();
  if (be==BK_FE && ctrl==FE_02::M_R_USB_CONNECTSTATUS) return print_usb_status();
  if (be==BK_SF && ctrl==SF_02::M_R_USB_CONNECTSTATUS) return print_usb_status();

  // ----- DISCOVER (alle Familien) -----
  if ((be==BK_SOLD || be==BK_SOLD1) && (ctrl==SOLD_02::M_R_DISCOVER)) return print_discover();
  if (be==BK_HA  && ctrl==HA_02::M_R_DISCOVER) return print_discover();
  if (be==BK_PH  && ctrl==PH_02::M_R_DISCOVER) return print_discover();
  if (be==BK_FE  && ctrl==FE_02::M_R_DISCOVER) return print_discover();
  if (be==BK_SF  && ctrl==SF_02::M_R_DISCOVER) return print_discover();


  // ----- PIN (SOLDER/HA/FE/SF) -----
  if ((be==BK_SOLD || be==BK_SOLD1) && (ctrl==SOLD_02::M_R_PIN || ctrl==SOLD_01::M_R_PIN)) return print_pin();
  if (be==BK_HA && ctrl==HA_02::M_R_PIN) return print_pin();
  if (be==BK_FE && ctrl==FE_02::M_R_PIN) return print_pin();
  if (be==BK_SF && ctrl==SF_02::M_R_PIN) return print_pin();

  // ----- PINENABLED -----
  if (be==BK_HA && ctrl==HA_02::M_R_PINENABLED && len>=1){ print_hdr_line(be, F("M_R_PINENABLED")); kv_u(F("on"), d[0]); Serial.println(); return true; }
  if (be==BK_FE && ctrl==FE_02::M_R_PINENABLED && len>=1){ print_hdr_line(be, F("M_R_PINENABLED")); kv_u(F("on"), d[0]); Serial.println(); return true; }
  if (be==BK_SF && ctrl==SF_02::M_R_PINENABLED && len>=1){ print_hdr_line(be, F("M_R_PINENABLED")); kv_u(F("on"), d[0]); Serial.println(); return true; }

  // ----- STATIONLOCKED -----
  if (be==BK_HA && ctrl==HA_02::M_R_STATIONLOCKED && len>=1){ print_hdr_line(be, F("M_R_STATIONLOCKED")); kv_u(F("on"), d[0]); Serial.println(); return true; }
  if (be==BK_FE && ctrl==FE_02::M_R_STATIONLOCKED && len>=1){ print_hdr_line(be, F("M_R_STATIONLOCKED")); kv_u(F("on"), d[0]); Serial.println(); return true; }
  if (be==BK_SF && ctrl==SF_02::M_R_STATIONLOCKED && len>=1){ print_hdr_line(be, F("M_R_STATIONLOCKED")); kv_u(F("on"), d[0]); Serial.println(); return true; }

  // ----- BEEP -----
  if ((be==BK_SOLD || be==BK_SOLD1) && (ctrl==SOLD_02::M_R_BEEP || ctrl==SOLD_01::M_R_BEEP) && len>=1){ print_hdr_line(be, F("BEEP")); kv_u(F("on"), d[0]); Serial.println(); return true; }
  if (be==BK_HA && ctrl==HA_02::M_R_BEEP && len>=1){ print_hdr_line(be, F("M_R_BEEP")); kv_u(F("on"), d[0]); Serial.println(); return true; }
  if (be==BK_FE && ctrl==FE_02::M_R_BEEP && len>=1){ print_hdr_line(be, F("M_R_BEEP")); kv_u(F("on"), d[0]); Serial.println(); return true; }
  if (be==BK_SF && ctrl==SF_02::M_R_BEEP && len>=1){ print_hdr_line(be, F("M_R_BEEP")); kv_u(F("on"), d[0]); Serial.println(); return true; }

  // ----- REMOTEMODE (jetzt SOLDER/SOLDER_01 + HA + PH) -----
  if ((be==BK_SOLD || be==BK_SOLD1) && (ctrl==SOLD_02::M_R_REMOTEMODE || ctrl==SOLD_01::M_R_REMOTEMODE)) return print_remotemode();
  if (be==BK_HA && ctrl==HA_02::M_R_REMOTEMODE) return print_remotemode();
  if (be==BK_PH && ctrl==PH_02::M_R_REMOTEMODE) return print_remotemode();

  // ----- TOOLERROR (SOLDER/SOLDER_01/HA/PH) -> Klartext falls bekannt -----
  if (ctrl==SOLD_02::M_R_TOOLERROR || ctrl==SOLD_01::M_R_TOOLERROR || ctrl==HA_02::M_R_TOOLERROR){
    if (len >= 1){
      uint8_t code = d[0];
      print_hdr_line(be, F("M_R_TOOLERROR"));
      kv_hex(F("code"), code, 2);
      if (const __FlashStringHelper* n = tool_error_name_fam(be, code)) kv_fs(F("name"), n);
      Serial.println();
      return true;
    }
  }

  // ----- ROBOT / RBT: Verbindungs-Config -----
  if (ctrl == jbc_cmd::SOLD_02::M_R_RBT_CONNCONFIG) {
    print_hdr_line(be, F("M_R_RBT_CONNCONFIG"));
    if (len < 5) { kv_s(F("raw"), hex_string(d,len)); Serial.println(); return true; }

    // 0) Baud aus Code
    const uint8_t sc = d[0];
    static const uint32_t BAUDS[] = { 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200,230400,250000 };
    const uint8_t nBauds = sizeof(BAUDS)/sizeof(BAUDS[0]);
    const uint32_t baud = (sc < nBauds) ? BAUDS[sc] : 0;

    kv_u(F("speed_code"), sc);
    if (baud) kv_u(F("baud"), baud); else kv_fs(F("baud"), F("?"));

    // 1) Datenbits (ASCII)
    uint8_t databits = (d[1] >= '0' && d[1] <= '9') ? uint8_t(d[1] - '0') : d[1];
    kv_u(F("databits"), databits);

    // 2) Parität (ASCII)
    char p = (char)d[2]; if (p>='a' && p<='z') p -= 32;
    const __FlashStringHelper* pname =
        (p=='N')?F("NONE"):(p=='E')?F("EVEN"):(p=='O')?F("ODD"):(p=='M')?F("MARK"):(p=='S')?F("SPACE"):nullptr;
    if (pname) kv_fs(F("parity"), pname); else kv_hex(F("parity_raw"), (uint8_t)d[2], 2);

    // 3) Stopbits (ASCII '0'/'1')
    uint8_t stopbits = (d[3] >= '0' && d[3] <= '9') ? uint8_t(d[3] - '0') : 0xFF;
    if (stopbits != 0xFF) kv_u(F("stopbits"), stopbits); else kv_hex(F("stopbits_raw"), d[3], 2);

    // 4) Addressing EIN/AUS (ASCII '0'/'1')
    uint8_t addr_en = (d[4] >= '0' && d[4] <= '9') ? uint8_t(d[4] - '0') : 0xFF;
    if (addr_en != 0xFF) kv_u(F("addr_enabled"), addr_en); else kv_hex(F("addr_enabled_raw"), d[4], 2);

    // 5) Adresse aus den letzten 1–2 ASCII-Ziffern zusammensetzen
    uint8_t tail_len = (len > 5) ? (uint8_t)(len - 5) : 0;
    int addr = -1;
    if (tail_len >= 2 && d[len-2] >= '0' && d[len-2] <= '9' && d[len-1] >= '0' && d[len-1] <= '9') {
      addr = (int(d[len-2]-'0') * 10) + int(d[len-1]-'0');
    } else if (tail_len >= 1 && d[len-1] >= '0' && d[len-1] <= '9') {
      addr = int(d[len-1]-'0');
    }

    if (addr >= 0) {
      kv_u(F("addr"), (uint32_t)addr);
    }

    // falls noch mehr als 2 Bytes Tail übrig sind, zur Diagnose zeigen
    if (tail_len > 2) {
      kv_s(F("tail"), hex_string(&d[5], (uint8_t)(len-5)));
    }

    Serial.println();
    return true;
  }

  // ----- ROBOT / RBT: Connect-Status (ein Buchstabe) -----
  if (ctrl == jbc_cmd::SOLD_02::M_R_RBT_CONNECTSTATUS) {
    print_hdr_line(be, F("M_R_RBT_CONNECTSTATUS"));
    if (!len) { kv_fs(F("state"), F("")); Serial.println(); return true; }
    char c = (char)d[0]; if (c>='a' && c<='z') c -= 32;
    const __FlashStringHelper* s =
        (c=='C')?F("CONNECTED") :
        (c=='N')?F("NOT_CONNECTED") :
        (c=='O')?F("OPEN") :
        (c=='K')?F("OK") : nullptr;
    if (s) kv_fs(F("state"), s); else kv_hex(F("state_char"), (uint8_t)d[0], 2);
    Serial.println();
    return true;
  }

  
  return false;
}


// Connected Tool
static bool decode_connecttool(Backend be, uint8_t ctrl, const uint8_t* d, uint8_t len){
  using namespace jbc_cmd;
  if (ctrl==SOLD_02::M_R_CONNECTTOOL || ctrl==SOLD_01::M_R_CONNECTTOOL || ctrl==HA_02::M_R_CONNECTTOOL){
     if(len==0){ print_hdr_line(be, F("M_R_CONNECTTOOL")); kv_fs(F("name"), F("")); Serial.println(); return true; }
    uint8_t code = d[0];

    const __FlashStringHelper* tn = nullptr;
    if (be == BK_HA) tn = ha_tool_name(code);
    else             tn = tool_name_from_code(code);

    print_hdr_line(be, F("M_R_CONNECTTOOL"));
    kv_hex(F("code"), code, 2);
    if (tn) kv_fs(F("name"), tn); else kv_fs(F("name"), F("UNKNOWN"));
    Serial.println();    
    return true;
  }
  return false;
}

// Gibt die SOLD-Flags als "NAME|NAME|…" oder "NONE" zurück (keine Ausgabe!)
static String sold_flags_to_string(uint8_t f){
  String out; auto add=[&](const char* n){ if(out.length()) out+='|'; out+=n; };
  if (f & 0x01) add("STAND");
  if (f & 0x02) add("SLEEP");
  if (f & 0x04) add("HIBERNATION");
  if (f & 0x08) add("EXTRACTOR");
  if (f & 0x10) add("DESOLDER");
  if (f & 0x20) add("PORT_LOCKED");   
  // Rest generisch:
  for (uint8_t b=6; b<8; ++b) if (f & (1u<<b)){ if(out.length()) out+='|'; out += String("BIT")+String(b); }
  if (!out.length()) out = "NONE";
  return out;
}

// ---- helper: HA status bits -> "NAME|NAME|..." ----
static String ha_status_to_string(uint8_t s){
  String out; auto add=[&](const char* n){ if(out.length()) out+='|'; out+=n; };
  if (s & 0x01) add("HEATER");
  if (s & 0x02) add("HEATER_REQUESTED");
  if (s & 0x04) add("COOLING");
  if (s & 0x08) add("SUCTION");
  if (s & 0x10) add("SUCTION_REQUESTED");
  if (s & 0x20) add("PEDAL_CONNECTED");
  if (s & 0x40) add("PEDAL_PRESSED");
  if (s & 0x80) add("STAND");
  if (!out.length()) out = "NONE";
  return out;
}


// 16-bit STATUSTOOL-Maske → "NAME|NAME|..." (SOLDER/SOLDER1)
static String sold_statustool_bits_to_string(uint16_t v){
  String out; auto add=[&](const char* n){ if(out.length()) out+='|'; out+=n; };

  if (v & 0x0001) add("STAND");
  if (v & 0x0002) add("SLEEP");
  if (v & 0x0004) add("HIBERNATION");
  if (v & 0x0008) add("EXTRACTOR");
  if (v & 0x0010) add("DESOLDER");        // (falls jemals gesetzt)
  if (v & 0x0020) add("PORT_LOCKED");     // dein beobachtetes Bit5
  if (v & 0x0100) add("DESOLDER_TOOL");   // dein beobachtetes Bit8 (DR-Tool)

  // Unbekannte gesetzte Bits ergänzen
  const uint16_t known = 0x0001|0x0002|0x0004|0x0008|0x0010|0x0020|0x0100;
  for (uint8_t b=0; b<16; ++b){
    uint16_t m = (uint16_t)1u<<b;
    if ((v & m) && !(known & m)){
      if(out.length()) out+='|';
      out += String("BIT") + String(b);
    }
  }
  if (!out.length()) out = "NONE";
  return out;
}


static String sold_changes_to_string(uint8_t c){
  String out; auto add=[&](const char* n){ if(out.length()) out+='|'; out+=n; };
  if (c & (1u<<0)) add("SELECTTEMP_CHANGED");
  if (c & (1u<<1)) add("STATION_PARAM_CHANGED");
  if (c & (1u<<2)) add("TOOL_PARAM_GRP0_CHANGED");
  if (c & (1u<<3)) add("TOOL_PARAM_GRP1_CHANGED");
  if (c & (1u<<4)) add("TOOL_PARAM_GRP2_CHANGED");
  if (c & (1u<<5)) add("TOOL_PARAM_GRP3_CHANGED");
  if (c & (1u<<7)) add("COUNTER_CHANGED");
  for (uint8_t b=0; b<8; ++b){
    if ((c & (1u<<b)) && b!=0 && b!=1 && b!=2 && b!=3 && b!=4 && b!=5 && b!=7){
      if(out.length()) out+='|'; out += String("BIT")+String(b);
    }
  }
  if (!out.length()) out = "NONE";
  return out;
}


// --- Status-Namen je Familie für M_INF_PORT::status ---
static const char* sold_status_name(uint8_t s){
  switch(s){
    case 0:  return "NONE";
    case 1:  return "STAND";
    case 2:  return "SLEEP";
    case 4:  return "HIBERNATION";
    case 8:  return "EXTRACTOR";
    case 16: return "DESOLDER";
    default: return nullptr;
  }
}



static void sold_changes_print(uint8_t c){
  struct { uint8_t bit; const char* name; } B[] = {
    {0, "SELECTTEMP_CHANGED"},
    {1, "STATION_PARAM_CHANGED"},
    {2, "TOOL_PARAM_GRP0_CHANGED"},
    {3, "TOOL_PARAM_GRP1_CHANGED"},
    {4, "TOOL_PARAM_GRP2_CHANGED"},
    {5, "TOOL_PARAM_GRP3_CHANGED"},
    {7, "COUNTER_CHANGED"},
  };
  bool any=false;
  Serial.print(F("  changesText="));
  for (auto &b : B){
    if (c & (1u<<b.bit)){ if(any) Serial.print('|'); Serial.print(b.name); any=true; }
  }
  for(uint8_t b=0;b<8;b++){
    if ((c & (1u<<b)) && b!=0 && b!=1 && b!=2 && b!=3 && b!=4 && b!=5 && b!=7){
      if(any) Serial.print('|'); Serial.print(F("BIT")); Serial.print(b); any=true;
    }
  }
  if(!any) Serial.println(F("NONE")); else Serial.println();
}

// ---------- SOLD-spezifische Extras ----------
static bool decode_sold_extras(Backend be, uint8_t ctrl, const uint8_t* d, uint8_t len){
  using namespace jbc_cmd;

  // LEVELSTEMPS (onoff,sel, L1,L2,L3 je on+uti, optional port/tool)
  if (ctrl == SOLD_02::M_R_LEVELSTEMPS && len >= 11){
    auto U16LE = [&](uint8_t lo, uint8_t hi)->uint16_t{ return (uint16_t)lo | ((uint16_t)hi<<8); };
    uint8_t onoff=d[0], sel=d[1];
    uint8_t l1_on=d[2];  uint16_t l1_uti=U16LE(d[3],d[4]);
    uint8_t l2_on=d[5];  uint16_t l2_uti=U16LE(d[6],d[7]);
    uint8_t l3_on=d[8];  uint16_t l3_uti=U16LE(d[9],d[10]);

    print_hdr_line(be, F("M_R_LEVELSTEMPS"));
    kv_u(F("on"), onoff); kv_u(F("sel"), sel);
    kv_u(F("l1_on"), l1_on); kv_c(F("l1_c"), l1_uti/9.0f, 1); kv_hex(F("l1_uti"), l1_uti, 4);
    kv_u(F("l2_on"), l2_on); kv_c(F("l2_c"), l2_uti/9.0f, 1); kv_hex(F("l2_uti"), l2_uti, 4);
    kv_u(F("l3_on"), l3_on); kv_c(F("l3_c"), l3_uti/9.0f, 1); kv_hex(F("l3_uti"), l3_uti, 4);
    if (len >= 13){
      uint8_t port=d[11], tool=d[12];
      kv_u(F("port"), port); kv_u(F("tool"), tool);
      if (const __FlashStringHelper* tn = tool_name_from_code(tool)) kv_fs(F("tool_name"), tn);
    }
    Serial.println();
    return true;
  }

  // CARTRIDGE (on, number, adj300/400, group/family, port/tool)
  if (ctrl == SOLD_02::M_R_CARTRIDGE && len >= 11){
    uint8_t  onoff=d[0];
    uint16_t nbr=u16le(&d[1]);
    int16_t  adj300=s16le(&d[3]);
    int16_t  adj400=s16le(&d[5]);
    uint8_t  group=d[7], family=d[8], port=d[9], tool=d[10];

    print_hdr_line(be, F("M_R_CARTRIDGE"));
    kv_u(F("on"), onoff);
    kv_u(F("nbr"), nbr);
    kv_c(F("adj300_c"), adj300/9.0f, 1);
    kv_c(F("adj400_c"), adj400/9.0f, 1);
    kv_u(F("group"), group);
    kv_u(F("family"), family);
    kv_u(F("port"), port);
    kv_u(F("tool"), tool);
    if (const __FlashStringHelper* tn = tool_name_from_code(tool)) kv_fs(F("tool_name"), tn);
    Serial.println();
    return true;
  }

  // SLEEPDELAY / HIBERDELAY  (val, on, port, tool)
  if ((ctrl == SOLD_02::M_R_SLEEPDELAY || ctrl == SOLD_02::M_R_HIBERDELAY) && len >= 4){
    const __FlashStringHelper* tag = (ctrl==SOLD_02::M_R_SLEEPDELAY) ? F("M_R_SLEEPDELAY") : F("M_R_HIBERDELAY");
    print_hdr_line(be, tag);
    kv_u(F("min"), d[0]);
    kv_u(F("on"),  d[1]);
    kv_u(F("port"),d[2]);
    kv_u(F("tool"),d[3]);
    if (const __FlashStringHelper* tn = tool_name_from_code(d[3])) kv_fs(F("tool_name"), tn);
    Serial.println();
    return true;
  }

  

  // --- SLEEPTEMP (tempLE,port,tool)  UTI = °C*9 ---
  if (ctrl == SOLD_02::M_R_SLEEPTEMP && len>=4){
    print_hdr_line(be, F("M_R_SLEEPTEMP"));
    uint16_t v=u16le(d); kv_c(F("c"), uti_to_c(v), 1); kv_hex(F("uti"), v, 4);
    kv_u(F("port"), d[2]); kv_u(F("tool"), d[3]);
    if (const __FlashStringHelper* tn = tool_name_from_code(d[3])) kv_fs(F("tool_name"), tn);
    Serial.println(); return true;
  }

  // --- AJUSTTEMP (deltaLE(int16),port,tool)  UTI = °C*9 ---
  if (ctrl == SOLD_02::M_R_AJUSTTEMP && len>=4){
    print_hdr_line(be, F("M_R_AJUSTTEMP"));
    kv_c(F("delta_c"), (int16_t)u16le(d)/9.0f, 1);
    kv_u(F("port"), d[2]); kv_u(F("tool"), d[3]);
    if (const __FlashStringHelper* tn = tool_name_from_code(d[3])) kv_fs(F("tool_name"), tn);
    Serial.println(); return true;
  }


  // Interne Temps
  if ((ctrl==SOLD_02::M_R_TRAFOTEMP || ctrl==SOLD_01::M_R_TRAFOTEMP) && len>=2){
    print_hdr_line(be, F("M_R_TRAFOTEMP")); uint16_t v=u16le(d);
    kv_c(F("c"), uti_to_c(v),1); kv_hex(F("uti"), v,4); Serial.println(); return true;
  }
  if ((ctrl==SOLD_02::M_R_MOSTEMP || ctrl==SOLD_01::M_R_MOSTEMP) && len>=2){
    print_hdr_line(be, F("M_R_MOSTEMP")); uint16_t v=u16le(d);
    kv_c(F("c"), uti_to_c(v),1); kv_hex(F("uti"), v,4); Serial.println(); return true;
  }

  // --- POWER (% aus Promille), optional mit port/tool ---
  if ((ctrl == SOLD_02::M_R_POWER || ctrl == SOLD_01::M_R_POWER) && len >= 2){
    uint16_t raw = u16le(d);
    uint16_t ppm = raw > 1000 ? 1000 : raw;   // clamp 0..1000
    print_hdr_line(be, F("M_R_POWER"));
    kv_c(F("pct"), ppm / 10.0f, 1);
    kv_u(F("raw"), raw);

    
    Serial.println();
    return true;
  }


  // QST_STATUS
  if (ctrl==SOLD_02::M_R_QST_STATUS || ctrl==SOLD_01::M_R_QST_STATUS){
    print_hdr_line(be, F("M_R_QST_STATUS"));
    if (!len){ kv_fs(F("state"), F("")); Serial.println(); return true; }

    if (len==1 && (d[0]==0 || d[0]==1)){
      kv_u(F("on"), d[0]); Serial.println(); return true;
    }

    const uint16_t v = (len>=2) ? u16le(d) : d[0];
    kv_hex(F("mask"), v, (len>=2)?4:2);

    // hübscher Bits-String
    if (len>=2){
      String bits = sold_statustool_bits_to_string(v);
      kv_s(F("bits"), bits);
    }
    Serial.println();
    return true;
  }


  // --- SOLD: DELAYTIME (u16 sec, tag, port) ---
  if ((ctrl == SOLD_02::M_R_DELAYTIME || ctrl == SOLD_01::M_R_DELAYTIME) && len >= 4) {
    uint16_t secs = u16le(d);
    if (secs == 0xFFFF) secs = 0;        // wie JBC-Connect: 65535 => 0
    uint8_t tag  = d[2];
    uint8_t port = d[3];

    print_hdr_line(be, F("M_R_DELAYTIME"));

    // hübsch als mm:ss
    char mmss[6];
    uint16_t mm = secs / 60;
    uint8_t  ss = secs % 60;
    snprintf(mmss, sizeof(mmss), "%02u:%02u", (unsigned)mm, ss);
    kv_s(F("mmss"), String(mmss));
    kv_u(F("sec"), secs);
    kv_hex(F("tag"), tag, 2);

    // Tag -> Name
    if (tag == 'S')      kv_fs(F("next"), F("SLEEP"));
    else if (tag == 'H') kv_fs(F("next"), F("HIBERNATION"));
    else if (tag == 'N' || secs == 0) kv_fs(F("next"), F("NONE"));

    kv_u(F("port"), port);
    Serial.println();
    return true;
  }



  // ETH TCP/IP Config
  if (ctrl==SOLD_02::M_R_ETH_TCPIPCONFIG){
    print_hdr_line(be, F("M_R_ETH_TCPIPCONFIG"));
    if (len < 19){ kv_s(F("raw"), hex_string(d,len)); Serial.println(); return true; }
    auto ip2s = [](const uint8_t* p){ return String((uint8_t)p[0])+"."+String((uint8_t)p[1])+"."+String((uint8_t)p[2])+"."+String((uint8_t)p[3]); };
    kv_u(F("dhcp"), d[0]); kv_s(F("ip"), ip2s(&d[1])); kv_s(F("mask"), ip2s(&d[5]));
    kv_s(F("gw"), ip2s(&d[9])); kv_s(F("dns"), ip2s(&d[13])); kv_u(F("port"), u16le(&d[17]));
    Serial.println(); return true;
  }

  // ETH Connect-Status (roh)
  if (ctrl==SOLD_02::M_R_ETH_CONNECTSTATUS){
    print_hdr_line(be, F("M_R_ETH_CONNECTSTATUS")); kv_s(F("raw"), hex_string(d,len)); Serial.println(); return true;
  }

  // --- ALARME: MAX/MIN/aktuelle Temp ---
  if (ctrl == SOLD_02::M_R_ALARMMAXTEMP || ctrl == SOLD_02::M_R_ALARMMINTEMP) {
    const __FlashStringHelper* tag = (ctrl==SOLD_02::M_R_ALARMMAXTEMP) ? F("M_R_ALARM_MAXTEMP")
                                                                      : F("M_R_ALARM_MINTEMP");
    if (len < 2) { print_hdr_line(be, tag); kv_s(F("raw"), hex_string(d,len)); Serial.println(); return true; }

    const uint16_t uti   = u16le(d);           // Schwelle (UTI)
    const bool     off   = (uti == 0xFFFF);    // UTI=0xFFFF => deaktiviert
    const bool     hasPt = (len >= 5);
    const uint8_t  port  = hasPt ? d[4] : 0xFF;

    print_hdr_line(be, tag);
    kv_hex(F("uti"), uti, 4);
    if (!off) kv_c(F("c"), uti_to_c(uti), 1);
    kv_fs(F("state"), off ? F("DISABLED") : F("ENABLED"));
    if (hasPt) kv_u(F("port"), port);
    // d[2]/d[3] sind hier kein Tool; evtl. reserviert → nicht ausgeben
    Serial.println();
    return true;
  }

  if (ctrl == SOLD_02::M_R_ALARMTEMP) {
    if (len < 2) { print_hdr_line(be, F("M_R_ALARM_TEMP")); kv_s(F("raw"), hex_string(d,len)); Serial.println(); return true; }
    const uint16_t uti = u16le(d);             // aktuelle/gesetzte Alarmtemperatur
    print_hdr_line(be, F("M_R_ALARM_TEMP"));
    kv_hex(F("uti"), uti, 4);
    kv_c(F("c"), uti_to_c(uti), 1);
    Serial.println();
    return true;
  }

  // --- MAXTEMP / MINTEMP (UTI → °C) ---
  if ((ctrl == SOLD_02::M_R_MAXTEMP || ctrl == SOLD_01::M_R_MAXTEMP) && len >= 2) {
    const uint16_t v = u16le(d);
    print_hdr_line(be, F("M_R_MAXTEMP"));
    kv_c (F("c"),  uti_to_c(v), 1);
    kv_hex(F("uti"), v, 4);
    Serial.println();
    return true;
  }
  if ((ctrl == SOLD_02::M_R_MINTEMP || ctrl == SOLD_01::M_R_MINTEMP) && len >= 2) {
    const uint16_t v = u16le(d);
    print_hdr_line(be, F("M_R_MINTEMP"));
    kv_c (F("c"),  uti_to_c(v), 1);
    kv_hex(F("uti"), v, 4);
    Serial.println();
    return true;
  }

  // --- POWERLIM: Promille → Prozent ---
  if ((ctrl == SOLD_02::M_R_POWERLIM || ctrl == SOLD_01::M_R_POWERLIM) && len >= 2) {
    uint16_t raw = u16le(d);
    if (raw > 1000) raw = 1000;            // clamp 0..1000
    print_hdr_line(be, F("M_R_POWERLIM"));
    kv_c(F("pct"), raw / 10.0f, 1);        // z. B. 1000 → 100.0 %
    kv_u(F("raw"), raw);
    Serial.println();
    return true;
  }



  // CONTIMODE
  if (ctrl==SOLD_02::M_R_CONTIMODE || ctrl==SOLD_01::M_R_CONTIMODE){
    if(len>=1){ uint16_t v=(len>=2)?u16le(d):d[0]; print_hdr_line(be, F("M_R_CONTIMODE")); kv_hex(F("mask"), v,4); Serial.println(); return true; }
  }

  // LOCK_PORT (SOLDER_02 / SOLDER_01)
  // Antwortformat: [ state , port ]  -> state: 1=ON, 0=OFF
  if (ctrl==SOLD_02::M_R_LOCK_PORT || ctrl==SOLD_01::M_R_LOCK_PORT){
    print_hdr_line(be, F("M_R_LOCK_PORT"));
    if (len>=2){ kv_u(F("port"), d[1]); kv_u(F("on"), d[0]); }
    else if (len==1){ kv_u(F("on"), d[0]); }
    Serial.println(); return true;
  }



  // QST_ACTIVATE → ON/OFF
  if (ctrl==SOLD_02::M_R_QST_ACTIVATE || ctrl==SOLD_01::M_R_QST_ACTIVATE){
    if (len>=1){ print_hdr_line(be, F("M_R_QST_ACTIVATE")); kv_u(F("on"), d[0]); Serial.println(); return true; }
  }

  // TYPEOFGROUND
  if (ctrl==SOLD_02::M_R_TYPEOFGROUND && len>=1){
    print_hdr_line(be, F("M_R_TYPEOFGROUND")); kv_u(F("code"), d[0]);
    if (const __FlashStringHelper* n = typeofground_name(d[0])) kv_fs(F("name"), n);
    Serial.println(); return true;
  }


  

  // ---- Staterror
  if (ctrl==SOLD_02::M_R_STATERROR || ctrl==SOLD_01::M_R_STATERROR){
    uint16_t v = len? ((len>=2)?u16le(d):d[0]) : 0;
    print_hdr_line(be, F("M_R_STATERROR")); kv_hex(F("mask"), v, 4);
    if (const __FlashStringHelper* sn = station_error_name((uint8_t)(v & 0xFF))) kv_fs(F("lowbyte_name"), sn);
    Serial.println(); return true;
  }


  

  // Peripherie
  if (ctrl==SOLD_02::M_R_PERIPHCOUNT){
    print_hdr_line(be, F("M_R_PERIPHCOUNT")); if (len>=1) kv_u(F("count"), d[0]); else kv_s(F("raw"), hex_string(d,len)); Serial.println(); return true;
  }
  
  if (ctrl==SOLD_02::M_R_PERIPHCONFIG){
    print_hdr_line(be, F("M_R_PERIPHCONFIG"));
    if (len){ String s = sanitize_ascii(d,len); if(s.length()) kv_s(F("text"), s); else kv_s(F("raw"), hex_string(d,len)); }
    else kv_fs(F("text"), F(""));
    Serial.println(); return true;
  }
  
  if (ctrl==SOLD_02::M_R_PERIPHSTATUS){
    print_hdr_line(be, F("M_R_PERIPHSTATUS"));
    if (len==3){
      kv_u(F("active"), d[0]); char st=(char)d[1];
      kv_u(F("idx"), d[2]);
      const char* stTxt=(st=='C')?"CONNECTED":(st=='O')?"OPEN":(st=='K')?"OK":nullptr;
      if (stTxt) kv_s(F("state"), String(stTxt)); else kv_hex(F("state_char"), (uint8_t)st, 2);
    } else { kv_s(F("raw"), hex_string(d,len)); }
    Serial.println(); return true;
  }
  

  

  return false;
}

// --- SOLD-Minuten- & Zyklen-Zähler ---
static bool decode_sold_counters(Backend be, uint8_t ctrl, const uint8_t* d, uint8_t len){
  using namespace jbc_cmd;

  auto read_u32 = [&](const uint8_t* p, uint8_t L)->uint32_t{
    return (L >= 4) ? u32le(p) : 0;
  };

  auto kv_minutes = [&](const __FlashStringHelper* tag){
    if (len < 4){
      print_hdr_line(be, tag); kv_u(F("payload"), len); kv_s(F("raw"), hex_string(d,len)); Serial.println();
      return true;
    }
    const uint32_t v = read_u32(d, len);
    print_hdr_line(be, tag);
    kv_u(F("min"), v);
    // hübsch als H:MM
    uint32_t h = v / 60UL; uint8_t m = (uint8_t)(v % 60UL);
    char buf[6]; snprintf(buf, sizeof(buf), "%lu:%02u", (unsigned long)h, m);
    kv_s(F("hm"), String(buf));
    Serial.println();
    return true;
  };

  auto kv_u32 = [&](const __FlashStringHelper* tag){
    if (len < 4){
      print_hdr_line(be, tag); kv_u(F("payload"), len); kv_s(F("raw"), hex_string(d,len)); Serial.println();
      return true;
    }
    print_hdr_line(be, tag); kv_u(F("value"), u32le(d)); Serial.println();
    return true;
  };

  switch(ctrl){
    // Gesamt-Zeiten (Minuten)
    case SOLD_02::M_R_PLUGTIME:     return kv_minutes(F("M_R_PLUGTIME"));
    case SOLD_02::M_R_WORKTIME:     return kv_minutes(F("M_R_WORKTIME"));
    case SOLD_02::M_R_SLEEPTIME:    return kv_minutes(F("M_R_SLEEPTIME"));
    case SOLD_02::M_R_HIBERTIME:    return kv_minutes(F("M_R_HIBERTIME"));
    case SOLD_02::M_R_NOTOOLTIME:   return kv_minutes(F("M_R_NOTOOLTIME"));

    // Zyklen
    case SOLD_02::M_R_SLEEPCYCLES:  return kv_u32(F("M_R_SLEEPCYCLES"));
    case SOLD_02::M_R_DESOLCYCLES:  return kv_u32(F("M_R_DESOLCYCLES"));

    // Partial-Zeiten
    case SOLD_02::M_R_PLUGTIMEP:    return kv_minutes(F("M_R_PLUGTIMEP"));
    case SOLD_02::M_R_WORKTIMEP:    return kv_minutes(F("M_R_WORKTIMEP"));
    case SOLD_02::M_R_SLEEPTIMEP:   return kv_minutes(F("M_R_SLEEPTIMEP"));
    case SOLD_02::M_R_HIBERTIMEP:   return kv_minutes(F("M_R_HIBERTIMEP"));
    case SOLD_02::M_R_NOTOOLTIMEP:  return kv_minutes(F("M_R_NOTOOLTIMEP"));

    // Partial-Zyklen
    case SOLD_02::M_R_SLEEPCYCLESP: return kv_u32(F("M_R_SLEEPCYCLESP"));
    case SOLD_02::M_R_DESOLCYCLESP: return kv_u32(F("M_R_DESOLCYCLESP"));
  }
  return false;
}


// ---------- HA-Extras ----------
static void ha_changes_print(uint8_t c){
  struct { uint8_t bit; const char* name; } B[] = {
    {0, "SELECTTEMP_CHANGED"},
    {1, "STATION_PARAM_CHANGED"},
    {2, "TOOL_PARAM_GRP0_CHANGED"},
    {3, "TOOL_PARAM_GRP1_CHANGED"},
    {4, "TOOL_PARAM_GRP2_CHANGED"},
    {5, "TOOL_PARAM_GRP3_CHANGED"},
    {7, "COUNTER_CHANGED"},
  };
  bool any=false;
  Serial.print(F("  changesText="));
  for (auto &b : B){
    if (c & (1u<<b.bit)){ if(any) Serial.print('|'); Serial.print(b.name); any=true; }
  }
  for(uint8_t b=0;b<8;b++){
    if ((c & (1u<<b)) && b!=0 && b!=1 && b!=2 && b!=3 && b!=4 && b!=5 && b!=7){
      if(any) Serial.print('|'); Serial.print(F("BIT")); Serial.print(b); any=true;
    }
  }
  if(!any) Serial.println(F("NONE")); else Serial.println();
}

// --- HA-Minuten- & Zyklen-Zähler ---
static bool decode_ha_counters(Backend be, uint8_t ctrl, const uint8_t* d, uint8_t len){
  using namespace jbc_cmd;

  auto read_u32_from_5_or_4 = [&](const uint8_t* p, uint8_t L)->uint32_t{
    if (L >= 4) return u32le(p);
    return 0;
  };

  auto print_minutes = [&](const __FlashStringHelper* tag){
    if(len<4){ print_hdr_line(be, tag); kv_s(F("raw"), hex_string(d,len)); Serial.println(); return true; }
    uint32_t v = read_u32_from_5_or_4(d,len);
    print_hdr_line(be, tag); kv_u(F("min"), v);
    uint32_t h=v/60UL; uint8_t m=(uint8_t)(v%60UL); char buf[6]; snprintf(buf,sizeof(buf),"%lu:%02u",(unsigned long)h,m);
    kv_s(F("hm"), String(buf)); Serial.println(); return true;
  };

  auto print_u32v = [&](const __FlashStringHelper* tag){
    if(len<4){ print_hdr_line(be, tag); kv_s(F("raw"), hex_string(d,len)); Serial.println(); return true; }
    print_hdr_line(be, tag); kv_u(F("value"), read_u32_from_5_or_4(d,len)); Serial.println(); return true;
  };

  switch(ctrl){
    // Gesamt-Zeiten (Minuten)
    case HA_02::M_R_PLUGTIME:        return print_minutes(F("M_R_PLUGTIME"));
    case HA_02::M_R_WORKTIME:        return print_minutes(F("M_R_WORKTIME"));

    // Zyklen
    case HA_02::M_R_WORKCYCLES:      return print_u32v(F("M_R_WORKCYCLES"));
    case HA_02::M_R_SUCTIONCYCLES:   return print_u32v(F("M_R_SUCTIONCYCLES"));

    // Partial
    case HA_02::M_R_PLUGTIMEP:       return print_minutes(F("M_R_PLUGTIMEP"));
    case HA_02::M_R_WORKTIMEP:       return print_minutes(F("M_R_WORKTIMEP"));
    case HA_02::M_R_WORKCYCLESP:     return print_u32v(F("M_R_WORKCYCLESP"));
    case HA_02::M_R_SUCTIONCYCLESP:  return print_u32v(F("M_R_SUCTIONCYCLESP"));
  }
  return false;
}

static bool decode_ha_extras(Backend be, uint8_t ctrl, const uint8_t* d, uint8_t len){
  using namespace jbc_cmd;

  // Prozent aus Promille
  if (ctrl==HA_02::M_R_AIRFLOW || ctrl==HA_02::M_R_POWER){
    if (len < 2){ print_hdr_line(be, (ctrl==HA_02::M_R_AIRFLOW)?F("M_R_AIRFLOW"):F("M_R_POWER")); kv_s(F("raw"), hex_string(d,len)); Serial.println(); return true; }
    uint16_t ppm=u16le(d); if(ppm>1000) ppm=1000;
    print_hdr_line(be, (ctrl==HA_02::M_R_AIRFLOW)?F("AIRFLOW"):F("POWER"));
    kv_c(F("pct"), ppm/10.0f, 1); kv_u(F("raw"), ppm); Serial.println(); return true;
  }

  // ---- in decode_ha_extras(...) ERSETZEN ----
  if (ctrl==HA_02::M_R_STATERROR){
    uint16_t v = len? ((len>=2)?u16le(d):d[0]) : 0;
    print_hdr_line(be, F("M_R_STATERROR")); kv_hex(F("mask"), v, 4);
    if (const __FlashStringHelper* sn = station_error_name((uint8_t)(v & 0xFF))) kv_fs(F("lowbyte_name"), sn);
    Serial.println(); return true;
  }

  // --- HA: STARTMODE (Bitfeld) + AJUSTTEMP (Offset in °C) ---
  // STARTMODE (Bitfeld): 1=TOOL_BUTTON, 2=STAND_OUT, 4=PEDAL_PULSE, 8=PEDAL_HOLD_DOWN
  if (ctrl==HA_02::M_R_STARTMODE){
    if (len>=1){
      const uint8_t v = d[0];
      print_hdr_line(BK_HA, F("M_R_STARTMODE"));
      kv_hex(F("value"), v, 2);
      String bits;
      auto add=[&](const char* n){ if(bits.length()) bits += '|'; bits += n; };
      if (v & 0x01) add("TOOL_BUTTON");
      if (v & 0x02) add("STAND_OUT");
      if (v & 0x04) add("PEDAL_PULSE");
      if (v & 0x08) add("PEDAL_HOLD_DOWN");
      if (!bits.length()) bits = "NONE";
      kv_s(F("bits"), bits);
      Serial.println();
      return true;
    }    
  }

  // AJUSTTEMP: deltaLE(int16), port, tool   (UTI = °C*9)
  if (ctrl==HA_02::M_R_AJUSTTEMP){
    if (len>=4){
      print_hdr_line(be, F("M_R_AJUSTTEMP"));
      kv_c(F("delta_c"), (int16_t)u16le(d)/9.0f, 1);
      kv_u(F("port"), d[2]); kv_u(F("tool"), d[3]);
      if (const __FlashStringHelper* tn = ha_tool_name(d[3])) kv_fs(F("tool_name"), tn);
      Serial.println(); return true;
    } else if (len>=2){
      print_hdr_line(be, F("M_R_AJUSTTEMP")); kv_c(F("delta_c"), (int16_t)u16le(d)/9.0f, 1); Serial.println(); return true;
    }
  }


  // --- EXTTCMODE (on/off, optional port,tool) ---
  if (ctrl == HA_02::M_R_EXTTCMODE){
    if (len>=1){
      print_hdr_line(be, F("M_R_EXTTCMODE")); kv_u(F("on"), d[0]);
      if (len>=3){ kv_u(F("port"), d[1]); kv_u(F("tool"), d[2]); if (const __FlashStringHelper* tn=ha_tool_name(d[2])) kv_fs(F("tool_name"), tn); }
      Serial.println(); return true;
    }
  }

  
  // --- HA: CONTIMODE (Bitmaske, wie bei SOLD) ---
  if (ctrl == HA_02::M_R_CONTIMODE){
    print_hdr_line(be, F("M_R_CONTIMODE"));
    if (len >= 2){
      kv_hex(F("mask"), u16le(d), 4);
    } else if (len == 1){
      kv_hex(F("mask"), d[0], 2);
    } else {
      kv_fs(F("mask"), F("?"));
    }
    Serial.println();
    return true;
  }
  

  

  // THEME
  if (ctrl==HA_02::M_R_THEME && len>=1){
    print_hdr_line(be, F("M_R_THEME")); kv_u(F("code"), d[0]); if(const char* nm=theme_name(d[0])) kv_s(F("name"), String(nm)); Serial.println(); return true;
  }

  // DATETIME
  if (ctrl==HA_02::M_R_DATETIME){ return decode_datetime_payload(BK_HA, d,len); }

  

  

  
  // einfache u8-Status
  if (len>=1){
    if (ctrl==HA_02::M_R_HEATERSTATUS){ print_hdr_line(be, F("M_R_HEATERSTATUS")); kv_u(F("on"), d[0]); Serial.println(); return true; }
    if (ctrl==HA_02::M_R_SUCTIONSTATUS){ print_hdr_line(be, F("M_R_SUCTIONSTATUS")); kv_u(F("on"), d[0]); Serial.println(); return true; }
    if (ctrl==HA_02::M_R_PROFILEMODE){ print_hdr_line(be, F("M_R_PROFILEMODE")); kv_u(F("code"), d[0]); Serial.println(); return true; }
  }

  if (ctrl==HA_02::M_R_BEEP && len>=1){ print_hdr_line(be, F("M_R_BEEP")); kv_u(F("on"), d[0]); Serial.println(); return true; }

  // SELECTFLOW / SELECTEXTTEMP / TIMETOSTOP
  if (ctrl==HA_02::M_R_SELECTFLOW && len>=2){ uint16_t ppm=u16le(d); if(ppm>1000) ppm=1000; print_hdr_line(be, F("M_R_SELECTFLOW")); kv_c(F("pct"), ppm/10.0f,1); kv_u(F("raw"), ppm); Serial.println(); return true; }
  if (ctrl==HA_02::M_R_SELECTEXTTEMP && len>=2){ uint16_t v=u16le(d); print_hdr_line(be, F("M_R_SELECTEXTTEMP")); kv_c(F("c"), uti_to_c(v),1); kv_hex(F("uti"), v,4); Serial.println(); return true; }
  if (ctrl==HA_02::M_R_TIMETOSTOP && len>=2){ uint16_t ds=u16le(d); print_hdr_line(be, F("M_R_TIMETOSTOP")); kv_s(F("mmss"), fmt_mmss_tenths(ds)); kv_u(F("ds"), ds); Serial.println(); return true; }  

  // MAX/MIN Paare
  if (len>=4){
    if (ctrl==HA_02::M_R_MAXMINTEMP){
      print_hdr_line(be, F("M_R_MAXMINTEMP"));
      kv_c(F("max_c"), uti_to_c(u16le(&d[0])),1); kv_c(F("min_c"), uti_to_c(u16le(&d[2])),1); Serial.println(); return true;
    }
    if (ctrl==HA_02::M_R_MAXMINFLOW){
      uint16_t a=u16le(&d[0]), b=u16le(&d[2]); if(a>1000) a=1000; if(b>1000) b=1000;
      print_hdr_line(be, F("M_R_MAXMINFLOW")); kv_c(F("max_pct"), a/10.0f,1); kv_c(F("min_pct"), b/10.0f,1); Serial.println(); return true;
    }
    if (ctrl==HA_02::M_R_MAXMINEXTTEMP){
      print_hdr_line(be, F("M_R_MAXMINEXTTEMP"));
      kv_c(F("max_c"), uti_to_c(u16le(&d[0])),1); kv_c(F("min_c"), uti_to_c(u16le(&d[2])),1); Serial.println(); return true;
    }
  }

  // Station Locked (HA)
  if (ctrl==HA_02::M_R_STATIONLOCKED && len>=1){ print_hdr_line(be, F("M_R_STATIONLOCKED")); kv_u(F("on"), d[0]); Serial.println(); return true; }

  return false;
}

// Date/Time – robust, mehrere Varianten
static bool decode_datetime_payload(Backend be, const uint8_t* d, uint8_t len){
  auto print_iso = [&](uint16_t Y,uint8_t M,uint8_t D,uint8_t h,uint8_t m,uint8_t s){
    char buf[20];
    snprintf(buf,sizeof(buf),"%04u-%02u-%02u %02u:%02u:%02u",(unsigned)Y,M,D,h,m,s);
    print_hdr_line(be, F("M_R_DATETIME"));
    kv_s(F("iso"), String(buf));
    kv_u(F("year"), Y); kv_u(F("month"), M); kv_u(F("day"), D);
    kv_u(F("hour"), h); kv_u(F("min"), m);  kv_u(F("sec"), s);
    Serial.println();
  };  

  auto norm12 = [](uint8_t v)->int{
    if (v>=1 && v<=12) return v;
    uint8_t b = bcd_swap(v);
    return (b!=0xFF && b>=1 && b<=12) ? (int)b : -1;
  };
  auto norm31 = [](uint8_t v)->int{
    if (v>=1 && v<=31) return v;
    uint8_t b = bcd_swap(v);
    return (b!=0xFF && b>=1 && b<=31) ? (int)b : -1;
  };
  auto print_time_only_line = [&](uint16_t Y, int M, int D, uint32_t sod){
    print_hdr_line(be, F("M_R_DATETIME"));
    kv_u(F("sod"), sod);
    if (Y)     kv_u(F("year"),  Y);
    if (M > 0) kv_u(F("month"), (uint8_t)M);
    if (D > 0) kv_u(F("day"),   (uint8_t)D);
    Serial.println();
  };


  // Kandidat A: HA-Layout (u24 SOD, YearOff, Mon, Day)
  if (len == 7){
    const uint32_t sod = u24le(&d[0]);
    if (sod <= 86399UL){
      const uint16_t yLE = u16le(&d[3]);
      const uint16_t yBE = (uint16_t(d[3])<<8) | d[4];
      const uint16_t yOff = (yLE <= 300) ? yLE : (yBE <= 300 ? yBE : 0xFFFF);
      const int monA = norm12(d[5]);
      const int dayA = norm31(d[6]);
      if (yOff != 0xFFFF){
        const uint16_t Y = 2000 + yOff;
        const uint8_t  h = (uint8_t)(sod/3600UL);
        const uint8_t  m = (uint8_t)((sod%3600UL)/60UL);
        const uint8_t  s = (uint8_t)(sod%60UL);
        if (monA>0 && dayA>0){ print_iso(Y,(uint8_t)monA,(uint8_t)dayA,h,m,s); return true; }
        print_time_only_line(Y, monA, dayA, sod);
        return true;
      }
      // Kandidat A’ (Mon/Day vertauscht)
      const int monB = norm12(d[6]);
      const int dayB = norm31(d[5]);
      if (yLE <= 300 || yBE <= 300){
        const uint16_t Y = 2000 + ((yLE <= 300) ? yLE : yBE);
        const uint8_t  h = (uint8_t)(sod/3600UL);
        const uint8_t  m = (uint8_t)((sod%3600UL)/60UL);
        const uint8_t  s = (uint8_t)(sod%60UL);
        if (monB>0 && dayB>0){ print_iso(Y,(uint8_t)monB,(uint8_t)dayB,h,m,s); return true; }
        print_time_only_line(Y, monB, dayB, sod);
        return true;
      }
    }

    // Alt: u16 SOD + YearOff BE + Mon + Day
    const uint16_t sod16  = u16le(&d[0]);
    const uint16_t yOffBE = (uint16_t(d[2])<<8) | d[3];
    const int monC = norm12(d[4]);
    const int dayC = norm31(d[5]);
    if (sod16 <= 86399 && yOffBE <= 300){
      const uint16_t Y = 2000 + yOffBE;
      const uint8_t  h = (uint8_t)(sod16/3600U);
      const uint8_t  m = (uint8_t)((sod16%3600U)/60U);
      const uint8_t  s = (uint8_t)(sod16%60U);
      if (monC>0 && dayC>0){ print_iso(Y,(uint8_t)monC,(uint8_t)dayC,h,m,s); return true; }
      print_time_only_line(Y, monC, dayC, sod16);
      return true;
    }
  }

  // Kandidat B: 7B: year(u16 LE), mon, day, hh, mm, ss
  if (len >= 7){
    const uint16_t Y = u16le(&d[0]);
    const uint8_t  M = d[2], D=d[3], h=d[4], m=d[5], s=d[6];
    if (Y>=2000 && Y<=2099 && M>=1 && M<=12 && D>=1 && D<=31 && h<=23 && m<=59 && s<=59){
      print_iso(Y,M,D,h,m,s);
      return true;
    }
  }

  // Kandidat C: 6B BCD: YY MM DD hh mm ss
  if (len >= 6){
    bool bcdok=true; for(uint8_t i=0;i<6;i++) bcdok &= is_bcd(d[i]);
    if (bcdok){
      const uint16_t Y = 2000 + bcd2(d[0]);
      const uint8_t  M = bcd2(d[1]), D=bcd2(d[2]), h=bcd2(d[3]), m=bcd2(d[4]), s=bcd2(d[5]);
      if (M>=1 && M<=12 && D>=1 && D<=31 && h<=23 && m<=59 && s<=59){
        print_iso(Y,M,D,h,m,s);
        return true;
      }
    }
  }

  // Fallback – roh
  print_hdr_line(be, F("M_R_DATETIME")); kv_s(F("raw"), hex_string(d,len)); Serial.println();
  return true;
}

// ---------- PH-Extras ----------
static bool decode_ph_extras(Backend be, uint8_t ctrl, const uint8_t* d, uint8_t len){
  using namespace jbc_cmd;

  // SELECTPOWER (0..1000 → %)
  if (ctrl==PH_02::M_R_SELECTPOWER){
    if(len<2){ print_hdr_line(be, F("M_R_SELECTPOWER")); kv_s(F("raw"), hex_string(d,len)); Serial.println(); return true; }
    uint16_t ppm=u16le(d); if(ppm>1000) ppm=1000;
    print_hdr_line(be, F("M_R_SELECTPOWER")); kv_c(F("pct"), ppm/10.0f,1); kv_u(F("raw"), ppm); Serial.println(); return true;
  }

  // WARNINGS (roh)
  if (ctrl==PH_02::M_R_WARNING){ print_hdr_line(be, F("M_R_WARNING")); kv_s(F("raw"), hex_string(d,len)); Serial.println(); return true; }

  // ACTIVEZONES (roh)
  if (ctrl==PH_02::M_R_ACTIVEZONES && len>=1){ print_hdr_line(be, F("M_R_ACTIVEZONES")); kv_hex(F("mask"), d[0], 2); Serial.println(); return true; }

  // einfache u8-Status
  if (len>=1){
    if (ctrl==PH_02::M_R_WORKMODE){ print_hdr_line(be, F("M_R_WORKMODE")); kv_u(F("code"), d[0]); Serial.println(); return true; }
    if (ctrl==PH_02::M_R_HEATERSTATUS){ print_hdr_line(be, F("M_R_HEATERSTATUS")); kv_u(F("on"), d[0]); Serial.println(); return true; }
    if (ctrl==PH_02::M_R_EXTTCMODE){ print_hdr_line(be, F("M_R_EXTTCMODE")); kv_u(F("on"), d[0]); Serial.println(); return true; }
  }
  // TIMETOSTOP (PH)
  if (ctrl==PH_02::M_R_TIMETOSTOP && len>=2){
    uint16_t ds=u16le(d);
    print_hdr_line(be, F("M_R_TIMETOSTOP")); kv_s(F("mmss"), fmt_mmss_tenths(ds)); kv_u(F("ds"), ds); Serial.println(); return true;
  }

  return false;
}

// ---------- FE-Extras ----------
static bool decode_fe_extras(Backend be, uint8_t ctrl, const uint8_t* d, uint8_t len){
  using namespace jbc_cmd;

  if (ctrl==FE_02::M_R_FLOW){
    if(len<2){ print_hdr_line(be, F("M_R_FLOW")); kv_s(F("raw"), hex_string(d,len)); Serial.println(); return true; }
    uint16_t ppm=u16le(d); if(ppm>1000) ppm=1000; print_hdr_line(be, F("M_R_FLOW")); kv_c(F("pct"), ppm/10.0f,1); kv_u(F("raw"), ppm); Serial.println(); return true;
  }
  if (ctrl==FE_02::M_R_SPEED){
    if(len<2){ print_hdr_line(be, F("M_R_SPEED")); kv_s(F("raw"), hex_string(d,len)); Serial.println(); return true; }
    print_hdr_line(be, F("M_R_SPEED")); kv_u(F("raw"), u16le(d)); Serial.println(); return true;
  }
  if (ctrl==FE_02::M_R_SELECTFLOW){
    if(len<2){ print_hdr_line(be, F("M_R_SELECTFLOW")); kv_s(F("raw"), hex_string(d,len)); Serial.println(); return true; }
    uint16_t ppm=u16le(d); if(ppm>1000) ppm=1000; print_hdr_line(be, F("M_R_SELECTFLOW")); kv_c(F("pct"), ppm/10.0f,1); kv_u(F("raw"), ppm); Serial.println(); return true;
  }
  if (ctrl==FE_02::M_R_SUCTIONLEVEL){
    if(len<2){ print_hdr_line(be, F("M_R_SUCTIONLEVEL")); kv_s(F("raw"), hex_string(d,len)); Serial.println(); return true; }
    uint16_t v=u16le(d); print_hdr_line(be, F("M_R_SUCTIONLEVEL")); if(v<=1000) kv_c(F("pct"), (v>1000?1000:v)/10.0f,1); kv_u(F("raw"), v); Serial.println(); return true;
  }
  if (ctrl==FE_02::M_R_FILTERSTATUS && len>=1){ print_hdr_line(be, F("M_R_FILTERSTATUS")); kv_u(F("code"), d[0]); Serial.println(); return true; }
  if (ctrl==FE_02::M_R_CONNECTEDPEDAL && len>=1){ print_hdr_line(be, F("M_R_CONNECTEDPEDAL")); kv_u(F("connected"), d[0]); Serial.println(); return true; }
  if (ctrl==FE_02::M_R_RESETFILTER && len>=1){ print_hdr_line(be, F("M_R_RESETFILTER")); kv_u(F("code"), d[0]); Serial.println(); return true; }

  if (ctrl==FE_02::M_R_COUNTERS){ print_hdr_line(be, F("M_R_COUNTERS")); kv_s(F("raw"), hex_string(d,len)); Serial.println(); return true; }

  // PIN / PINENABLED / STATIONLOCKED / BEEP – durch crossfamily bereits abgedeckt; hier nur Fallbacks
  if (ctrl==FE_02::M_R_PIN){
    if (len==4 && d[0]>='0'&&d[0]<='9' && d[1]>='0'&&d[1]<='9' &&
                 d[2]>='0'&&d[2]<='9' && d[3]>='0'&&d[3]<='9'){
      print_hdr_tag(F("PIN")); for(uint8_t i=0;i<4;i++) Serial.print((char)d[i]); Serial.println();
    } else { Serial.print(F("[PIN] raw ")); print_hex(d,len); Serial.println(); }
    return true;
  }
  if (ctrl==FE_02::M_R_PINENABLED && len>=1){ print_onoff(F("M_R_PINENABLED"), d[0]); return true; }
  if (ctrl==FE_02::M_R_STATIONLOCKED && len>=1){ print_onoff(F("M_R_STATIONLOCKED"), d[0]); return true; }
  if (ctrl==FE_02::M_R_BEEP && len>=1){ print_onoff(F("M_R_BEEP"), d[0]); return true; }

  // Pedal/Intake Settings
  if (ctrl==FE_02::M_R_ACTIVATIONPEDAL && len>=1){ print_hdr_line(be, F("M_R_ACTIVATIONPEDAL")); kv_u(F("on"), d[0]); Serial.println(); return true; }
  if (ctrl==FE_02::M_R_PEDALMODE && len>=1){ print_hdr_line(be, F("M_R_PEDALMODE")); kv_u(F("code"), d[0]); Serial.println(); return true; }
  if (ctrl==FE_02::M_R_INTAKEACTIVATION && len>=1){ print_hdr_line(be, F("M_R_INTAKEACTIVATION")); kv_u(F("on"), d[0]); Serial.println(); return true; }
  if (ctrl==FE_02::M_R_SUCTIONDELAY && len>=2){ print_hdr_line(be, F("M_R_SUCTIONDELAY")); kv_u(F("ms"), u16le(d)); Serial.println(); return true; }

  return false;
}

// ---------- SF-Extras ----------
static bool decode_sf_extras(Backend be, uint8_t ctrl, const uint8_t* d, uint8_t len){
  using namespace jbc_cmd;

  if (ctrl==SF_02::M_R_SPEED){
    if(len<2){ print_hdr_line(be, F("M_R_SPEED")); kv_s(F("raw"), hex_string(d,len)); Serial.println(); return true; }
    print_hdr_line(be, F("M_R_SPEED")); kv_u(F("raw"), u16le(d)); Serial.println(); return true;
  }
  if (ctrl==SF_02::M_R_LENGTH){
    if(len<2){ print_hdr_line(be, F("M_R_LENGTH")); kv_s(F("raw"), hex_string(d,len)); Serial.println(); return true; }
    print_hdr_line(be, F("M_R_LENGTH")); kv_u(F("raw"), u16le(d)); Serial.println(); return true;
  }
  if (ctrl==SF_02::M_R_PROGRAM && len>=1){ print_hdr_line(be, F("M_R_PROGRAM")); kv_u(F("code"), d[0]); Serial.println(); return true; }
  if (ctrl==SF_02::M_R_DISPENSERMODE && len>=1){ print_hdr_line(be, F("M_R_DISPENSERMODE")); kv_u(F("code"), d[0]); Serial.println(); return true; }
  if (ctrl==SF_02::M_R_BACKWARDMODE && len>=1){ print_hdr_line(be, F("M_R_BACKWARDMODE")); kv_u(F("code"), d[0]); Serial.println(); return true; }
  if (ctrl==SF_02::M_R_TOOLENABLED && len>=1){ print_hdr_line(be, F("M_R_TOOLENABLED")); kv_u(F("on"), d[0]); Serial.println(); return true; }
  if (ctrl==SF_02::M_R_LENGTHUNIT && len>=1){ print_hdr_line(be, F("M_R_LENGTHUNIT")); kv_u(F("code"), d[0]); Serial.println(); return true; }
  if (ctrl==SF_02::M_R_PROGRAMLIST || ctrl==SF_02::M_R_COUNTERS){ print_hdr_line(be, F("SF_RAW")); kv_s(F("raw"), hex_string(d,len)); Serial.println(); return true; }

  

  return false;
}

// --- Write-ACKs nur für echte Write-Controls ---
static bool decode_write_acks(Backend be, uint8_t ctrl, const uint8_t* d, uint8_t len){
  using namespace jbc_cmd;

  // 0) Reines ACK-Frame (M_ACK) -> sofort OK
  if (is_ack_ctrl(ctrl)){
    print_hdr_line(be, F("ACK")); Serial.print(F("OK"));
    if (len){ Serial.print(F(" extra=")); print_hex(d, len); }
    Serial.println();
    return true;
  }

  // 1) Spezielle Multi-Byte-ACKs zuerst (schöne Ausgabe)
  if (be == BK_SOLD && ctrl == SOLD_02::M_W_LEVELSTEMPS && len >= 1 && d[0] == 0x06){
    print_hdr_line(be, F("LEVELSTEMPS/ACK")); Serial.print(F("OK"));
    if (len >= 2){ Serial.print(F(" port=")); Serial.print(d[1]); }
    if (len >= 3){
      Serial.print(F(" tool=")); Serial.print(d[2]);
      if (const __FlashStringHelper* tn = tool_name_from_code(d[2])){
        Serial.print(F(" (")); Serial.print(tn); Serial.print(')');
      }
    }
    Serial.println();
    return true;
  }
  if (be == BK_SOLD && ctrl == SOLD_02::M_W_LOCK_PORT && len >= 1 && d[0] == 0x06){
    print_hdr_line(be, F("LOCK_PORT/ACK")); Serial.print(F("OK"));
    if (len >= 2){ Serial.print(F(" port=")); Serial.print(d[1]); }
    Serial.println();
    return true;
  }

  // 2) Gating: nur für bekannte WRITE-Controls überhaupt ACKs akzeptieren
  auto is_known_write = [&](Backend b)->bool{
    switch (b){
      case BK_SOLD:
        switch (ctrl){
          // Identität / Strings / Basics
          case SOLD_02::M_W_DEVICEID:
          case SOLD_02::M_W_DEVICENAME:
          case SOLD_02::M_W_PIN:
          case SOLD_02::M_W_BEEP:
          case SOLD_02::M_W_LANGUAGE:
          case SOLD_02::M_W_TEMPUNIT:
          case SOLD_02::M_W_TYPEOFGROUND:

          // USB / ETH
          case SOLD_02::M_W_USB_CONNECTSTATUS:
          case SOLD_02::M_W_ETH_TCPIPCONFIG:
          case SOLD_02::M_W_ETH_CONNECTSTATUS:

          // Level-/Port-/Tool-Settings
          case SOLD_02::M_W_LEVELSTEMPS:
          case SOLD_02::M_W_SLEEPDELAY:
          case SOLD_02::M_W_SLEEPTEMP:
          case SOLD_02::M_W_HIBERDELAY:
          case SOLD_02::M_W_AJUSTTEMP:
          case SOLD_02::M_W_CARTRIDGE:
          case SOLD_02::M_W_SELECTTEMP:
          case SOLD_02::M_W_SELECTTEMPVOLATILE:
          case SOLD_02::M_W_LOCK_PORT:

          // Alarm / Limits / Modes
          case SOLD_02::M_W_ALARMMAXTEMP:
          case SOLD_02::M_W_ALARMMINTEMP:
          case SOLD_02::M_W_POWERLIM:
          case SOLD_02::M_W_QST_ACTIVATE:
          case SOLD_02::M_W_QST_STATUS:
          case SOLD_02::M_W_CONTIMODE:
          case SOLD_02::M_W_REMOTEMODE:

          // Peripherie / Maintenance
          case SOLD_02::M_W_PERIPHCONFIG:
          case SOLD_02::M_W_PERIPHSTATUS:
          case SOLD_02::M_W_RESETCOUNTERS:

          // (nur falls vorhanden in deinen Enums)
          case SOLD_02::M_W_MAXTEMP:
          case SOLD_02::M_W_MINTEMP:
            return true;
          default: return false;
        }
      case BK_HA:
        switch (ctrl){
          case HA_02::M_W_DEVICEID:
          case HA_02::M_W_DEVICENAME:
          case HA_02::M_W_BEEP:
          case HA_02::M_W_PIN:
          case HA_02::M_W_PINENABLED:
          case HA_02::M_W_STATIONLOCKED:
          case HA_02::M_W_SELECTTEMP:
          case HA_02::M_W_SELECTFLOW:
          case HA_02::M_W_EXTTCMODE:
          case HA_02::M_W_PROFILEMODE:
          case HA_02::M_W_AJUSTTEMP:
          case HA_02::M_W_SELECTEXTTEMP:
          case HA_02::M_W_TIMETOSTOP:
          case HA_02::M_W_STARTMODE:
          case HA_02::M_W_REMOTEMODE:
          case HA_02::M_W_TEMPUNIT:
          case HA_02::M_W_LANGUAGE:
          case HA_02::M_W_MAXMINTEMP:
          case HA_02::M_W_MAXMINFLOW:
          case HA_02::M_W_MAXMINEXTTEMP:
          case HA_02::M_W_LEVELSTEMPS:
          case HA_02::M_W_HEATERSTATUS:
          case HA_02::M_W_SUCTIONSTATUS:
          case HA_02::M_W_USB_CONNECTSTATUS:
          case HA_02::M_W_DATETIME:
          case HA_02::M_W_THEME:
            return true;
          default: return false;
        }
      case BK_PH:
        switch (ctrl){
          case PH_02::M_W_DEVICEID:
          case PH_02::M_W_WORKMODE:
          case PH_02::M_W_HEATERSTATUS:
          case PH_02::M_W_EXTTCMODE:
          case PH_02::M_W_TIMETOSTOP:
          case PH_02::M_W_SELECTTEMP:
          case PH_02::M_W_SELECTPOWER:
          case PH_02::M_W_ACTIVEZONES:
          case PH_02::M_W_REMOTEMODE:
          case PH_02::M_W_CONTIMODE:
          case PH_02::M_W_PROFILE:
          case PH_02::M_W_SETTINGSPROFILE:
          case PH_02::M_W_PROFILETEACH:
          case PH_02::M_W_MAXMINPOWER:
          case PH_02::M_W_MAXMINTEMP:
          case PH_02::M_W_PINENABLED:
          case PH_02::M_W_STATIONLOCKED:
          case PH_02::M_W_PIN:
          case PH_02::M_W_DEVICENAME:
          case PH_02::M_W_BEEP:
          case PH_02::M_W_USB_CONNECTSTATUS:
            return true;
          default: return false;
        }
      case BK_FE:
        switch (ctrl){
          case FE_02::M_W_DEVICEID:
          case FE_02::M_W_SUCTIONLEVEL:
          case FE_02::M_W_SELECTFLOW:
          case FE_02::M_W_STANDINTAKES:
          case FE_02::M_W_INTAKEACTIVATION:
          case FE_02::M_W_SUCTIONDELAY:
          case FE_02::M_W_ACTIVATIONPEDAL:
          case FE_02::M_W_PEDALMODE:
          case FE_02::M_W_PIN:
          case FE_02::M_W_STATIONLOCKED:
          case FE_02::M_W_BEEP:
          case FE_02::M_W_CONTINUOUSSUCTION:
          case FE_02::M_W_DEVICENAME:
          case FE_02::M_W_PINENABLED:
          case FE_02::M_W_WORKINTAKES:
          case FE_02::M_W_USB_CONNECTSTATUS:
            return true;
          default: return false;
        }
      case BK_SF:
        switch (ctrl){
          case SF_02::M_W_DEVICEID:
          case SF_02::M_W_DISPENSERMODE:
          case SF_02::M_W_PROGRAM:
          case SF_02::M_W_PROGRAMLIST:
          case SF_02::M_W_SPEED:
          case SF_02::M_W_LENGTH:
          case SF_02::M_W_BACKWARDMODE:
          case SF_02::M_W_PIN:
          case SF_02::M_W_STATIONLOCKED:
          case SF_02::M_W_BEEP:
          case SF_02::M_W_LENGTHUNIT:
          case SF_02::M_W_DEVICENAME:
          case SF_02::M_W_TOOLENABLED:
          case SF_02::M_W_USB_CONNECTSTATUS:
            return true;
          default: return false;
        }
      default: return false;
    }
  };


  // 3) Generischer Mehrbyte-ACK: erstes Byte 0x06 => OK (z. B. "06 00")
  if (is_known_write(be) && len >= 1 && d[0] == 0x06) {
    print_hdr_line(be, F("ACK"));             // gibt [ACK] und ggf. <fid=...> aus
    Serial.print(F("OK"));
    if (len >= 2) { Serial.print(F(" port=")); Serial.print(d[1]); }
    if (len >= 3) {
      Serial.print(F(" tool=")); Serial.print(d[2]);
      if (const __FlashStringHelper* tn =
            (be==BK_HA ? ha_tool_name(d[2]) : tool_name_from_code(d[2]))){
        Serial.print(F(" (")); Serial.print(tn); Serial.print(')');
      }
    }
    if (len > 3) { Serial.print(F(" extra=")); print_hex(&d[3], len-3); }
    Serial.println();
    return true;
  }

  

  return false; // alles andere ist kein (reiner) ACK
}



// NACK hübsch ausgeben (Family-agnostisch)
static bool decode_nack(Backend be, uint8_t ctrl, const uint8_t* d, uint8_t len){
  if (!is_nack_ctrl(ctrl)) return false;

  print_hdr_line(be, F("NACK"));

  // reason + Name
  if (len >= 1){
    const uint8_t r = d[0];
    kv_hex(F("reason"), r, 2);
    if (const char* rn = nack_reason_name(r)) kv_s(F("reason_name"), String(rn));
  } else {
    kv_fs(F("reason"), F("?"));
  }

  // echo command
  if (len >= 2) kv_hex(F("cmd"), d[1], 2);

  // häufiges Layout: arg0[, port[, tool[, tail…]]]
  if (len >= 3){
    kv_hex(F("arg0"), d[2], 2);
  }
  if (len >= 4){
    kv_u(F("port"), d[3]);
  }
  if (len >= 5){
    const uint8_t tool = d[4];
    kv_u(F("tool"), tool);
    // optional Toolname je Familie
    const __FlashStringHelper* tn =
        (be==BK_HA)   ? ha_tool_name(tool) :
        (be==BK_SOLD || be==BK_SOLD1) ? tool_name_from_code(tool) : nullptr;
    if (tn) kv_fs(F("tool_name"), tn);
  }
  // alles darüber als Tail anzeigen
  if (len > 5){
    kv_s(F("tail"), hex_string(&d[5], (uint8_t)(len-5)));
  }

  Serial.println();
  return true;
}


// ---- decoders ----

static bool decode_devicename(Backend be, uint8_t ctrl, const uint8_t* d, uint8_t len){
  if (ctrl==SOLD_02::M_R_DEVICENAME || ctrl==SOLD_01::M_R_DEVICENAME ||
      ctrl==HA_02::M_R_DEVICENAME   || ctrl==FE_02::M_R_DEVICENAME   ||
      ctrl==PH_02::M_R_DEVICENAME   || ctrl==SF_02::M_R_DEVICENAME){
    if (len < 1) return false;
    String s = sanitize_ascii(d,len);
    if (s.length()==0) return false;
    print_hdr_line(be, F("M_R_DEVICENAME")); kv_s(F("name"), s); Serial.println();
    return true;
  }
   return false;
 }

static bool decode_firmware(Backend be, uint8_t ctrl, const uint8_t* d, uint8_t len){
  using namespace jbc_cmd;
  if (!(ctrl == BASE::M_FIRMWARE ||
        ctrl == SOLD_02::M_FIRMWARE || ctrl == SOLD_01::M_FIRMWARE ||
        ctrl == HA_02::M_FIRMWARE   || ctrl == PH_02::M_FIRMWARE  ||
        ctrl == FE_02::M_FIRMWARE   || ctrl == SF_02::M_FIRMWARE)) return false;


  String s = sanitize_ascii(d, len);

  // Erste Zeile: kompletter String (wie bisher)
  print_hdr_line(be, F("M_FIRMWARE")); 
  kv_s(F("string"), s);
  Serial.println();

  // Erwartetes Format: PROTO:MODELSTR:SW:HW
  int p1 = s.indexOf(':'), p2 = s.indexOf(':', p1+1), p3 = s.indexOf(':', p2+1);
  if (p1 > 0 && p2 > p1 && p3 > p2){
    String proto    = s.substring(0,   p1);
    String modelStr = s.substring(p1+1,p2);
    String sw       = s.substring(p2+1,p3);
    String hw       = s.substring(p3+1);

    // Hübsch: Proto / SW / HW
    print_hdr_line(be, F("PK/FW/HW"));
    kv_s(F("proto"), proto);
    kv_s(F("sw"),    sw);
    kv_s(F("hw"),    hw);
    Serial.println();

    // MODELSTR → Model / ModelType / ModelVersion
    int u1 = modelStr.indexOf('_');
    int u2 = (u1 >= 0) ? modelStr.indexOf('_', u1+1) : -1;
    if (u1 > 0 && u2 > u1){
      String model   = modelStr.substring(0,    u1);   // z.B. "DDE"
      String mtype   = modelStr.substring(u1+1, u2);   // z.B. "CAP26"
      String verStr  = modelStr.substring(u2+1);       // z.B. "06"
      uint32_t mver  = (uint32_t)verStr.toInt();       // → 6

      print_hdr_line(be, F("MODEL"));
      kv_s(F("name"), model);
      kv_s(F("type"), mtype);
      kv_u(F("ver"),  mver);
      Serial.println();
    } else {
      // Fallback: nur der Mittelteil als Name
      print_hdr_line(be, F("MODEL"));
      kv_s(F("name"), modelStr);
      Serial.println();
    }
    return true;
  }

  // Fallback für ältere/abweichende Strings: alter Zweizeiler
  int a = s.indexOf(':'), b = s.indexOf(':', a+1);
  if (a > 0 && b > a){
    String model = s.substring(a+1, b);
    print_hdr_line(be, F("MODEL")); kv_s(F("name"), model); Serial.println();
  }
  return true;
}


static bool decode_deviceid(Backend be, uint8_t ctrl, const uint8_t* d, uint8_t len){
  if (ctrl==SOLD_02::M_R_DEVICEID || ctrl==HA_02::M_R_DEVICEID ||
      ctrl==FE_02::M_R_DEVICEID   || ctrl==PH_02::M_R_DEVICEID ||
      ctrl==SF_02::M_R_DEVICEID){
    print_hdr_line(be, F("M_R_DEVICEID"));
    if(len==0){ kv_fs(F("text"), F("")); Serial.println(); return true; }
    String s = sanitize_ascii(d,len);
    if (s.length()) kv_s(F("text"), s); else kv_s(F("raw"), hex_string(d,len));
    Serial.println();
    return true;
  }
  return false;
}

// DEVICEIDORIGINAL – familienübergreifend
static bool decode_deviceid_original(Backend be, uint8_t ctrl, const uint8_t* d, uint8_t len){
  using namespace jbc_cmd;
  const bool match =
    ctrl == SOLD_02::M_R_DEVICEIDORIGINAL ||
    ctrl == HA_02::M_R_DEVICEIDORIGINAL   ||
    ctrl == PH_02::M_R_DEVICEIDORIGINAL   ||
    ctrl == FE_02::M_R_DEVICEIDORIGINAL   ||
    ctrl == SF_02::M_R_DEVICEIDORIGINAL;

  if(!match) return false;

  print_hdr_line(be, F("M_R_DEVICEIDORIGINAL"));
  if(len != 16){ Serial.print(F(" raw=")); print_hex(d,len); Serial.println(); return true; }

  // 4x u32 LE
  uint32_t w0 = u32le(&d[0]);
  uint32_t w1 = u32le(&d[4]);
  uint32_t w2 = u32le(&d[8]);
  uint32_t w3 = u32le(&d[12]);

  uint16_t vid = u16le(&d[0]);
  uint16_t pid = u16le(&d[2]);
  kv_u(F("vid"), vid);
  kv_u(F("pid"), pid);
  kv_hex(F("w0"), w0, 8);
  kv_hex(F("w1"), w1, 8);
  kv_hex(F("w2"), w2, 8);
  kv_hex(F("w3"), w3, 8);
  Serial.println();
  return true;
}

// FE_02/SF_02 haben kein INF_PORT
static bool decode_inf_port(Backend be, uint8_t ctrl, const uint8_t* d, uint8_t len){
  using namespace jbc_cmd;

  if (ctrl!=SOLD_02::M_INF_PORT && ctrl!=SOLD_01::M_INF_PORT &&
      ctrl!=HA_02::M_INF_PORT   && ctrl!=PH_02::M_INF_PORT) return false;

  // Einheitlicher Header: [FAM_INF_PORT] ... (einzeilig KV) 

  // ---- HOT AIR (HA_02) ----
  if (be == BK_HA) {
    if (len < 14) {
      print_hdr_line(be, F("M_INF_PORT")); Serial.print(F(" payload=")); Serial.print(len); Serial.print('B');
      Serial.print(F(" raw=")); print_hex(d,len); Serial.println();
      return true;
    }

    const uint8_t  tool        = d[0];
    const uint8_t  statusFlags = d[12];
    const uint16_t airTempUTI  = u16le(&d[2]);
    const uint16_t protTcUTI   = u16le(&d[4]);
    const uint16_t powerRaw    = u16le(&d[6]);   // x_mil → % = /10
    const uint16_t flowRaw     = u16le(&d[8]);   // x_mil → % = /10
    const uint16_t tts_ds      = u16le(&d[10]);  // Zehntelsekunden
    const bool     hasChanges  = (len >= 15);
    const uint8_t  changesMask = hasChanges ? d[len-1] : 0;
    print_hdr_line(be, F("M_INF_PORT"));
    if (const __FlashStringHelper* tn = ha_tool_name(tool)) kv_fs(F("tool"), tn); else kv_u(F("tool_code"), tool);
    kv_c(F("air_c"), uti_to_c(airTempUTI), 1);
    kv_c(F("prot_tc_c"), uti_to_c(protTcUTI), 1);
    { uint16_t v = powerRaw>1000?1000:powerRaw; kv_c(F("power_pct"), v/10.0f, 1); }
    { uint16_t v = flowRaw >1000?1000:flowRaw;  kv_c(F("flow_pct"),  v/10.0f, 1); }
    { const uint32_t total_s = tts_ds/10; const uint8_t t=tts_ds%10;
      char buf[8]; snprintf(buf, sizeof(buf), "%02u:%02u", (unsigned)(total_s/60), (unsigned)(total_s%60));
      String s(buf); if (t) { s += '.'; s += char('0'+t); } kv_s(F("tts"), s);
    }
    kv_hex(F("status"), statusFlags, 2);
    if (hasChanges) kv_hex(F("changes"), changesMask, 2);
    kv_s  (F("status_text"), ha_status_to_string(statusFlags));
    Serial.println();
    return true;
  }
    
  

  // ---- PREHEATER (PH_02) ----
  if (be == BK_PH) {
    if (len < 12) {
      print_hdr_line(be, F("M_INF_PORT")); Serial.print(F(" payload=")); Serial.print(len); Serial.print('B');
      Serial.print(F(" raw=")); print_hex(d,len); Serial.println();
      return true;
    }
    const uint8_t  tool      = d[0];
    const uint16_t tempUTI   = u16le(&d[2]);
    const uint16_t extTcUTI  = u16le(&d[4]);
    const uint16_t heaterRaw = u16le(&d[6]);
    const uint16_t powerRaw  = u16le(&d[8]);
    const uint8_t  flags8    = (len >= 11) ? d[10] : 0;

    print_hdr_line(be, F("M_INF_PORT"));
    if (const __FlashStringHelper* tn = tool_name_from_code(tool)) kv_fs(F("tool"), tn); else kv_u(F("tool_code"), tool);
    kv_c(F("temp_c"),  uti_to_c(tempUTI),1);
    kv_c(F("ext_tc_c"),uti_to_c(extTcUTI),1);
    kv_u(F("heater_raw"), heaterRaw);
    kv_u(F("power_raw"),  powerRaw);
    kv_hex(F("flags"), flags8, 2);
    Serial.println();
    return true;
  }

  // ---- SOLDER (SOLD_02/SOLD_01) ----
  if (len < 12) {
    Serial.print(F("  payload ")); Serial.print(len); Serial.println(F(" bytes (zu kurz)"));
    Serial.print(F("  raw: ")); print_hex(d,len); Serial.println();
    return true;
  }

  const uint8_t  tool    = d[0];
  const uint8_t  toolErr = d[1];              // Byte 1 = ToolError (V02)
  const uint16_t tip1UTI = u16le(&d[2]);
  const uint16_t tip2UTI = u16le(&d[4]);

  print_hdr_line(be, F("M_INF_PORT"));
  if (const __FlashStringHelper* tn = tool_name_from_code(tool)) kv_fs(F("tool"), tn); else kv_u(F("tool_code"), tool);
  kv_u(F("tool_err"), toolErr);
  if (const __FlashStringHelper* te = tool_error_name_fam(be, toolErr)) kv_fs(F("tool_err_name"), te);
  kv_c(F("tip1_c"), uti_to_c(tip1UTI), 1);
  kv_c(F("tip2_c"), uti_to_c(tip2UTI), 1); 

  const uint16_t pwr1_ppm = u16le(&d[6]);
  const uint16_t pwr2_ppm = u16le(&d[8]);
  const float pwr1_pct = (pwr1_ppm > 1000 ? 1000 : pwr1_ppm) / 10.0f;
  const float pwr2_pct = (pwr2_ppm > 1000 ? 1000 : pwr2_ppm) / 10.0f;
  kv_c(F("pwr1_pct"), pwr1_pct, 1);
  kv_c(F("pwr2_pct"), pwr2_pct, 1);

  if (be == BK_SOLD || be == BK_UNKNOWN) {
    if (len >= 11) {
      const uint8_t flags8 = d[10];
      kv_hex(F("flags"), flags8, 2);
      kv_s  (F("flags_bits"), sold_flags_to_string(flags8));
    }
    if (len >= 12) {
      const uint8_t changes = d[11];
      kv_hex(F("changes"), changes, 2);
      kv_s  (F("changes_bits"), sold_changes_to_string(changes));
    }
    Serial.println();
  } else if (be == BK_SOLD1) {
    if (len >= 12) {
      const uint8_t changes = d[11];
      kv_hex(F("changes"), changes, 2);
      kv_s  (F("changes_bits"), sold_changes_to_string(changes));
    }
    Serial.println();
  }

  return true;
}

// ---- andere Decoder ----

static bool decode_sold_statustool(Backend be, uint8_t ctrl, const uint8_t* d, uint8_t len){
  using namespace jbc_cmd;
  if ((ctrl==SOLD_02::M_R_STATUSTOOL || ctrl==SOLD_01::M_R_STATUSTOOL) && len>=1){
    const bool is16 = (len >= 2);
    const uint16_t v = is16 ? u16le(d) : d[0];

    print_hdr_line(be, F("M_R_STATUSTOOL"));
    kv_hex(F("mask"), v, is16 ? 4 : 2);

    if (v == 0){
      kv_fs(F("bits"), F("NONE"));
      Serial.println();
      return true;
    }

    if (is16){
      kv_s(F("bits"), sold_statustool_bits_to_string(v));   // <<< neu
    } else {
      kv_s(F("bits"), sold_flags_to_string(uint8_t(v)));    // 8-Bit Altfall
    }
    Serial.println();
    return true;
  }
  return false;
}



static bool decode_ha_statustool(Backend be, uint8_t ctrl, const uint8_t* d, uint8_t len){
  using namespace jbc_cmd;
  if (ctrl != HA_02::M_R_STATUSTOOL) return false;
  if (len == 0) return true;

  const uint16_t v = (len >= 2) ? u16le(d) : (uint16_t)d[0];
  const uint8_t  s = uint8_t(v & 0xFF);
  print_hdr_line(be, F("M_R_STATUSTOOL")); kv_hex(F("mask"), s, 2);
  String bits; struct { uint8_t bit; const char* name; } B[]={
    {1,"HEATER"},{2,"HEATER_REQUESTED"},{4,"COOLING"},{8,"SUCTION"},
    {16,"SUCTION_REQUESTED"},{32,"PEDAL_CONNECTED"},{64,"PEDAL_PRESSED"},{128,"STAND"}
  };
  for (auto &b:B){ if (s & b.bit){ if(bits.length()) bits+='|'; bits+=b.name; } }
  if(bits.length()) kv_s(F("bits"), bits);
  Serial.println();
  return true;
}


// konservativ: häufige u16-Reads
static bool decode_common_u16(Backend be, uint8_t ctrl, const uint8_t* d, uint8_t len){
  if(len<2) return false;

  // SELECTTEMP (read, UTI)
  if ((be==BK_SOLD || be==BK_SOLD1) && (ctrl==SOLD_02::M_R_SELECTTEMP || ctrl==SOLD_01::M_R_SELECTTEMP)) {
    uint16_t v=u16le(d);
    print_hdr_line(be, F("M_R_SELECTTEMP")); kv_c(F("c"), uti_to_c(v),1); kv_hex(F("uti"), v, 4); Serial.println();
    return true;
  }
  if (be==BK_HA && ctrl==HA_02::M_R_SELECTTEMP) {
    uint16_t v=u16le(d);
    print_hdr_line(be, F("M_R_SELECTTEMP")); kv_c(F("c"), uti_to_c(v),1); kv_hex(F("uti"), v, 4); Serial.println();
    return true;
  }
  if (be==BK_PH && ctrl==PH_02::M_R_SELECTTEMP) {
    uint16_t v=u16le(d);
    print_hdr_line(be, F("M_R_SELECTTEMP")); kv_c(F("c"), uti_to_c(v),1); kv_hex(F("uti"), v, 4); Serial.println();
    return true;
  }

  // TIPTEMP (Solder)
  if ((be==BK_SOLD || be==BK_SOLD1) &&
      (ctrl==SOLD_02::M_R_TIPTEMP || ctrl==SOLD_01::M_R_TIPTEMP)){
    uint16_t v=u16le(d);
    print_hdr_line(be, F("M_R_TIPTEMP")); kv_c(F("c"), uti_to_c(v),1); kv_hex(F("uti"), v, 4); Serial.println();
    return true;
  }

  // POWER (roh)
  if ((be==BK_SOLD || be==BK_SOLD1) &&
      (ctrl==SOLD_02::M_R_POWER || ctrl==SOLD_01::M_R_POWER)){
    uint16_t v=u16le(d);
    print_hdr_line(be, F("M_R_POWER")); kv_u(F("raw"), v); Serial.println();
    return true;
  }

  // CURRENT (roh)
  if ((be==BK_SOLD || be==BK_SOLD1) &&
      (ctrl==SOLD_02::M_R_CURRENT || ctrl==SOLD_01::M_R_CURRENT)){
    uint16_t v=u16le(d);
    print_hdr_line(be, F("M_R_CURRENT")); kv_u(F("raw"), v); Serial.println();
    return true;
  }

  // AIRTEMP (HA)
  if (be==BK_HA && ctrl==HA_02::M_R_AIRTEMP){
    uint16_t v=u16le(d);
    print_hdr_line(be, F("M_R_AIRTEMP")); kv_c(F("c"), uti_to_c(v),1); kv_hex(F("uti"), v, 4); Serial.println();
    return true;
  }

  // EXTTCTEMP (HA + PH)
  if (be==BK_HA && ctrl==HA_02::M_R_EXTTCTEMP){
    uint16_t v=u16le(d);
    print_hdr_line(be, F("M_R_EXTTCTEMP")); kv_c(F("c"), uti_to_c(v),1); kv_hex(F("uti"), v, 4); Serial.println();
    return true;
  }
  if (be==BK_PH && ctrl==PH_02::M_R_EXTTCTEMP){
    uint16_t v=u16le(d);
    print_hdr_line(be, F("M_R_EXTTCTEMP")); kv_c(F("c"), uti_to_c(v),1); kv_hex(F("uti"), v, 4); Serial.println();
    return true;
  }

  // SELECTPOWER (PH) → %
  if (be==BK_PH && ctrl==PH_02::M_R_SELECTPOWER){
    uint16_t ppm=u16le(d); if(ppm>1000) ppm=1000;
    print_hdr_line(be, F("M_R_SELECTPOWER")); kv_c(F("pct"), ppm/10.0f, 1); kv_u(F("raw"), ppm); Serial.println();
    return true;
  }

  return false;
}

// --- dispatcher API ---
static bool decode_payload_and_print(Backend be, uint8_t ctrl, const uint8_t* d, uint8_t len){
  if (!g_log_show_syn && is_syn_ctrl(ctrl)) return true;

  // zuerst NACK/ACK behandeln
  if (decode_nack(be, ctrl, d, len))            return true;
  if (decode_write_acks(be, ctrl, d, len))  return true;

  // generisch (familienübergreifend)
  if (decode_firmware(be, ctrl,d,len))             return true;
  if (decode_devicename(be, ctrl,d,len))           return true;
  if (decode_deviceid_original(be, ctrl,d,len))    return true;
  if (decode_deviceid(be, ctrl,d,len))             return true;
  if (decode_inf_port(be, ctrl, d, len))       return true;
  if (decode_connecttool(be, ctrl, d, len))    return true;

  // NEU: Einmalige, familienübergreifende Read-Ausgaben (TEMPUNIT/LANGUAGE/USB/PIN/…)
  if (decode_crossfamily_reads(be, ctrl, d, len)) return true;

  // family-gated
  switch(be){
    case BK_SOLD:
    case BK_SOLD1:
    case BK_UNKNOWN:
      if (decode_sold_statustool(be, ctrl,d,len)) return true;
      if (decode_sold_counters(be, ctrl,d,len))   return true;
      if (decode_sold_extras(be, ctrl,d,len))     return true;
      break;

    case BK_HA:
      if (decode_ha_statustool(be, ctrl,d,len))   return true;
      if (decode_ha_counters(be, ctrl,d,len))     return true;
      if (decode_ha_extras(be, ctrl,d,len))       return true;
      break;

    case BK_PH:
      if (decode_ph_extras(be, ctrl,d,len))       return true;
      break;

    case BK_FE:
      if (decode_fe_extras(be, ctrl,d,len))       return true;
      break;

    case BK_SF:
      if (decode_sf_extras(be, ctrl,d,len))       return true;
      break;
  }

  // generische u16
  if (decode_common_u16(be, ctrl, d, len)) return true;

  return false;
}

static bool print(Backend be, uint8_t ctrl, const uint8_t* d, uint8_t len){
  if (decode_payload_and_print(be, ctrl, d, len)) return true;
  if (!g_log_show_syn && is_syn_ctrl(ctrl)) return true;
  print_hdr_tag(F("PAYLOAD")); Serial.print(len); Serial.print(F(" bytes: "));
  print_hex(d,len); Serial.println();
  return false;
}

} // namespace jbc_decode
