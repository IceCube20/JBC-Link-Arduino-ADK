// SPDX-License-Identifier: MIT OR GPL-2.0-only

#pragma once
#include <Arduino.h>
#include "jbc_commands_full.h"
#include "jbc_cmd_names.h"   // Backend

namespace jbc_cli {

// -------- Argument-Formate --------
enum ArgFmt : uint8_t {
  FMT_NONE = 0,        // keine Payload
  FMT_PORT,            // u8: Port
  FMT_PORT_TOOL,
  FMT_U8,              // u8
  FMT_U16,             // u16 (LE)
  FMT_TEMP,            // u16 (°C → UTI)

  // Mehrfachwerte (nur values, ohne Port-first Varianten)
  FMT_U16_U16,         // zwei u16 (LE)
  FMT_TEMP_TEMP,       // zwei Temps (°C → UTI)

  // Strings / Rohbytes / IP-Config
  FMT_STRING,          // ASCII (alles nach dem Cmd als Bytes)
  FMT_RAWHEX,          // Folge von Zahlen (0x.. oder dezimal) → Bytes
  FMT_IPCFG,           // dhcp ip mask gw [dns] [port] → (u8 + 4+4+4+4 + u16)

  // -------- WRITE-Formate (CLI: port-first, Wire: values-first) --------
  FMT_W_PORT_U8,        // CLI:  <port> <u8>           → Wire: <u8> <port>
  FMT_W_PORT_U16,       // CLI:  <port> <u16>          → Wire: <u16_lo> <u16_hi> <port>
  FMT_W_PORT_TEMP,      // CLI:  <port> <°C>           → Wire: <UTI_lo> <UTI_hi> <port>
  FMT_W_PORT_3TEMP,     // CLI:  <port> <t1> <t2> <t3> → Wire: <UTI1> <UTI2> <UTI3> <port>

  // Optional: Tool am Ende
  FMT_W_PORT_TOOL_U8,        // CLI:  <port> <tool> <u8>          → Wire: <u8> <port> <tool>
  FMT_W_PORT_TOOL_U16,       // CLI:  <port> <tool> <u16>         → Wire: <u16> <port> <tool>
  FMT_W_PORT_TOOL_TEMP,      // CLI:  <port> <tool> <°C>          → Wire: <UTI> <port> <tool>
  FMT_W_PORT_TOOL_3TEMP,     // CLI:  <port> <tool> <t1> <t2> <t3>→ Wire: <UTI1> <UTI2> <UTI3> <port> <tool>

  // ----- SOLD_02-spezifisch -----
  FMT_W_PORT_TOOL_U8_U8,     // CLI:  <port> <tool> <value> <onoff> → Wire: <value> <onoff> <port> <tool>
  FMT_W_LEVELSTEMPS_S02,     // CLI:  <port> <tool> <onoff> <sel> <l1_on> <l1°C> <l2_on> <l2°C> <l3_on> <l3°C>
                              // Wire: onoff,sel,l1_on,L1_UTI,l2_on,L2_UTI,l3_on,L3_UTI,port,tool
  FMT_W_PORT_TOOL_TEMPDELTA,  // CLI:  <port> <tool> <Δ°C> → Wire: (int16)(Δ°C*9), port, tool

