// SPDX-License-Identifier: MIT OR GPL-2.0-only

#pragma once
#include <Arduino.h>
#include "jbc_commands_full.h"

// ---- Backend-Types (global) ----
enum Backend : uint8_t { BK_UNKNOWN=0, BK_SOLD=1, BK_HA=2, BK_FE=3, BK_PH=4, BK_SF=5, BK_SOLD1=6 };

namespace jbc_name {

inline const __FlashStringHelper* backend_name(Backend b){
  switch(b){
    case BK_SOLD:  return F("SOLD_02");
    case BK_SOLD1: return F("SOLD_01");
    case BK_HA:    return F("HA_02");
    case BK_FE:    return F("FE_02");
    case BK_PH:    return F("PH_02");
    case BK_SF:    return F("SF_02");
    default:       return F("UNKNOWN");
  }
}

// ---- BASE
inline const __FlashStringHelper* name_BASE(uint8_t c){
  using namespace jbc_cmd::BASE;
  switch(c){
    case M_HS:       return F("BASE::M_HS");
    case M_ACK:      return F("BASE::M_ACK");
    case M_NACK:     return F("BASE::M_NACK");
    case M_SYN:      return F("BASE::M_SYN");
    case M_RESET:    return F("BASE::M_RESET");
    case M_FIRMWARE: return F("BASE::M_FIRMWARE");
    default:         return F("BASE::?");
  }
}

// ---- SOLD_02 (vollständig)
inline const __FlashStringHelper* name_SOLD_02(uint8_t c){
  using namespace jbc_cmd::SOLD_02;
  switch(c){
    case M_HS: return F("SOLD_02::M_HS");
    case M_EOT: return F("SOLD_02::M_EOT");
    case M_ACK: return F("SOLD_02::M_ACK");
    case M_NACK: return F("SOLD_02::M_NACK");
    case M_SYN: return F("SOLD_02::M_SYN");
    case M_R_DEVICEIDORIGINAL: return F("SOLD_02::M_R_DEVICEIDORIGINAL");
    case M_R_DISCOVER: return F("SOLD_02::M_R_DISCOVER");
    case M_R_DEVICEID: return F("SOLD_02::M_R_DEVICEID");
    case M_W_DEVICEID: return F("SOLD_02::M_W_DEVICEID");
    case M_RESET: return F("SOLD_02::M_RESET");
    case M_FIRMWARE: return F("SOLD_02::M_FIRMWARE");
    case M_CLEARMEMFLASH: return F("SOLD_02::M_CLEARMEMFLASH");
    case M_SENDMEMADDRESS: return F("SOLD_02::M_SENDMEMADDRESS");
    case M_SENDMEMDATA: return F("SOLD_02::M_SENDMEMDATA");
    case M_ENDPROGR: return F("SOLD_02::M_ENDPROGR");
    case M_ENDUPD: return F("SOLD_02::M_ENDUPD");
    case M_CONTINUEUPD: return F("SOLD_02::M_CONTINUEUPD");
    case M_CLEARING: return F("SOLD_02::M_CLEARING");
    case M_INF_PORT: return F("SOLD_02::M_INF_PORT");
    case M_RESET_PORTTOOL: return F("SOLD_02::M_RESET_PORTTOOL");
    case M_R_LEVELSTEMPS: return F("SOLD_02::M_R_LEVELSTEMPS");
    case M_W_LEVELSTEMPS: return F("SOLD_02::M_W_LEVELSTEMPS");
    case M_R_SLEEPDELAY: return F("SOLD_02::M_R_SLEEPDELAY");
    case M_W_SLEEPDELAY: return F("SOLD_02::M_W_SLEEPDELAY");
    case M_R_SLEEPTEMP: return F("SOLD_02::M_R_SLEEPTEMP");
    case M_W_SLEEPTEMP: return F("SOLD_02::M_W_SLEEPTEMP");
    case M_R_HIBERDELAY: return F("SOLD_02::M_R_HIBERDELAY");
    case M_W_HIBERDELAY: return F("SOLD_02::M_W_HIBERDELAY");
    case M_R_AJUSTTEMP: return F("SOLD_02::M_R_AJUSTTEMP");
    case M_W_AJUSTTEMP: return F("SOLD_02::M_W_AJUSTTEMP");
    case M_R_CARTRIDGE: return F("SOLD_02::M_R_CARTRIDGE");
    case M_W_CARTRIDGE: return F("SOLD_02::M_W_CARTRIDGE");
    case M_R_SELECTTEMP: return F("SOLD_02::M_R_SELECTTEMP");
    case M_W_SELECTTEMP: return F("SOLD_02::M_W_SELECTTEMP");
    case M_R_TIPTEMP: return F("SOLD_02::M_R_TIPTEMP");
    case M_R_CURRENT: return F("SOLD_02::M_R_CURRENT");
    case M_R_POWER: return F("SOLD_02::M_R_POWER");
    case M_R_CONNECTTOOL: return F("SOLD_02::M_R_CONNECTTOOL");
    case M_R_TOOLERROR: return F("SOLD_02::M_R_TOOLERROR");
    case M_R_STATUSTOOL: return F("SOLD_02::M_R_STATUSTOOL");
    case M_W_SELECTTEMPVOLATILE: return F("SOLD_02::M_W_SELECTTEMPVOLATILE");
    case M_R_MOSTEMP: return F("SOLD_02::M_R_MOSTEMP");
    case M_R_DELAYTIME: return F("SOLD_02::M_R_DELAYTIME");
    case M_R_REMOTEMODE: return F("SOLD_02::M_R_REMOTEMODE");
    case M_W_REMOTEMODE: return F("SOLD_02::M_W_REMOTEMODE");
    case M_R_CONTIMODE: return F("SOLD_02::M_R_CONTIMODE");
    case M_W_CONTIMODE: return F("SOLD_02::M_W_CONTIMODE");
    case M_I_CONTIMODE: return F("SOLD_02::M_I_CONTIMODE");
    case M_R_ALARMMAXTEMP: return F("SOLD_02::M_R_ALARMMAXTEMP");
    case M_W_ALARMMAXTEMP: return F("SOLD_02::M_W_ALARMMAXTEMP");
    case M_R_ALARMMINTEMP: return F("SOLD_02::M_R_ALARMMINTEMP");
    case M_W_ALARMMINTEMP: return F("SOLD_02::M_W_ALARMMINTEMP");
    case M_R_ALARMTEMP: return F("SOLD_02::M_R_ALARMTEMP");
    case M_R_LOCK_PORT: return F("SOLD_02::M_R_LOCK_PORT");
    case M_W_LOCK_PORT: return F("SOLD_02::M_W_LOCK_PORT");
    case M_READSTARTFILE: return F("SOLD_02::M_READSTARTFILE");
    case M_READFILEBLOCK: return F("SOLD_02::M_READFILEBLOCK");
    case M_READENDOFFILE: return F("SOLD_02::M_READENDOFFILE");
    case M_WRITESTARTFILE: return F("SOLD_02::M_WRITESTARTFILE");
    case M_WRITEFILEBLOCK: return F("SOLD_02::M_WRITEFILEBLOCK");
    case M_WRITEENDOFFILE: return F("SOLD_02::M_WRITEENDOFFILE");
    case M_R_FILESCOUNT: return F("SOLD_02::M_R_FILESCOUNT");
    case M_R_GETFILENAME: return F("SOLD_02::M_R_GETFILENAME");
    case M_DELETEFILE: return F("SOLD_02::M_DELETEFILE");
    case M_R_SELECTEDFILENAME: return F("SOLD_02::M_R_SELECTEDFILENAME");
    case M_W_SELECTEDFILENAME: return F("SOLD_02::M_W_SELECTEDFILENAME");
    case M_R_QST_ACTIVATE: return F("SOLD_02::M_R_QST_ACTIVATE");
    case M_W_QST_ACTIVATE: return F("SOLD_02::M_W_QST_ACTIVATE");
    case M_R_QST_STATUS: return F("SOLD_02::M_R_QST_STATUS");
    case M_W_QST_STATUS: return F("SOLD_02::M_W_QST_STATUS");
    case M_R_TEMPUNIT: return F("SOLD_02::M_R_TEMPUNIT");
    case M_W_TEMPUNIT: return F("SOLD_02::M_W_TEMPUNIT");
    case M_R_MAXTEMP: return F("SOLD_02::M_R_MAXTEMP");
    case M_W_MAXTEMP: return F("SOLD_02::M_W_MAXTEMP");
    case M_R_MINTEMP: return F("SOLD_02::M_R_MINTEMP");
    case M_W_MINTEMP: return F("SOLD_02::M_W_MINTEMP");
    case M_R_POWERLIM: return F("SOLD_02::M_R_POWERLIM");
    case M_W_POWERLIM: return F("SOLD_02::M_W_POWERLIM");
    case M_R_PIN: return F("SOLD_02::M_R_PIN");
    case M_W_PIN: return F("SOLD_02::M_W_PIN");
    case M_R_STATERROR: return F("SOLD_02::M_R_STATERROR");
    case M_R_TRAFOTEMP: return F("SOLD_02::M_R_TRAFOTEMP");
    case M_RESETSTATION: return F("SOLD_02::M_RESETSTATION");
    case M_R_DEVICENAME: return F("SOLD_02::M_R_DEVICENAME");
    case M_W_DEVICENAME: return F("SOLD_02::M_W_DEVICENAME");
    case M_R_BEEP: return F("SOLD_02::M_R_BEEP");
    case M_W_BEEP: return F("SOLD_02::M_W_BEEP");
    case M_R_LANGUAGE: return F("SOLD_02::M_R_LANGUAGE");
    case M_W_LANGUAGE: return F("SOLD_02::M_W_LANGUAGE");
    case M_W_TYPEOFGROUND: return F("SOLD_02::M_W_TYPEOFGROUND");
    case M_R_TYPEOFGROUND: return F("SOLD_02::M_R_TYPEOFGROUND");
    case M_R_PLUGTIME: return F("SOLD_02::M_R_PLUGTIME");
    case M_W_PLUGTIME: return F("SOLD_02::M_W_PLUGTIME");
    case M_R_WORKTIME: return F("SOLD_02::M_R_WORKTIME");
    case M_W_WORKTIME: return F("SOLD_02::M_W_WORKTIME");
    case M_R_SLEEPTIME: return F("SOLD_02::M_R_SLEEPTIME");
    case M_W_SLEEPTIME: return F("SOLD_02::M_W_SLEEPTIME");
    case M_R_HIBERTIME: return F("SOLD_02::M_R_HIBERTIME");
    case M_W_HIBERTIME: return F("SOLD_02::M_W_HIBERTIME");
    case M_R_NOTOOLTIME: return F("SOLD_02::M_R_NOTOOLTIME");
    case M_W_NOTOOLTIME: return F("SOLD_02::M_W_NOTOOLTIME");
    case M_R_SLEEPCYCLES: return F("SOLD_02::M_R_SLEEPCYCLES");
    case M_W_SLEEPCYCLES: return F("SOLD_02::M_W_SLEEPCYCLES");
    case M_R_DESOLCYCLES: return F("SOLD_02::M_R_DESOLCYCLES");
    case M_W_DESOLCYCLES: return F("SOLD_02::M_W_DESOLCYCLES");
    case M_W_RESETCOUNTERS: return F("SOLD_02::M_W_RESETCOUNTERS");
    case M_R_PLUGTIMEP: return F("SOLD_02::M_R_PLUGTIMEP");
    case M_W_PLUGTIMEP: return F("SOLD_02::M_W_PLUGTIMEP");
    case M_R_WORKTIMEP: return F("SOLD_02::M_R_WORKTIMEP");
    case M_W_WORKTIMEP: return F("SOLD_02::M_W_WORKTIMEP");
    case M_R_SLEEPTIMEP: return F("SOLD_02::M_R_SLEEPTIMEP");
    case M_W_SLEEPTIMEP: return F("SOLD_02::M_W_SLEEPTIMEP");
    case M_R_HIBERTIMEP: return F("SOLD_02::M_R_HIBERTIMEP");
    case M_W_HIBERTIMEP: return F("SOLD_02::M_W_HIBERTIMEP");
    case M_R_NOTOOLTIMEP: return F("SOLD_02::M_R_NOTOOLTIMEP");
    case M_W_NOTOOLTIMEP: return F("SOLD_02::M_W_NOTOOLTIMEP");
    case M_R_SLEEPCYCLESP: return F("SOLD_02::M_R_SLEEPCYCLESP");
    case M_W_SLEEPCYCLESP: return F("SOLD_02::M_W_SLEEPCYCLESP");
    case M_R_DESOLCYCLESP: return F("SOLD_02::M_R_DESOLCYCLESP");
    case M_W_DESOLCYCLESP: return F("SOLD_02::M_W_DESOLCYCLESP");
    case M_W_RESETCOUNTERSP: return F("SOLD_02::M_W_RESETCOUNTERSP");
    case M_R_USB_CONNECTSTATUS: return F("SOLD_02::M_R_USB_CONNECTSTATUS");
    case M_W_USB_CONNECTSTATUS: return F("SOLD_02::M_W_USB_CONNECTSTATUS");
    case M_R_ETH_TCPIPCONFIG: return F("SOLD_02::M_R_ETH_TCPIPCONFIG");
    case M_W_ETH_TCPIPCONFIG: return F("SOLD_02::M_W_ETH_TCPIPCONFIG");
    case M_R_ETH_CONNECTSTATUS: return F("SOLD_02::M_R_ETH_CONNECTSTATUS");
    case M_W_ETH_CONNECTSTATUS: return F("SOLD_02::M_W_ETH_CONNECTSTATUS");
    case M_R_RBT_CONNCONFIG: return F("SOLD_02::M_R_RBT_CONNCONFIG");
    case M_W_RBT_CONNCONFIG: return F("SOLD_02::M_W_RBT_CONNCONFIG");
    case M_R_RBT_CONNECTSTATUS: return F("SOLD_02::M_R_RBT_CONNECTSTATUS");
    case M_W_RBT_CONNECTSTATUS: return F("SOLD_02::M_W_RBT_CONNECTSTATUS");
    case M_R_PERIPHCOUNT: return F("SOLD_02::M_R_PERIPHCOUNT");
    case M_R_PERIPHCONFIG: return F("SOLD_02::M_R_PERIPHCONFIG");
    case M_W_PERIPHCONFIG: return F("SOLD_02::M_W_PERIPHCONFIG");
    case M_R_PERIPHSTATUS: return F("SOLD_02::M_R_PERIPHSTATUS");
    case M_W_PERIPHSTATUS: return F("SOLD_02::M_W_PERIPHSTATUS");
    default: return F("SOLD_02::?");
  }
}



// ---- SOLD_01 (vollständig) ----
inline const __FlashStringHelper* name_SOLD_01(uint8_t c){
  using namespace jbc_cmd::SOLD_01;
  switch(c){
    case M_NULL: return F("SOLD_01::M_NULL");
    case M_EOT: return F("SOLD_01::M_EOT");
    case M_ACK: return F("SOLD_01::M_ACK");
    case M_NACK: return F("SOLD_01::M_NACK");
    case M_SYN: return F("SOLD_01::M_SYN");
    case M_R_CONNECTSTATUS: return F("SOLD_01::M_R_CONNECTSTATUS");
    case M_W_CONNECTSTATUS: return F("SOLD_01::M_W_CONNECTSTATUS");
    case M_RESET: return F("SOLD_01::M_RESET");
    case M_FIRMWARE: return F("SOLD_01::M_FIRMWARE");
    case M_INF_PORT: return F("SOLD_01::M_INF_PORT");
    case M_R_FIXTEMP: return F("SOLD_01::M_R_FIXTEMP");
    case M_W_FIXTEMP: return F("SOLD_01::M_W_FIXTEMP");
    case M_R_LEVELTEMP: return F("SOLD_01::M_R_LEVELTEMP");
    case M_W_LEVELTEMP: return F("SOLD_01::M_W_LEVELTEMP");
    case M_R_LEVEL1: return F("SOLD_01::M_R_LEVEL1");
    case M_W_LEVEL1: return F("SOLD_01::M_W_LEVEL1");
    case M_R_LEVEL2: return F("SOLD_01::M_R_LEVEL2");
    case M_W_LEVEL2: return F("SOLD_01::M_W_LEVEL2");
    case M_R_LEVEL3: return F("SOLD_01::M_R_LEVEL3");
    case M_W_LEVEL3: return F("SOLD_01::M_W_LEVEL3");
    case M_R_SLEEPDELAY: return F("SOLD_01::M_R_SLEEPDELAY");
    case M_W_SLEEPDELAY: return F("SOLD_01::M_W_SLEEPDELAY");
    case M_R_SLEEPTEMP: return F("SOLD_01::M_R_SLEEPTEMP");
    case M_W_SLEEPTEMP: return F("SOLD_01::M_W_SLEEPTEMP");
    case M_R_HIBERDELAY: return F("SOLD_01::M_R_HIBERDELAY");
    case M_W_HIBERDELAY: return F("SOLD_01::M_W_HIBERDELAY");
    case M_R_AJUSTTEMP: return F("SOLD_01::M_R_AJUSTTEMP");
    case M_W_AJUSTTEMP: return F("SOLD_01::M_W_AJUSTTEMP");
    case M_R_SELECTTEMP: return F("SOLD_01::M_R_SELECTTEMP");
    case M_W_SELECTTEMP: return F("SOLD_01::M_W_SELECTTEMP");
    case M_R_TIPTEMP: return F("SOLD_01::M_R_TIPTEMP");
    case M_R_CURRENT: return F("SOLD_01::M_R_CURRENT");
    case M_R_POWER: return F("SOLD_01::M_R_POWER");
    case M_R_CONNECTTOOL: return F("SOLD_01::M_R_CONNECTTOOL");
    case M_R_TOOLERROR: return F("SOLD_01::M_R_TOOLERROR");
    case M_R_STATUSTOOL: return F("SOLD_01::M_R_STATUSTOOL");
    case M_R_MOSTEMP: return F("SOLD_01::M_R_MOSTEMP");
    case M_R_DELAYTIME: return F("SOLD_01::M_R_DELAYTIME");
    case M_R_REMOTEMODE: return F("SOLD_01::M_R_REMOTEMODE");
    case M_W_REMOTEMODE: return F("SOLD_01::M_W_REMOTEMODE");
    case M_R_STATUSREMOTEMODE: return F("SOLD_01::M_R_STATUSREMOTEMODE");
    case M_R_CONTIMODE: return F("SOLD_01::M_R_CONTIMODE");
    case M_W_CONTIMODE: return F("SOLD_01::M_W_CONTIMODE");
    case M_I_CONTIMODE: return F("SOLD_01::M_I_CONTIMODE");
    case M_R_TEMPUNIT: return F("SOLD_01::M_R_TEMPUNIT");
    case M_W_TEMPUNIT: return F("SOLD_01::M_W_TEMPUNIT");
    case M_R_MAXTEMP: return F("SOLD_01::M_R_MAXTEMP");
    case M_W_MAXTEMP: return F("SOLD_01::M_W_MAXTEMP");
    case M_R_MINTEMP: return F("SOLD_01::M_R_MINTEMP");
    case M_W_MINTEMP: return F("SOLD_01::M_W_MINTEMP");
    case M_R_NITROMODE: return F("SOLD_01::M_R_NITROMODE");
    case M_W_NITROMODE: return F("SOLD_01::M_W_NITROMODE");
    case M_R_HELPTEXT: return F("SOLD_01::M_R_HELPTEXT");
    case M_W_HELPTEXT: return F("SOLD_01::M_W_HELPTEXT");
    case M_R_POWERLIM: return F("SOLD_01::M_R_POWERLIM");
    case M_W_POWERLIM: return F("SOLD_01::M_W_POWERLIM");
    case M_R_PIN: return F("SOLD_01::M_R_PIN");
    case M_W_PIN: return F("SOLD_01::M_W_PIN");
    case M_R_STATERROR: return F("SOLD_01::M_R_STATERROR");
    case M_R_TRAFOTEMP: return F("SOLD_01::M_R_TRAFOTEMP");
    case M_RESETSTATION: return F("SOLD_01::M_RESETSTATION");
    case M_R_DEVICENAME: return F("SOLD_01::M_R_DEVICENAME");
    case M_W_DEVICENAME: return F("SOLD_01::M_W_DEVICENAME");
    case M_R_BEEP: return F("SOLD_01::M_R_BEEP");
    case M_W_BEEP: return F("SOLD_01::M_W_BEEP");
    case M_R_LANGUAGE: return F("SOLD_01::M_R_LANGUAGE");
    case M_W_LANGUAGE: return F("SOLD_01::M_W_LANGUAGE");
    case M_R_TEMPERRORTRAFO: return F("SOLD_01::M_R_TEMPERRORTRAFO");
    case M_R_TEMPERRORMOS: return F("SOLD_01::M_R_TEMPERRORMOS");
    case M_R_DEVICEID: return F("SOLD_01::M_R_DEVICEID");
    case M_W_DEVICEID: return F("SOLD_01::M_W_DEVICEID");
    case M_R_PLUGTIME: return F("SOLD_01::M_R_PLUGTIME");
    case M_W_PLUGTIME: return F("SOLD_01::M_W_PLUGTIME");
    case M_R_WORKTIME: return F("SOLD_01::M_R_WORKTIME");
    case M_W_WORKTIME: return F("SOLD_01::M_W_WORKTIME");
    case M_R_SLEEPTIME: return F("SOLD_01::M_R_SLEEPTIME");
    case M_W_SLEEPTIME: return F("SOLD_01::M_W_SLEEPTIME");
    case M_R_HIBERTIME: return F("SOLD_01::M_R_HIBERTIME");
    case M_W_HIBERTIME: return F("SOLD_01::M_W_HIBERTIME");
    case M_R_NOTOOLTIME: return F("SOLD_01::M_R_NOTOOLTIME");
    case M_W_NOTOOLTIME: return F("SOLD_01::M_W_NOTOOLTIME");
    case M_R_SLEEPCYCLES: return F("SOLD_01::M_R_SLEEPCYCLES");
    case M_W_SLEEPCYCLES: return F("SOLD_01::M_W_SLEEPCYCLES");
    case M_R_DESOLCYCLES: return F("SOLD_01::M_R_DESOLCYCLES");
    case M_W_DESOLCYCLES: return F("SOLD_01::M_W_DESOLCYCLES");
    case M_R_QST_ACTIVATE: return F("SOLD_01::M_R_QST_ACTIVATE");
    case M_W_QST_ACTIVATE: return F("SOLD_01::M_W_QST_ACTIVATE");
    case M_R_QST_STATUS: return F("SOLD_01::M_R_QST_STATUS");
    case M_W_QST_STATUS: return F("SOLD_01::M_W_QST_STATUS");
    case M_R_LOCK_PORT: return F("SOLD_01::M_R_LOCK_PORT");
    case M_W_LOCK_PORT: return F("SOLD_01::M_W_LOCK_PORT");
    default: return F("SOLD_01::?");
  }
}

// ---- HA_02 (vollständig) ----
inline const __FlashStringHelper* name_HA_02(uint8_t c){
  using namespace jbc_cmd::HA_02;
  switch(c){
    case M_HS: return F("HA_02::M_HS");
    case M_EOT: return F("HA_02::M_EOT");
    case M_ACK: return F("HA_02::M_ACK");
    case M_NACK: return F("HA_02::M_NACK");
    case M_SYN: return F("HA_02::M_SYN");
    case M_R_DEVICEIDORIGINAL: return F("HA_02::M_R_DEVICEIDORIGINAL");
    case M_R_DISCOVER: return F("HA_02::M_R_DISCOVER");
    case M_R_DEVICEID: return F("HA_02::M_R_DEVICEID");
    case M_W_DEVICEID: return F("HA_02::M_W_DEVICEID");
    case M_RESET: return F("HA_02::M_RESET");
    case M_FIRMWARE: return F("HA_02::M_FIRMWARE");
    case M_CLEARMEMFLASH: return F("HA_02::M_CLEARMEMFLASH");
    case M_SENDMEMADDRESS: return F("HA_02::M_SENDMEMADDRESS");
    case M_SENDMEMDATA: return F("HA_02::M_SENDMEMDATA");
    case M_ENDPROGR: return F("HA_02::M_ENDPROGR");
    case M_ENDUPD: return F("HA_02::M_ENDUPD");
    case M_CONTINUEUPD: return F("HA_02::M_CONTINUEUPD");
    case M_CLEARING: return F("HA_02::M_CLEARING");
    case M_FORCEUPDATE: return F("HA_02::M_FORCEUPDATE");
    case M_INF_PORT: return F("HA_02::M_INF_PORT");
    case M_RESET_PORTTOOL: return F("HA_02::M_RESET_PORTTOOL");
    case M_R_PROFILEMODE: return F("HA_02::M_R_PROFILEMODE");
    case M_W_PROFILEMODE: return F("HA_02::M_W_PROFILEMODE");
    case M_R_HEATERSTATUS: return F("HA_02::M_R_HEATERSTATUS");
    case M_W_HEATERSTATUS: return F("HA_02::M_W_HEATERSTATUS");
    case M_R_SUCTIONSTATUS: return F("HA_02::M_R_SUCTIONSTATUS");
    case M_W_SUCTIONSTATUS: return F("HA_02::M_W_SUCTIONSTATUS");
    case M_R_EXTTCMODE: return F("HA_02::M_R_EXTTCMODE");
    case M_W_EXTTCMODE: return F("HA_02::M_W_EXTTCMODE");
    case M_R_LEVELSTEMPS: return F("HA_02::M_R_LEVELSTEMPS");
    case M_W_LEVELSTEMPS: return F("HA_02::M_W_LEVELSTEMPS");
    case M_R_AJUSTTEMP: return F("HA_02::M_R_AJUSTTEMP");
    case M_W_AJUSTTEMP: return F("HA_02::M_W_AJUSTTEMP");
    case M_R_TIMETOSTOP: return F("HA_02::M_R_TIMETOSTOP");
    case M_W_TIMETOSTOP: return F("HA_02::M_W_TIMETOSTOP");
    case M_R_STARTMODE: return F("HA_02::M_R_STARTMODE");
    case M_W_STARTMODE: return F("HA_02::M_W_STARTMODE");
    case M_R_SELECTTEMP: return F("HA_02::M_R_SELECTTEMP");
    case M_W_SELECTTEMP: return F("HA_02::M_W_SELECTTEMP");
    case M_R_AIRTEMP: return F("HA_02::M_R_AIRTEMP");
    case M_R_POWER: return F("HA_02::M_R_POWER");
    case M_R_CONNECTTOOL: return F("HA_02::M_R_CONNECTTOOL");
    case M_R_TOOLERROR: return F("HA_02::M_R_TOOLERROR");
    case M_R_STATUSTOOL: return F("HA_02::M_R_STATUSTOOL");
    case M_R_SELECTFLOW: return F("HA_02::M_R_SELECTFLOW");
    case M_W_SELECTFLOW: return F("HA_02::M_W_SELECTFLOW");
    case M_R_SELECTEXTTEMP: return F("HA_02::M_R_SELECTEXTTEMP");
    case M_W_SELECTEXTTEMP: return F("HA_02::M_W_SELECTEXTTEMP");
    case M_R_AIRFLOW: return F("HA_02::M_R_AIRFLOW");
    case M_R_EXTTCTEMP: return F("HA_02::M_R_EXTTCTEMP");
    case M_R_REMOTEMODE: return F("HA_02::M_R_REMOTEMODE");
    case M_W_REMOTEMODE: return F("HA_02::M_W_REMOTEMODE");
    case M_R_CONTIMODE: return F("HA_02::M_R_CONTIMODE");
    case M_W_CONTIMODE: return F("HA_02::M_W_CONTIMODE");
    case M_I_CONTIMODE: return F("HA_02::M_I_CONTIMODE");
    case M_READSTARTFILE: return F("HA_02::M_READSTARTFILE");
    case M_READFILEBLOCK: return F("HA_02::M_READFILEBLOCK");
    case M_READENDOFFILE: return F("HA_02::M_READENDOFFILE");
    case M_WRITESTARTFILE: return F("HA_02::M_WRITESTARTFILE");
    case M_WRITEFILEBLOCK: return F("HA_02::M_WRITEFILEBLOCK");
    case M_WRITEENDOFFILE: return F("HA_02::M_WRITEENDOFFILE");
    case M_R_FILESCOUNT: return F("HA_02::M_R_FILESCOUNT");
    case M_R_GETFILENAME: return F("HA_02::M_R_GETFILENAME");
    case M_DELETEFILE: return F("HA_02::M_DELETEFILE");
    case M_R_SELECTEDFILENAME: return F("HA_02::M_R_SELECTEDFILENAME");
    case M_W_SELECTEDFILENAME: return F("HA_02::M_W_SELECTEDFILENAME");
    case M_R_TEMPUNIT: return F("HA_02::M_R_TEMPUNIT");
    case M_W_TEMPUNIT: return F("HA_02::M_W_TEMPUNIT");
    case M_R_MAXMINTEMP: return F("HA_02::M_R_MAXMINTEMP");
    case M_W_MAXMINTEMP: return F("HA_02::M_W_MAXMINTEMP");
    case M_R_MAXMINFLOW: return F("HA_02::M_R_MAXMINFLOW");
    case M_W_MAXMINFLOW: return F("HA_02::M_W_MAXMINFLOW");
    case M_R_MAXMINEXTTEMP: return F("HA_02::M_R_MAXMINEXTTEMP");
    case M_W_MAXMINEXTTEMP: return F("HA_02::M_W_MAXMINEXTTEMP");
    case M_R_PINENABLED: return F("HA_02::M_R_PINENABLED");
    case M_W_PINENABLED: return F("HA_02::M_W_PINENABLED");
    case M_R_STATIONLOCKED: return F("HA_02::M_R_STATIONLOCKED");
    case M_W_STATIONLOCKED: return F("HA_02::M_W_STATIONLOCKED");
    case M_R_PIN: return F("HA_02::M_R_PIN");
    case M_W_PIN: return F("HA_02::M_W_PIN");
    case M_R_STATERROR: return F("HA_02::M_R_STATERROR");
    case M_RESETSTATION: return F("HA_02::M_RESETSTATION");
    case M_R_DEVICENAME: return F("HA_02::M_R_DEVICENAME");
    case M_W_DEVICENAME: return F("HA_02::M_W_DEVICENAME");
    case M_R_BEEP: return F("HA_02::M_R_BEEP");
    case M_W_BEEP: return F("HA_02::M_W_BEEP");
    case M_R_LANGUAGE: return F("HA_02::M_R_LANGUAGE");
    case M_W_LANGUAGE: return F("HA_02::M_W_LANGUAGE");
    case M_R_DATETIME: return F("HA_02::M_R_DATETIME");
    case M_W_DATETIME: return F("HA_02::M_W_DATETIME");
    case M_R_THEME: return F("HA_02::M_R_THEME");
    case M_W_THEME: return F("HA_02::M_W_THEME");
    case M_R_PLUGTIME: return F("HA_02::M_R_PLUGTIME");
    case M_W_PLUGTIME: return F("HA_02::M_W_PLUGTIME");
    case M_R_WORKTIME: return F("HA_02::M_R_WORKTIME");
    case M_W_WORKTIME: return F("HA_02::M_W_WORKTIME");
    case M_R_WORKCYCLES: return F("HA_02::M_R_WORKCYCLES");
    case M_W_WORKCYCLES: return F("HA_02::M_W_WORKCYCLES");
    case M_R_SUCTIONCYCLES: return F("HA_02::M_R_SUCTIONCYCLES");
    case M_W_SUCTIONCYCLES: return F("HA_02::M_W_SUCTIONCYCLES");
    case M_R_PLUGTIMEP: return F("HA_02::M_R_PLUGTIMEP");
    case M_W_PLUGTIMEP: return F("HA_02::M_W_PLUGTIMEP");
    case M_R_WORKTIMEP: return F("HA_02::M_R_WORKTIMEP");
    case M_W_WORKTIMEP: return F("HA_02::M_W_WORKTIMEP");
    case M_R_WORKCYCLESP: return F("HA_02::M_R_WORKCYCLESP");
    case M_W_WORKCYCLESP: return F("HA_02::M_W_WORKCYCLESP");
    case M_R_SUCTIONCYCLESP: return F("HA_02::M_R_SUCTIONCYCLESP");
    case M_W_SUCTIONCYCLESP: return F("HA_02::M_W_SUCTIONCYCLESP");
    case M_R_USB_CONNECTSTATUS: return F("HA_02::M_R_USB_CONNECTSTATUS");
    case M_W_USB_CONNECTSTATUS: return F("HA_02::M_W_USB_CONNECTSTATUS");
    case M_R_RBT_CONNCONFIG: return F("HA_02::M_R_RBT_CONNCONFIG");
    case M_W_RBT_CONNCONFIG: return F("HA_02::M_W_RBT_CONNCONFIG");
    case M_R_RBT_CONNECTSTATUS: return F("HA_02::M_R_RBT_CONNECTSTATUS");
    case M_W_RBT_CONNECTSTATUS: return F("HA_02::M_W_RBT_CONNECTSTATUS");
    default: return F("HA_02::?");
  }
}


// ---- PH_02 (vollständig) ----
inline const __FlashStringHelper* name_PH_02(uint8_t c){
  using namespace jbc_cmd::PH_02;
  switch(c){
    case M_HS: return F("PH_02::M_HS");
    case M_EOT: return F("PH_02::M_EOT");
    case M_ACK: return F("PH_02::M_ACK");
    case M_NACK: return F("PH_02::M_NACK");
    case M_SYN: return F("PH_02::M_SYN");
    case M_R_DEVICEIDORIGINAL: return F("PH_02::M_R_DEVICEIDORIGINAL");
    case M_R_DISCOVER: return F("PH_02::M_R_DISCOVER");
    case M_R_DEVICEID: return F("PH_02::M_R_DEVICEID");
    case M_W_DEVICEID: return F("PH_02::M_W_DEVICEID");
    case M_RESET: return F("PH_02::M_RESET");
    case M_FIRMWARE: return F("PH_02::M_FIRMWARE");
    case M_CLEARMEMFLASH: return F("PH_02::M_CLEARMEMFLASH");
    case M_SENDMEMADDRESS: return F("PH_02::M_SENDMEMADDRESS");
    case M_SENDMEMDATA: return F("PH_02::M_SENDMEMDATA");
    case M_ENDPROGR: return F("PH_02::M_ENDPROGR");
    case M_ENDUPD: return F("PH_02::M_ENDUPD");
    case M_CONTINUEUPD: return F("PH_02::M_CONTINUEUPD");
    case M_CLEARING: return F("PH_02::M_CLEARING");
    case M_FORCEUPDATE: return F("PH_02::M_FORCEUPDATE");
    case M_INF_PORT: return F("PH_02::M_INF_PORT");
    case M_R_WORKMODE: return F("PH_02::M_R_WORKMODE");
    case M_W_WORKMODE: return F("PH_02::M_W_WORKMODE");
    case M_R_HEATERSTATUS: return F("PH_02::M_R_HEATERSTATUS");
    case M_W_HEATERSTATUS: return F("PH_02::M_W_HEATERSTATUS");
    case M_R_EXTTCMODE: return F("PH_02::M_R_EXTTCMODE");
    case M_W_EXTTCMODE: return F("PH_02::M_W_EXTTCMODE");
    case M_R_TIMETOSTOP: return F("PH_02::M_R_TIMETOSTOP");
    case M_W_TIMETOSTOP: return F("PH_02::M_W_TIMETOSTOP");
    case M_R_SELECTTEMP: return F("PH_02::M_R_SELECTTEMP");
    case M_W_SELECTTEMP: return F("PH_02::M_W_SELECTTEMP");
    case M_R_SELECTPOWER: return F("PH_02::M_R_SELECTPOWER");
    case M_W_SELECTPOWER: return F("PH_02::M_W_SELECTPOWER");
    case M_R_WARNING: return F("PH_02::M_R_WARNING");
    case M_R_ACTIVEZONES: return F("PH_02::M_R_ACTIVEZONES");
    case M_W_ACTIVEZONES: return F("PH_02::M_W_ACTIVEZONES");
    case M_R_EXTTCTEMP: return F("PH_02::M_R_EXTTCTEMP");
    case M_R_REMOTEMODE: return F("PH_02::M_R_REMOTEMODE");
    case M_W_REMOTEMODE: return F("PH_02::M_W_REMOTEMODE");
    case M_R_CONTIMODE: return F("PH_02::M_R_CONTIMODE");
    case M_W_CONTIMODE: return F("PH_02::M_W_CONTIMODE");
    case M_I_CONTIMODE: return F("PH_02::M_I_CONTIMODE");
    case M_R_PROFILE: return F("PH_02::M_R_PROFILE");
    case M_W_PROFILE: return F("PH_02::M_W_PROFILE");
    case M_R_SETTINGSPROFILE: return F("PH_02::M_R_SETTINGSPROFILE");
    case M_W_SETTINGSPROFILE: return F("PH_02::M_W_SETTINGSPROFILE");
    case M_R_PROFILETEACH: return F("PH_02::M_R_PROFILETEACH");
    case M_W_PROFILETEACH: return F("PH_02::M_W_PROFILETEACH");
    case M_R_MAXMINPOWER: return F("PH_02::M_R_MAXMINPOWER");
    case M_W_MAXMINPOWER: return F("PH_02::M_W_MAXMINPOWER");
    case M_R_MAXMINTEMP: return F("PH_02::M_R_MAXMINTEMP");
    case M_W_MAXMINTEMP: return F("PH_02::M_W_MAXMINTEMP");
    case M_R_PINENABLED: return F("PH_02::M_R_PINENABLED");
    case M_W_PINENABLED: return F("PH_02::M_W_PINENABLED");
    case M_R_STATIONLOCKED: return F("PH_02::M_R_STATIONLOCKED");
    case M_W_STATIONLOCKED: return F("PH_02::M_W_STATIONLOCKED");
    case M_R_PIN: return F("PH_02::M_R_PIN");
    case M_W_PIN: return F("PH_02::M_W_PIN");
    case M_R_STATERROR: return F("PH_02::M_R_STATERROR");
    case M_RESETSTATION: return F("PH_02::M_RESETSTATION");
    case M_R_DEVICENAME: return F("PH_02::M_R_DEVICENAME");
    case M_W_DEVICENAME: return F("PH_02::M_W_DEVICENAME");
    case M_R_BEEP: return F("PH_02::M_R_BEEP");
    case M_W_BEEP: return F("PH_02::M_W_BEEP");
    case M_R_PLUGTIME: return F("PH_02::M_R_PLUGTIME");
    case M_W_PLUGTIME: return F("PH_02::M_W_PLUGTIME");
    case M_R_WORKTIME: return F("PH_02::M_R_WORKTIME");
    case M_W_WORKTIME: return F("PH_02::M_W_WORKTIME");
    case M_R_WORKCYCLES: return F("PH_02::M_R_WORKCYCLES");
    case M_W_WORKCYCLES: return F("PH_02::M_W_WORKCYCLES");
    case M_R_PLUGTIMEP: return F("PH_02::M_R_PLUGTIMEP");
    case M_W_PLUGTIMEP: return F("PH_02::M_W_PLUGTIMEP");
    case M_R_WORKTIMEP: return F("PH_02::M_R_WORKTIMEP");
    case M_W_WORKTIMEP: return F("PH_02::M_W_WORKTIMEP");
    case M_R_WORKCYCLESP: return F("PH_02::M_R_WORKCYCLESP");
    case M_W_WORKCYCLESP: return F("PH_02::M_W_WORKCYCLESP");
    case M_R_USB_CONNECTSTATUS: return F("PH_02::M_R_USB_CONNECTSTATUS");
    case M_W_USB_CONNECTSTATUS: return F("PH_02::M_W_USB_CONNECTSTATUS");
    case M_R_RBT_CONNCONFIG: return F("PH_02::M_R_RBT_CONNCONFIG");
    case M_W_RBT_CONNCONFIG: return F("PH_02::M_W_RBT_CONNCONFIG");
    case M_R_RBT_CONNECTSTATUS: return F("PH_02::M_R_RBT_CONNECTSTATUS");
    case M_W_RBT_CONNECTSTATUS: return F("PH_02::M_W_RBT_CONNECTSTATUS");
    default: return F("PH_02::?");
  }
}

// ---- FE_02 (vollständig) ----
inline const __FlashStringHelper* name_FE_02(uint8_t c){
  using namespace jbc_cmd::FE_02;
  switch(c){
    case M_HS: return F("FE_02::M_HS");
    case M_EOT: return F("FE_02::M_EOT");
    case M_ACK: return F("FE_02::M_ACK");
    case M_NACK: return F("FE_02::M_NACK");
    case M_SYN: return F("FE_02::M_SYN");
    case M_R_DEVICEIDORIGINAL: return F("FE_02::M_R_DEVICEIDORIGINAL");
    case M_R_DISCOVER: return F("FE_02::M_R_DISCOVER");
    case M_R_DEVICEID: return F("FE_02::M_R_DEVICEID");
    case M_W_DEVICEID: return F("FE_02::M_W_DEVICEID");
    case M_RESET: return F("FE_02::M_RESET");
    case M_FIRMWARE: return F("FE_02::M_FIRMWARE");
    case M_CLEARMEMFLASH: return F("FE_02::M_CLEARMEMFLASH");
    case M_SENDMEMADDRESS: return F("FE_02::M_SENDMEMADDRESS");
    case M_SENDMEMDATA: return F("FE_02::M_SENDMEMDATA");
    case M_ENDPROGR: return F("FE_02::M_ENDPROGR");
    case M_ENDUPD: return F("FE_02::M_ENDUPD");
    case M_CONTINUEUPD: return F("FE_02::M_CONTINUEUPD");
    case M_CLEARING: return F("FE_02::M_CLEARING");
    case M_FORCEUPDATE: return F("FE_02::M_FORCEUPDATE");
    case M_R_SUCTIONLEVEL: return F("FE_02::M_R_SUCTIONLEVEL");
    case M_W_SUCTIONLEVEL: return F("FE_02::M_W_SUCTIONLEVEL");
    case M_R_FLOW: return F("FE_02::M_R_FLOW");
    case M_R_SPEED: return F("FE_02::M_R_SPEED");
    case M_R_SELECTFLOW: return F("FE_02::M_R_SELECTFLOW");
    case M_W_SELECTFLOW: return F("FE_02::M_W_SELECTFLOW");
    case M_R_STANDINTAKES: return F("FE_02::M_R_STANDINTAKES");
    case M_W_STANDINTAKES: return F("FE_02::M_W_STANDINTAKES");
    case M_R_INTAKEACTIVATION: return F("FE_02::M_R_INTAKEACTIVATION");
    case M_W_INTAKEACTIVATION: return F("FE_02::M_W_INTAKEACTIVATION");
    case M_R_SUCTIONDELAY: return F("FE_02::M_R_SUCTIONDELAY");
    case M_W_SUCTIONDELAY: return F("FE_02::M_W_SUCTIONDELAY");
    case M_R_DELAYTIME: return F("FE_02::M_R_DELAYTIME");
    case M_R_ACTIVATIONPEDAL: return F("FE_02::M_R_ACTIVATIONPEDAL");
    case M_W_ACTIVATIONPEDAL: return F("FE_02::M_W_ACTIVATIONPEDAL");
    case M_R_PEDALMODE: return F("FE_02::M_R_PEDALMODE");
    case M_W_PEDALMODE: return F("FE_02::M_W_PEDALMODE");
    case M_R_FILTERSTATUS: return F("FE_02::M_R_FILTERSTATUS");
    case M_R_RESETFILTER: return F("FE_02::M_R_RESETFILTER");
    case M_R_CONNECTEDPEDAL: return F("FE_02::M_R_CONNECTEDPEDAL");
    case M_RESETSTATION: return F("FE_02::M_RESETSTATION");
    case M_R_PIN: return F("FE_02::M_R_PIN");
    case M_W_PIN: return F("FE_02::M_W_PIN");
    case M_R_STATIONLOCKED: return F("FE_02::M_R_STATIONLOCKED");
    case M_W_STATIONLOCKED: return F("FE_02::M_W_STATIONLOCKED");
    case M_R_BEEP: return F("FE_02::M_R_BEEP");
    case M_W_BEEP: return F("FE_02::M_W_BEEP");
    case M_R_CONTINUOUSSUCTION: return F("FE_02::M_R_CONTINUOUSSUCTION");
    case M_W_CONTINUOUSSUCTION: return F("FE_02::M_W_CONTINUOUSSUCTION");
    case M_R_STATERROR: return F("FE_02::M_R_STATERROR");
    case M_R_DEVICENAME: return F("FE_02::M_R_DEVICENAME");
    case M_W_DEVICENAME: return F("FE_02::M_W_DEVICENAME");
    case M_R_PINENABLED: return F("FE_02::M_R_PINENABLED");
    case M_W_PINENABLED: return F("FE_02::M_W_PINENABLED");
    case M_W_WORKINTAKES: return F("FE_02::M_W_WORKINTAKES");
    case M_R_COUNTERS: return F("FE_02::M_R_COUNTERS");
    case M_R_RESETCOUNTERS: return F("FE_02::M_R_RESETCOUNTERS");
    case M_R_COUNTERSP: return F("FE_02::M_R_COUNTERSP");
    case M_R_RESETCOUNTERSP: return F("FE_02::M_R_RESETCOUNTERSP");
    case M_R_USB_CONNECTSTATUS: return F("FE_02::M_R_USB_CONNECTSTATUS");
    case M_W_USB_CONNECTSTATUS: return F("FE_02::M_W_USB_CONNECTSTATUS");
    case M_R_RBT_CONNCONFIG: return F("FE_02::M_R_RBT_CONNCONFIG");
    case M_W_RBT_CONNCONFIG: return F("FE_02::M_W_RBT_CONNCONFIG");
    case M_R_RBT_CONNECTSTATUS: return F("FE_02::M_R_RBT_CONNECTSTATUS");
    case M_W_RBT_CONNECTSTATUS: return F("FE_02::M_W_RBT_CONNECTSTATUS");
    default: return F("FE_02::?");
  }
}

// ---- SF_02 (vollständig) ----
inline const __FlashStringHelper* name_SF_02(uint8_t c){
  using namespace jbc_cmd::SF_02;
  switch(c){
    case M_HS: return F("SF_02::M_HS");
    case M_EOT: return F("SF_02::M_EOT");
    case M_ACK: return F("SF_02::M_ACK");
    case M_NACK: return F("SF_02::M_NACK");
    case M_SYN: return F("SF_02::M_SYN");
    case M_R_DEVICEIDORIGINAL: return F("SF_02::M_R_DEVICEIDORIGINAL");
    case M_R_DISCOVER: return F("SF_02::M_R_DISCOVER");
    case M_R_DEVICEID: return F("SF_02::M_R_DEVICEID");
    case M_W_DEVICEID: return F("SF_02::M_W_DEVICEID");
    case M_RESET: return F("SF_02::M_RESET");
    case M_FIRMWARE: return F("SF_02::M_FIRMWARE");
    case M_CLEARMEMFLASH: return F("SF_02::M_CLEARMEMFLASH");
    case M_SENDMEMADDRESS: return F("SF_02::M_SENDMEMADDRESS");
    case M_SENDMEMDATA: return F("SF_02::M_SENDMEMDATA");
    case M_ENDPROGR: return F("SF_02::M_ENDPROGR");
    case M_ENDUPD: return F("SF_02::M_ENDUPD");
    case M_CONTINUEUPD: return F("SF_02::M_CONTINUEUPD");
    case M_CLEARING: return F("SF_02::M_CLEARING");
    case M_FORCEUPDATE: return F("SF_02::M_FORCEUPDATE");
    case M_R_DISPENSERMODE: return F("SF_02::M_R_DISPENSERMODE");
    case M_W_DISPENSERMODE: return F("SF_02::M_W_DISPENSERMODE");
    case M_R_PROGRAM: return F("SF_02::M_R_PROGRAM");
    case M_W_PROGRAM: return F("SF_02::M_W_PROGRAM");
    case M_R_PROGRAMLIST: return F("SF_02::M_R_PROGRAMLIST");
    case M_W_PROGRAMLIST: return F("SF_02::M_W_PROGRAMLIST");
    case M_R_SPEED: return F("SF_02::M_R_SPEED");
    case M_W_SPEED: return F("SF_02::M_W_SPEED");
    case M_R_LENGTH: return F("SF_02::M_R_LENGTH");
    case M_W_LENGTH: return F("SF_02::M_W_LENGTH");
    case M_R_STARTFEEDING: return F("SF_02::M_R_STARTFEEDING");
    case M_R_STOPTFEEDING: return F("SF_02::M_R_STOPTFEEDING");
    case M_R_FEEDING: return F("SF_02::M_R_FEEDING");
    case M_R_BACKWARDMODE: return F("SF_02::M_R_BACKWARDMODE");
    case M_W_BACKWARDMODE: return F("SF_02::M_W_BACKWARDMODE");
    case M_RESETSTATION: return F("SF_02::M_RESETSTATION");
    case M_R_PIN: return F("SF_02::M_R_PIN");
    case M_W_PIN: return F("SF_02::M_W_PIN");
    case M_R_STATIONLOCKED: return F("SF_02::M_R_STATIONLOCKED");
    case M_W_STATIONLOCKED: return F("SF_02::M_W_STATIONLOCKED");
    case M_R_BEEP: return F("SF_02::M_R_BEEP");
    case M_W_BEEP: return F("SF_02::M_W_BEEP");
    case M_R_LENGTHUNIT: return F("SF_02::M_R_LENGTHUNIT");
    case M_W_LENGTHUNIT: return F("SF_02::M_W_LENGTHUNIT");
    case M_R_STATERROR: return F("SF_02::M_R_STATERROR");
    case M_R_RESETERROR: return F("SF_02::M_R_RESETERROR");
    case M_R_DEVICENAME: return F("SF_02::M_R_DEVICENAME");
    case M_W_DEVICENAME: return F("SF_02::M_W_DEVICENAME");
    case M_R_TOOLENABLED: return F("SF_02::M_R_TOOLENABLED");
    case M_W_TOOLENABLED: return F("SF_02::M_W_TOOLENABLED");
    case M_R_PINENABLED: return F("SF_02::M_R_PINENABLED");
    case M_W_PINENABLED: return F("SF_02::M_W_PINENABLED");
    case M_R_COUNTERS: return F("SF_02::M_R_COUNTERS");
    case M_R_RESETCOUNTERS: return F("SF_02::M_R_RESETCOUNTERS");
    case M_R_COUNTERSP: return F("SF_02::M_R_COUNTERSP");
    case M_R_RESETCOUNTERSP: return F("SF_02::M_R_RESETCOUNTERSP");
    case M_R_USB_CONNECTSTATUS: return F("SF_02::M_R_USB_CONNECTSTATUS");
    case M_W_USB_CONNECTSTATUS: return F("SF_02::M_W_USB_CONNECTSTATUS");
    case M_R_RBT_CONNCONFIG: return F("SF_02::M_R_RBT_CONNCONFIG");
    case M_W_RBT_CONNCONFIG: return F("SF_02::M_W_RBT_CONNCONFIG");
    case M_R_RBT_CONNECTSTATUS: return F("SF_02::M_R_RBT_CONNECTSTATUS");
    case M_W_RBT_CONNECTSTATUS: return F("SF_02::M_W_RBT_CONNECTSTATUS");
    default: return F("SF_02::?");
  }
}

// unified selector
inline const __FlashStringHelper* cmd_name(Backend b, uint8_t c){
  switch(b){
    case BK_SOLD:  return name_SOLD_02(c);
    case BK_SOLD1: return name_SOLD_01(c);
    case BK_HA:    return name_HA_02(c);
    case BK_FE:    return name_FE_02(c);
    case BK_PH:    return name_PH_02(c);
    case BK_SF:    return name_SF_02(c);
    default:       return name_BASE(c);
  }
}


// ---- Backend detection ----
inline Backend backend_from_model(const String& model){
  if (model.length()>=2){
    if (model.startsWith("DD")) return BK_SOLD;  // DDE, DDU...
    if (model.startsWith("JT")) return BK_HA;    // JT, JTE, JTSE...
    if (model.startsWith("FE")) return BK_FE;
    if (model.startsWith("PH")) return BK_PH;
    if (model.startsWith("SF")) return BK_SF;
  }
  return BK_UNKNOWN;
}

// fwline format "pp:MODEL_VARIANT:fw:hw"
inline Backend backend_from_fwline(const String& fwline){
  int a = fwline.indexOf(':');
  int b = fwline.indexOf(':', a+1);
  if (a>0 && b>a){
    String proto = fwline.substring(0, a);
    String model = fwline.substring(a+1, b);
    if (proto == "01") return BK_SOLD1;
    Backend bk = backend_from_model(model);
    if (bk != BK_UNKNOWN) return bk;
    if (proto == "02") return BK_SOLD; // conservative fallback
  }
  return BK_UNKNOWN;
}

} // namespace jbc_name

// ---- convenience forwarders (optional, so .ino kann unqualifiziert rufen) ----
inline Backend backend_from_fwline(const String& s){ return jbc_name::backend_from_fwline(s); }
inline Backend backend_from_model (const String& s){ return jbc_name::backend_from_model(s); }

// ---- Logging Macros (pretty print with enums) ----
#define JBC_PRINT_TX(bk, ctrl, fid, dst) do {                 \
  Serial.print(F("[TX] "));                                    \
  Serial.print(jbc_name::cmd_name((bk),(ctrl)));               \
  Serial.print(F(" (0x")); Serial.print((ctrl),HEX);           \
  Serial.print(F(") fid=")); Serial.print((fid));              \
  Serial.print(F(" dst=0x")); Serial.println((dst),HEX);       \
} while(0)

#define JBC_PRINT_RX(bk, src, ctrl, fid, len) do {             \
  Serial.print(F("[RX] src=0x")); Serial.print((src),HEX);      \
  Serial.print(F(" "));                                         \
  Serial.print(jbc_name::cmd_name((bk),(ctrl)));                \
  Serial.print(F(" fid=")); Serial.print((fid));                \
  Serial.print(F(" len=")); Serial.println((len));              \
} while(0)