  // ----- NEU: HA DateTime -----
  FMT_W_DT6                  // CLI: yyyy mm dd HH MM SS → Wire: yearLE,mon,day,hh,mm,ss
};

// Was die Map zurückliefert
struct ConsoleCmd {
  uint8_t ctrl = 0;
  ArgFmt  fmt  = FMT_NONE;
};

// ======================================================
//                       MAPPER
// ======================================================

// -------- SOLD_02 --------
static bool map_sold02(const String& U, ConsoleCmd& out){
  using namespace jbc_cmd;

  // Reads
  if(U.equals(F("M_R_DEVICEIDORIGINAL"))){     out.ctrl=SOLD_02::M_R_DEVICEIDORIGINAL;    out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_DISCOVER"))){             out.ctrl=SOLD_02::M_R_DISCOVER;            out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_DEVICEID"))){             out.ctrl=SOLD_02::M_R_DEVICEID;            out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_FIRMWARE"))){               out.ctrl=SOLD_02::M_FIRMWARE;              out.fmt=FMT_NONE;   return true; }

  if(U.equals(F("M_INF_PORT"))){               out.ctrl=SOLD_02::M_INF_PORT;              out.fmt=FMT_PORT;   return true; } 
  if(U.equals(F("M_R_LEVELSTEMPS"))){          out.ctrl=SOLD_02::M_R_LEVELSTEMPS;         out.fmt=FMT_PORT_TOOL;   return true; } // M_R_LEVELSTEMPS <PORT> <TOOL>  Port 0-3 Tool z.B. T210, T245, DR usw.
  if(U.equals(F("M_R_SLEEPDELAY"))){           out.ctrl=SOLD_02::M_R_SLEEPDELAY;          out.fmt=FMT_PORT_TOOL;   return true; }
  if(U.equals(F("M_R_SLEEPTEMP"))){            out.ctrl=SOLD_02::M_R_SLEEPTEMP;           out.fmt=FMT_PORT_TOOL;   return true; }
  if(U.equals(F("M_R_HIBERDELAY"))){           out.ctrl=SOLD_02::M_R_HIBERDELAY;          out.fmt=FMT_PORT_TOOL;   return true; }
  if(U.equals(F("M_R_AJUSTTEMP"))){            out.ctrl=SOLD_02::M_R_AJUSTTEMP;           out.fmt=FMT_PORT_TOOL;   return true; }
  if(U.equals(F("M_R_CARTRIDGE"))){            out.ctrl=SOLD_02::M_R_CARTRIDGE;           out.fmt=FMT_PORT_TOOL;   return true; }
  if(U.equals(F("M_R_SELECTTEMP"))){           out.ctrl=SOLD_02::M_R_SELECTTEMP;          out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_TIPTEMP"))){              out.ctrl=SOLD_02::M_R_TIPTEMP;             out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_CURRENT"))){              out.ctrl=SOLD_02::M_R_CURRENT;             out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_POWER"))){                out.ctrl=SOLD_02::M_R_POWER;               out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_CONNECTTOOL"))){          out.ctrl=SOLD_02::M_R_CONNECTTOOL;         out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_TOOLERROR"))){            out.ctrl=SOLD_02::M_R_TOOLERROR;           out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_STATUSTOOL"))){           out.ctrl=SOLD_02::M_R_STATUSTOOL;          out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_MOSTEMP"))){              out.ctrl=SOLD_02::M_R_MOSTEMP;             out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_DELAYTIME"))){            out.ctrl=SOLD_02::M_R_DELAYTIME;           out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_REMOTEMODE"))){           out.ctrl=SOLD_02::M_R_REMOTEMODE;          out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_CONTIMODE"))){            out.ctrl=SOLD_02::M_R_CONTIMODE;           out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_I_CONTIMODE"))){            out.ctrl=SOLD_02::M_I_CONTIMODE;           out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_ALARMMAXTEMP"))){         out.ctrl=SOLD_02::M_R_ALARMMAXTEMP;        out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_ALARMMINTEMP"))){         out.ctrl=SOLD_02::M_R_ALARMMINTEMP;        out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_ALARMTEMP"))){            out.ctrl=SOLD_02::M_R_ALARMTEMP;           out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_LOCK_PORT"))){            out.ctrl=SOLD_02::M_R_LOCK_PORT;           out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_QST_ACTIVATE"))){         out.ctrl=SOLD_02::M_R_QST_ACTIVATE;        out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_QST_STATUS"))){           out.ctrl=SOLD_02::M_R_QST_STATUS;          out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_TEMPUNIT"))){             out.ctrl=SOLD_02::M_R_TEMPUNIT;            out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_MAXTEMP"))){              out.ctrl=SOLD_02::M_R_MAXTEMP;             out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_MINTEMP"))){              out.ctrl=SOLD_02::M_R_MINTEMP;             out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_POWERLIM"))){             out.ctrl=SOLD_02::M_R_POWERLIM;            out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_PIN"))){                  out.ctrl=SOLD_02::M_R_PIN;                 out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_STATERROR"))){            out.ctrl=SOLD_02::M_R_STATERROR;           out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_TRAFOTEMP"))){            out.ctrl=SOLD_02::M_R_TRAFOTEMP;           out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_DEVICENAME"))){           out.ctrl=SOLD_02::M_R_DEVICENAME;          out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_BEEP"))){                 out.ctrl=SOLD_02::M_R_BEEP;                out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_LANGUAGE"))){             out.ctrl=SOLD_02::M_R_LANGUAGE;            out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_TYPEOFGROUND"))){         out.ctrl=SOLD_02::M_R_TYPEOFGROUND;        out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_PLUGTIME"))){             out.ctrl=SOLD_02::M_R_PLUGTIME;            out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_WORKTIME"))){             out.ctrl=SOLD_02::M_R_WORKTIME;            out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_SLEEPTIME"))){            out.ctrl=SOLD_02::M_R_SLEEPTIME;           out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_HIBERTIME"))){            out.ctrl=SOLD_02::M_R_HIBERTIME;           out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_NOTOOLTIME"))){           out.ctrl=SOLD_02::M_R_NOTOOLTIME;          out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_SLEEPCYCLES"))){          out.ctrl=SOLD_02::M_R_SLEEPCYCLES;         out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_DESOLCYCLES"))){          out.ctrl=SOLD_02::M_R_DESOLCYCLES;         out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_PLUGTIMEP"))){            out.ctrl=SOLD_02::M_R_PLUGTIMEP;           out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_WORKTIMEP"))){            out.ctrl=SOLD_02::M_R_WORKTIMEP;           out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_SLEEPTIMEP"))){           out.ctrl=SOLD_02::M_R_SLEEPTIMEP;          out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_HIBERTIMEP"))){           out.ctrl=SOLD_02::M_R_HIBERTIMEP;          out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_NOTOOLTIMEP"))){          out.ctrl=SOLD_02::M_R_NOTOOLTIMEP;         out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_SLEEPCYCLESP"))){         out.ctrl=SOLD_02::M_R_SLEEPCYCLESP;        out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_DESOLCYCLESP"))){         out.ctrl=SOLD_02::M_R_DESOLCYCLESP;        out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_USB_CONNECTSTATUS"))){    out.ctrl=SOLD_02::M_R_USB_CONNECTSTATUS;   out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_ETH_TCPIPCONFIG"))){      out.ctrl=SOLD_02::M_R_ETH_TCPIPCONFIG;     out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_ETH_CONNECTSTATUS"))){    out.ctrl=SOLD_02::M_R_ETH_CONNECTSTATUS;   out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_RBT_CONNCONFIG"))){       out.ctrl=SOLD_02::M_R_RBT_CONNCONFIG;      out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_RBT_CONNECTSTATUS"))){    out.ctrl=SOLD_02::M_R_RBT_CONNECTSTATUS;   out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_PERIPHCOUNT"))){          out.ctrl=SOLD_02::M_R_PERIPHCOUNT;         out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_PERIPHCONFIG"))){         out.ctrl=SOLD_02::M_R_PERIPHCONFIG;        out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_PERIPHSTATUS"))){         out.ctrl=SOLD_02::M_R_PERIPHSTATUS;        out.fmt=FMT_PORT;   return true; }
  
  // Writes (values-first auf dem Draht)
  if(U.equals(F("M_W_DEVICEID"))){             out.ctrl=SOLD_02::M_W_DEVICEID;            out.fmt=FMT_RAWHEX;                 return true; }
  if(U.equals(F("M_W_LEVELSTEMPS"))){          out.ctrl=SOLD_02::M_W_LEVELSTEMPS;         out.fmt=FMT_W_LEVELSTEMPS_S02;      return true; } // M_W_LEVELSTEMPS <PORT> <TOOL> <1/0 Levelsstemps> <SelL1-L3> <L1 1/0> <L1 TEMP> <L2 1/0> <L1 TEMP> <L3 1/0> <L3 TEMP> Beispiel: M_W_LEVELSTEMPS 0 T245 1 0 1 350 1 320 1 280
  if(U.equals(F("M_W_SLEEPDELAY"))){           out.ctrl=SOLD_02::M_W_SLEEPDELAY;          out.fmt=FMT_W_PORT_TOOL_U8_U8;      return true; }
  if(U.equals(F("M_W_SLEEPTEMP"))){            out.ctrl=SOLD_02::M_W_SLEEPTEMP;           out.fmt=FMT_W_PORT_TOOL_TEMP;       return true; }
  if(U.equals(F("M_W_HIBERDELAY"))){           out.ctrl=SOLD_02::M_W_HIBERDELAY;          out.fmt=FMT_W_PORT_TOOL_U8_U8;      return true; }
  if(U.equals(F("M_W_AJUSTTEMP"))){            out.ctrl=SOLD_02::M_W_AJUSTTEMP;           out.fmt=FMT_W_PORT_TOOL_TEMPDELTA;  return true; } // Δ°C (signed)
  if(U.equals(F("M_W_CARTRIDGE"))){            out.ctrl=SOLD_02::M_W_CARTRIDGE;           out.fmt=FMT_RAWHEX;                 return true; } 
  if(U.equals(F("M_W_SELECTTEMP"))){           out.ctrl=SOLD_02::M_W_SELECTTEMP;          out.fmt=FMT_W_PORT_TEMP;            return true; }
  if(U.equals(F("M_W_SELECTTEMPVOLATILE"))){   out.ctrl=SOLD_02::M_W_SELECTTEMPVOLATILE;  out.fmt=FMT_W_PORT_TEMP;            return true; }
  if(U.equals(F("M_W_REMOTEMODE"))){           out.ctrl=SOLD_02::M_W_REMOTEMODE;          out.fmt=FMT_U8;                     return true; } // 0=OFF,1=ON
  if(U.equals(F("M_W_CONTIMODE"))){            out.ctrl=SOLD_02::M_W_CONTIMODE;           out.fmt=FMT_U8;                     return true; } // 0=OFF,1=ON
  if(U.equals(F("M_W_ALARMMAXTEMP"))){         out.ctrl=SOLD_02::M_W_ALARMMAXTEMP;        out.fmt=FMT_W_PORT_TEMP;                   return true; }
  if(U.equals(F("M_W_ALARMMINTEMP"))){         out.ctrl=SOLD_02::M_W_ALARMMINTEMP;        out.fmt=FMT_W_PORT_TEMP;                   return true; }
  if(U.equals(F("M_W_LOCK_PORT"))){            out.ctrl=SOLD_02::M_W_LOCK_PORT;           out.fmt=FMT_W_PORT_U8;              return true; }
  if(U.equals(F("M_W_QST_ACTIVATE"))){         out.ctrl=SOLD_02::M_W_QST_ACTIVATE;        out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_QST_STATUS"))){           out.ctrl=SOLD_02::M_W_QST_STATUS;          out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_TEMPUNIT"))){             out.ctrl=SOLD_02::M_W_TEMPUNIT;            out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_MAXTEMP"))){              out.ctrl=SOLD_02::M_W_MAXTEMP;             out.fmt=FMT_TEMP;                   return true; }
  if(U.equals(F("M_W_MINTEMP"))){              out.ctrl=SOLD_02::M_W_MINTEMP;             out.fmt=FMT_TEMP;                   return true; }
  if(U.equals(F("M_W_POWERLIM"))){             out.ctrl=SOLD_02::M_W_POWERLIM;            out.fmt=FMT_U16;                    return true; }
  if(U.equals(F("M_W_PIN"))){                  out.ctrl=SOLD_02::M_W_PIN;                 out.fmt=FMT_STRING;                 return true; }
  if(U.equals(F("M_W_DEVICENAME"))){           out.ctrl=SOLD_02::M_W_DEVICENAME;          out.fmt=FMT_STRING;                 return true; }
  if(U.equals(F("M_W_BEEP"))){                 out.ctrl=SOLD_02::M_W_BEEP;                out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_LANGUAGE"))){             out.ctrl=SOLD_02::M_W_LANGUAGE;            out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_TYPEOFGROUND"))){         out.ctrl=SOLD_02::M_W_TYPEOFGROUND;        out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_USB_CONNECTSTATUS"))){    out.ctrl=SOLD_02::M_W_USB_CONNECTSTATUS;   out.fmt=FMT_STRING;                 return true; }
  if(U.equals(F("M_W_ETH_TCPIPCONFIG"))){      out.ctrl=SOLD_02::M_W_ETH_TCPIPCONFIG;     out.fmt=FMT_IPCFG;                  return true; }
  if(U.equals(F("M_W_ETH_CONNECTSTATUS"))){    out.ctrl=SOLD_02::M_W_ETH_CONNECTSTATUS;   out.fmt=FMT_STRING;                 return true; }
  // Noch nicht Getestet!
  if(U.equals(F("M_W_PERIPHCONFIG"))){         out.ctrl=SOLD_02::M_W_PERIPHCONFIG;        out.fmt=FMT_RAWHEX;                 return true; }
  if(U.equals(F("M_W_PERIPHSTATUS"))){         out.ctrl=SOLD_02::M_W_PERIPHSTATUS;        out.fmt=FMT_RAWHEX;                 return true; }
  if(U.equals(F("M_W_RESETCOUNTERS"))){        out.ctrl=SOLD_02::M_W_RESETCOUNTERS;       out.fmt=FMT_NONE;                   return true; }

  return false;
}


// -------- SOLD_01 --------
static bool map_sold01(const String& U, ConsoleCmd& out){
  using namespace jbc_cmd;

  // Reads
  if(U.equals(F("M_R_CONNECTSTATUS"))){        out.ctrl=SOLD_01::M_R_CONNECTSTATUS;       out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_FIRMWARE"))){               out.ctrl=SOLD_01::M_FIRMWARE;              out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_INF_PORT"))){               out.ctrl=SOLD_01::M_INF_PORT;              out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_FIXTEMP"))){              out.ctrl=SOLD_01::M_R_FIXTEMP;             out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_LEVEL1"))){               out.ctrl=SOLD_01::M_R_LEVEL1;              out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_LEVEL2"))){               out.ctrl=SOLD_01::M_R_LEVEL2;              out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_LEVEL3"))){               out.ctrl=SOLD_01::M_R_LEVEL3;              out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_SLEEPDELAY"))){           out.ctrl=SOLD_01::M_R_SLEEPDELAY;          out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_SLEEPTEMP"))){            out.ctrl=SOLD_01::M_R_SLEEPTEMP;           out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_HIBERDELAY"))){           out.ctrl=SOLD_01::M_R_HIBERDELAY;          out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_AJUSTTEMP"))){            out.ctrl=SOLD_01::M_R_AJUSTTEMP;           out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_SELECTTEMP"))){           out.ctrl=SOLD_01::M_R_SELECTTEMP;          out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_TIPTEMP"))){              out.ctrl=SOLD_01::M_R_TIPTEMP;             out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_CURRENT"))){              out.ctrl=SOLD_01::M_R_CURRENT;             out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_POWER"))){                out.ctrl=SOLD_01::M_R_POWER;               out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_CONNECTTOOL"))){          out.ctrl=SOLD_01::M_R_CONNECTTOOL;         out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_TOOLERROR"))){            out.ctrl=SOLD_01::M_R_TOOLERROR;           out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_STATUSTOOL"))){           out.ctrl=SOLD_01::M_R_STATUSTOOL;          out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_MOSTEMP"))){              out.ctrl=SOLD_01::M_R_MOSTEMP;             out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_DELAYTIME"))){            out.ctrl=SOLD_01::M_R_DELAYTIME;           out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_REMOTEMODE"))){           out.ctrl=SOLD_01::M_R_REMOTEMODE;          out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_STATUSREMOTEMODE"))){     out.ctrl=SOLD_01::M_R_STATUSREMOTEMODE;    out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_CONTIMODE"))){            out.ctrl=SOLD_01::M_R_CONTIMODE;           out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_I_CONTIMODE"))){            out.ctrl=SOLD_01::M_I_CONTIMODE;           out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_TEMPUNIT"))){             out.ctrl=SOLD_01::M_R_TEMPUNIT;            out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_MAXTEMP"))){              out.ctrl=SOLD_01::M_R_MAXTEMP;             out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_MINTEMP"))){              out.ctrl=SOLD_01::M_R_MINTEMP;             out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_NITROMODE"))){            out.ctrl=SOLD_01::M_R_NITROMODE;           out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_HELPTEXT"))){             out.ctrl=SOLD_01::M_R_HELPTEXT;            out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_POWERLIM"))){             out.ctrl=SOLD_01::M_R_POWERLIM;            out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_PIN"))){                  out.ctrl=SOLD_01::M_R_PIN;                 out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_STATERROR"))){            out.ctrl=SOLD_01::M_R_STATERROR;           out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_TRAFOTEMP"))){            out.ctrl=SOLD_01::M_R_TRAFOTEMP;           out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_DEVICENAME"))){           out.ctrl=SOLD_01::M_R_DEVICENAME;          out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_BEEP"))){                 out.ctrl=SOLD_01::M_R_BEEP;                out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_LANGUAGE"))){             out.ctrl=SOLD_01::M_R_LANGUAGE;            out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_TEMPERRORTRAFO"))){       out.ctrl=SOLD_01::M_R_TEMPERRORTRAFO;      out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_TEMPERRORMOS"))){         out.ctrl=SOLD_01::M_R_TEMPERRORMOS;        out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_DEVICEID"))){             out.ctrl=SOLD_01::M_R_DEVICEID;            out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_PLUGTIME"))){             out.ctrl=SOLD_01::M_R_PLUGTIME;            out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_WORKTIME"))){             out.ctrl=SOLD_01::M_R_WORKTIME;            out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_SLEEPTIME"))){            out.ctrl=SOLD_01::M_R_SLEEPTIME;           out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_HIBERTIME"))){            out.ctrl=SOLD_01::M_R_HIBERTIME;           out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_NOTOOLTIME"))){           out.ctrl=SOLD_01::M_R_NOTOOLTIME;          out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_SLEEPCYCLES"))){          out.ctrl=SOLD_01::M_R_SLEEPCYCLES;         out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_DESOLCYCLES"))){          out.ctrl=SOLD_01::M_R_DESOLCYCLES;         out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_QST_ACTIVATE"))){         out.ctrl=SOLD_01::M_R_QST_ACTIVATE;        out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_QST_STATUS"))){           out.ctrl=SOLD_01::M_R_QST_STATUS;          out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_LOCK_PORT"))){            out.ctrl=SOLD_01::M_R_LOCK_PORT;           out.fmt=FMT_PORT;   return true; }
  

  // Writes (values-first)
  if(U.equals(F("M_W_CONNECTSTATUS"))){        out.ctrl=SOLD_01::M_W_CONNECTSTATUS;       out.fmt=FMT_STRING;                 return true; }
  if(U.equals(F("M_W_FIXTEMP"))){              out.ctrl=SOLD_01::M_W_FIXTEMP;             out.fmt=FMT_W_PORT_TEMP;            return true; }
  if(U.equals(F("M_W_LEVELTEMP"))){            out.ctrl=SOLD_01::M_W_LEVELTEMP;           out.fmt=FMT_W_PORT_TEMP;            return true; }
  if(U.equals(F("M_W_LEVEL1"))){               out.ctrl=SOLD_01::M_W_LEVEL1;              out.fmt=FMT_W_PORT_TEMP;            return true; }
  if(U.equals(F("M_W_LEVEL2"))){               out.ctrl=SOLD_01::M_W_LEVEL2;              out.fmt=FMT_W_PORT_TEMP;            return true; }
  if(U.equals(F("M_W_LEVEL3"))){               out.ctrl=SOLD_01::M_W_LEVEL3;              out.fmt=FMT_W_PORT_TEMP;            return true; }
  if(U.equals(F("M_W_SLEEPDELAY"))){           out.ctrl=SOLD_01::M_W_SLEEPDELAY;          out.fmt=FMT_W_PORT_U16;             return true; }
  if(U.equals(F("M_W_SLEEPTEMP"))){            out.ctrl=SOLD_01::M_W_SLEEPTEMP;           out.fmt=FMT_W_PORT_TEMP;            return true; }
  if(U.equals(F("M_W_HIBERDELAY"))){           out.ctrl=SOLD_01::M_W_HIBERDELAY;          out.fmt=FMT_W_PORT_U16;             return true; }
  if(U.equals(F("M_W_AJUSTTEMP"))){            out.ctrl=SOLD_01::M_W_AJUSTTEMP;           out.fmt=FMT_W_PORT_TOOL_TEMPDELTA;  return true; } // Δ°C (signed)
  if(U.equals(F("M_W_SELECTTEMP"))){           out.ctrl=SOLD_01::M_W_SELECTTEMP;          out.fmt=FMT_W_PORT_TEMP;            return true; }
  if(U.equals(F("M_W_REMOTEMODE"))){           out.ctrl=SOLD_01::M_W_REMOTEMODE;          out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_CONTIMODE"))){            out.ctrl=SOLD_01::M_W_CONTIMODE;           out.fmt=FMT_U8;                     return true; } // 0=OFF,1=ON
  if(U.equals(F("M_W_DEVICEID"))){             out.ctrl=SOLD_01::M_W_DEVICEID;            out.fmt=FMT_RAWHEX;                 return true; }
  if(U.equals(F("M_W_TEMPUNIT"))){             out.ctrl=SOLD_01::M_W_TEMPUNIT;            out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_MAXTEMP"))){              out.ctrl=SOLD_01::M_R_MAXTEMP;             out.fmt=FMT_TEMP;                   return true; }
  if(U.equals(F("M_W_MINTEMP"))){              out.ctrl=SOLD_01::M_R_MINTEMP;             out.fmt=FMT_TEMP;                   return true; }
  if(U.equals(F("M_W_NITROMODE"))){            out.ctrl=SOLD_01::M_W_NITROMODE;           out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_HELPTEXT"))){             out.ctrl=SOLD_01::M_W_HELPTEXT;            out.fmt=FMT_STRING;                 return true; } 
  if(U.equals(F("M_W_POWERLIM"))){             out.ctrl=SOLD_01::M_W_POWERLIM;            out.fmt=FMT_U16;                    return true; }
  if(U.equals(F("M_W_PIN"))){                  out.ctrl=SOLD_01::M_W_PIN;                 out.fmt=FMT_STRING;                 return true; }
  if(U.equals(F("M_W_DEVICENAME"))){           out.ctrl=SOLD_01::M_W_DEVICENAME;          out.fmt=FMT_STRING;                 return true; }
  if(U.equals(F("M_W_BEEP"))){                 out.ctrl=SOLD_01::M_W_BEEP;                out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_LANGUAGE"))){             out.ctrl=SOLD_01::M_W_LANGUAGE;            out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_QST_ACTIVATE"))){         out.ctrl=SOLD_01::M_W_QST_ACTIVATE;        out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_QST_STATUS"))){           out.ctrl=SOLD_01::M_W_QST_STATUS;          out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_LOCK_PORT"))){            out.ctrl=SOLD_01::M_W_LOCK_PORT;           out.fmt=FMT_W_PORT_U8;              return true; } 

  return false;
}

// -------- HA_02 --------
static bool map_ha02(const String& U, ConsoleCmd& out){
  using namespace jbc_cmd;

  // Reads
  if(U.equals(F("M_R_DEVICEIDORIGINAL"))){     out.ctrl=HA_02::M_R_DEVICEIDORIGINAL;    out.fmt=FMT_NONE;        return true; } // Nicht Implementiert
  if(U.equals(F("M_R_DISCOVER"))){             out.ctrl=HA_02::M_R_DISCOVER;            out.fmt=FMT_NONE;        return true; } // Liefert ein nur ACK 
  if(U.equals(F("M_R_DEVICEID"))){             out.ctrl=HA_02::M_R_DEVICEID;            out.fmt=FMT_NONE;        return true; } // JTSE hat keine, wird von der JCB Software zugewiesen
  if(U.equals(F("M_FIRMWARE"))){               out.ctrl=HA_02::M_FIRMWARE;              out.fmt=FMT_NONE;        return true; } // Liefert Model und Firmwaredaten
  if(U.equals(F("M_INF_PORT"))){               out.ctrl=HA_02::M_INF_PORT;              out.fmt=FMT_PORT;        return true; } // M_INF_PORT <PORT> liefert viele wichtige Daten über den Port 
  if(U.equals(F("M_R_PROFILEMODE"))){          out.ctrl=HA_02::M_R_PROFILEMODE;         out.fmt=FMT_PORT;        return true; } // M_R_PROFILEMODE <PORT> Profilmode ON/OFF
  if(U.equals(F("M_R_HEATERSTATUS"))){         out.ctrl=HA_02::M_R_HEATERSTATUS;        out.fmt=FMT_PORT;        return true; } // M_R_HEATERSTATUS <PORT> Heater ON/OFF
  if(U.equals(F("M_R_SUCTIONSTATUS"))){        out.ctrl=HA_02::M_R_SUCTIONSTATUS;       out.fmt=FMT_PORT;        return true; } // M_R_SUCTIONSTATUS <PORT> Suction 1/0
  if(U.equals(F("M_R_EXTTCMODE"))){            out.ctrl=HA_02::M_R_EXTTCMODE;           out.fmt=FMT_PORT_TOOL;   return true; } // M_R_EXTTCMODE <PORT> <TOOL> liefert den Status in On Off
  if(U.equals(F("M_R_LEVELSTEMPS"))){          out.ctrl=HA_02::M_R_LEVELSTEMPS;         out.fmt=FMT_PORT_TOOL;   return true; } // Nicht Implementiert
  if(U.equals(F("M_R_AJUSTTEMP"))){            out.ctrl=HA_02::M_R_AJUSTTEMP;           out.fmt=FMT_PORT_TOOL;   return true; } // M_R_AJUSTTEMP <PORT> <TOOL> liefert den Status in °C
  if(U.equals(F("M_R_TIMETOSTOP"))){           out.ctrl=HA_02::M_R_TIMETOSTOP;          out.fmt=FMT_PORT_TOOL;   return true; } // M_R_TIMETOSTOP <PORT> <TOOL> liefert den Status in Min
  if(U.equals(F("M_R_STARTMODE"))){            out.ctrl=HA_02::M_R_STARTMODE;           out.fmt=FMT_PORT_TOOL;   return true; } // M_R_STARTMODE <PORT> <TOOL> Liefert die Startbedingungen z.B [STARTMODE] 0x09 [TOOL_BUTTON|PEDAL_HOLD_DOWN]
  if(U.equals(F("M_R_SELECTTEMP"))){           out.ctrl=HA_02::M_R_SELECTTEMP;          out.fmt=FMT_PORT;        return true; } // M_R_SELECTTEMP <PORT> liefert den Status der eingestellten Temperatur in °C
  if(U.equals(F("M_R_AIRTEMP"))){              out.ctrl=HA_02::M_R_AIRTEMP;             out.fmt=FMT_PORT;        return true; } // M_R_AIRTEMP <PORT> liefert den Status der aktuellen Düsentemperatur in °C
  if(U.equals(F("M_R_POWER"))){                out.ctrl=HA_02::M_R_POWER;               out.fmt=FMT_PORT;        return true; } // aktuelle Leistung/Power in %
  if(U.equals(F("M_R_CONNECTTOOL"))){          out.ctrl=HA_02::M_R_CONNECTTOOL;         out.fmt=FMT_PORT;        return true; } // Liefert welches Handstück angeschlossen ist TE/JT
  if(U.equals(F("M_R_TOOLERROR"))){            out.ctrl=HA_02::M_R_TOOLERROR;           out.fmt=FMT_PORT;        return true; } // liefert Toolerrors z.B. HA_NOTOOL
  if(U.equals(F("M_R_STATUSTOOL"))){           out.ctrl=HA_02::M_R_STATUSTOOL;          out.fmt=FMT_PORT;        return true; } // liefert Status des Tools z.B. Status=STAND
  if(U.equals(F("M_R_SELECTFLOW"))){           out.ctrl=HA_02::M_R_SELECTFLOW;          out.fmt=FMT_PORT;        return true; } // liefert eingestellten Flow des Tools in %
  if(U.equals(F("M_R_SELECTEXTTEMP"))){        out.ctrl=HA_02::M_R_SELECTEXTTEMP;       out.fmt=FMT_PORT;        return true; } // liefert eingestellten Ext Temperatur in °C
  if(U.equals(F("M_R_AIRFLOW"))){              out.ctrl=HA_02::M_R_AIRFLOW;             out.fmt=FMT_PORT;        return true; }
  if(U.equals(F("M_R_EXTTCTEMP"))){            out.ctrl=HA_02::M_R_EXTTCTEMP;           out.fmt=FMT_PORT;        return true; }
  if(U.equals(F("M_R_REMOTEMODE"))){           out.ctrl=HA_02::M_R_REMOTEMODE;          out.fmt=FMT_NONE;        return true; }
  if(U.equals(F("M_R_CONTIMODE"))){            out.ctrl=HA_02::M_R_CONTIMODE;           out.fmt=FMT_NONE;        return true; }
  if(U.equals(F("M_I_CONTIMODE"))){            out.ctrl=HA_02::M_I_CONTIMODE;           out.fmt=FMT_NONE;        return true; }
  if(U.equals(F("M_R_TEMPUNIT"))){             out.ctrl=HA_02::M_R_TEMPUNIT;            out.fmt=FMT_NONE;        return true; }
  if(U.equals(F("M_R_MAXMINTEMP"))){           out.ctrl=HA_02::M_R_MAXMINTEMP;          out.fmt=FMT_NONE;        return true; }
  if(U.equals(F("M_R_MAXMINFLOW"))){           out.ctrl=HA_02::M_R_MAXMINFLOW;          out.fmt=FMT_NONE;        return true; }
  if(U.equals(F("M_R_PINENABLED"))){           out.ctrl=HA_02::M_R_PINENABLED;          out.fmt=FMT_NONE;        return true; }
  if(U.equals(F("M_R_STATIONLOCKED"))){        out.ctrl=HA_02::M_R_STATIONLOCKED;       out.fmt=FMT_NONE;        return true; }
  if(U.equals(F("M_R_PIN"))){                  out.ctrl=HA_02::M_R_PIN;                 out.fmt=FMT_NONE;        return true; }
  if(U.equals(F("M_R_STATERROR"))){            out.ctrl=HA_02::M_R_STATERROR;           out.fmt=FMT_NONE;        return true; }
  if(U.equals(F("M_R_DEVICENAME"))){           out.ctrl=HA_02::M_R_DEVICENAME;          out.fmt=FMT_NONE;        return true; }
  if(U.equals(F("M_R_BEEP"))){                 out.ctrl=HA_02::M_R_BEEP;                out.fmt=FMT_NONE;        return true; }
  if(U.equals(F("M_R_LANGUAGE"))){             out.ctrl=HA_02::M_R_LANGUAGE;            out.fmt=FMT_NONE;        return true; }
  if(U.equals(F("M_R_DATETIME"))){             out.ctrl=HA_02::M_R_DATETIME;            out.fmt=FMT_NONE;        return true; }
  if(U.equals(F("M_R_THEME"))){                out.ctrl=HA_02::M_R_THEME;               out.fmt=FMT_NONE;        return true; }
  if(U.equals(F("M_R_PLUGTIME"))){             out.ctrl=HA_02::M_R_PLUGTIME;            out.fmt=FMT_PORT;        return true; }
  if(U.equals(F("M_R_WORKTIME"))){             out.ctrl=HA_02::M_R_WORKTIME;            out.fmt=FMT_PORT;        return true; }
  if(U.equals(F("M_R_WORKCYCLES"))){           out.ctrl=HA_02::M_R_WORKCYCLES;          out.fmt=FMT_PORT;        return true; }
  if(U.equals(F("M_R_SUCTIONCYCLES"))){        out.ctrl=HA_02::M_R_SUCTIONCYCLES;       out.fmt=FMT_PORT;        return true; }
  if(U.equals(F("M_R_PLUGTIMEP"))){            out.ctrl=HA_02::M_R_PLUGTIMEP;           out.fmt=FMT_PORT;        return true; }
  if(U.equals(F("M_R_WORKTIMEP"))){            out.ctrl=HA_02::M_R_WORKTIMEP;           out.fmt=FMT_PORT;        return true; }
  if(U.equals(F("M_R_WORKCYCLESP"))){          out.ctrl=HA_02::M_R_WORKCYCLESP;         out.fmt=FMT_PORT;        return true; }
  if(U.equals(F("M_R_SUCTIONCYCLESP"))){       out.ctrl=HA_02::M_R_SUCTIONCYCLESP;      out.fmt=FMT_PORT;        return true; }
  if(U.equals(F("M_R_RBT_CONNCONFIG"))){       out.ctrl=HA_02::M_R_RBT_CONNCONFIG;      out.fmt=FMT_NONE;        return true; }
  if(U.equals(F("M_R_RBT_CONNECTSTATUS"))){    out.ctrl=HA_02::M_R_RBT_CONNECTSTATUS;   out.fmt=FMT_NONE;        return true; }
  if(U.equals(F("M_R_USB_CONNECTSTATUS"))){    out.ctrl=HA_02::M_R_USB_CONNECTSTATUS;   out.fmt=FMT_NONE;        return true; }
  
  
  // Writes (values-first)
  if(U.equals(F("M_W_DEVICEID"))){             out.ctrl=HA_02::M_W_DEVICEID;            out.fmt=FMT_STRING;                 return true; } // Nicht getestet, wird aber die Device ID überschreiben!
  if(U.equals(F("M_W_PROFILEMODE"))){          out.ctrl=HA_02::M_W_PROFILEMODE;         out.fmt=FMT_W_PORT_U8;              return true; } // M_W_PROFILEMODE <PORT> <0/1> Schaltet nur den Profimodus ein und aus
  if(U.equals(F("M_W_HEATERSTATUS"))){         out.ctrl=HA_02::M_W_HEATERSTATUS;        out.fmt=FMT_W_PORT_U8;              return true; } // M_W_HEATERSTATUS <PORT> <0/1>
  if(U.equals(F("M_W_SUCTIONSTATUS"))){        out.ctrl=HA_02::M_W_SUCTIONSTATUS;       out.fmt=FMT_W_PORT_U8;              return true; } // M_W_SUCTIONSTATUS <PORT> <0/1>
  if(U.equals(F("M_W_EXTTCMODE"))){            out.ctrl=HA_02::M_W_EXTTCMODE;           out.fmt=FMT_W_PORT_TOOL_U8;         return true; }
  if(U.equals(F("M_W_LEVELSTEMPS"))){          out.ctrl=HA_02::M_W_LEVELSTEMPS;         out.fmt=FMT_W_PORT_3TEMP;           return true; }
  if(U.equals(F("M_W_AJUSTTEMP"))){            out.ctrl=HA_02::M_W_AJUSTTEMP;           out.fmt=FMT_W_PORT_TOOL_TEMPDELTA;  return true; } // Δ°C (signed)
  if(U.equals(F("M_W_TIMETOSTOP"))){           out.ctrl=HA_02::M_W_TIMETOSTOP;          out.fmt=FMT_W_PORT_TOOL_U16;        return true; } // M_W_TIMETOSTOP <PORT> <TOOL> <Sekunden>
  if(U.equals(F("M_W_STARTMODE"))){            out.ctrl=HA_02::M_W_STARTMODE;           out.fmt=FMT_W_PORT_TOOL_U8;         return true; } // M_W_STARTMODE <PORT> <TOOL> <1,4,8,9>
  if(U.equals(F("M_W_SELECTTEMP"))){           out.ctrl=HA_02::M_W_SELECTTEMP;          out.fmt=FMT_W_PORT_TEMP;            return true; } // M_W_SELECTTEMP <PORT> <TEMP> Port ist 0 Temp 150-450°C
  if(U.equals(F("M_W_SELECTFLOW"))){           out.ctrl=HA_02::M_W_SELECTFLOW;          out.fmt=FMT_W_PORT_U16;             return true; } // M_W_SELECTFLOW <PORT> <FLOW> Port ist 0 Flow 100-1000 (10-100%)
  if(U.equals(F("M_W_SELECTEXTTEMP"))){        out.ctrl=HA_02::M_W_SELECTEXTTEMP;       out.fmt=FMT_W_PORT_TEMP;            return true; } // M_W_SELECTEXTTEMP <PORT> <TEMP> JTSE hat nur einen Port 0. Temp 25-450°C
  if(U.equals(F("M_W_REMOTEMODE"))){           out.ctrl=HA_02::M_W_REMOTEMODE;          out.fmt=FMT_U8;                     return true; } // M_W_REMOTEMODE <1/0> bei meiner JTSE sehe ich keine Änderung
  if(U.equals(F("M_W_CONTIMODE"))){            out.ctrl=HA_02::M_W_CONTIMODE;           out.fmt=FMT_W_PORT_TOOL_U8;         return true; } // Gibt es bei JTSE nicht!
  if(U.equals(F("M_W_TEMPUNIT"))){             out.ctrl=HA_02::M_W_TEMPUNIT;            out.fmt=FMT_STRING;                 return true; } // M_W_TEMPUNIT <C/F> Celsius oder Fahrenheit
  if(U.equals(F("M_W_MAXMINTEMP"))){           out.ctrl=HA_02::M_W_MAXMINTEMP;          out.fmt=FMT_TEMP_TEMP;              return true; } // M_W_MAXMINTEMP <MAX> <MIN> 150-450°C 
  if(U.equals(F("M_W_MAXMINFLOW"))){           out.ctrl=HA_02::M_W_MAXMINFLOW;          out.fmt=FMT_U16_U16;                return true; } // M_W_MAXMINFLOW <MAX> <MIN> 100-1000 (10-100%)
  if(U.equals(F("M_W_MAXMINEXTTEMP"))){        out.ctrl=HA_02::M_W_MAXMINEXTTEMP;       out.fmt=FMT_TEMP_TEMP;              return true; } // M_W_MAXMINEXTTEMP <MAX> <MIN> 25-450°C
  if(U.equals(F("M_W_PINENABLED"))){           out.ctrl=HA_02::M_W_PINENABLED;          out.fmt=FMT_U8;                     return true; } // M_W_PINENABLED <0/1> mit 0 kannst du die Pinsperre Deaktivieren
  if(U.equals(F("M_W_STATIONLOCKED"))){        out.ctrl=HA_02::M_W_STATIONLOCKED;       out.fmt=FMT_U8;                     return true; } // M_W_STATIONLOCKED <1/0>
  if(U.equals(F("M_W_PIN"))){                  out.ctrl=HA_02::M_W_PIN;                 out.fmt=FMT_STRING;                 return true; } // M_W_PIN <1234> Nach dem Befehl ein Leerzeichen und dann ein Passwort aus 4 Zahlen
  if(U.equals(F("M_W_DEVICENAME"))){           out.ctrl=HA_02::M_W_DEVICENAME;          out.fmt=FMT_STRING;                 return true; } // M_W_DEVICENAME <ABCDEFGHIJKLMNOPQ> Maximal 17 Zeichen A-Z 0-9
  if(U.equals(F("M_W_BEEP"))){                 out.ctrl=HA_02::M_W_BEEP;                out.fmt=FMT_U8;                     return true; } // M_W_BEEP <1/0> 
  if(U.equals(F("M_W_LANGUAGE"))){             out.ctrl=HA_02::M_W_LANGUAGE;            out.fmt=FMT_U8;                     return true; } // Bei JTSE in Implementiert
  if(U.equals(F("M_W_DATETIME"))){             out.ctrl=HA_02::M_W_DATETIME;            out.fmt=FMT_W_DT6;                  return true; } // yyyy mm dd HH MM SS Bei JTSE in Implementiert
  if(U.equals(F("M_W_THEME"))){                out.ctrl=HA_02::M_W_THEME;               out.fmt=FMT_U8;                     return true; } // Bei JTSE in Implementiert
  if(U.equals(F("M_W_USB_CONNECTSTATUS"))){    out.ctrl=HA_02::M_W_USB_CONNECTSTATUS;   out.fmt=FMT_STRING;                 return true; } // M_W_USB_CONNECTSTATUS <:C/:M> wenn Ihr Befehle Schreiben wollt erst mal auf :C
  

  return false;
}

// -------- PH_02 --------
static bool map_ph02(const String& U, ConsoleCmd& out){
  using namespace jbc_cmd;

  // Reads
  if(U.equals(F("M_R_DEVICEIDORIGINAL"))){     out.ctrl=PH_02::M_R_DEVICEIDORIGINAL;    out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_DISCOVER"))){             out.ctrl=PH_02::M_R_DISCOVER;            out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_DEVICEID"))){             out.ctrl=PH_02::M_R_DEVICEID;            out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_FIRMWARE"))){               out.ctrl=PH_02::M_FIRMWARE;              out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_INF_PORT"))){               out.ctrl=PH_02::M_INF_PORT;              out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_WORKMODE"))){             out.ctrl=PH_02::M_R_WORKMODE;            out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_HEATERSTATUS"))){         out.ctrl=PH_02::M_R_HEATERSTATUS;        out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_EXTTCMODE"))){            out.ctrl=PH_02::M_R_EXTTCMODE;           out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_TIMETOSTOP"))){           out.ctrl=PH_02::M_R_TIMETOSTOP;          out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_SELECTTEMP"))){           out.ctrl=PH_02::M_R_SELECTTEMP;          out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_SELECTPOWER"))){          out.ctrl=PH_02::M_R_SELECTPOWER;         out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_WARNING"))){              out.ctrl=PH_02::M_R_WARNING;             out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_ACTIVEZONES"))){          out.ctrl=PH_02::M_R_ACTIVEZONES;         out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_EXTTCTEMP"))){            out.ctrl=PH_02::M_R_EXTTCTEMP;           out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_REMOTEMODE"))){           out.ctrl=PH_02::M_R_REMOTEMODE;          out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_CONTIMODE"))){            out.ctrl=PH_02::M_R_CONTIMODE;           out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_I_CONTIMODE"))){            out.ctrl=PH_02::M_I_CONTIMODE;           out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_PROFILE"))){              out.ctrl=PH_02::M_R_PROFILE;             out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_SETTINGSPROFILE"))){      out.ctrl=PH_02::M_R_SETTINGSPROFILE;     out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_PROFILETEACH"))){         out.ctrl=PH_02::M_R_PROFILETEACH;        out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_MAXMINPOWER"))){          out.ctrl=PH_02::M_R_MAXMINPOWER;         out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_MAXMINTEMP"))){           out.ctrl=PH_02::M_R_MAXMINTEMP;          out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_PINENABLED"))){           out.ctrl=PH_02::M_R_PINENABLED;          out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_STATIONLOCKED"))){        out.ctrl=PH_02::M_R_STATIONLOCKED;       out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_PIN"))){                  out.ctrl=PH_02::M_R_PIN;                 out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_STATERROR"))){            out.ctrl=PH_02::M_R_STATERROR;           out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_DEVICENAME"))){           out.ctrl=PH_02::M_R_DEVICENAME;          out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_BEEP"))){                 out.ctrl=PH_02::M_R_BEEP;                out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_PLUGTIME"))){             out.ctrl=PH_02::M_R_PLUGTIME;            out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_WORKTIME"))){             out.ctrl=PH_02::M_R_WORKTIME;            out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_WORKCYCLES"))){           out.ctrl=PH_02::M_R_WORKCYCLES;          out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_PLUGTIMEP"))){            out.ctrl=PH_02::M_R_PLUGTIMEP;           out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_WORKTIMEP"))){            out.ctrl=PH_02::M_R_WORKTIMEP;           out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_WORKCYCLESP"))){          out.ctrl=PH_02::M_R_WORKCYCLESP;         out.fmt=FMT_PORT;   return true; }
  if(U.equals(F("M_R_USB_CONNECTSTATUS"))){    out.ctrl=PH_02::M_R_USB_CONNECTSTATUS;   out.fmt=FMT_NONE;   return true; }
  

  // Writes (values-first)
  if(U.equals(F("M_W_DEVICEID"))){             out.ctrl=PH_02::M_W_DEVICEID;            out.fmt=FMT_RAWHEX;                 return true; }
  if(U.equals(F("M_W_WORKMODE"))){             out.ctrl=PH_02::M_W_WORKMODE;            out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_HEATERSTATUS"))){         out.ctrl=PH_02::M_W_HEATERSTATUS;        out.fmt=FMT_U8;                     return true; } // 0/1
  if(U.equals(F("M_W_EXTTCMODE"))){            out.ctrl=PH_02::M_W_EXTTCMODE;           out.fmt=FMT_U8;                     return true; } // 0/1
  if(U.equals(F("M_W_TIMETOSTOP"))){           out.ctrl=PH_02::M_W_TIMETOSTOP;          out.fmt=FMT_U16;                    return true; } // 1/10 s
  if(U.equals(F("M_W_SELECTTEMP"))){           out.ctrl=PH_02::M_W_SELECTTEMP;          out.fmt=FMT_W_PORT_TEMP;            return true; }
  if(U.equals(F("M_W_SELECTPOWER"))){          out.ctrl=PH_02::M_W_SELECTPOWER;         out.fmt=FMT_W_PORT_U16;             return true; } // 0..1000
  if(U.equals(F("M_W_ACTIVEZONES"))){          out.ctrl=PH_02::M_W_ACTIVEZONES;         out.fmt=FMT_U8;                     return true; } // Bitmaske
  if(U.equals(F("M_W_REMOTEMODE"))){           out.ctrl=PH_02::M_W_REMOTEMODE;          out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_CONTIMODE"))){            out.ctrl=PH_02::M_W_CONTIMODE;           out.fmt=FMT_U8;                     return true; } // 0=OFF,1=ON
  if(U.equals(F("M_W_PROFILE"))){              out.ctrl=PH_02::M_W_PROFILE;             out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_SETTINGSPROFILE"))){      out.ctrl=PH_02::M_W_SETTINGSPROFILE;     out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_PROFILETEACH"))){         out.ctrl=PH_02::M_W_PROFILETEACH;        out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_MAXMINPOWER"))){          out.ctrl=PH_02::M_W_MAXMINPOWER;         out.fmt=FMT_U16_U16;                return true; }
  if(U.equals(F("M_W_MAXMINTEMP"))){           out.ctrl=PH_02::M_W_MAXMINTEMP;          out.fmt=FMT_TEMP_TEMP;              return true; }
  if(U.equals(F("M_W_PINENABLED"))){           out.ctrl=PH_02::M_W_PINENABLED;          out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_STATIONLOCKED"))){        out.ctrl=PH_02::M_W_STATIONLOCKED;       out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_PIN"))){                  out.ctrl=PH_02::M_W_PIN;                 out.fmt=FMT_STRING;                 return true; }
  if(U.equals(F("M_W_DEVICENAME"))){           out.ctrl=PH_02::M_W_DEVICENAME;          out.fmt=FMT_STRING;                 return true; }
  if(U.equals(F("M_W_BEEP"))){                 out.ctrl=PH_02::M_W_BEEP;                out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_USB_CONNECTSTATUS"))){    out.ctrl=PH_02::M_W_USB_CONNECTSTATUS;   out.fmt=FMT_STRING;                 return true; }


  return false;
}

// -------- FE_02 --------
static bool map_fe02(const String& U, ConsoleCmd& out){
  using namespace jbc_cmd;
  if(U.equals(F("M_R_DEVICEIDORIGINAL"))){     out.ctrl=FE_02::M_R_DEVICEIDORIGINAL;    out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_DISCOVER"))){             out.ctrl=FE_02::M_R_DISCOVER;            out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_DEVICEID"))){             out.ctrl=FE_02::M_R_DEVICEID;            out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_FIRMWARE"))){               out.ctrl=FE_02::M_FIRMWARE;              out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_SUCTIONLEVEL"))){         out.ctrl=FE_02::M_R_SUCTIONLEVEL;        out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_FLOW"))){                 out.ctrl=FE_02::M_R_FLOW;                out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_SPEED"))){                out.ctrl=FE_02::M_R_SPEED;               out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_SELECTFLOW"))){           out.ctrl=FE_02::M_R_SELECTFLOW;          out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_STANDINTAKES"))){         out.ctrl=FE_02::M_R_STANDINTAKES;        out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_INTAKEACTIVATION"))){     out.ctrl=FE_02::M_R_INTAKEACTIVATION;    out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_SUCTIONDELAY"))){         out.ctrl=FE_02::M_R_SUCTIONDELAY;        out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_DELAYTIME"))){            out.ctrl=FE_02::M_R_DELAYTIME;           out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_ACTIVATIONPEDAL"))){      out.ctrl=FE_02::M_R_ACTIVATIONPEDAL;     out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_PEDALMODE"))){            out.ctrl=FE_02::M_R_PEDALMODE;           out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_FILTERSTATUS"))){         out.ctrl=FE_02::M_R_FILTERSTATUS;        out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_RESETFILTER"))){          out.ctrl=FE_02::M_R_RESETFILTER;         out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_CONNECTEDPEDAL"))){       out.ctrl=FE_02::M_R_CONNECTEDPEDAL;      out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_PIN"))){                  out.ctrl=FE_02::M_R_PIN;                 out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_STATIONLOCKED"))){        out.ctrl=FE_02::M_R_STATIONLOCKED;       out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_BEEP"))){                 out.ctrl=FE_02::M_R_BEEP;                out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_CONTINUOUSSUCTION"))){    out.ctrl=FE_02::M_R_CONTINUOUSSUCTION;   out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_STATERROR"))){            out.ctrl=FE_02::M_R_STATERROR;           out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_DEVICENAME"))){           out.ctrl=FE_02::M_R_DEVICENAME;          out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_PINENABLED"))){           out.ctrl=FE_02::M_R_PINENABLED;          out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_COUNTERS"))){             out.ctrl=FE_02::M_R_COUNTERS;            out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_RESETCOUNTERS"))){        out.ctrl=FE_02::M_R_RESETCOUNTERS;       out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_RESETCOUNTERSP"))){       out.ctrl=FE_02::M_R_RESETCOUNTERSP;      out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_USB_CONNECTSTATUS"))){    out.ctrl=FE_02::M_R_USB_CONNECTSTATUS;   out.fmt=FMT_NONE;   return true; }

  

  
  
  
 
  // Writes (keine Port-Writes hier)
  if(U.equals(F("M_W_DEVICEID"))){             out.ctrl=FE_02::M_W_DEVICEID;            out.fmt=FMT_RAWHEX;                 return true; }
  if(U.equals(F("M_W_SUCTIONLEVEL"))){         out.ctrl=FE_02::M_W_SUCTIONLEVEL;        out.fmt=FMT_U16;                    return true; }
  if(U.equals(F("M_W_SELECTFLOW"))){           out.ctrl=FE_02::M_W_SELECTFLOW;          out.fmt=FMT_U16;                    return true; }
  if(U.equals(F("M_W_STANDINTAKES"))){         out.ctrl=FE_02::M_W_STANDINTAKES;        out.fmt=FMT_U8;                     return true; } // 0/1
  if(U.equals(F("M_W_INTAKEACTIVATION"))){     out.ctrl=FE_02::M_W_INTAKEACTIVATION;    out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_SUCTIONDELAY"))){         out.ctrl=FE_02::M_W_SUCTIONDELAY;        out.fmt=FMT_U16;                    return true; }
  if(U.equals(F("M_W_ACTIVATIONPEDAL"))){      out.ctrl=FE_02::M_W_ACTIVATIONPEDAL;     out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_PEDALMODE"))){            out.ctrl=FE_02::M_W_PEDALMODE;           out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_PIN"))){                  out.ctrl=FE_02::M_W_PIN;                 out.fmt=FMT_U16;                    return true; }
  if(U.equals(F("M_W_STATIONLOCKED"))){        out.ctrl=FE_02::M_W_STATIONLOCKED;       out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_BEEP"))){                 out.ctrl=FE_02::M_W_BEEP;                out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_CONTINUOUSSUCTION"))){    out.ctrl=FE_02::M_W_CONTINUOUSSUCTION;   out.fmt=FMT_U8;                     return true; } // 0/1
  if(U.equals(F("M_W_DEVICENAME"))){           out.ctrl=FE_02::M_W_DEVICENAME;          out.fmt=FMT_STRING;                 return true; }
  if(U.equals(F("M_W_PINENABLED"))){           out.ctrl=FE_02::M_W_PINENABLED;          out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_WORKINTAKES"))){          out.ctrl=FE_02::M_W_WORKINTAKES;         out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_USB_CONNECTSTATUS"))){    out.ctrl=FE_02::M_W_USB_CONNECTSTATUS;   out.fmt=FMT_STRING;                 return true; }

  return false;
}

// -------- SF_02 --------
static bool map_sf02(const String& U, ConsoleCmd& out){
  using namespace jbc_cmd;
  if(U.equals(F("M_R_DEVICEIDORIGINAL"))){     out.ctrl=SF_02::M_R_DEVICEIDORIGINAL;    out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_DISCOVER"))){             out.ctrl=SF_02::M_R_DISCOVER;            out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_DEVICEID"))){             out.ctrl=SF_02::M_R_DEVICEID;            out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_FIRMWARE"))){               out.ctrl=SF_02::M_FIRMWARE;              out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_DISPENSERMODE"))){        out.ctrl=SF_02::M_R_DISPENSERMODE;       out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_PROGRAM"))){              out.ctrl=SF_02::M_R_PROGRAM;             out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_PROGRAMLIST"))){          out.ctrl=SF_02::M_R_PROGRAMLIST;         out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_SPEED"))){                out.ctrl=SF_02::M_R_SPEED;               out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_LENGTH"))){               out.ctrl=SF_02::M_R_LENGTH;              out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_STARTFEEDING"))){         out.ctrl=SF_02::M_R_STARTFEEDING;        out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_STOPTFEEDING"))){         out.ctrl=SF_02::M_R_STOPTFEEDING;        out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_FEEDING"))){              out.ctrl=SF_02::M_R_FEEDING;             out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_BACKWARDMODE"))){         out.ctrl=SF_02::M_R_BACKWARDMODE;        out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_PIN"))){                  out.ctrl=SF_02::M_R_PIN;                 out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_STATIONLOCKED"))){        out.ctrl=SF_02::M_R_STATIONLOCKED;       out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_BEEP"))){                 out.ctrl=SF_02::M_R_BEEP;                out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_LENGTHUNIT"))){           out.ctrl=SF_02::M_R_LENGTHUNIT;          out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_STATERROR"))){            out.ctrl=SF_02::M_R_STATERROR;           out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_RESETERROR"))){           out.ctrl=SF_02::M_R_RESETERROR;          out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_DEVICENAME"))){           out.ctrl=SF_02::M_R_DEVICENAME;          out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_TOOLENABLED"))){          out.ctrl=SF_02::M_R_TOOLENABLED;         out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_PINENABLED"))){           out.ctrl=SF_02::M_R_PINENABLED;          out.fmt=FMT_NONE;     return true; }
  if(U.equals(F("M_R_COUNTERS"))){             out.ctrl=SF_02::M_R_COUNTERS;            out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_RESETCOUNTERS"))){        out.ctrl=SF_02::M_R_RESETCOUNTERS;       out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_COUNTERSP"))){            out.ctrl=SF_02::M_R_COUNTERSP;           out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_RESETCOUNTERSP"))){       out.ctrl=SF_02::M_R_RESETCOUNTERSP;      out.fmt=FMT_NONE;   return true; }
  if(U.equals(F("M_R_USB_CONNECTSTATUS"))){    out.ctrl=SF_02::M_R_USB_CONNECTSTATUS;   out.fmt=FMT_NONE;   return true; }
  


  // Writes
  if(U.equals(F("M_W_DEVICEID"))){             out.ctrl=SF_02::M_W_DEVICEID;            out.fmt=FMT_RAWHEX;                 return true; }
  if(U.equals(F("M_W_DISPENSERMODE"))){        out.ctrl=SF_02::M_W_DISPENSERMODE;       out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_PROGRAM"))){              out.ctrl=SF_02::M_W_PROGRAM;             out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_PROGRAMLIST"))){          out.ctrl=SF_02::M_W_PROGRAMLIST;         out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_SPEED"))){                out.ctrl=SF_02::M_W_SPEED;               out.fmt=FMT_U16;                    return true; }
  if(U.equals(F("M_W_LENGTH"))){               out.ctrl=SF_02::M_W_LENGTH;              out.fmt=FMT_U16;                    return true; }
  if(U.equals(F("M_W_BACKWARDMODE"))){         out.ctrl=SF_02::M_W_BACKWARDMODE;        out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_PIN"))){                  out.ctrl=SF_02::M_W_PIN;                 out.fmt=FMT_U16;                    return true; }
  if(U.equals(F("M_W_STATIONLOCKED"))){        out.ctrl=SF_02::M_W_STATIONLOCKED;       out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_BEEP"))){                 out.ctrl=SF_02::M_W_BEEP;                out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_LENGTHUNIT"))){           out.ctrl=SF_02::M_W_LENGTHUNIT;          out.fmt=FMT_U8;                     return true; }
  if(U.equals(F("M_W_DEVICENAME"))){           out.ctrl=SF_02::M_W_DEVICENAME;          out.fmt=FMT_STRING;                 return true; }
  if(U.equals(F("M_W_TOOLENABLED"))){          out.ctrl=SF_02::M_W_TOOLENABLED;         out.fmt=FMT_U8;                     return true; } 
  if(U.equals(F("M_W_USB_CONNECTSTATUS"))){    out.ctrl=SF_02::M_W_USB_CONNECTSTATUS;   out.fmt=FMT_STRING;                 return true; }
  
  return false;
}

// --- Routing anhand Backend ---
static bool map_for_backend(const String& u, Backend be, ConsoleCmd& out){
  switch(be){
    case BK_SOLD:  return map_sold02(u, out);
    case BK_SOLD1: return map_sold01(u, out);
    case BK_HA:    return map_ha02(u, out);
    case BK_PH:    return map_ph02(u, out);
    case BK_FE:    return map_fe02(u, out);
    case BK_SF:    return map_sf02(u, out);
    default:       // Unknown → best effort
      return map_sold02(u, out) || map_sold01(u, out) ||
             map_ha02(u, out)   || map_ph02(u, out)   ||
             map_fe02(u, out)   || map_sf02(u, out);
  }
}

// ======================================================
//                Parser / Payload-Builder
// ======================================================

static uint8_t split_tokens(const String& line, String* tok, uint8_t maxTok){
  uint8_t n=0; int start=0;
  while(n<maxTok){
    int sp = line.indexOf(' ', start);
    String t = (sp<0)? line.substring(start) : line.substring(start, sp);
    t.trim();
    if(t.length()) tok[n++] = t;
    if(sp<0) break;
    start = sp+1;
  }
  return n;
}

static bool port_allowed(Backend be, uint8_t port){
  switch(be){
    case BK_SOLD:  return port < 4;   // 0..3
    case BK_SOLD1: return port < 1;   // 0
    case BK_HA:    return port < 1;   // 0
    default:       return true;
  }
}

template<typename SendFn>
bool handle(const String& line, Backend be, uint8_t /*dst_ignored*/, SendFn send_cb){
  String L = line; L.trim();
  if(!L.length()) return false;

  if(L.equalsIgnoreCase("HELP")){
    Serial.println(F("CMD Beispiele:"));
    Serial.println(F("  M_R_DEVICEID | M_R_DEVICENAME"));
    Serial.println(F("  M_INF_PORT 0|1"));
    Serial.println(F("  SOLD02:"));
    Serial.println(F("    M_W_SELECTTEMP 0 350"));
    Serial.println(F("    M_W_LEVELSTEMPS 0 T245 1 0  1 300  1 350  1 380"));
    Serial.println(F("    M_W_SLEEPDELAY 0 T245 30 1   |   M_W_HIBERDELAY 0 T245 30 1"));
    Serial.println(F("    M_W_SLEEPTEMP  0 T245 200    |   M_W_AJUSTTEMP  0 T245 -5"));
    Serial.println(F("    M_W_ALARMMAXTEMP 420 | M_W_ALARMMINTEMP 150"));
    Serial.println(F("    M_W_USB_CONNECTSTATUS :C   |   M_W_REMOTEMODE 1"));
    Serial.println(F("    M_W_ETH_TCPIPCONFIG 0 192.168.1.50 255.255.255.0 192.168.1.1 8.8.8.8 2000"));
    Serial.println(F("    M_W_PERIPHCONFIG <hex...>   (z.B. AA BB 01 02)"));
    Serial.println(F("  HA_02:"));
    Serial.println(F("    M_W_MAXMINTEMP 250 400 | M_W_MAXMINFLOW 200 800"));
    Serial.println(F("    M_W_HEATERSTATUS 1 | M_W_SUCTIONSTATUS 0"));
    Serial.println(F("    M_W_DATETIME 2025 08 27 14 30 00  |  M_W_THEME 1"));
    Serial.println(F("    M_W_LANGUAGE 2  |  M_W_USB_CONNECTSTATUS :C"));
    Serial.println(F("  PH_02:"));
    Serial.println(F("    M_W_MAXMINPOWER 100 900 | M_W_ACTIVEZONES 0x07"));
    return true;
  }

  String tok[12];
  uint8_t ntok = split_tokens(L, tok, 12);
  if(ntok==0) return false;

  String U = tok[0]; U.trim(); U.toUpperCase();
  ConsoleCmd cc;
  if(!map_for_backend(U, be, cc)){
    Serial.println(F("[CLI] Unbekannter CMD für aktuelles Backend. 'HELP' eingeben."));
    return false;
  }

  uint8_t payload[48]; uint8_t plen=0; // etwas mehr Puffer für komplexe Writes

  auto parse_u16 = [](const String& s)->uint16_t{
    if(s.startsWith("0X") || s.startsWith("0x")) return (uint16_t) strtol(s.c_str(), nullptr, 16);
    return (uint16_t) s.toInt();
  };
  auto parse_u8 = [](const String& s)->uint8_t{
    if(s.startsWith("0X") || s.startsWith("0x")) return (uint8_t) strtol(s.c_str(), nullptr, 16);
    return (uint8_t) s.toInt();
  };
  auto parse_i16 = [](const String& s)->int16_t{
    if(s.startsWith("0X") || s.startsWith("0x")) return (int16_t) strtol(s.c_str(), nullptr, 16);
    return (int16_t) s.toInt(); // erlaubt auch negative Zahlen
  };

  auto push_u16 = [&](uint16_t v){ payload[plen++] = (uint8_t)(v & 0xFF); payload[plen++] = (uint8_t)(v >> 8); };
  auto push_i16 = [&](int16_t v){ payload[plen++] = (uint8_t)(v & 0xFF); payload[plen++] = (uint8_t)((uint16_t)v >> 8); };
  auto push_temp = [&](uint16_t cdeg){ push_u16((uint16_t)(cdeg * 9U)); };

  auto parse_ip = [&](const String& s, uint8_t out[4])->bool{
    int a=s.indexOf('.'); if(a<0) return false;
    int b=s.indexOf('.',a+1); if(b<0) return false;
    int c=s.indexOf('.',b+1); if(c<0) return false;
    int d=s.length();
    long o0=s.substring(0,a).toInt();
    long o1=s.substring(a+1,b).toInt();
    long o2=s.substring(b+1,c).toInt();
    long o3=s.substring(c+1,d).toInt();
    if(o0<0||o0>255||o1<0||o1>255||o2<0||o2>255||o3<0||o3>255) return false;
    out[0]=(uint8_t)o0; out[1]=(uint8_t)o1; out[2]=(uint8_t)o2; out[3]=(uint8_t)o3;
    return true;
  };

  // optional: Tool-Aliase (nur für *_PORT_TOOL_* Formate)
  auto parse_tool = [&](const String& s)->uint8_t{
    if (s.length()==0) return 0;

    // hex direkt erlauben (z.B. 0x06)
    if (s.startsWith("0X") || s.startsWith("0x"))
      return (uint8_t) strtol(s.c_str(), nullptr, 16);

    String up = s; up.toUpperCase();

    // generische none
    if (up=="NONE" || up=="NOTOOL" || up=="0") return 0;

    // HA-Familie (JT/TE): 0:NOTOOL, 1:JT, 2:TE
    if (be == BK_HA){
      if (up=="JT") return 1;
      if (up=="TE") return 2;
      return (uint8_t) up.toInt(); // Fallback
    }

    // SOLDER-Familie: 0:NOTOOL, 1:T210, 2:T245, 3:PA, 4:HT, 5:DS, 6:DR, 7:NT105, 8:NP105, 9:T470
    if (up=="T210")   return 1;
    if (up=="T245")   return 2;
    if (up=="PA" || up=="PA120") return 3;
    if (up=="HT")     return 4;
    if (up=="DS" || up=="DS360") return 5;
    if (up=="DR" || up=="DR560") return 6;
    if (up=="NT105")  return 7;
    if (up=="NP105")  return 8;
    if (up=="T470")   return 9;

    return (uint8_t) up.toInt(); // Fallback
  };


  switch(cc.fmt){
    case FMT_NONE: break;

    case FMT_PORT: {
      if(ntok < 2){ Serial.println(F("[CLI] Port fehlt")); return false; }
      uint8_t port = parse_u8(tok[1]);
      if(!port_allowed(be, port)){ Serial.println(F("[CLI] Ungültiger Port.")); return false; }
      payload[plen++] = port;
      break;
    }

    case FMT_PORT_TOOL: {
      if (ntok < 3){ Serial.println(F("[CLI] Port, Tool erwartet")); return false; }
      uint8_t port = parse_u8(tok[1]); if(!port_allowed(be, port)){ Serial.println(F("[CLI] Ungültiger Port.")); return false; }
      uint8_t tool = parse_tool(tok[2]);
      payload[plen++] = port;
      payload[plen++] = tool;
      break;
    }

    case FMT_U8: {
      if(ntok < 2){ Serial.println(F("[CLI] Wert (u8) fehlt")); return false; }
      payload[plen++] = parse_u8(tok[1]);
      break;
    }

    case FMT_U16: {
      if(ntok < 2){ Serial.println(F("[CLI] Wert (u16) fehlt")); return false; }
      push_u16(parse_u16(tok[1]));
      break;
    }

    case FMT_TEMP: {
      if(ntok < 2){ Serial.println(F("[CLI] Temperatur [°C] fehlt")); return false; }
      push_temp(parse_u16(tok[1]));
      break;
    }

    case FMT_U16_U16: {
      if(ntok < 3){ Serial.println(F("[CLI] Zwei Werte (u16 u16) erwartet")); return false; }
      push_u16(parse_u16(tok[1])); push_u16(parse_u16(tok[2]));
      break;
    }

    case FMT_TEMP_TEMP: {
      if(ntok < 3){ Serial.println(F("[CLI] Zwei Temperaturen [°C °C] erwartet")); return false; }
      push_temp(parse_u16(tok[1]));
      push_temp(parse_u16(tok[2]));
      break;
    }

    case FMT_STRING: {
      // alles nach dem 1. Leerzeichen als ASCII-Bytes
      int p = L.indexOf(' ');
      if(p<0 || p+1 >= L.length()){ Serial.println(F("[CLI] Text fehlt")); return false; }
      {
        String s = L.substring(p+1);
        for (size_t i=0; i<(size_t)s.length(); ++i) payload[plen++] = (uint8_t)s[i];
      }
      break;
    }

    case FMT_RAWHEX: {
      if(ntok < 2){ Serial.println(F("[CLI] Mindestens 1 Byte erwartet (z.B. AA oder 0xAA)")); return false; }
      for(uint8_t i=1;i<ntok;i++){ payload[plen++] = parse_u8(tok[i]); }
      break;
    }

    case FMT_IPCFG: {
      // dhcp ip mask gw [dns] [port]
      if(ntok < 3){ Serial.println(F("[CLI] Erwartet: dhcp ip mask gw [dns] [port]")); return false; }
      uint8_t dhcp = parse_u8(tok[1]);
      uint8_t ip[4]={0}, mask[4]={0}, gw[4]={0}, dns[4]={0};
      uint16_t port=0;
      if(dhcp>1){ Serial.println(F("[CLI] dhcp muss 0 oder 1 sein")); return false; }
      if(!parse_ip(tok[2], ip)){ Serial.println(F("[CLI] Ungültige IP")); return false; }
      if(ntok < 4 || !parse_ip(tok[3], mask)){ Serial.println(F("[CLI] Ungültige Maske")); return false; }
      if(ntok < 5 || !parse_ip(tok[4], gw)){ Serial.println(F("[CLI] Ungültiges Gateway")); return false; }
      if(ntok >= 6 && tok[5].length()) { if(!parse_ip(tok[5], dns)){ Serial.println(F("[CLI] Ungültiges DNS")); return false; } }
      if(ntok >= 7 && tok[6].length()) { port = parse_u16(tok[6]); }

      payload[plen++] = dhcp;
      for(int i=0;i<4;i++) payload[plen++]=ip[i];
      for(int i=0;i<4;i++) payload[plen++]=mask[i];
      for(int i=0;i<4;i++) payload[plen++]=gw[i];
      for(int i=0;i<4;i++) payload[plen++]=dns[i];
      push_u16(port);
      break;
    }

    // -------- values-first (mit Tool/Flags) --------
    case FMT_W_PORT_U8: {
      if (ntok < 3){ Serial.println(F("[CLI] Port oder Wert (u8) fehlt")); return false; }
      uint8_t port = parse_u8(tok[1]); if(!port_allowed(be, port)){ Serial.println(F("[CLI] Ungültiger Port.")); return false; }
      uint8_t v = parse_u8(tok[2]);
      payload[plen++] = v;
      payload[plen++] = port;
      break;
    }

    case FMT_W_PORT_U16: {
      if (ntok < 3){ Serial.println(F("[CLI] Port oder Wert (u16) fehlt")); return false; }
      uint8_t port = parse_u8(tok[1]); if(!port_allowed(be, port)){ Serial.println(F("[CLI] Ungültiger Port.")); return false; }
      uint16_t v = parse_u16(tok[2]);
      push_u16(v);
      payload[plen++] = port;
      break;
    }

    case FMT_W_PORT_TEMP: {
      if (ntok < 3){ Serial.println(F("[CLI] Port oder Temperatur [°C] fehlt")); return false; }
      uint8_t port = parse_u8(tok[1]); if(!port_allowed(be, port)){ Serial.println(F("[CLI] Ungültiger Port.")); return false; }
      push_temp(parse_u16(tok[2]));
      payload[plen++] = port;
      break;
    }

    case FMT_W_PORT_3TEMP: {
      if (ntok < 5){ Serial.println(F("[CLI] Port und 3 Temperaturen [°C °C °C] erwartet")); return false; }
      uint8_t port = parse_u8(tok[1]); if(!port_allowed(be, port)){ Serial.println(F("[CLI] Ungültiger Port.")); return false; }
      push_temp(parse_u16(tok[2]));
      push_temp(parse_u16(tok[3]));
      push_temp(parse_u16(tok[4]));
      payload[plen++] = port;
      break;
    }

    case FMT_W_PORT_TOOL_U8: {
      if (ntok < 4){ Serial.println(F("[CLI] Port, Tool, Wert (u8) erwartet")); return false; }
      uint8_t port = parse_u8(tok[1]); if(!port_allowed(be, port)){ Serial.println(F("[CLI] Ungültiger Port.")); return false; }
      uint8_t tool = parse_tool(tok[2]);
      uint8_t v = parse_u8(tok[3]);
      payload[plen++] = v;
      payload[plen++] = port;
      payload[plen++] = tool;
      break;
    }

    case FMT_W_PORT_TOOL_U16: {
      if (ntok < 4){ Serial.println(F("[CLI] Port, Tool, Wert (u16) erwartet")); return false; }
      uint8_t port = parse_u8(tok[1]); if(!port_allowed(be, port)){ Serial.println(F("[CLI] Ungültiger Port.")); return false; }
      uint8_t tool = parse_tool(tok[2]);
      push_u16(parse_u16(tok[3]));
      payload[plen++] = port;
      payload[plen++] = tool;
      break;
    }

    case FMT_W_PORT_TOOL_TEMP: {
      if (ntok < 4){ Serial.println(F("[CLI] Port, Tool, Temperatur [°C] erwartet")); return false; }
      uint8_t port = parse_u8(tok[1]); if(!port_allowed(be, port)){ Serial.println(F("[CLI] Ungültiger Port.")); return false; }
      uint8_t tool = parse_tool(tok[2]);
      push_temp(parse_u16(tok[3]));
      payload[plen++] = port;
      payload[plen++] = tool;
      break;
    }

    case FMT_W_PORT_TOOL_3TEMP: {
      if (ntok < 6){ Serial.println(F("[CLI] Port, Tool, 3 Temperaturen erwartet")); return false; }
      uint8_t port = parse_u8(tok[1]); if(!port_allowed(be, port)){ Serial.println(F("[CLI] Ungültiger Port.")); return false; }
      uint8_t tool = parse_tool(tok[2]);
      push_temp(parse_u16(tok[3]));
      push_temp(parse_u16(tok[4]));
      push_temp(parse_u16(tok[5]));
      payload[plen++] = port;
      payload[plen++] = tool;
      break;
    }

    case FMT_W_PORT_TOOL_U8_U8: {
      if (ntok < 5){ Serial.println(F("[CLI] Port, Tool, value, onoff erwartet")); return false; }
      uint8_t port = parse_u8(tok[1]); if(!port_allowed(be, port)){ Serial.println(F("[CLI] Ungültiger Port.")); return false; }
      uint8_t tool  = parse_tool(tok[2]);
      uint8_t value = parse_u8(tok[3]);
      uint8_t onoff = parse_u8(tok[4]);
      payload[plen++] = value;
      payload[plen++] = onoff;
      payload[plen++] = port;
      payload[plen++] = tool;
      break;
    }

    case FMT_W_LEVELSTEMPS_S02: {
      if (ntok < 11){ Serial.println(F("[CLI] Port, Tool, onoff, sel, l1_on, l1, l2_on, l2, l3_on, l3 erwartet")); return false; }
      uint8_t port = parse_u8(tok[1]); if(!port_allowed(be, port)){ Serial.println(F("[CLI] Ungültiger Port.")); return false; }
      uint8_t tool = parse_tool(tok[2]);
      uint8_t onoff = parse_u8(tok[3]);
      uint8_t sel   = parse_u8(tok[4]);
      uint8_t l1_on = parse_u8(tok[5]); uint16_t l1 = (uint16_t)(parse_u16(tok[6]) * 9U);
      uint8_t l2_on = parse_u8(tok[7]); uint16_t l2 = (uint16_t)(parse_u16(tok[8]) * 9U);
      uint8_t l3_on = parse_u8(tok[9]); uint16_t l3 = (uint16_t)(parse_u16(tok[10]) * 9U);
      payload[plen++] = onoff;
      payload[plen++] = sel;
      payload[plen++] = l1_on;  push_u16(l1);
      payload[plen++] = l2_on;  push_u16(l2);
      payload[plen++] = l3_on;  push_u16(l3);
      payload[plen++] = port;
      payload[plen++] = tool;
      break;
    }

    case FMT_W_PORT_TOOL_TEMPDELTA: {
      if (ntok < 4){ Serial.println(F("[CLI] Port, Tool, Δ[°C] erwartet")); return false; }
      uint8_t port = parse_u8(tok[1]); if(!port_allowed(be, port)){ Serial.println(F("[CLI] Ungültiger Port.")); return false; }
      uint8_t tool = parse_tool(tok[2]);
      int16_t dC   = parse_i16(tok[3]);
      int16_t uti  = (int16_t)(dC * 9); // ±50°C → ±450 UTI
      push_i16(uti);
      payload[plen++] = port;
      payload[plen++] = tool;
      break;
    }

    // ----- NEU: DateTime (HA) -----
    case FMT_W_DT6: {
      if (ntok < 7){ Serial.println(F("[CLI] yyyy mm dd HH MM SS erwartet")); return false; }
      uint16_t year = parse_u16(tok[1]);
      uint8_t  mon  = parse_u8(tok[2]);
      uint8_t  day  = parse_u8(tok[3]);
      uint8_t  hh   = parse_u8(tok[4]);
      uint8_t  mm   = parse_u8(tok[5]);
      uint8_t  ss   = parse_u8(tok[6]);
      push_u16(year);                   // LE
      payload[plen++] = mon;
      payload[plen++] = day;
      payload[plen++] = hh;
      payload[plen++] = mm;
      payload[plen++] = ss;
      break;
    }
  }

  send_cb(cc.ctrl, payload, plen);
  return true;
}

} // namespace jbc_cli
