#pragma once


#define paramDelay(time) (uint32_t)( \
            (time & 0xC000) == 0xC000 ? (time & 0x3FFF) * 100 : \
            (time & 0xC000) == 0x0000 ? (time & 0x3FFF) * 1000 : \
            (time & 0xC000) == 0x4000 ? (time & 0x3FFF) * 60000 : \
            (time & 0xC000) == 0x8000 ? ((time & 0x3FFF) > 1000 ? 3600000 : \
                                         (time & 0x3FFF) * 3600000 ) : 0 )
                                             
#define ETS_ModuleId_NONE 0
#define ETS_ModuleId_BASE 1
#define ETS_ModuleId_NET 2
#define ETS_ModuleId_UCT 3
#define ETS_ModuleId_SENS 4
#define ETS_ModuleId_AIR 5
#define ETS_ModuleId_LOG 6
#define ETS_ModuleId_FCB 7
#define MAIN_FirmwareName "Klimaanlagen KNX Adapter"
#define MAIN_OpenKnxId 0xAE
#define MAIN_ApplicationNumber 54
#define MAIN_ApplicationVersion 9
#define MAIN_FirmwareRevision 0
#define MAIN_ApplicationEncoding iso-8859-15
#define MAIN_ParameterSize 9298
#define MAIN_MaxKoNumber 465
#define MAIN_OrderNumber "MGKnxAIR"
#define BASE_ModuleVersion 24
#define NET_ModuleVersion 6
#define UCT_ModuleVersion 5
#define SENS_ModuleVersion 75
#define LOG_ModuleVersion 67
#define FCB_ModuleVersion 10
// Parameter with single occurrence


#define BASE_StartupDelayBase                     0      // 2 Bits, Bit 7-6
#define     BASE_StartupDelayBaseMask 0xC0
#define     BASE_StartupDelayBaseShift 6
#define BASE_StartupDelayTime                     0      // 14 Bits, Bit 13-0
#define     BASE_StartupDelayTimeMask 0x3FFF
#define     BASE_StartupDelayTimeShift 0
#define BASE_HeartbeatDelayBase                   2      // 2 Bits, Bit 7-6
#define     BASE_HeartbeatDelayBaseMask 0xC0
#define     BASE_HeartbeatDelayBaseShift 6
#define BASE_HeartbeatDelayTime                   2      // 14 Bits, Bit 13-0
#define     BASE_HeartbeatDelayTimeMask 0x3FFF
#define     BASE_HeartbeatDelayTimeShift 0
#define BASE_Timezone                             4      // 5 Bits, Bit 7-3
#define     BASE_TimezoneMask 0xF8
#define     BASE_TimezoneShift 3
#define BASE_CombinedTimeDate                     4      // 1 Bit, Bit 2
#define     BASE_CombinedTimeDateMask 0x04
#define     BASE_CombinedTimeDateShift 2
#define BASE_SummertimeAll                        4      // 2 Bits, Bit 1-0
#define     BASE_SummertimeAllMask 0x03
#define     BASE_SummertimeAllShift 0
#define BASE_SummertimeDE                         4      // 2 Bits, Bit 1-0
#define     BASE_SummertimeDEMask 0x03
#define     BASE_SummertimeDEShift 0
#define BASE_SummertimeWorld                      4      // 2 Bits, Bit 1-0
#define     BASE_SummertimeWorldMask 0x03
#define     BASE_SummertimeWorldShift 0
#define BASE_SummertimeKO                         4      // 2 Bits, Bit 1-0
#define     BASE_SummertimeKOMask 0x03
#define     BASE_SummertimeKOShift 0
#define BASE_TimezoneCustom                       5      // char*, 63 Byte
#define     BASE_TimezoneCustomLength 63
#define BASE_Latitude                            69      // float (4 Byte)
#define BASE_Longitude                           73      // float (4 Byte)
#define BASE_Diagnose                            78      // 1 Bit, Bit 7
#define     BASE_DiagnoseMask 0x80
#define     BASE_DiagnoseShift 7
#define BASE_Watchdog                            78      // 1 Bit, Bit 6
#define     BASE_WatchdogMask 0x40
#define     BASE_WatchdogShift 6
#define BASE_ReadTimeDate                        78      // 1 Bit, Bit 5
#define     BASE_ReadTimeDateMask 0x20
#define     BASE_ReadTimeDateShift 5
#define BASE_HeartbeatExtended                   78      // 1 Bit, Bit 4
#define     BASE_HeartbeatExtendedMask 0x10
#define     BASE_HeartbeatExtendedShift 4
#define BASE_InternalTime                        78      // 1 Bit, Bit 3
#define     BASE_InternalTimeMask 0x08
#define     BASE_InternalTimeShift 3
#define BASE_ManualSave                          78      // 3 Bits, Bit 2-0
#define     BASE_ManualSaveMask 0x07
#define     BASE_ManualSaveShift 0
#define BASE_PeriodicSave                        79      // 8 Bits, Bit 7-0
#define BASE_Info1LedFunc                        80      // 16 Bits, Bit 15-0
#define BASE_Info2LedFunc                        82      // 16 Bits, Bit 15-0
#define BASE_Info3LedFunc                        84      // 16 Bits, Bit 15-0
#define BASE_DefaultLedFunc                      86      // 1 Bit, Bit 7
#define     BASE_DefaultLedFuncMask 0x80
#define     BASE_DefaultLedFuncShift 7
#define BASE_Dummy                               109      // uint8_t
#define BASE_ModuleEnabled_NET                   110      // 1 Bit, Bit 6
#define     BASE_ModuleEnabled_NETMask 0x40
#define     BASE_ModuleEnabled_NETShift 6
#define BASE_ModuleEnabled_UCT                   110      // 1 Bit, Bit 5
#define     BASE_ModuleEnabled_UCTMask 0x20
#define     BASE_ModuleEnabled_UCTShift 5
#define BASE_ModuleEnabled_SENS                  110      // 1 Bit, Bit 4
#define     BASE_ModuleEnabled_SENSMask 0x10
#define     BASE_ModuleEnabled_SENSShift 4
#define BASE_ModuleEnabled_AIR                   110      // 1 Bit, Bit 3
#define     BASE_ModuleEnabled_AIRMask 0x08
#define     BASE_ModuleEnabled_AIRShift 3
#define BASE_ModuleEnabled_LOG                   110      // 1 Bit, Bit 2
#define     BASE_ModuleEnabled_LOGMask 0x04
#define     BASE_ModuleEnabled_LOGShift 2
#define BASE_ModuleEnabled_FCB                   110      // 1 Bit, Bit 1
#define     BASE_ModuleEnabled_FCBMask 0x02
#define     BASE_ModuleEnabled_FCBShift 1

// Zeitbasis
#define ParamBASE_StartupDelayBase                    ((knx.paramByte(BASE_StartupDelayBase) & BASE_StartupDelayBaseMask) >> BASE_StartupDelayBaseShift)
// Zeit
#define ParamBASE_StartupDelayTime                    (knx.paramWord(BASE_StartupDelayTime) & BASE_StartupDelayTimeMask)
// Zeit (in Millisekunden)
#define ParamBASE_StartupDelayTimeMS                  (paramDelay(knx.paramWord(BASE_StartupDelayTime)))
// Zeitbasis
#define ParamBASE_HeartbeatDelayBase                  ((knx.paramByte(BASE_HeartbeatDelayBase) & BASE_HeartbeatDelayBaseMask) >> BASE_HeartbeatDelayBaseShift)
// Zeit
#define ParamBASE_HeartbeatDelayTime                  (knx.paramWord(BASE_HeartbeatDelayTime) & BASE_HeartbeatDelayTimeMask)
// Zeit (in Millisekunden)
#define ParamBASE_HeartbeatDelayTimeMS                (paramDelay(knx.paramWord(BASE_HeartbeatDelayTime)))
// Zeitzone
#define ParamBASE_Timezone                            ((knx.paramByte(BASE_Timezone) & BASE_TimezoneMask) >> BASE_TimezoneShift)
// Empfangen über
#define ParamBASE_CombinedTimeDate                    ((bool)(knx.paramByte(BASE_CombinedTimeDate) & BASE_CombinedTimeDateMask))
// Sommerzeit ermitteln durch
#define ParamBASE_SummertimeAll                       (knx.paramByte(BASE_SummertimeAll) & BASE_SummertimeAllMask)
// Sommerzeit ermitteln durch
#define ParamBASE_SummertimeDE                        (knx.paramByte(BASE_SummertimeDE) & BASE_SummertimeDEMask)
// Sommerzeit ermitteln durch
#define ParamBASE_SummertimeWorld                     (knx.paramByte(BASE_SummertimeWorld) & BASE_SummertimeWorldMask)
// Sommerzeit ermitteln durch
#define ParamBASE_SummertimeKO                        (knx.paramByte(BASE_SummertimeKO) & BASE_SummertimeKOMask)
// POSIX TZ-String
#define ParamBASE_TimezoneCustom                      (knx.paramData(BASE_TimezoneCustom))
#define ParamBASE_TimezoneCustomStr                   (knx.paramString(BASE_TimezoneCustom, BASE_TimezoneCustomLength))
// Breitengrad
#define ParamBASE_Latitude                            (knx.paramFloat(BASE_Latitude, Float_Enc_IEEE754Single))
// Längengrad
#define ParamBASE_Longitude                           (knx.paramFloat(BASE_Longitude, Float_Enc_IEEE754Single))
// Diagnoseobjekt anzeigen
#define ParamBASE_Diagnose                            ((bool)(knx.paramByte(BASE_Diagnose) & BASE_DiagnoseMask))
// Watchdog aktivieren
#define ParamBASE_Watchdog                            ((bool)(knx.paramByte(BASE_Watchdog) & BASE_WatchdogMask))
// Bei Neustart vom Bus lesen
#define ParamBASE_ReadTimeDate                        ((bool)(knx.paramByte(BASE_ReadTimeDate) & BASE_ReadTimeDateMask))
// Erweitertes "In Betrieb"
#define ParamBASE_HeartbeatExtended                   ((bool)(knx.paramByte(BASE_HeartbeatExtended) & BASE_HeartbeatExtendedMask))
// InternalTime
#define ParamBASE_InternalTime                        ((bool)(knx.paramByte(BASE_InternalTime) & BASE_InternalTimeMask))
// Manuelles speichern
#define ParamBASE_ManualSave                          (knx.paramByte(BASE_ManualSave) & BASE_ManualSaveMask)
// Zyklisches speichern
#define ParamBASE_PeriodicSave                        (knx.paramByte(BASE_PeriodicSave))
// Info1
#define ParamBASE_Info1LedFunc                        (knx.paramWord(BASE_Info1LedFunc))
// Info2
#define ParamBASE_Info2LedFunc                        (knx.paramWord(BASE_Info2LedFunc))
// Info3
#define ParamBASE_Info3LedFunc                        (knx.paramWord(BASE_Info3LedFunc))
// 
#define ParamBASE_DefaultLedFunc                      ((bool)(knx.paramByte(BASE_DefaultLedFunc) & BASE_DefaultLedFuncMask))
// 
#define ParamBASE_Dummy                               (knx.paramByte(BASE_Dummy))
// NET
#define ParamBASE_ModuleEnabled_NET                   ((bool)(knx.paramByte(BASE_ModuleEnabled_NET) & BASE_ModuleEnabled_NETMask))
// UCT
#define ParamBASE_ModuleEnabled_UCT                   ((bool)(knx.paramByte(BASE_ModuleEnabled_UCT) & BASE_ModuleEnabled_UCTMask))
// SENS
#define ParamBASE_ModuleEnabled_SENS                  ((bool)(knx.paramByte(BASE_ModuleEnabled_SENS) & BASE_ModuleEnabled_SENSMask))
// AIR
#define ParamBASE_ModuleEnabled_AIR                   ((bool)(knx.paramByte(BASE_ModuleEnabled_AIR) & BASE_ModuleEnabled_AIRMask))
// LOG
#define ParamBASE_ModuleEnabled_LOG                   ((bool)(knx.paramByte(BASE_ModuleEnabled_LOG) & BASE_ModuleEnabled_LOGMask))
// FCB
#define ParamBASE_ModuleEnabled_FCB                   ((bool)(knx.paramByte(BASE_ModuleEnabled_FCB) & BASE_ModuleEnabled_FCBMask))

#define BASE_KoHeartbeat 1
#define BASE_KoTime 2
#define BASE_KoDate 3
#define BASE_KoDateTime 4
#define BASE_KoIsSummertime 5
#define BASE_KoManualSave 6
#define BASE_KoDiagnose 7

// In Betrieb
#define KoBASE_Heartbeat                           (knx.getGroupObject(BASE_KoHeartbeat))
// Uhrzeit
#define KoBASE_Time                                (knx.getGroupObject(BASE_KoTime))
// Datum
#define KoBASE_Date                                (knx.getGroupObject(BASE_KoDate))
// Uhrzeit/Datum
#define KoBASE_DateTime                            (knx.getGroupObject(BASE_KoDateTime))
// Sommerzeit aktiv
#define KoBASE_IsSummertime                        (knx.getGroupObject(BASE_KoIsSummertime))
// Speichern
#define KoBASE_ManualSave                          (knx.getGroupObject(BASE_KoManualSave))
// Diagnose
#define KoBASE_Diagnose                            (knx.getGroupObject(BASE_KoDiagnose))

#define NET_HostAddress                         114      // IP address, 4 Byte
#define NET_SubnetMask                          118      // IP address, 4 Byte
#define NET_GatewayAddress                      122      // IP address, 4 Byte
#define NET_NameserverAddress                   126      // IP address, 4 Byte
#define NET_CustomHostname                      130      // 1 Bit, Bit 7
#define     NET_CustomHostnameMask 0x80
#define     NET_CustomHostnameShift 7
#define NET_StaticIP                            130      // 1 Bit, Bit 6
#define     NET_StaticIPMask 0x40
#define     NET_StaticIPShift 6
#define NET_mDNS                                131      // 1 Bit, Bit 7
#define     NET_mDNSMask 0x80
#define     NET_mDNSShift 7
#define NET_HTTP                                131      // 1 Bit, Bit 6
#define     NET_HTTPMask 0x40
#define     NET_HTTPShift 6
#define NET_NTP                                 131      // 1 Bit, Bit 5
#define     NET_NTPMask 0x20
#define     NET_NTPShift 5
#define NET_OTAUpdate                           131      // 2 Bits, Bit 4-3
#define     NET_OTAUpdateMask 0x18
#define     NET_OTAUpdateShift 3
#define NET_MQTT                                131      // 1 Bit, Bit 2
#define     NET_MQTTMask 0x04
#define     NET_MQTTShift 2
#define NET_HostName                            132      // char*, 24 Byte
#define     NET_HostNameLength 24
#define NET_LanMode                             173      // 4 Bits, Bit 7-4
#define     NET_LanModeMask 0xF0
#define     NET_LanModeShift 4
#define NET_NTPServer                           174      // char*, 50 Byte
#define     NET_NTPServerLength 50
#define NET_MQTTServer                          225      // char*, 20 Byte
#define     NET_MQTTServerLength 20
#define NET_MQTTUsername                        246      // char*, 20 Byte
#define     NET_MQTTUsernameLength 20
#define NET_MQTTPassword                        267      // char*, 20 Byte
#define     NET_MQTTPasswordLength 20
#define NET_MQTTPrefix                          288      // char*, 20 Byte
#define     NET_MQTTPrefixLength 20
#define NET_MQTTPort                            309      // uint16_t
#define NET_MQTTTPRawData                       311      // 1 Bit, Bit 7
#define     NET_MQTTTPRawDataMask 0x80
#define     NET_MQTTTPRawDataShift 7
#define NET_MQTTMode                            311      // 1 Bit, Bit 6
#define     NET_MQTTModeMask 0x40
#define     NET_MQTTModeShift 6

// IP-Adresse
#define ParamNET_HostAddress                         (knx.paramInt(NET_HostAddress))
// Subnetzsmaske
#define ParamNET_SubnetMask                          (knx.paramInt(NET_SubnetMask))
// Standardgateway
#define ParamNET_GatewayAddress                      (knx.paramInt(NET_GatewayAddress))
// Nameserver
#define ParamNET_NameserverAddress                   (knx.paramInt(NET_NameserverAddress))
// Hostname anpassen
#define ParamNET_CustomHostname                      ((bool)(knx.paramByte(NET_CustomHostname) & NET_CustomHostnameMask))
// DHCP
#define ParamNET_StaticIP                            ((bool)(knx.paramByte(NET_StaticIP) & NET_StaticIPMask))
// mDNS
#define ParamNET_mDNS                                ((bool)(knx.paramByte(NET_mDNS) & NET_mDNSMask))
// Weberver (BETA)
#define ParamNET_HTTP                                ((bool)(knx.paramByte(NET_HTTP) & NET_HTTPMask))
// NTP-Client
#define ParamNET_NTP                                 ((bool)(knx.paramByte(NET_NTP) & NET_NTPMask))
// OTA-Update
#define ParamNET_OTAUpdate                           ((knx.paramByte(NET_OTAUpdate) & NET_OTAUpdateMask) >> NET_OTAUpdateShift)
// MQTT
#define ParamNET_MQTT                                ((bool)(knx.paramByte(NET_MQTT) & NET_MQTTMask))
// Hostname
#define ParamNET_HostName                            (knx.paramData(NET_HostName))
#define ParamNET_HostNameStr                         (knx.paramString(NET_HostName, NET_HostNameLength))
// LAN-Modus
#define ParamNET_LanMode                             ((knx.paramByte(NET_LanMode) & NET_LanModeMask) >> NET_LanModeShift)
// Zeitserver
#define ParamNET_NTPServer                           (knx.paramData(NET_NTPServer))
#define ParamNET_NTPServerStr                        (knx.paramString(NET_NTPServer, NET_NTPServerLength))
// Server
#define ParamNET_MQTTServer                          (knx.paramData(NET_MQTTServer))
#define ParamNET_MQTTServerStr                       (knx.paramString(NET_MQTTServer, NET_MQTTServerLength))
// Benutzer
#define ParamNET_MQTTUsername                        (knx.paramData(NET_MQTTUsername))
#define ParamNET_MQTTUsernameStr                     (knx.paramString(NET_MQTTUsername, NET_MQTTUsernameLength))
// Passwort
#define ParamNET_MQTTPassword                        (knx.paramData(NET_MQTTPassword))
#define ParamNET_MQTTPasswordStr                     (knx.paramString(NET_MQTTPassword, NET_MQTTPasswordLength))
// Prefix
#define ParamNET_MQTTPrefix                          (knx.paramData(NET_MQTTPrefix))
#define ParamNET_MQTTPrefixStr                       (knx.paramString(NET_MQTTPrefix, NET_MQTTPrefixLength))
// Port
#define ParamNET_MQTTPort                            (knx.paramWord(NET_MQTTPort))
// Sende KNX TP Rohdaten
#define ParamNET_MQTTTPRawData                       ((bool)(knx.paramByte(NET_MQTTTPRawData) & NET_MQTTTPRawDataMask))
// Modus
#define ParamNET_MQTTMode                            ((bool)(knx.paramByte(NET_MQTTMode) & NET_MQTTModeMask))



#define SENS_Error                               312      // 1 Bit, Bit 7
#define     SENS_ErrorMask 0x80
#define     SENS_ErrorShift 7
#define SENS_Dewpoint                            312      // 1 Bit, Bit 6
#define     SENS_DewpointMask 0x40
#define     SENS_DewpointShift 6
#define SENS_Comfort                             312      // 1 Bit, Bit 5
#define     SENS_ComfortMask 0x20
#define     SENS_ComfortShift 5
#define SENS_Airquality                          312      // 1 Bit, Bit 4
#define     SENS_AirqualityMask 0x10
#define     SENS_AirqualityShift 4
#define SENS_Accuracy                            312      // 1 Bit, Bit 3
#define     SENS_AccuracyMask 0x08
#define     SENS_AccuracyShift 3
#define SENS_DeleteData                          312      // 1 Bit, Bit 2
#define     SENS_DeleteDataMask 0x04
#define     SENS_DeleteDataShift 2
#define SENS_TempOffset                          313      // int8_t
#define SENS_TempCycleBase                       314      // 2 Bits, Bit 7-6
#define     SENS_TempCycleBaseMask 0xC0
#define     SENS_TempCycleBaseShift 6
#define SENS_TempCycleTime                       314      // 14 Bits, Bit 13-0
#define     SENS_TempCycleTimeMask 0x3FFF
#define     SENS_TempCycleTimeShift 0
#define SENS_TempDeltaAbs                        316      // uint16_t
#define SENS_TempDeltaPercent                    318      // uint8_t
#define SENS_TempSmooth                          319      // uint8_t
#define SENS_TempExtCount                        320      // 2 Bits, Bit 1-0
#define     SENS_TempExtCountMask 0x03
#define     SENS_TempExtCountShift 0
#define SENS_TempExtRead                         320      // 1 Bit, Bit 2
#define     SENS_TempExtReadMask 0x04
#define     SENS_TempExtReadShift 2
#define SENS_TempIntPercent                      321      // uint8_t
#define SENS_TempExt1Percent                     322      // uint8_t
#define SENS_TempExt2Percent                     323      // uint8_t
#define SENS_HumOffset                           324      // int8_t
#define SENS_HumCycleBase                        325      // 2 Bits, Bit 7-6
#define     SENS_HumCycleBaseMask 0xC0
#define     SENS_HumCycleBaseShift 6
#define SENS_HumCycleTime                        325      // 14 Bits, Bit 13-0
#define     SENS_HumCycleTimeMask 0x3FFF
#define     SENS_HumCycleTimeShift 0
#define SENS_HumDeltaAbs                         327      // uint16_t
#define SENS_HumDeltaPercent                     329      // uint8_t
#define SENS_HumSmooth                           330      // uint8_t
#define SENS_HumExtCount                         331      // 2 Bits, Bit 1-0
#define     SENS_HumExtCountMask 0x03
#define     SENS_HumExtCountShift 0
#define SENS_HumExtRead                          331      // 1 Bit, Bit 2
#define     SENS_HumExtReadMask 0x04
#define     SENS_HumExtReadShift 2
#define SENS_HumIntPercent                       332      // uint8_t
#define SENS_HumExt1Percent                      333      // uint8_t
#define SENS_HumExt2Percent                      334      // uint8_t
#define SENS_PreOffset                           335      // int8_t
#define SENS_PreCycleBase                        336      // 2 Bits, Bit 7-6
#define     SENS_PreCycleBaseMask 0xC0
#define     SENS_PreCycleBaseShift 6
#define SENS_PreCycleTime                        336      // 14 Bits, Bit 13-0
#define     SENS_PreCycleTimeMask 0x3FFF
#define     SENS_PreCycleTimeShift 0
#define SENS_PreDeltaAbs                         338      // uint16_t
#define SENS_PreDeltaPercent                     340      // uint8_t
#define SENS_PreSmooth                           341      // uint8_t
#define SENS_PreExtCount                         342      // 2 Bits, Bit 1-0
#define     SENS_PreExtCountMask 0x03
#define     SENS_PreExtCountShift 0
#define SENS_PreExtRead                          342      // 1 Bit, Bit 2
#define     SENS_PreExtReadMask 0x04
#define     SENS_PreExtReadShift 2
#define SENS_PreIntPercent                       343      // uint8_t
#define SENS_PreExt1Percent                      344      // uint8_t
#define SENS_PreExt2Percent                      345      // uint8_t
#define SENS_VocOffset                           346      // int8_t
#define SENS_VocCycleBase                        347      // 2 Bits, Bit 7-6
#define     SENS_VocCycleBaseMask 0xC0
#define     SENS_VocCycleBaseShift 6
#define SENS_VocCycleTime                        347      // 14 Bits, Bit 13-0
#define     SENS_VocCycleTimeMask 0x3FFF
#define     SENS_VocCycleTimeShift 0
#define SENS_VocDeltaAbs                         349      // uint16_t
#define SENS_VocDeltaPercent                     351      // uint8_t
#define SENS_VocSmooth                           352      // uint8_t
#define SENS_VocExtCount                         353      // 2 Bits, Bit 1-0
#define     SENS_VocExtCountMask 0x03
#define     SENS_VocExtCountShift 0
#define SENS_VocExtRead                          353      // 1 Bit, Bit 2
#define     SENS_VocExtReadMask 0x04
#define     SENS_VocExtReadShift 2
#define SENS_VocIntPercent                       354      // uint8_t
#define SENS_VocExt1Percent                      355      // uint8_t
#define SENS_VocExt2Percent                      356      // uint8_t
#define SENS_Co2Offset                           357      // int8_t
#define SENS_Co2CycleBase                        358      // 2 Bits, Bit 7-6
#define     SENS_Co2CycleBaseMask 0xC0
#define     SENS_Co2CycleBaseShift 6
#define SENS_Co2CycleTime                        358      // 14 Bits, Bit 13-0
#define     SENS_Co2CycleTimeMask 0x3FFF
#define     SENS_Co2CycleTimeShift 0
#define SENS_Co2DeltaAbs                         360      // uint16_t
#define SENS_Co2DeltaPercent                     362      // uint8_t
#define SENS_Co2Smooth                           363      // uint8_t
#define SENS_Co2ExtCount                         364      // 2 Bits, Bit 1-0
#define     SENS_Co2ExtCountMask 0x03
#define     SENS_Co2ExtCountShift 0
#define SENS_Co2ExtRead                          364      // 1 Bit, Bit 2
#define     SENS_Co2ExtReadMask 0x04
#define     SENS_Co2ExtReadShift 2
#define SENS_Co2IntPercent                       365      // uint8_t
#define SENS_Co2Ext1Percent                      366      // uint8_t
#define SENS_Co2Ext2Percent                      367      // uint8_t
#define SENS_DewOffset                           369      // int8_t
#define SENS_DewCycleBase                        370      // 2 Bits, Bit 7-6
#define     SENS_DewCycleBaseMask 0xC0
#define     SENS_DewCycleBaseShift 6
#define SENS_DewCycleTime                        370      // 14 Bits, Bit 13-0
#define     SENS_DewCycleTimeMask 0x3FFF
#define     SENS_DewCycleTimeShift 0
#define SENS_DewDeltaAbs                         372      // uint16_t
#define SENS_DewDeltaPercent                     374      // uint8_t
#define SENS_DewSmooth                           375      // uint8_t
#define SENS_LuxOffset                           376      // int8_t
#define SENS_LuxCycleBase                        377      // 2 Bits, Bit 7-6
#define     SENS_LuxCycleBaseMask 0xC0
#define     SENS_LuxCycleBaseShift 6
#define SENS_LuxCycleTime                        377      // 14 Bits, Bit 13-0
#define     SENS_LuxCycleTimeMask 0x3FFF
#define     SENS_LuxCycleTimeShift 0
#define SENS_LuxDeltaAbs                         379      // uint16_t
#define SENS_LuxDeltaPercent                     381      // uint8_t
#define SENS_LuxSmooth                           382      // uint8_t
#define SENS_LuxExtCount                         383      // 2 Bits, Bit 1-0
#define     SENS_LuxExtCountMask 0x03
#define     SENS_LuxExtCountShift 0
#define SENS_LuxExtRead                          383      // 1 Bit, Bit 2
#define     SENS_LuxExtReadMask 0x04
#define     SENS_LuxExtReadShift 2
#define SENS_LuxIntPercent                       384      // uint8_t
#define SENS_LuxExt1Percent                      385      // uint8_t
#define SENS_LuxExt2Percent                      386      // uint8_t
#define SENS_TofOffset                           387      // int8_t
#define SENS_TofCycleBase                        388      // 2 Bits, Bit 7-6
#define     SENS_TofCycleBaseMask 0xC0
#define     SENS_TofCycleBaseShift 6
#define SENS_TofCycleTime                        388      // 14 Bits, Bit 13-0
#define     SENS_TofCycleTimeMask 0x3FFF
#define     SENS_TofCycleTimeShift 0
#define SENS_TofDeltaAbs                         390      // uint16_t
#define SENS_TofDeltaPercent                     392      // uint8_t
#define SENS_TofSmooth                           393      // uint8_t
#define SENS_TofExtCount                         394      // 2 Bits, Bit 1-0
#define     SENS_TofExtCountMask 0x03
#define     SENS_TofExtCountShift 0
#define SENS_TofExtRead                          394      // 1 Bit, Bit 2
#define     SENS_TofExtReadMask 0x04
#define     SENS_TofExtReadShift 2
#define SENS_TofIntPercent                       395      // uint8_t
#define SENS_TofExt1Percent                      396      // uint8_t
#define SENS_TofExt2Percent                      397      // uint8_t
#define SENS_TempSensor                          398      // 4 Bits, Bit 7-4
#define     SENS_TempSensorMask 0xF0
#define     SENS_TempSensorShift 4
#define SENS_HumSensor                           398      // 4 Bits, Bit 3-0
#define     SENS_HumSensorMask 0x0F
#define     SENS_HumSensorShift 0
#define SENS_PreSensor                           399      // 4 Bits, Bit 7-4
#define     SENS_PreSensorMask 0xF0
#define     SENS_PreSensorShift 4
#define SENS_VocSensor                           399      // 4 Bits, Bit 3-0
#define     SENS_VocSensorMask 0x0F
#define     SENS_VocSensorShift 0
#define SENS_Co2Sensor                           400      // 4 Bits, Bit 7-4
#define     SENS_Co2SensorMask 0xF0
#define     SENS_Co2SensorShift 4
#define SENS_LuxSensor                           400      // 4 Bits, Bit 3-0
#define     SENS_LuxSensorMask 0x0F
#define     SENS_LuxSensorShift 0
#define SENS_TofSensor                           401      // 4 Bits, Bit 7-4
#define     SENS_TofSensorMask 0xF0
#define     SENS_TofSensorShift 4
#define SENS_SCD41MeasureIntervalDelayBase       402      // 2 Bits, Bit 7-6
#define     SENS_SCD41MeasureIntervalDelayBaseMask 0xC0
#define     SENS_SCD41MeasureIntervalDelayBaseShift 6
#define SENS_SCD41MeasureIntervalDelayTime       402      // 14 Bits, Bit 13-0
#define     SENS_SCD41MeasureIntervalDelayTimeMask 0x3FFF
#define     SENS_SCD41MeasureIntervalDelayTimeShift 0
#define SENS_PT1000NumWires                      404      // 2 Bits, Bit 7-6
#define     SENS_PT1000NumWiresMask 0xC0
#define     SENS_PT1000NumWiresShift 6
#define SENS_PT100PT1000                         404      // 1 Bit, Bit 5
#define     SENS_PT100PT1000Mask 0x20
#define     SENS_PT100PT1000Shift 5

// Fehlerobjekt für Standardmesswerte anzeigen
#define ParamSENS_Error                               ((bool)(knx.paramByte(SENS_Error) & SENS_ErrorMask))
// Taupunkt berechnen
#define ParamSENS_Dewpoint                            ((bool)(knx.paramByte(SENS_Dewpoint) & SENS_DewpointMask))
// Behaglichkeitszone ausgeben
#define ParamSENS_Comfort                             ((bool)(knx.paramByte(SENS_Comfort) & SENS_ComfortMask))
// Luftqualitätsampel ausgeben
#define ParamSENS_Airquality                          ((bool)(knx.paramByte(SENS_Airquality) & SENS_AirqualityMask))
// Kalibrierungsfortschritt ausgeben
#define ParamSENS_Accuracy                            ((bool)(knx.paramByte(SENS_Accuracy) & SENS_AccuracyMask))
// Kalibrierungsdaten löschen
#define ParamSENS_DeleteData                          ((bool)(knx.paramByte(SENS_DeleteData) & SENS_DeleteDataMask))
// Temperatur anpassen (interner Messwert)
#define ParamSENS_TempOffset                          ((int8_t)knx.paramByte(SENS_TempOffset))
// Zeitbasis
#define ParamSENS_TempCycleBase                       ((knx.paramByte(SENS_TempCycleBase) & SENS_TempCycleBaseMask) >> SENS_TempCycleBaseShift)
// Zeit
#define ParamSENS_TempCycleTime                       (knx.paramWord(SENS_TempCycleTime) & SENS_TempCycleTimeMask)
// Zeit (in Millisekunden)
#define ParamSENS_TempCycleTimeMS                     (paramDelay(knx.paramWord(SENS_TempCycleTime)))
// Temperatur bei absoluter Abweichung senden(0=nicht senden)
#define ParamSENS_TempDeltaAbs                        (knx.paramWord(SENS_TempDeltaAbs))
// Temperatur bei Abweichung vom vorherigen Wert senden(0=nicht senden)
#define ParamSENS_TempDeltaPercent                    (knx.paramByte(SENS_TempDeltaPercent))
// Temperatur glätten: P =
#define ParamSENS_TempSmooth                          (knx.paramByte(SENS_TempSmooth))
// Externe Messwerte berücksichtigen
#define ParamSENS_TempExtCount                        (knx.paramByte(SENS_TempExtCount) & SENS_TempExtCountMask)
//     Externe Messwerte beim Start lesen
#define ParamSENS_TempExtRead                         ((bool)(knx.paramByte(SENS_TempExtRead) & SENS_TempExtReadMask))
//     Anteil interner Messwert
#define ParamSENS_TempIntPercent                      (knx.paramByte(SENS_TempIntPercent))
//     Anteil externer Messwert 1
#define ParamSENS_TempExt1Percent                     (knx.paramByte(SENS_TempExt1Percent))
//     Anteil externer Messwert 2
#define ParamSENS_TempExt2Percent                     (knx.paramByte(SENS_TempExt2Percent))
// Luftfeuchte anpassen (interner Messwert)
#define ParamSENS_HumOffset                           ((int8_t)knx.paramByte(SENS_HumOffset))
// Zeitbasis
#define ParamSENS_HumCycleBase                        ((knx.paramByte(SENS_HumCycleBase) & SENS_HumCycleBaseMask) >> SENS_HumCycleBaseShift)
// Zeit
#define ParamSENS_HumCycleTime                        (knx.paramWord(SENS_HumCycleTime) & SENS_HumCycleTimeMask)
// Zeit (in Millisekunden)
#define ParamSENS_HumCycleTimeMS                      (paramDelay(knx.paramWord(SENS_HumCycleTime)))
// Luftfeuchte bei absoluter Abweichung senden(0=nicht senden)
#define ParamSENS_HumDeltaAbs                         (knx.paramWord(SENS_HumDeltaAbs))
// Luftfeuchte bei Abweichung vom vorherigen Wert senden(0=nicht senden)
#define ParamSENS_HumDeltaPercent                     (knx.paramByte(SENS_HumDeltaPercent))
// Luftfeuchte glätten: P =
#define ParamSENS_HumSmooth                           (knx.paramByte(SENS_HumSmooth))
// Externe Messwerte berücksichtigen
#define ParamSENS_HumExtCount                         (knx.paramByte(SENS_HumExtCount) & SENS_HumExtCountMask)
//     Externe Messwerte beim Start lesen
#define ParamSENS_HumExtRead                          ((bool)(knx.paramByte(SENS_HumExtRead) & SENS_HumExtReadMask))
//     Anteil interner Messwert
#define ParamSENS_HumIntPercent                       (knx.paramByte(SENS_HumIntPercent))
//     Anteil externer Messwert 1
#define ParamSENS_HumExt1Percent                      (knx.paramByte(SENS_HumExt1Percent))
//     Anteil externer Messwert 2
#define ParamSENS_HumExt2Percent                      (knx.paramByte(SENS_HumExt2Percent))
// Luftdruck anpassen (interner Messwert)
#define ParamSENS_PreOffset                           ((int8_t)knx.paramByte(SENS_PreOffset))
// Zeitbasis
#define ParamSENS_PreCycleBase                        ((knx.paramByte(SENS_PreCycleBase) & SENS_PreCycleBaseMask) >> SENS_PreCycleBaseShift)
// Zeit
#define ParamSENS_PreCycleTime                        (knx.paramWord(SENS_PreCycleTime) & SENS_PreCycleTimeMask)
// Zeit (in Millisekunden)
#define ParamSENS_PreCycleTimeMS                      (paramDelay(knx.paramWord(SENS_PreCycleTime)))
// Luftdruck bei absoluter Abweichung senden(0=nicht senden)
#define ParamSENS_PreDeltaAbs                         (knx.paramWord(SENS_PreDeltaAbs))
// Luftdruck bei Abweichung vom vorherigen Wert senden(0=nicht senden)
#define ParamSENS_PreDeltaPercent                     (knx.paramByte(SENS_PreDeltaPercent))
// Luftdruck glätten: P =
#define ParamSENS_PreSmooth                           (knx.paramByte(SENS_PreSmooth))
// Externe Messwerte berücksichtigen
#define ParamSENS_PreExtCount                         (knx.paramByte(SENS_PreExtCount) & SENS_PreExtCountMask)
//     Externe Messwerte beim Start lesen
#define ParamSENS_PreExtRead                          ((bool)(knx.paramByte(SENS_PreExtRead) & SENS_PreExtReadMask))
//     Anteil interner Messwert
#define ParamSENS_PreIntPercent                       (knx.paramByte(SENS_PreIntPercent))
//     Anteil externer Messwert 1
#define ParamSENS_PreExt1Percent                      (knx.paramByte(SENS_PreExt1Percent))
//     Anteil externer Messwert 2
#define ParamSENS_PreExt2Percent                      (knx.paramByte(SENS_PreExt2Percent))
// VOC anpassen (interner Messwert)
#define ParamSENS_VocOffset                           ((int8_t)knx.paramByte(SENS_VocOffset))
// Zeitbasis
#define ParamSENS_VocCycleBase                        ((knx.paramByte(SENS_VocCycleBase) & SENS_VocCycleBaseMask) >> SENS_VocCycleBaseShift)
// Zeit
#define ParamSENS_VocCycleTime                        (knx.paramWord(SENS_VocCycleTime) & SENS_VocCycleTimeMask)
// Zeit (in Millisekunden)
#define ParamSENS_VocCycleTimeMS                      (paramDelay(knx.paramWord(SENS_VocCycleTime)))
// VOC bei absoluter Abweichung senden(0=nicht senden)
#define ParamSENS_VocDeltaAbs                         (knx.paramWord(SENS_VocDeltaAbs))
// VOC bei Abweichung vom vorherigen Wert senden(0=nicht senden)
#define ParamSENS_VocDeltaPercent                     (knx.paramByte(SENS_VocDeltaPercent))
// VOC glätten: P =
#define ParamSENS_VocSmooth                           (knx.paramByte(SENS_VocSmooth))
// Externe Messwerte berücksichtigen
#define ParamSENS_VocExtCount                         (knx.paramByte(SENS_VocExtCount) & SENS_VocExtCountMask)
//     Externe Messwerte beim Start lesen
#define ParamSENS_VocExtRead                          ((bool)(knx.paramByte(SENS_VocExtRead) & SENS_VocExtReadMask))
//     Anteil interner Messwert
#define ParamSENS_VocIntPercent                       (knx.paramByte(SENS_VocIntPercent))
//     Anteil externer Messwert 1
#define ParamSENS_VocExt1Percent                      (knx.paramByte(SENS_VocExt1Percent))
//     Anteil externer Messwert 2
#define ParamSENS_VocExt2Percent                      (knx.paramByte(SENS_VocExt2Percent))
// CO2 anpassen (interner Messwert)
#define ParamSENS_Co2Offset                           ((int8_t)knx.paramByte(SENS_Co2Offset))
// Zeitbasis
#define ParamSENS_Co2CycleBase                        ((knx.paramByte(SENS_Co2CycleBase) & SENS_Co2CycleBaseMask) >> SENS_Co2CycleBaseShift)
// Zeit
#define ParamSENS_Co2CycleTime                        (knx.paramWord(SENS_Co2CycleTime) & SENS_Co2CycleTimeMask)
// Zeit (in Millisekunden)
#define ParamSENS_Co2CycleTimeMS                      (paramDelay(knx.paramWord(SENS_Co2CycleTime)))
// CO2 bei absoluter Abweichung senden(0=nicht senden)
#define ParamSENS_Co2DeltaAbs                         (knx.paramWord(SENS_Co2DeltaAbs))
// CO2 bei Abweichung vom vorherigen Wert senden(0=nicht senden)
#define ParamSENS_Co2DeltaPercent                     (knx.paramByte(SENS_Co2DeltaPercent))
// CO2 glätten: P =
#define ParamSENS_Co2Smooth                           (knx.paramByte(SENS_Co2Smooth))
// Externe Messwerte berücksichtigen
#define ParamSENS_Co2ExtCount                         (knx.paramByte(SENS_Co2ExtCount) & SENS_Co2ExtCountMask)
//     Externe Messwerte beim Start lesen
#define ParamSENS_Co2ExtRead                          ((bool)(knx.paramByte(SENS_Co2ExtRead) & SENS_Co2ExtReadMask))
//     Anteil interner Messwert
#define ParamSENS_Co2IntPercent                       (knx.paramByte(SENS_Co2IntPercent))
//     Anteil externer Messwert 1
#define ParamSENS_Co2Ext1Percent                      (knx.paramByte(SENS_Co2Ext1Percent))
//     Anteil externer Messwert 2
#define ParamSENS_Co2Ext2Percent                      (knx.paramByte(SENS_Co2Ext2Percent))
// Taupunkt anpassen
#define ParamSENS_DewOffset                           ((int8_t)knx.paramByte(SENS_DewOffset))
// Zeitbasis
#define ParamSENS_DewCycleBase                        ((knx.paramByte(SENS_DewCycleBase) & SENS_DewCycleBaseMask) >> SENS_DewCycleBaseShift)
// Zeit
#define ParamSENS_DewCycleTime                        (knx.paramWord(SENS_DewCycleTime) & SENS_DewCycleTimeMask)
// Zeit (in Millisekunden)
#define ParamSENS_DewCycleTimeMS                      (paramDelay(knx.paramWord(SENS_DewCycleTime)))
// Taupunkt bei absoluter Abweichung senden(0=nicht senden)
#define ParamSENS_DewDeltaAbs                         (knx.paramWord(SENS_DewDeltaAbs))
// Taupunkt bei Abweichung vom vorherigen Wert senden(0=nicht senden)
#define ParamSENS_DewDeltaPercent                     (knx.paramByte(SENS_DewDeltaPercent))
// Taupunkt glätten: P =
#define ParamSENS_DewSmooth                           (knx.paramByte(SENS_DewSmooth))
// Helligkeit anpassen (interner Messwert)
#define ParamSENS_LuxOffset                           ((int8_t)knx.paramByte(SENS_LuxOffset))
// Zeitbasis
#define ParamSENS_LuxCycleBase                        ((knx.paramByte(SENS_LuxCycleBase) & SENS_LuxCycleBaseMask) >> SENS_LuxCycleBaseShift)
// Zeit
#define ParamSENS_LuxCycleTime                        (knx.paramWord(SENS_LuxCycleTime) & SENS_LuxCycleTimeMask)
// Zeit (in Millisekunden)
#define ParamSENS_LuxCycleTimeMS                      (paramDelay(knx.paramWord(SENS_LuxCycleTime)))
// Helligkeit bei absoluter Abweichung senden(0=nicht senden)
#define ParamSENS_LuxDeltaAbs                         (knx.paramWord(SENS_LuxDeltaAbs))
// Helligkeit bei Abweichung vom vorherigen Wert senden(0=nicht senden)
#define ParamSENS_LuxDeltaPercent                     (knx.paramByte(SENS_LuxDeltaPercent))
// Helligkeit glätten: P =
#define ParamSENS_LuxSmooth                           (knx.paramByte(SENS_LuxSmooth))
// Externe Messwerte berücksichtigen
#define ParamSENS_LuxExtCount                         (knx.paramByte(SENS_LuxExtCount) & SENS_LuxExtCountMask)
//     Externe Messwerte beim Start lesen
#define ParamSENS_LuxExtRead                          ((bool)(knx.paramByte(SENS_LuxExtRead) & SENS_LuxExtReadMask))
//     Anteil interner Messwert
#define ParamSENS_LuxIntPercent                       (knx.paramByte(SENS_LuxIntPercent))
//     Anteil externer Messwert 1
#define ParamSENS_LuxExt1Percent                      (knx.paramByte(SENS_LuxExt1Percent))
//     Anteil externer Messwert 2
#define ParamSENS_LuxExt2Percent                      (knx.paramByte(SENS_LuxExt2Percent))
// Entfernung anpassen (interner Messwert)
#define ParamSENS_TofOffset                           ((int8_t)knx.paramByte(SENS_TofOffset))
// Zeitbasis
#define ParamSENS_TofCycleBase                        ((knx.paramByte(SENS_TofCycleBase) & SENS_TofCycleBaseMask) >> SENS_TofCycleBaseShift)
// Zeit
#define ParamSENS_TofCycleTime                        (knx.paramWord(SENS_TofCycleTime) & SENS_TofCycleTimeMask)
// Zeit (in Millisekunden)
#define ParamSENS_TofCycleTimeMS                      (paramDelay(knx.paramWord(SENS_TofCycleTime)))
// Entfernung bei absoluter Abweichung senden(0=nicht senden)
#define ParamSENS_TofDeltaAbs                         (knx.paramWord(SENS_TofDeltaAbs))
// Entfernung bei Abweichung vom vorherigen Wert senden(0=nicht senden)
#define ParamSENS_TofDeltaPercent                     (knx.paramByte(SENS_TofDeltaPercent))
// Entfernung glätten: P =
#define ParamSENS_TofSmooth                           (knx.paramByte(SENS_TofSmooth))
// Externe Messwerte berücksichtigen
#define ParamSENS_TofExtCount                         (knx.paramByte(SENS_TofExtCount) & SENS_TofExtCountMask)
//     Externe Messwerte beim Start lesen
#define ParamSENS_TofExtRead                          ((bool)(knx.paramByte(SENS_TofExtRead) & SENS_TofExtReadMask))
//     Anteil interner Messwert
#define ParamSENS_TofIntPercent                       (knx.paramByte(SENS_TofIntPercent))
//     Anteil externer Messwert 1
#define ParamSENS_TofExt1Percent                      (knx.paramByte(SENS_TofExt1Percent))
//     Anteil externer Messwert 2
#define ParamSENS_TofExt2Percent                      (knx.paramByte(SENS_TofExt2Percent))
// Temperatursensor
#define ParamSENS_TempSensor                          ((knx.paramByte(SENS_TempSensor) & SENS_TempSensorMask) >> SENS_TempSensorShift)
// Luftfeuchtesensor
#define ParamSENS_HumSensor                           (knx.paramByte(SENS_HumSensor) & SENS_HumSensorMask)
// Luftdrucksensor
#define ParamSENS_PreSensor                           ((knx.paramByte(SENS_PreSensor) & SENS_PreSensorMask) >> SENS_PreSensorShift)
// Voc-Sensor
#define ParamSENS_VocSensor                           (knx.paramByte(SENS_VocSensor) & SENS_VocSensorMask)
// Co2-Sensor
#define ParamSENS_Co2Sensor                           ((knx.paramByte(SENS_Co2Sensor) & SENS_Co2SensorMask) >> SENS_Co2SensorShift)
// Helligkeitssensor
#define ParamSENS_LuxSensor                           (knx.paramByte(SENS_LuxSensor) & SENS_LuxSensorMask)
// Füllstands- und Näherungssensor
#define ParamSENS_TofSensor                           ((knx.paramByte(SENS_TofSensor) & SENS_TofSensorMask) >> SENS_TofSensorShift)
// Zeitbasis
#define ParamSENS_SCD41MeasureIntervalDelayBase       ((knx.paramByte(SENS_SCD41MeasureIntervalDelayBase) & SENS_SCD41MeasureIntervalDelayBaseMask) >> SENS_SCD41MeasureIntervalDelayBaseShift)
// Zeit
#define ParamSENS_SCD41MeasureIntervalDelayTime       (knx.paramWord(SENS_SCD41MeasureIntervalDelayTime) & SENS_SCD41MeasureIntervalDelayTimeMask)
// Zeit (in Millisekunden)
#define ParamSENS_SCD41MeasureIntervalDelayTimeMS     (paramDelay(knx.paramWord(SENS_SCD41MeasureIntervalDelayTime)))
// Anzahl Anschlussadern
#define ParamSENS_PT1000NumWires                      ((knx.paramByte(SENS_PT1000NumWires) & SENS_PT1000NumWiresMask) >> SENS_PT1000NumWiresShift)
// Art des Analogsensors
#define ParamSENS_PT100PT1000                         ((bool)(knx.paramByte(SENS_PT100PT1000) & SENS_PT100PT1000Mask))

#define SENS_KoRequestValues 43
#define SENS_KoError 44
#define SENS_KoTemp 45
#define SENS_KoExt1Temp 46
#define SENS_KoExt2Temp 47
#define SENS_KoHum 48
#define SENS_KoExt1Hum 49
#define SENS_KoExt2Hum 50
#define SENS_KoPre 51
#define SENS_KoExt1Pre 52
#define SENS_KoExt2Pre 53
#define SENS_KoVoc 54
#define SENS_KoExt1Voc 55
#define SENS_KoExt2Voc 56
#define SENS_KoCo2 57
#define SENS_KoExt1Co2 59
#define SENS_KoExt2Co2 60
#define SENS_KoLux 61
#define SENS_KoExt1Lux 62
#define SENS_KoExt2Lux 63
#define SENS_KoTof 64
#define SENS_KoExt1Tof 65
#define SENS_KoExt2Tof 66
#define SENS_KoCo2b 58
#define SENS_KoDewpoint 85
#define SENS_KoComfort 86
#define SENS_KoAirquality 87
#define SENS_KoSensorAccuracy 88

// Sensorwerte anfordern
#define KoSENS_RequestValues                       (knx.getGroupObject(SENS_KoRequestValues))
// Sensorfehler
#define KoSENS_Error                               (knx.getGroupObject(SENS_KoError))
// Temperatur
#define KoSENS_Temp                                (knx.getGroupObject(SENS_KoTemp))
// Externer Temperatur 1
#define KoSENS_Ext1Temp                            (knx.getGroupObject(SENS_KoExt1Temp))
// Externer Temperatur 2
#define KoSENS_Ext2Temp                            (knx.getGroupObject(SENS_KoExt2Temp))
// Luftfeuchte
#define KoSENS_Hum                                 (knx.getGroupObject(SENS_KoHum))
// Externer Luftfeuchte 1
#define KoSENS_Ext1Hum                             (knx.getGroupObject(SENS_KoExt1Hum))
// Externer Luftfeuchte 2
#define KoSENS_Ext2Hum                             (knx.getGroupObject(SENS_KoExt2Hum))
// Luftdruck
#define KoSENS_Pre                                 (knx.getGroupObject(SENS_KoPre))
// Externer Luftdruck 1
#define KoSENS_Ext1Pre                             (knx.getGroupObject(SENS_KoExt1Pre))
// Externer Luftdruck 2
#define KoSENS_Ext2Pre                             (knx.getGroupObject(SENS_KoExt2Pre))
// VOC
#define KoSENS_Voc                                 (knx.getGroupObject(SENS_KoVoc))
// Externer VOC 1
#define KoSENS_Ext1Voc                             (knx.getGroupObject(SENS_KoExt1Voc))
// Externer VOC 2
#define KoSENS_Ext2Voc                             (knx.getGroupObject(SENS_KoExt2Voc))
// CO2
#define KoSENS_Co2                                 (knx.getGroupObject(SENS_KoCo2))
// Externer CO2 1
#define KoSENS_Ext1Co2                             (knx.getGroupObject(SENS_KoExt1Co2))
// Externer CO2 2
#define KoSENS_Ext2Co2                             (knx.getGroupObject(SENS_KoExt2Co2))
// Helligkeit
#define KoSENS_Lux                                 (knx.getGroupObject(SENS_KoLux))
// Externer Helligkeit 1
#define KoSENS_Ext1Lux                             (knx.getGroupObject(SENS_KoExt1Lux))
// Externer Helligkeit 2
#define KoSENS_Ext2Lux                             (knx.getGroupObject(SENS_KoExt2Lux))
// Entfernung
#define KoSENS_Tof                                 (knx.getGroupObject(SENS_KoTof))
// Externer Entfernung 1
#define KoSENS_Ext1Tof                             (knx.getGroupObject(SENS_KoExt1Tof))
// Externer Entfernung 2
#define KoSENS_Ext2Tof                             (knx.getGroupObject(SENS_KoExt2Tof))
// CO2-VOC
#define KoSENS_Co2b                                (knx.getGroupObject(SENS_KoCo2b))
// Taupunkt
#define KoSENS_Dewpoint                            (knx.getGroupObject(SENS_KoDewpoint))
// Behaglichkeit (2=behaglich, 1=geht noch, 0=unbehaglich)
#define KoSENS_Comfort                             (knx.getGroupObject(SENS_KoComfort))
// Luftqualitätsampel (1-6)
#define KoSENS_Airquality                          (knx.getGroupObject(SENS_KoAirquality))
// Kalibrierungfortschritt
#define KoSENS_SensorAccuracy                      (knx.getGroupObject(SENS_KoSensorAccuracy))

#define AIR_DeviceType                          405      // 4 Bits, Bit 7-4
#define     AIR_DeviceTypeMask 0xF0
#define     AIR_DeviceTypeShift 4
#define AIR_StartBehavior                       405      // 2 Bits, Bit 3-2
#define     AIR_StartBehaviorMask 0x0C
#define     AIR_StartBehaviorShift 2
#define AIR_LockReleaseKo                       406      // 2 Bits, Bit 7-6
#define     AIR_LockReleaseKoMask 0xC0
#define     AIR_LockReleaseKoShift 6
#define AIR_ReleaseBehaviorOn                   406      // 2 Bits, Bit 5-4
#define     AIR_ReleaseBehaviorOnMask 0x30
#define     AIR_ReleaseBehaviorOnShift 4
#define AIR_ReleaseBehaviorOff                  406      // 2 Bits, Bit 3-2
#define     AIR_ReleaseBehaviorOffMask 0x0C
#define     AIR_ReleaseBehaviorOffShift 2
#define AIR_LockBehaviorOn                      406      // 2 Bits, Bit 1-0
#define     AIR_LockBehaviorOnMask 0x03
#define     AIR_LockBehaviorOnShift 0
#define AIR_LockBehaviorOff                     407      // 2 Bits, Bit 7-6
#define     AIR_LockBehaviorOffMask 0xC0
#define     AIR_LockBehaviorOffShift 6
#define AIR_WifiLED                             407      // 2 Bits, Bit 5-4
#define     AIR_WifiLEDMask 0x30
#define     AIR_WifiLEDShift 4
#define AIR_ExternalRoomTemperature             407      // 1 Bit, Bit 3
#define     AIR_ExternalRoomTemperatureMask 0x08
#define     AIR_ExternalRoomTemperatureShift 3
#define AIR_ClimateSetTemperature               407      // 1 Bit, Bit 2
#define     AIR_ClimateSetTemperatureMask 0x04
#define     AIR_ClimateSetTemperatureShift 2
#define AIR_ExternTempWatchdog                  407      // 2 Bits, Bit 1-0
#define     AIR_ExternTempWatchdogMask 0x03
#define     AIR_ExternTempWatchdogShift 0
#define AIR_MonitoringWDTTimeoutDelayBase       408      // 2 Bits, Bit 7-6
#define     AIR_MonitoringWDTTimeoutDelayBaseMask 0xC0
#define     AIR_MonitoringWDTTimeoutDelayBaseShift 6
#define AIR_MonitoringWDTTimeoutDelayTime       408      // 14 Bits, Bit 13-0
#define     AIR_MonitoringWDTTimeoutDelayTimeMask 0x3FFF
#define     AIR_MonitoringWDTTimeoutDelayTimeShift 0
#define AIR_Mit_MinTemp                         410      // 8 Bits, Bit 7-0
#define AIR_SCAActive                           425      // 1 Bit, Bit 7
#define     AIR_SCAActiveMask 0x80
#define     AIR_SCAActiveShift 7
#define AIR_SCANumber                           426      // 7 Bits, Bit 7-1
#define     AIR_SCANumberMask 0xFE
#define     AIR_SCANumberShift 1
#define AIR_SCAOnOff                            427      // 8 Bits, Bit 7-0
#define AIR_SCAOperationMode                    430      // 8 Bits, Bit 7-0
#define AIR_SCASceneTemperature                 431      // 8 Bits, Bit 7-0
#define AIR_SCASceneFan                         432      // 8 Bits, Bit 7-0
#define AIR_SCASceneSwing                       433      // 8 Bits, Bit 7-0
#define AIR_SCAScenePosition                    434      // 8 Bits, Bit 7-0
#define AIR_SCAScenePowerLimit                  435      // 8 Bits, Bit 7-0
#define AIR_SCASceneDeviceMode                  436      // 8 Bits, Bit 7-0
#define AIR_SCASceneAirPurification             437      // 8 Bits, Bit 7-0
#define AIR_SCAOperationModeDaikin              430      // 8 Bits, Bit 7-0
#define AIR_SCASceneTemperatureDaikin           431      // 8 Bits, Bit 7-0
#define AIR_SCASceneFanDaikin                   432      // 8 Bits, Bit 7-0
#define AIR_SCASceneSwingDaikin                 433      // 8 Bits, Bit 7-0
#define AIR_SCASceneDeviceModeDaikin            436      // 8 Bits, Bit 7-0
#define AIR_SCASceneAirPurificationDaikin       437      // 8 Bits, Bit 7-0
#define AIR_SCAOperationModeMitsubishi          430      // 8 Bits, Bit 7-0
#define AIR_SCASceneTemperatureMitsubishi       431      // 8 Bits, Bit 7-0
#define AIR_SCASceneFanMitsubishi               432      // 8 Bits, Bit 7-0
#define AIR_SCASceneSwingMitsubishi             433      // 8 Bits, Bit 7-0
#define AIR_SCAScenePositionMitsubishi          434      // 8 Bits, Bit 7-0
#define AIR_SCBActive                           445      // 1 Bit, Bit 7
#define     AIR_SCBActiveMask 0x80
#define     AIR_SCBActiveShift 7
#define AIR_SCBNumber                           446      // 7 Bits, Bit 7-1
#define     AIR_SCBNumberMask 0xFE
#define     AIR_SCBNumberShift 1
#define AIR_SCBOnOff                            447      // 8 Bits, Bit 7-0
#define AIR_SCBOperationMode                    450      // 8 Bits, Bit 7-0
#define AIR_SCBSceneTemperature                 451      // 8 Bits, Bit 7-0
#define AIR_SCBSceneFan                         452      // 8 Bits, Bit 7-0
#define AIR_SCBSceneSwing                       453      // 8 Bits, Bit 7-0
#define AIR_SCBScenePosition                    454      // 8 Bits, Bit 7-0
#define AIR_SCBScenePowerLimit                  455      // 8 Bits, Bit 7-0
#define AIR_SCBSceneDeviceMode                  456      // 8 Bits, Bit 7-0
#define AIR_SCBSceneAirPurification             457      // 8 Bits, Bit 7-0
#define AIR_SCBOperationModeDaikin              450      // 8 Bits, Bit 7-0
#define AIR_SCBSceneTemperatureDaikin           451      // 8 Bits, Bit 7-0
#define AIR_SCBSceneFanDaikin                   452      // 8 Bits, Bit 7-0
#define AIR_SCBSceneSwingDaikin                 453      // 8 Bits, Bit 7-0
#define AIR_SCBSceneDeviceModeDaikin            456      // 8 Bits, Bit 7-0
#define AIR_SCBSceneAirPurificationDaikin       457      // 8 Bits, Bit 7-0
#define AIR_SCBOperationModeMitsubishi          450      // 8 Bits, Bit 7-0
#define AIR_SCBSceneTemperatureMitsubishi       451      // 8 Bits, Bit 7-0
#define AIR_SCBSceneFanMitsubishi               452      // 8 Bits, Bit 7-0
#define AIR_SCBSceneSwingMitsubishi             453      // 8 Bits, Bit 7-0
#define AIR_SCBScenePositionMitsubishi          454      // 8 Bits, Bit 7-0
#define AIR_SCCActive                           465      // 1 Bit, Bit 7
#define     AIR_SCCActiveMask 0x80
#define     AIR_SCCActiveShift 7
#define AIR_SCCNumber                           466      // 7 Bits, Bit 7-1
#define     AIR_SCCNumberMask 0xFE
#define     AIR_SCCNumberShift 1
#define AIR_SCCOnOff                            467      // 8 Bits, Bit 7-0
#define AIR_SCCOperationMode                    470      // 8 Bits, Bit 7-0
#define AIR_SCCSceneTemperature                 471      // 8 Bits, Bit 7-0
#define AIR_SCCSceneFan                         472      // 8 Bits, Bit 7-0
#define AIR_SCCSceneSwing                       473      // 8 Bits, Bit 7-0
#define AIR_SCCScenePosition                    474      // 8 Bits, Bit 7-0
#define AIR_SCCScenePowerLimit                  475      // 8 Bits, Bit 7-0
#define AIR_SCCSceneDeviceMode                  476      // 8 Bits, Bit 7-0
#define AIR_SCCSceneAirPurification             477      // 8 Bits, Bit 7-0
#define AIR_SCCOperationModeDaikin              470      // 8 Bits, Bit 7-0
#define AIR_SCCSceneTemperatureDaikin           471      // 8 Bits, Bit 7-0
#define AIR_SCCSceneFanDaikin                   472      // 8 Bits, Bit 7-0
#define AIR_SCCSceneSwingDaikin                 473      // 8 Bits, Bit 7-0
#define AIR_SCCSceneDeviceModeDaikin            476      // 8 Bits, Bit 7-0
#define AIR_SCCSceneAirPurificationDaikin       477      // 8 Bits, Bit 7-0
#define AIR_SCCOperationModeMitsubishi          470      // 8 Bits, Bit 7-0
#define AIR_SCCSceneTemperatureMitsubishi       471      // 8 Bits, Bit 7-0
#define AIR_SCCSceneFanMitsubishi               472      // 8 Bits, Bit 7-0
#define AIR_SCCSceneSwingMitsubishi             473      // 8 Bits, Bit 7-0
#define AIR_SCCScenePositionMitsubishi          474      // 8 Bits, Bit 7-0
#define AIR_SCDActive                           485      // 1 Bit, Bit 7
#define     AIR_SCDActiveMask 0x80
#define     AIR_SCDActiveShift 7
#define AIR_SCDNumber                           486      // 7 Bits, Bit 7-1
#define     AIR_SCDNumberMask 0xFE
#define     AIR_SCDNumberShift 1
#define AIR_SCDOnOff                            487      // 8 Bits, Bit 7-0
#define AIR_SCDOperationMode                    490      // 8 Bits, Bit 7-0
#define AIR_SCDSceneTemperature                 491      // 8 Bits, Bit 7-0
#define AIR_SCDSceneFan                         492      // 8 Bits, Bit 7-0
#define AIR_SCDSceneSwing                       493      // 8 Bits, Bit 7-0
#define AIR_SCDScenePosition                    494      // 8 Bits, Bit 7-0
#define AIR_SCDScenePowerLimit                  495      // 8 Bits, Bit 7-0
#define AIR_SCDSceneDeviceMode                  496      // 8 Bits, Bit 7-0
#define AIR_SCDSceneAirPurification             497      // 8 Bits, Bit 7-0
#define AIR_SCDOperationModeDaikin              490      // 8 Bits, Bit 7-0
#define AIR_SCDSceneTemperatureDaikin           491      // 8 Bits, Bit 7-0
#define AIR_SCDSceneFanDaikin                   492      // 8 Bits, Bit 7-0
#define AIR_SCDSceneSwingDaikin                 493      // 8 Bits, Bit 7-0
#define AIR_SCDSceneDeviceModeDaikin            496      // 8 Bits, Bit 7-0
#define AIR_SCDSceneAirPurificationDaikin       497      // 8 Bits, Bit 7-0
#define AIR_SCDOperationModeMitsubishi          490      // 8 Bits, Bit 7-0
#define AIR_SCDSceneTemperatureMitsubishi       491      // 8 Bits, Bit 7-0
#define AIR_SCDSceneFanMitsubishi               492      // 8 Bits, Bit 7-0
#define AIR_SCDSceneSwingMitsubishi             493      // 8 Bits, Bit 7-0
#define AIR_SCDScenePositionMitsubishi          494      // 8 Bits, Bit 7-0
#define AIR_SCEActive                           505      // 1 Bit, Bit 7
#define     AIR_SCEActiveMask 0x80
#define     AIR_SCEActiveShift 7
#define AIR_SCENumber                           506      // 7 Bits, Bit 7-1
#define     AIR_SCENumberMask 0xFE
#define     AIR_SCENumberShift 1
#define AIR_SCEOnOff                            507      // 8 Bits, Bit 7-0
#define AIR_SCEOperationMode                    510      // 8 Bits, Bit 7-0
#define AIR_SCESceneTemperature                 511      // 8 Bits, Bit 7-0
#define AIR_SCESceneFan                         512      // 8 Bits, Bit 7-0
#define AIR_SCESceneSwing                       513      // 8 Bits, Bit 7-0
#define AIR_SCEScenePosition                    514      // 8 Bits, Bit 7-0
#define AIR_SCEScenePowerLimit                  515      // 8 Bits, Bit 7-0
#define AIR_SCESceneDeviceMode                  516      // 8 Bits, Bit 7-0
#define AIR_SCESceneAirPurification             517      // 8 Bits, Bit 7-0
#define AIR_SCEOperationModeDaikin              510      // 8 Bits, Bit 7-0
#define AIR_SCESceneTemperatureDaikin           511      // 8 Bits, Bit 7-0
#define AIR_SCESceneFanDaikin                   512      // 8 Bits, Bit 7-0
#define AIR_SCESceneSwingDaikin                 513      // 8 Bits, Bit 7-0
#define AIR_SCESceneDeviceModeDaikin            516      // 8 Bits, Bit 7-0
#define AIR_SCESceneAirPurificationDaikin       517      // 8 Bits, Bit 7-0
#define AIR_SCEOperationModeMitsubishi          510      // 8 Bits, Bit 7-0
#define AIR_SCESceneTemperatureMitsubishi       511      // 8 Bits, Bit 7-0
#define AIR_SCESceneFanMitsubishi               512      // 8 Bits, Bit 7-0
#define AIR_SCESceneSwingMitsubishi             513      // 8 Bits, Bit 7-0
#define AIR_SCEScenePositionMitsubishi          514      // 8 Bits, Bit 7-0
#define AIR_SCFActive                           525      // 1 Bit, Bit 7
#define     AIR_SCFActiveMask 0x80
#define     AIR_SCFActiveShift 7
#define AIR_SCFNumber                           526      // 7 Bits, Bit 7-1
#define     AIR_SCFNumberMask 0xFE
#define     AIR_SCFNumberShift 1
#define AIR_SCFOnOff                            527      // 8 Bits, Bit 7-0
#define AIR_SCFOperationMode                    530      // 8 Bits, Bit 7-0
#define AIR_SCFSceneTemperature                 531      // 8 Bits, Bit 7-0
#define AIR_SCFSceneFan                         532      // 8 Bits, Bit 7-0
#define AIR_SCFSceneSwing                       533      // 8 Bits, Bit 7-0
#define AIR_SCFScenePosition                    534      // 8 Bits, Bit 7-0
#define AIR_SCFScenePowerLimit                  535      // 8 Bits, Bit 7-0
#define AIR_SCFSceneDeviceMode                  536      // 8 Bits, Bit 7-0
#define AIR_SCFSceneAirPurification             537      // 8 Bits, Bit 7-0
#define AIR_SCFOperationModeDaikin              530      // 8 Bits, Bit 7-0
#define AIR_SCFSceneTemperatureDaikin           531      // 8 Bits, Bit 7-0
#define AIR_SCFSceneFanDaikin                   532      // 8 Bits, Bit 7-0
#define AIR_SCFSceneSwingDaikin                 533      // 8 Bits, Bit 7-0
#define AIR_SCFSceneDeviceModeDaikin            536      // 8 Bits, Bit 7-0
#define AIR_SCFSceneAirPurificationDaikin       537      // 8 Bits, Bit 7-0
#define AIR_SCFOperationModeMitsubishi          530      // 8 Bits, Bit 7-0
#define AIR_SCFSceneTemperatureMitsubishi       531      // 8 Bits, Bit 7-0
#define AIR_SCFSceneFanMitsubishi               532      // 8 Bits, Bit 7-0
#define AIR_SCFSceneSwingMitsubishi             533      // 8 Bits, Bit 7-0
#define AIR_SCFScenePositionMitsubishi          534      // 8 Bits, Bit 7-0
#define AIR_SCGActive                           545      // 1 Bit, Bit 7
#define     AIR_SCGActiveMask 0x80
#define     AIR_SCGActiveShift 7
#define AIR_SCGNumber                           546      // 7 Bits, Bit 7-1
#define     AIR_SCGNumberMask 0xFE
#define     AIR_SCGNumberShift 1
#define AIR_SCGOnOff                            547      // 8 Bits, Bit 7-0
#define AIR_SCGOperationMode                    550      // 8 Bits, Bit 7-0
#define AIR_SCGSceneTemperature                 551      // 8 Bits, Bit 7-0
#define AIR_SCGSceneFan                         552      // 8 Bits, Bit 7-0
#define AIR_SCGSceneSwing                       553      // 8 Bits, Bit 7-0
#define AIR_SCGScenePosition                    554      // 8 Bits, Bit 7-0
#define AIR_SCGScenePowerLimit                  555      // 8 Bits, Bit 7-0
#define AIR_SCGSceneDeviceMode                  556      // 8 Bits, Bit 7-0
#define AIR_SCGSceneAirPurification             557      // 8 Bits, Bit 7-0
#define AIR_SCGOperationModeDaikin              550      // 8 Bits, Bit 7-0
#define AIR_SCGSceneTemperatureDaikin           551      // 8 Bits, Bit 7-0
#define AIR_SCGSceneFanDaikin                   552      // 8 Bits, Bit 7-0
#define AIR_SCGSceneSwingDaikin                 553      // 8 Bits, Bit 7-0
#define AIR_SCGSceneDeviceModeDaikin            556      // 8 Bits, Bit 7-0
#define AIR_SCGSceneAirPurificationDaikin       557      // 8 Bits, Bit 7-0
#define AIR_SCGOperationModeMitsubishi          550      // 8 Bits, Bit 7-0
#define AIR_SCGSceneTemperatureMitsubishi       551      // 8 Bits, Bit 7-0
#define AIR_SCGSceneFanMitsubishi               552      // 8 Bits, Bit 7-0
#define AIR_SCGSceneSwingMitsubishi             553      // 8 Bits, Bit 7-0
#define AIR_SCGScenePositionMitsubishi          554      // 8 Bits, Bit 7-0
#define AIR_SCHActive                           565      // 1 Bit, Bit 7
#define     AIR_SCHActiveMask 0x80
#define     AIR_SCHActiveShift 7
#define AIR_SCHNumber                           566      // 7 Bits, Bit 7-1
#define     AIR_SCHNumberMask 0xFE
#define     AIR_SCHNumberShift 1
#define AIR_SCHOnOff                            567      // 8 Bits, Bit 7-0
#define AIR_SCHOperationMode                    570      // 8 Bits, Bit 7-0
#define AIR_SCHSceneTemperature                 571      // 8 Bits, Bit 7-0
#define AIR_SCHSceneFan                         572      // 8 Bits, Bit 7-0
#define AIR_SCHSceneSwing                       573      // 8 Bits, Bit 7-0
#define AIR_SCHScenePosition                    574      // 8 Bits, Bit 7-0
#define AIR_SCHScenePowerLimit                  575      // 8 Bits, Bit 7-0
#define AIR_SCHSceneDeviceMode                  576      // 8 Bits, Bit 7-0
#define AIR_SCHSceneAirPurification             577      // 8 Bits, Bit 7-0
#define AIR_SCHOperationModeDaikin              570      // 8 Bits, Bit 7-0
#define AIR_SCHSceneTemperatureDaikin           571      // 8 Bits, Bit 7-0
#define AIR_SCHSceneFanDaikin                   572      // 8 Bits, Bit 7-0
#define AIR_SCHSceneSwingDaikin                 573      // 8 Bits, Bit 7-0
#define AIR_SCHSceneDeviceModeDaikin            576      // 8 Bits, Bit 7-0
#define AIR_SCHSceneAirPurificationDaikin       577      // 8 Bits, Bit 7-0
#define AIR_SCHOperationModeMitsubishi          570      // 8 Bits, Bit 7-0
#define AIR_SCHSceneTemperatureMitsubishi       571      // 8 Bits, Bit 7-0
#define AIR_SCHSceneFanMitsubishi               572      // 8 Bits, Bit 7-0
#define AIR_SCHSceneSwingMitsubishi             573      // 8 Bits, Bit 7-0
#define AIR_SCHScenePositionMitsubishi          574      // 8 Bits, Bit 7-0
#define AIR_SCIActive                           585      // 1 Bit, Bit 7
#define     AIR_SCIActiveMask 0x80
#define     AIR_SCIActiveShift 7
#define AIR_SCINumber                           586      // 7 Bits, Bit 7-1
#define     AIR_SCINumberMask 0xFE
#define     AIR_SCINumberShift 1
#define AIR_SCIOnOff                            587      // 8 Bits, Bit 7-0
#define AIR_SCIOperationMode                    590      // 8 Bits, Bit 7-0
#define AIR_SCISceneTemperature                 591      // 8 Bits, Bit 7-0
#define AIR_SCISceneFan                         592      // 8 Bits, Bit 7-0
#define AIR_SCISceneSwing                       593      // 8 Bits, Bit 7-0
#define AIR_SCIScenePosition                    594      // 8 Bits, Bit 7-0
#define AIR_SCIScenePowerLimit                  595      // 8 Bits, Bit 7-0
#define AIR_SCISceneDeviceMode                  596      // 8 Bits, Bit 7-0
#define AIR_SCISceneAirPurification             597      // 8 Bits, Bit 7-0
#define AIR_SCIOperationModeDaikin              590      // 8 Bits, Bit 7-0
#define AIR_SCISceneTemperatureDaikin           591      // 8 Bits, Bit 7-0
#define AIR_SCISceneFanDaikin                   592      // 8 Bits, Bit 7-0
#define AIR_SCISceneSwingDaikin                 593      // 8 Bits, Bit 7-0
#define AIR_SCISceneDeviceModeDaikin            596      // 8 Bits, Bit 7-0
#define AIR_SCISceneAirPurificationDaikin       597      // 8 Bits, Bit 7-0
#define AIR_SCIOperationModeMitsubishi          590      // 8 Bits, Bit 7-0
#define AIR_SCISceneTemperatureMitsubishi       591      // 8 Bits, Bit 7-0
#define AIR_SCISceneFanMitsubishi               592      // 8 Bits, Bit 7-0
#define AIR_SCISceneSwingMitsubishi             593      // 8 Bits, Bit 7-0
#define AIR_SCIScenePositionMitsubishi          594      // 8 Bits, Bit 7-0
#define AIR_SCJActive                           605      // 1 Bit, Bit 7
#define     AIR_SCJActiveMask 0x80
#define     AIR_SCJActiveShift 7
#define AIR_SCJNumber                           606      // 7 Bits, Bit 7-1
#define     AIR_SCJNumberMask 0xFE
#define     AIR_SCJNumberShift 1
#define AIR_SCJOnOff                            607      // 8 Bits, Bit 7-0
#define AIR_SCJOperationMode                    610      // 8 Bits, Bit 7-0
#define AIR_SCJSceneTemperature                 611      // 8 Bits, Bit 7-0
#define AIR_SCJSceneFan                         612      // 8 Bits, Bit 7-0
#define AIR_SCJSceneSwing                       613      // 8 Bits, Bit 7-0
#define AIR_SCJScenePosition                    614      // 8 Bits, Bit 7-0
#define AIR_SCJScenePowerLimit                  615      // 8 Bits, Bit 7-0
#define AIR_SCJSceneDeviceMode                  616      // 8 Bits, Bit 7-0
#define AIR_SCJSceneAirPurification             617      // 8 Bits, Bit 7-0
#define AIR_SCJOperationModeDaikin              610      // 8 Bits, Bit 7-0
#define AIR_SCJSceneTemperatureDaikin           611      // 8 Bits, Bit 7-0
#define AIR_SCJSceneFanDaikin                   612      // 8 Bits, Bit 7-0
#define AIR_SCJSceneSwingDaikin                 613      // 8 Bits, Bit 7-0
#define AIR_SCJSceneDeviceModeDaikin            616      // 8 Bits, Bit 7-0
#define AIR_SCJSceneAirPurificationDaikin       617      // 8 Bits, Bit 7-0
#define AIR_SCJOperationModeMitsubishi          610      // 8 Bits, Bit 7-0
#define AIR_SCJSceneTemperatureMitsubishi       611      // 8 Bits, Bit 7-0
#define AIR_SCJSceneFanMitsubishi               612      // 8 Bits, Bit 7-0
#define AIR_SCJSceneSwingMitsubishi             613      // 8 Bits, Bit 7-0
#define AIR_SCJScenePositionMitsubishi          614      // 8 Bits, Bit 7-0

// Hersteller
#define ParamAIR_DeviceType                          (PT_AIRDeviceType)((knx.paramByte(AIR_DeviceType) & AIR_DeviceTypeMask) >> AIR_DeviceTypeShift)
// Nach Busspannungsausfall / Neustart
#define ParamAIR_StartBehavior                       (PT_AIRStartBehavior)((knx.paramByte(AIR_StartBehavior) & AIR_StartBehaviorMask) >> AIR_StartBehaviorShift)
// Freigabe- / Sperrobjekt
#define ParamAIR_LockReleaseKo                       (PT_AIRLockReleaseKo)((knx.paramByte(AIR_LockReleaseKo) & AIR_LockReleaseKoMask) >> AIR_LockReleaseKoShift)
// Verhalten bei Freigabe
#define ParamAIR_ReleaseBehaviorOn                   (PT_AIRLockReleaseBehavior)((knx.paramByte(AIR_ReleaseBehaviorOn) & AIR_ReleaseBehaviorOnMask) >> AIR_ReleaseBehaviorOnShift)
// Verhalten bei Freigabe Ende
#define ParamAIR_ReleaseBehaviorOff                  (PT_AIRLockReleaseBehavior)((knx.paramByte(AIR_ReleaseBehaviorOff) & AIR_ReleaseBehaviorOffMask) >> AIR_ReleaseBehaviorOffShift)
// Verhalten bei Sperre
#define ParamAIR_LockBehaviorOn                      (PT_AIRLockReleaseBehavior)(knx.paramByte(AIR_LockBehaviorOn) & AIR_LockBehaviorOnMask)
// Verhalten bei Sperre Ende
#define ParamAIR_LockBehaviorOff                     (PT_AIRLockReleaseBehavior)((knx.paramByte(AIR_LockBehaviorOff) & AIR_LockBehaviorOffMask) >> AIR_LockBehaviorOffShift)
// Wifi LED
#define ParamAIR_WifiLED                             (PT_AIRWifiLED)((knx.paramByte(AIR_WifiLED) & AIR_WifiLEDMask) >> AIR_WifiLEDShift)
// Eingang für externe Raumtemperatur
#define ParamAIR_ExternalRoomTemperature             ((bool)(knx.paramByte(AIR_ExternalRoomTemperature) & AIR_ExternalRoomTemperatureMask))
// Gruppenobjekt 'Klimagerät Solltemperatur Ausgang' (für Diagnose)
#define ParamAIR_ClimateSetTemperature               ((bool)(knx.paramByte(AIR_ClimateSetTemperature) & AIR_ClimateSetTemperatureMask))
// Externe Raumtemperatur überwachen
#define ParamAIR_ExternTempWatchdog                  (PT_AIRExternTemperatureWatchdogBehavior)(knx.paramByte(AIR_ExternTempWatchdog) & AIR_ExternTempWatchdogMask)
// Überwachung Zeitbasis
#define ParamAIR_MonitoringWDTTimeoutDelayBase       ((knx.paramByte(AIR_MonitoringWDTTimeoutDelayBase) & AIR_MonitoringWDTTimeoutDelayBaseMask) >> AIR_MonitoringWDTTimeoutDelayBaseShift)
// Überwachung Zeit
#define ParamAIR_MonitoringWDTTimeoutDelayTime       (knx.paramWord(AIR_MonitoringWDTTimeoutDelayTime) & AIR_MonitoringWDTTimeoutDelayTimeMask)
// Überwachung Zeit (in Millisekunden)
#define ParamAIR_MonitoringWDTTimeoutDelayTimeMS     (paramDelay(knx.paramWord(AIR_MonitoringWDTTimeoutDelayTime)))
// Minimale Soll-Temperatur
#define ParamAIR_Mit_MinTemp                         (knx.paramByte(AIR_Mit_MinTemp))
// Szene A
#define ParamAIR_SCAActive                           ((bool)(knx.paramByte(AIR_SCAActive) & AIR_SCAActiveMask))
// Szenennummer
#define ParamAIR_SCANumber                           ((knx.paramByte(AIR_SCANumber) & AIR_SCANumberMask) >> AIR_SCANumberShift)
// Schalten
#define ParamAIR_SCAOnOff                            (PT_AIRSceneOnOff)(knx.paramByte(AIR_SCAOnOff))
// Betriebsmodus
#define ParamAIR_SCAOperationMode                    (knx.paramByte(AIR_SCAOperationMode))
// Solltemperatur
#define ParamAIR_SCASceneTemperature                 (knx.paramByte(AIR_SCASceneTemperature))
// Lüfter
#define ParamAIR_SCASceneFan                         (knx.paramByte(AIR_SCASceneFan))
// Lamellenbewegung
#define ParamAIR_SCASceneSwing                       (knx.paramByte(AIR_SCASceneSwing))
// Vertikale Lamellenstellung
#define ParamAIR_SCAScenePosition                    (knx.paramByte(AIR_SCAScenePosition))
// Leistungsbegrenzung
#define ParamAIR_SCAScenePowerLimit                  (knx.paramByte(AIR_SCAScenePowerLimit))
// Gerätemodus
#define ParamAIR_SCASceneDeviceMode                  (knx.paramByte(AIR_SCASceneDeviceMode))
// Luftreinigung
#define ParamAIR_SCASceneAirPurification             (knx.paramByte(AIR_SCASceneAirPurification))
// Betriebsmodus
#define ParamAIR_SCAOperationModeDaikin              (knx.paramByte(AIR_SCAOperationModeDaikin))
// Solltemperatur
#define ParamAIR_SCASceneTemperatureDaikin           (knx.paramByte(AIR_SCASceneTemperatureDaikin))
// Lüfter
#define ParamAIR_SCASceneFanDaikin                   (knx.paramByte(AIR_SCASceneFanDaikin))
// Lamellenbewegung
#define ParamAIR_SCASceneSwingDaikin                 (knx.paramByte(AIR_SCASceneSwingDaikin))
// Gerätemodus
#define ParamAIR_SCASceneDeviceModeDaikin            (knx.paramByte(AIR_SCASceneDeviceModeDaikin))
// Luftreinigung/Streamer
#define ParamAIR_SCASceneAirPurificationDaikin       (knx.paramByte(AIR_SCASceneAirPurificationDaikin))
// Betriebsmodus
#define ParamAIR_SCAOperationModeMitsubishi          (knx.paramByte(AIR_SCAOperationModeMitsubishi))
// Solltemperatur
#define ParamAIR_SCASceneTemperatureMitsubishi       (knx.paramByte(AIR_SCASceneTemperatureMitsubishi))
// Lüfter
#define ParamAIR_SCASceneFanMitsubishi               (knx.paramByte(AIR_SCASceneFanMitsubishi))
// Lamellenbewegung
#define ParamAIR_SCASceneSwingMitsubishi             (knx.paramByte(AIR_SCASceneSwingMitsubishi))
// Vertikale Lamellenstellung
#define ParamAIR_SCAScenePositionMitsubishi          (knx.paramByte(AIR_SCAScenePositionMitsubishi))
// Szene B
#define ParamAIR_SCBActive                           ((bool)(knx.paramByte(AIR_SCBActive) & AIR_SCBActiveMask))
// Szenennummer
#define ParamAIR_SCBNumber                           ((knx.paramByte(AIR_SCBNumber) & AIR_SCBNumberMask) >> AIR_SCBNumberShift)
// Schalten
#define ParamAIR_SCBOnOff                            (PT_AIRSceneOnOff)(knx.paramByte(AIR_SCBOnOff))
// Betriebsmodus
#define ParamAIR_SCBOperationMode                    (knx.paramByte(AIR_SCBOperationMode))
// Solltemperatur
#define ParamAIR_SCBSceneTemperature                 (knx.paramByte(AIR_SCBSceneTemperature))
// Lüfter
#define ParamAIR_SCBSceneFan                         (knx.paramByte(AIR_SCBSceneFan))
// Lamellenbewegung
#define ParamAIR_SCBSceneSwing                       (knx.paramByte(AIR_SCBSceneSwing))
// Vertikale Lamellenstellung
#define ParamAIR_SCBScenePosition                    (knx.paramByte(AIR_SCBScenePosition))
// Leistungsbegrenzung
#define ParamAIR_SCBScenePowerLimit                  (knx.paramByte(AIR_SCBScenePowerLimit))
// Gerätemodus
#define ParamAIR_SCBSceneDeviceMode                  (knx.paramByte(AIR_SCBSceneDeviceMode))
// Luftreinigung
#define ParamAIR_SCBSceneAirPurification             (knx.paramByte(AIR_SCBSceneAirPurification))
// Betriebsmodus
#define ParamAIR_SCBOperationModeDaikin              (knx.paramByte(AIR_SCBOperationModeDaikin))
// Solltemperatur
#define ParamAIR_SCBSceneTemperatureDaikin           (knx.paramByte(AIR_SCBSceneTemperatureDaikin))
// Lüfter
#define ParamAIR_SCBSceneFanDaikin                   (knx.paramByte(AIR_SCBSceneFanDaikin))
// Lamellenbewegung
#define ParamAIR_SCBSceneSwingDaikin                 (knx.paramByte(AIR_SCBSceneSwingDaikin))
// Gerätemodus
#define ParamAIR_SCBSceneDeviceModeDaikin            (knx.paramByte(AIR_SCBSceneDeviceModeDaikin))
// Luftreinigung/Streamer
#define ParamAIR_SCBSceneAirPurificationDaikin       (knx.paramByte(AIR_SCBSceneAirPurificationDaikin))
// Betriebsmodus
#define ParamAIR_SCBOperationModeMitsubishi          (knx.paramByte(AIR_SCBOperationModeMitsubishi))
// Solltemperatur
#define ParamAIR_SCBSceneTemperatureMitsubishi       (knx.paramByte(AIR_SCBSceneTemperatureMitsubishi))
// Lüfter
#define ParamAIR_SCBSceneFanMitsubishi               (knx.paramByte(AIR_SCBSceneFanMitsubishi))
// Lamellenbewegung
#define ParamAIR_SCBSceneSwingMitsubishi             (knx.paramByte(AIR_SCBSceneSwingMitsubishi))
// Vertikale Lamellenstellung
#define ParamAIR_SCBScenePositionMitsubishi          (knx.paramByte(AIR_SCBScenePositionMitsubishi))
// Szene C
#define ParamAIR_SCCActive                           ((bool)(knx.paramByte(AIR_SCCActive) & AIR_SCCActiveMask))
// Szenennummer
#define ParamAIR_SCCNumber                           ((knx.paramByte(AIR_SCCNumber) & AIR_SCCNumberMask) >> AIR_SCCNumberShift)
// Schalten
#define ParamAIR_SCCOnOff                            (PT_AIRSceneOnOff)(knx.paramByte(AIR_SCCOnOff))
// Betriebsmodus
#define ParamAIR_SCCOperationMode                    (knx.paramByte(AIR_SCCOperationMode))
// Solltemperatur
#define ParamAIR_SCCSceneTemperature                 (knx.paramByte(AIR_SCCSceneTemperature))
// Lüfter
#define ParamAIR_SCCSceneFan                         (knx.paramByte(AIR_SCCSceneFan))
// Lamellenbewegung
#define ParamAIR_SCCSceneSwing                       (knx.paramByte(AIR_SCCSceneSwing))
// Vertikale Lamellenstellung
#define ParamAIR_SCCScenePosition                    (knx.paramByte(AIR_SCCScenePosition))
// Leistungsbegrenzung
#define ParamAIR_SCCScenePowerLimit                  (knx.paramByte(AIR_SCCScenePowerLimit))
// Gerätemodus
#define ParamAIR_SCCSceneDeviceMode                  (knx.paramByte(AIR_SCCSceneDeviceMode))
// Luftreinigung
#define ParamAIR_SCCSceneAirPurification             (knx.paramByte(AIR_SCCSceneAirPurification))
// Betriebsmodus
#define ParamAIR_SCCOperationModeDaikin              (knx.paramByte(AIR_SCCOperationModeDaikin))
// Solltemperatur
#define ParamAIR_SCCSceneTemperatureDaikin           (knx.paramByte(AIR_SCCSceneTemperatureDaikin))
// Lüfter
#define ParamAIR_SCCSceneFanDaikin                   (knx.paramByte(AIR_SCCSceneFanDaikin))
// Lamellenbewegung
#define ParamAIR_SCCSceneSwingDaikin                 (knx.paramByte(AIR_SCCSceneSwingDaikin))
// Gerätemodus
#define ParamAIR_SCCSceneDeviceModeDaikin            (knx.paramByte(AIR_SCCSceneDeviceModeDaikin))
// Luftreinigung/Streamer
#define ParamAIR_SCCSceneAirPurificationDaikin       (knx.paramByte(AIR_SCCSceneAirPurificationDaikin))
// Betriebsmodus
#define ParamAIR_SCCOperationModeMitsubishi          (knx.paramByte(AIR_SCCOperationModeMitsubishi))
// Solltemperatur
#define ParamAIR_SCCSceneTemperatureMitsubishi       (knx.paramByte(AIR_SCCSceneTemperatureMitsubishi))
// Lüfter
#define ParamAIR_SCCSceneFanMitsubishi               (knx.paramByte(AIR_SCCSceneFanMitsubishi))
// Lamellenbewegung
#define ParamAIR_SCCSceneSwingMitsubishi             (knx.paramByte(AIR_SCCSceneSwingMitsubishi))
// Vertikale Lamellenstellung
#define ParamAIR_SCCScenePositionMitsubishi          (knx.paramByte(AIR_SCCScenePositionMitsubishi))
// Szene D
#define ParamAIR_SCDActive                           ((bool)(knx.paramByte(AIR_SCDActive) & AIR_SCDActiveMask))
// Szenennummer
#define ParamAIR_SCDNumber                           ((knx.paramByte(AIR_SCDNumber) & AIR_SCDNumberMask) >> AIR_SCDNumberShift)
// Schalten
#define ParamAIR_SCDOnOff                            (PT_AIRSceneOnOff)(knx.paramByte(AIR_SCDOnOff))
// Betriebsmodus
#define ParamAIR_SCDOperationMode                    (knx.paramByte(AIR_SCDOperationMode))
// Solltemperatur
#define ParamAIR_SCDSceneTemperature                 (knx.paramByte(AIR_SCDSceneTemperature))
// Lüfter
#define ParamAIR_SCDSceneFan                         (knx.paramByte(AIR_SCDSceneFan))
// Lamellenbewegung
#define ParamAIR_SCDSceneSwing                       (knx.paramByte(AIR_SCDSceneSwing))
// Vertikale Lamellenstellung
#define ParamAIR_SCDScenePosition                    (knx.paramByte(AIR_SCDScenePosition))
// Leistungsbegrenzung
#define ParamAIR_SCDScenePowerLimit                  (knx.paramByte(AIR_SCDScenePowerLimit))
// Gerätemodus
#define ParamAIR_SCDSceneDeviceMode                  (knx.paramByte(AIR_SCDSceneDeviceMode))
// Luftreinigung
#define ParamAIR_SCDSceneAirPurification             (knx.paramByte(AIR_SCDSceneAirPurification))
// Betriebsmodus
#define ParamAIR_SCDOperationModeDaikin              (knx.paramByte(AIR_SCDOperationModeDaikin))
// Solltemperatur
#define ParamAIR_SCDSceneTemperatureDaikin           (knx.paramByte(AIR_SCDSceneTemperatureDaikin))
// Lüfter
#define ParamAIR_SCDSceneFanDaikin                   (knx.paramByte(AIR_SCDSceneFanDaikin))
// Lamellenbewegung
#define ParamAIR_SCDSceneSwingDaikin                 (knx.paramByte(AIR_SCDSceneSwingDaikin))
// Gerätemodus
#define ParamAIR_SCDSceneDeviceModeDaikin            (knx.paramByte(AIR_SCDSceneDeviceModeDaikin))
// Luftreinigung/Streamer
#define ParamAIR_SCDSceneAirPurificationDaikin       (knx.paramByte(AIR_SCDSceneAirPurificationDaikin))
// Betriebsmodus
#define ParamAIR_SCDOperationModeMitsubishi          (knx.paramByte(AIR_SCDOperationModeMitsubishi))
// Solltemperatur
#define ParamAIR_SCDSceneTemperatureMitsubishi       (knx.paramByte(AIR_SCDSceneTemperatureMitsubishi))
// Lüfter
#define ParamAIR_SCDSceneFanMitsubishi               (knx.paramByte(AIR_SCDSceneFanMitsubishi))
// Lamellenbewegung
#define ParamAIR_SCDSceneSwingMitsubishi             (knx.paramByte(AIR_SCDSceneSwingMitsubishi))
// Vertikale Lamellenstellung
#define ParamAIR_SCDScenePositionMitsubishi          (knx.paramByte(AIR_SCDScenePositionMitsubishi))
// Szene E
#define ParamAIR_SCEActive                           ((bool)(knx.paramByte(AIR_SCEActive) & AIR_SCEActiveMask))
// Szenennummer
#define ParamAIR_SCENumber                           ((knx.paramByte(AIR_SCENumber) & AIR_SCENumberMask) >> AIR_SCENumberShift)
// Schalten
#define ParamAIR_SCEOnOff                            (PT_AIRSceneOnOff)(knx.paramByte(AIR_SCEOnOff))
// Betriebsmodus
#define ParamAIR_SCEOperationMode                    (knx.paramByte(AIR_SCEOperationMode))
// Solltemperatur
#define ParamAIR_SCESceneTemperature                 (knx.paramByte(AIR_SCESceneTemperature))
// Lüfter
#define ParamAIR_SCESceneFan                         (knx.paramByte(AIR_SCESceneFan))
// Lamellenbewegung
#define ParamAIR_SCESceneSwing                       (knx.paramByte(AIR_SCESceneSwing))
// Vertikale Lamellenstellung
#define ParamAIR_SCEScenePosition                    (knx.paramByte(AIR_SCEScenePosition))
// Leistungsbegrenzung
#define ParamAIR_SCEScenePowerLimit                  (knx.paramByte(AIR_SCEScenePowerLimit))
// Gerätemodus
#define ParamAIR_SCESceneDeviceMode                  (knx.paramByte(AIR_SCESceneDeviceMode))
// Luftreinigung
#define ParamAIR_SCESceneAirPurification             (knx.paramByte(AIR_SCESceneAirPurification))
// Betriebsmodus
#define ParamAIR_SCEOperationModeDaikin              (knx.paramByte(AIR_SCEOperationModeDaikin))
// Solltemperatur
#define ParamAIR_SCESceneTemperatureDaikin           (knx.paramByte(AIR_SCESceneTemperatureDaikin))
// Lüfter
#define ParamAIR_SCESceneFanDaikin                   (knx.paramByte(AIR_SCESceneFanDaikin))
// Lamellenbewegung
#define ParamAIR_SCESceneSwingDaikin                 (knx.paramByte(AIR_SCESceneSwingDaikin))
// Gerätemodus
#define ParamAIR_SCESceneDeviceModeDaikin            (knx.paramByte(AIR_SCESceneDeviceModeDaikin))
// Luftreinigung/Streamer
#define ParamAIR_SCESceneAirPurificationDaikin       (knx.paramByte(AIR_SCESceneAirPurificationDaikin))
// Betriebsmodus
#define ParamAIR_SCEOperationModeMitsubishi          (knx.paramByte(AIR_SCEOperationModeMitsubishi))
// Solltemperatur
#define ParamAIR_SCESceneTemperatureMitsubishi       (knx.paramByte(AIR_SCESceneTemperatureMitsubishi))
// Lüfter
#define ParamAIR_SCESceneFanMitsubishi               (knx.paramByte(AIR_SCESceneFanMitsubishi))
// Lamellenbewegung
#define ParamAIR_SCESceneSwingMitsubishi             (knx.paramByte(AIR_SCESceneSwingMitsubishi))
// Vertikale Lamellenstellung
#define ParamAIR_SCEScenePositionMitsubishi          (knx.paramByte(AIR_SCEScenePositionMitsubishi))
// Szene F
#define ParamAIR_SCFActive                           ((bool)(knx.paramByte(AIR_SCFActive) & AIR_SCFActiveMask))
// Szenennummer
#define ParamAIR_SCFNumber                           ((knx.paramByte(AIR_SCFNumber) & AIR_SCFNumberMask) >> AIR_SCFNumberShift)
// Schalten
#define ParamAIR_SCFOnOff                            (PT_AIRSceneOnOff)(knx.paramByte(AIR_SCFOnOff))
// Betriebsmodus
#define ParamAIR_SCFOperationMode                    (knx.paramByte(AIR_SCFOperationMode))
// Solltemperatur
#define ParamAIR_SCFSceneTemperature                 (knx.paramByte(AIR_SCFSceneTemperature))
// Lüfter
#define ParamAIR_SCFSceneFan                         (knx.paramByte(AIR_SCFSceneFan))
// Lamellenbewegung
#define ParamAIR_SCFSceneSwing                       (knx.paramByte(AIR_SCFSceneSwing))
// Vertikale Lamellenstellung
#define ParamAIR_SCFScenePosition                    (knx.paramByte(AIR_SCFScenePosition))
// Leistungsbegrenzung
#define ParamAIR_SCFScenePowerLimit                  (knx.paramByte(AIR_SCFScenePowerLimit))
// Gerätemodus
#define ParamAIR_SCFSceneDeviceMode                  (knx.paramByte(AIR_SCFSceneDeviceMode))
// Luftreinigung
#define ParamAIR_SCFSceneAirPurification             (knx.paramByte(AIR_SCFSceneAirPurification))
// Betriebsmodus
#define ParamAIR_SCFOperationModeDaikin              (knx.paramByte(AIR_SCFOperationModeDaikin))
// Solltemperatur
#define ParamAIR_SCFSceneTemperatureDaikin           (knx.paramByte(AIR_SCFSceneTemperatureDaikin))
// Lüfter
#define ParamAIR_SCFSceneFanDaikin                   (knx.paramByte(AIR_SCFSceneFanDaikin))
// Lamellenbewegung
#define ParamAIR_SCFSceneSwingDaikin                 (knx.paramByte(AIR_SCFSceneSwingDaikin))
// Gerätemodus
#define ParamAIR_SCFSceneDeviceModeDaikin            (knx.paramByte(AIR_SCFSceneDeviceModeDaikin))
// Luftreinigung/Streamer
#define ParamAIR_SCFSceneAirPurificationDaikin       (knx.paramByte(AIR_SCFSceneAirPurificationDaikin))
// Betriebsmodus
#define ParamAIR_SCFOperationModeMitsubishi          (knx.paramByte(AIR_SCFOperationModeMitsubishi))
// Solltemperatur
#define ParamAIR_SCFSceneTemperatureMitsubishi       (knx.paramByte(AIR_SCFSceneTemperatureMitsubishi))
// Lüfter
#define ParamAIR_SCFSceneFanMitsubishi               (knx.paramByte(AIR_SCFSceneFanMitsubishi))
// Lamellenbewegung
#define ParamAIR_SCFSceneSwingMitsubishi             (knx.paramByte(AIR_SCFSceneSwingMitsubishi))
// Vertikale Lamellenstellung
#define ParamAIR_SCFScenePositionMitsubishi          (knx.paramByte(AIR_SCFScenePositionMitsubishi))
// Szene G
#define ParamAIR_SCGActive                           ((bool)(knx.paramByte(AIR_SCGActive) & AIR_SCGActiveMask))
// Szenennummer
#define ParamAIR_SCGNumber                           ((knx.paramByte(AIR_SCGNumber) & AIR_SCGNumberMask) >> AIR_SCGNumberShift)
// Schalten
#define ParamAIR_SCGOnOff                            (PT_AIRSceneOnOff)(knx.paramByte(AIR_SCGOnOff))
// Betriebsmodus
#define ParamAIR_SCGOperationMode                    (knx.paramByte(AIR_SCGOperationMode))
// Solltemperatur
#define ParamAIR_SCGSceneTemperature                 (knx.paramByte(AIR_SCGSceneTemperature))
// Lüfter
#define ParamAIR_SCGSceneFan                         (knx.paramByte(AIR_SCGSceneFan))
// Lamellenbewegung
#define ParamAIR_SCGSceneSwing                       (knx.paramByte(AIR_SCGSceneSwing))
// Vertikale Lamellenstellung
#define ParamAIR_SCGScenePosition                    (knx.paramByte(AIR_SCGScenePosition))
// Leistungsbegrenzung
#define ParamAIR_SCGScenePowerLimit                  (knx.paramByte(AIR_SCGScenePowerLimit))
// Gerätemodus
#define ParamAIR_SCGSceneDeviceMode                  (knx.paramByte(AIR_SCGSceneDeviceMode))
// Luftreinigung
#define ParamAIR_SCGSceneAirPurification             (knx.paramByte(AIR_SCGSceneAirPurification))
// Betriebsmodus
#define ParamAIR_SCGOperationModeDaikin              (knx.paramByte(AIR_SCGOperationModeDaikin))
// Solltemperatur
#define ParamAIR_SCGSceneTemperatureDaikin           (knx.paramByte(AIR_SCGSceneTemperatureDaikin))
// Lüfter
#define ParamAIR_SCGSceneFanDaikin                   (knx.paramByte(AIR_SCGSceneFanDaikin))
// Lamellenbewegung
#define ParamAIR_SCGSceneSwingDaikin                 (knx.paramByte(AIR_SCGSceneSwingDaikin))
// Gerätemodus
#define ParamAIR_SCGSceneDeviceModeDaikin            (knx.paramByte(AIR_SCGSceneDeviceModeDaikin))
// Luftreinigung/Streamer
#define ParamAIR_SCGSceneAirPurificationDaikin       (knx.paramByte(AIR_SCGSceneAirPurificationDaikin))
// Betriebsmodus
#define ParamAIR_SCGOperationModeMitsubishi          (knx.paramByte(AIR_SCGOperationModeMitsubishi))
// Solltemperatur
#define ParamAIR_SCGSceneTemperatureMitsubishi       (knx.paramByte(AIR_SCGSceneTemperatureMitsubishi))
// Lüfter
#define ParamAIR_SCGSceneFanMitsubishi               (knx.paramByte(AIR_SCGSceneFanMitsubishi))
// Lamellenbewegung
#define ParamAIR_SCGSceneSwingMitsubishi             (knx.paramByte(AIR_SCGSceneSwingMitsubishi))
// Vertikale Lamellenstellung
#define ParamAIR_SCGScenePositionMitsubishi          (knx.paramByte(AIR_SCGScenePositionMitsubishi))
// Szene H
#define ParamAIR_SCHActive                           ((bool)(knx.paramByte(AIR_SCHActive) & AIR_SCHActiveMask))
// Szenennummer
#define ParamAIR_SCHNumber                           ((knx.paramByte(AIR_SCHNumber) & AIR_SCHNumberMask) >> AIR_SCHNumberShift)
// Schalten
#define ParamAIR_SCHOnOff                            (PT_AIRSceneOnOff)(knx.paramByte(AIR_SCHOnOff))
// Betriebsmodus
#define ParamAIR_SCHOperationMode                    (knx.paramByte(AIR_SCHOperationMode))
// Solltemperatur
#define ParamAIR_SCHSceneTemperature                 (knx.paramByte(AIR_SCHSceneTemperature))
// Lüfter
#define ParamAIR_SCHSceneFan                         (knx.paramByte(AIR_SCHSceneFan))
// Lamellenbewegung
#define ParamAIR_SCHSceneSwing                       (knx.paramByte(AIR_SCHSceneSwing))
// Vertikale Lamellenstellung
#define ParamAIR_SCHScenePosition                    (knx.paramByte(AIR_SCHScenePosition))
// Leistungsbegrenzung
#define ParamAIR_SCHScenePowerLimit                  (knx.paramByte(AIR_SCHScenePowerLimit))
// Gerätemodus
#define ParamAIR_SCHSceneDeviceMode                  (knx.paramByte(AIR_SCHSceneDeviceMode))
// Luftreinigung
#define ParamAIR_SCHSceneAirPurification             (knx.paramByte(AIR_SCHSceneAirPurification))
// Betriebsmodus
#define ParamAIR_SCHOperationModeDaikin              (knx.paramByte(AIR_SCHOperationModeDaikin))
// Solltemperatur
#define ParamAIR_SCHSceneTemperatureDaikin           (knx.paramByte(AIR_SCHSceneTemperatureDaikin))
// Lüfter
#define ParamAIR_SCHSceneFanDaikin                   (knx.paramByte(AIR_SCHSceneFanDaikin))
// Lamellenbewegung
#define ParamAIR_SCHSceneSwingDaikin                 (knx.paramByte(AIR_SCHSceneSwingDaikin))
// Gerätemodus
#define ParamAIR_SCHSceneDeviceModeDaikin            (knx.paramByte(AIR_SCHSceneDeviceModeDaikin))
// Luftreinigung/Streamer
#define ParamAIR_SCHSceneAirPurificationDaikin       (knx.paramByte(AIR_SCHSceneAirPurificationDaikin))
// Betriebsmodus
#define ParamAIR_SCHOperationModeMitsubishi          (knx.paramByte(AIR_SCHOperationModeMitsubishi))
// Solltemperatur
#define ParamAIR_SCHSceneTemperatureMitsubishi       (knx.paramByte(AIR_SCHSceneTemperatureMitsubishi))
// Lüfter
#define ParamAIR_SCHSceneFanMitsubishi               (knx.paramByte(AIR_SCHSceneFanMitsubishi))
// Lamellenbewegung
#define ParamAIR_SCHSceneSwingMitsubishi             (knx.paramByte(AIR_SCHSceneSwingMitsubishi))
// Vertikale Lamellenstellung
#define ParamAIR_SCHScenePositionMitsubishi          (knx.paramByte(AIR_SCHScenePositionMitsubishi))
// Szene I
#define ParamAIR_SCIActive                           ((bool)(knx.paramByte(AIR_SCIActive) & AIR_SCIActiveMask))
// Szenennummer
#define ParamAIR_SCINumber                           ((knx.paramByte(AIR_SCINumber) & AIR_SCINumberMask) >> AIR_SCINumberShift)
// Schalten
#define ParamAIR_SCIOnOff                            (PT_AIRSceneOnOff)(knx.paramByte(AIR_SCIOnOff))
// Betriebsmodus
#define ParamAIR_SCIOperationMode                    (knx.paramByte(AIR_SCIOperationMode))
// Solltemperatur
#define ParamAIR_SCISceneTemperature                 (knx.paramByte(AIR_SCISceneTemperature))
// Lüfter
#define ParamAIR_SCISceneFan                         (knx.paramByte(AIR_SCISceneFan))
// Lamellenbewegung
#define ParamAIR_SCISceneSwing                       (knx.paramByte(AIR_SCISceneSwing))
// Vertikale Lamellenstellung
#define ParamAIR_SCIScenePosition                    (knx.paramByte(AIR_SCIScenePosition))
// Leistungsbegrenzung
#define ParamAIR_SCIScenePowerLimit                  (knx.paramByte(AIR_SCIScenePowerLimit))
// Gerätemodus
#define ParamAIR_SCISceneDeviceMode                  (knx.paramByte(AIR_SCISceneDeviceMode))
// Luftreinigung
#define ParamAIR_SCISceneAirPurification             (knx.paramByte(AIR_SCISceneAirPurification))
// Betriebsmodus
#define ParamAIR_SCIOperationModeDaikin              (knx.paramByte(AIR_SCIOperationModeDaikin))
// Solltemperatur
#define ParamAIR_SCISceneTemperatureDaikin           (knx.paramByte(AIR_SCISceneTemperatureDaikin))
// Lüfter
#define ParamAIR_SCISceneFanDaikin                   (knx.paramByte(AIR_SCISceneFanDaikin))
// Lamellenbewegung
#define ParamAIR_SCISceneSwingDaikin                 (knx.paramByte(AIR_SCISceneSwingDaikin))
// Gerätemodus
#define ParamAIR_SCISceneDeviceModeDaikin            (knx.paramByte(AIR_SCISceneDeviceModeDaikin))
// Luftreinigung/Streamer
#define ParamAIR_SCISceneAirPurificationDaikin       (knx.paramByte(AIR_SCISceneAirPurificationDaikin))
// Betriebsmodus
#define ParamAIR_SCIOperationModeMitsubishi          (knx.paramByte(AIR_SCIOperationModeMitsubishi))
// Solltemperatur
#define ParamAIR_SCISceneTemperatureMitsubishi       (knx.paramByte(AIR_SCISceneTemperatureMitsubishi))
// Lüfter
#define ParamAIR_SCISceneFanMitsubishi               (knx.paramByte(AIR_SCISceneFanMitsubishi))
// Lamellenbewegung
#define ParamAIR_SCISceneSwingMitsubishi             (knx.paramByte(AIR_SCISceneSwingMitsubishi))
// Vertikale Lamellenstellung
#define ParamAIR_SCIScenePositionMitsubishi          (knx.paramByte(AIR_SCIScenePositionMitsubishi))
// Szene J
#define ParamAIR_SCJActive                           ((bool)(knx.paramByte(AIR_SCJActive) & AIR_SCJActiveMask))
// Szenennummer
#define ParamAIR_SCJNumber                           ((knx.paramByte(AIR_SCJNumber) & AIR_SCJNumberMask) >> AIR_SCJNumberShift)
// Schalten
#define ParamAIR_SCJOnOff                            (PT_AIRSceneOnOff)(knx.paramByte(AIR_SCJOnOff))
// Betriebsmodus
#define ParamAIR_SCJOperationMode                    (knx.paramByte(AIR_SCJOperationMode))
// Solltemperatur
#define ParamAIR_SCJSceneTemperature                 (knx.paramByte(AIR_SCJSceneTemperature))
// Lüfter
#define ParamAIR_SCJSceneFan                         (knx.paramByte(AIR_SCJSceneFan))
// Lamellenbewegung
#define ParamAIR_SCJSceneSwing                       (knx.paramByte(AIR_SCJSceneSwing))
// Vertikale Lamellenstellung
#define ParamAIR_SCJScenePosition                    (knx.paramByte(AIR_SCJScenePosition))
// Leistungsbegrenzung
#define ParamAIR_SCJScenePowerLimit                  (knx.paramByte(AIR_SCJScenePowerLimit))
// Gerätemodus
#define ParamAIR_SCJSceneDeviceMode                  (knx.paramByte(AIR_SCJSceneDeviceMode))
// Luftreinigung
#define ParamAIR_SCJSceneAirPurification             (knx.paramByte(AIR_SCJSceneAirPurification))
// Betriebsmodus
#define ParamAIR_SCJOperationModeDaikin              (knx.paramByte(AIR_SCJOperationModeDaikin))
// Solltemperatur
#define ParamAIR_SCJSceneTemperatureDaikin           (knx.paramByte(AIR_SCJSceneTemperatureDaikin))
// Lüfter
#define ParamAIR_SCJSceneFanDaikin                   (knx.paramByte(AIR_SCJSceneFanDaikin))
// Lamellenbewegung
#define ParamAIR_SCJSceneSwingDaikin                 (knx.paramByte(AIR_SCJSceneSwingDaikin))
// Gerätemodus
#define ParamAIR_SCJSceneDeviceModeDaikin            (knx.paramByte(AIR_SCJSceneDeviceModeDaikin))
// Luftreinigung/Streamer
#define ParamAIR_SCJSceneAirPurificationDaikin       (knx.paramByte(AIR_SCJSceneAirPurificationDaikin))
// Betriebsmodus
#define ParamAIR_SCJOperationModeMitsubishi          (knx.paramByte(AIR_SCJOperationModeMitsubishi))
// Solltemperatur
#define ParamAIR_SCJSceneTemperatureMitsubishi       (knx.paramByte(AIR_SCJSceneTemperatureMitsubishi))
// Lüfter
#define ParamAIR_SCJSceneFanMitsubishi               (knx.paramByte(AIR_SCJSceneFanMitsubishi))
// Lamellenbewegung
#define ParamAIR_SCJSceneSwingMitsubishi             (knx.paramByte(AIR_SCJSceneSwingMitsubishi))
// Vertikale Lamellenstellung
#define ParamAIR_SCJScenePositionMitsubishi          (knx.paramByte(AIR_SCJScenePositionMitsubishi))

#define AIR_KoPower 400
#define AIR_KoPowerState 401
#define AIR_KoOperationMode 402
#define AIR_KoOperationModeState 403
#define AIR_KoOperationModeAutomatic 404
#define AIR_KoOperationModeAutomaticState 405
#define AIR_KoOperationModeCooling 406
#define AIR_KoOperationModeCoolingState 407
#define AIR_KoOperationModeHeating 408
#define AIR_KoOperationModeHeatingState 409
#define AIR_KoOperationModeVentilation 410
#define AIR_KoOperationModeVentilationState 411
#define AIR_KoOperationModeDehumidification 412
#define AIR_KoOperationModeDehumidificationState 413
#define AIR_KoFanSpeed 414
#define AIR_KoFanSpeedState 415
#define AIR_KoFanSpeedUpDown 417
#define AIR_KoLouverVerticalSwing 418
#define AIR_KoLouverVerticalSwingState 419
#define AIR_KoLouverVerticalUpDown 420
#define AIR_KoLouverVerticalPosition 421
#define AIR_KoLouverVerticalPositionState 422
#define AIR_KoLouverHorizontalSwing 423
#define AIR_KoLouverHorizontalSwingState 424
#define AIR_KoLouverHorizontalLeftRight 425
#define AIR_KoLockRelease 426
#define AIR_KoLockReleaseState 427
#define AIR_KoSetTemperature 428
#define AIR_KoSetTemperatureState 429
#define AIR_KoSetTemperatureUpDown 430
#define AIR_KoRoomTemperatureState 431
#define AIR_KoOutsideTemperatureState 432
#define AIR_KoRoomTemperatureInput 433
#define AIR_KoScene 434
#define AIR_KoWifiLED 435
#define AIR_KoClimateTargetTemperatur 437
#define AIR_KoPowerAlarm 438
#define AIR_KoAirPurification 439
#define AIR_KoAirPurificationState 440
#define AIR_KoPowerLimit 448
#define AIR_KoPowerLimitState 449
#define AIR_KoDeviceMode 450
#define AIR_KoDeviceModeState 451
#define AIR_KoDeviceModeStandard 452
#define AIR_KoDeviceModeStandardState 453
#define AIR_KoDeviceModeHiPower 454
#define AIR_KoDeviceModeHiPowerState 455
#define AIR_KoDeviceModeEco 456
#define AIR_KoDeviceModeEcoState 457
#define AIR_KoDeviceModeSilent1 458
#define AIR_KoDeviceModeSilent1State 459
#define AIR_KoDeviceModeSilent2 460
#define AIR_KoDeviceModeSilent2State 461
#define AIR_KoHumidityState 462
#define AIR_KoOnlineState 463
#define AIR_KoHumidityModePercent 464
#define AIR_KoTotalEnergy 465

// Ein
#define KoAIR_Power                               (knx.getGroupObject(AIR_KoPower))
// Ein Status
#define KoAIR_PowerState                          (knx.getGroupObject(AIR_KoPowerState))
// Betriebsmodus
#define KoAIR_OperationMode                       (knx.getGroupObject(AIR_KoOperationMode))
// Betriebsmodus Status
#define KoAIR_OperationModeState                  (knx.getGroupObject(AIR_KoOperationModeState))
// Betriebsmodus Automatik
#define KoAIR_OperationModeAutomatic              (knx.getGroupObject(AIR_KoOperationModeAutomatic))
// Betriebsmodus Automatik Status
#define KoAIR_OperationModeAutomaticState         (knx.getGroupObject(AIR_KoOperationModeAutomaticState))
// Betriebsmodus Kühlen
#define KoAIR_OperationModeCooling                (knx.getGroupObject(AIR_KoOperationModeCooling))
// Betriebsmodus Kühlen Status
#define KoAIR_OperationModeCoolingState           (knx.getGroupObject(AIR_KoOperationModeCoolingState))
// Betriebsmodus Heizen
#define KoAIR_OperationModeHeating                (knx.getGroupObject(AIR_KoOperationModeHeating))
// Betriebsmodus Heizen Status
#define KoAIR_OperationModeHeatingState           (knx.getGroupObject(AIR_KoOperationModeHeatingState))
// Betriebsmodus Lüften
#define KoAIR_OperationModeVentilation            (knx.getGroupObject(AIR_KoOperationModeVentilation))
// Betriebsmodus Lüften Status
#define KoAIR_OperationModeVentilationState       (knx.getGroupObject(AIR_KoOperationModeVentilationState))
// Betriebsmodus Entfeuchten
#define KoAIR_OperationModeDehumidification       (knx.getGroupObject(AIR_KoOperationModeDehumidification))
// Betriebsmodus Entfeuchten Status
#define KoAIR_OperationModeDehumidificationState  (knx.getGroupObject(AIR_KoOperationModeDehumidificationState))
// Lüftergeschwindigkeit
#define KoAIR_FanSpeed                            (knx.getGroupObject(AIR_KoFanSpeed))
// Lüftergeschwindigkeit Status
#define KoAIR_FanSpeedState                       (knx.getGroupObject(AIR_KoFanSpeedState))
// Lüfter schneller/langsamer
#define KoAIR_FanSpeedUpDown                      (knx.getGroupObject(AIR_KoFanSpeedUpDown))
// Lamelle Vertikal Bewegen
#define KoAIR_LouverVerticalSwing                 (knx.getGroupObject(AIR_KoLouverVerticalSwing))
// Lamelle Vertikal Bewegen Status
#define KoAIR_LouverVerticalSwingState            (knx.getGroupObject(AIR_KoLouverVerticalSwingState))
// Lamelle Vertikal Höher/Tiefer
#define KoAIR_LouverVerticalUpDown                (knx.getGroupObject(AIR_KoLouverVerticalUpDown))
// Lamelle Vertikale Position
#define KoAIR_LouverVerticalPosition              (knx.getGroupObject(AIR_KoLouverVerticalPosition))
// Lamelle Vertikale Position Status
#define KoAIR_LouverVerticalPositionState         (knx.getGroupObject(AIR_KoLouverVerticalPositionState))
// Lamelle Horizontal Bewegen
#define KoAIR_LouverHorizontalSwing               (knx.getGroupObject(AIR_KoLouverHorizontalSwing))
// Lamelle Horizontal Bewegen Status
#define KoAIR_LouverHorizontalSwingState          (knx.getGroupObject(AIR_KoLouverHorizontalSwingState))
// Lamelle Horizontal Links/Rechts
#define KoAIR_LouverHorizontalLeftRight           (knx.getGroupObject(AIR_KoLouverHorizontalLeftRight))
// Sperre / Freigabe
#define KoAIR_LockRelease                         (knx.getGroupObject(AIR_KoLockRelease))
// Sperre / Freigabe Aktiv
#define KoAIR_LockReleaseState                    (knx.getGroupObject(AIR_KoLockReleaseState))
// Solltemperatur
#define KoAIR_SetTemperature                      (knx.getGroupObject(AIR_KoSetTemperature))
// Solltemperatur Status
#define KoAIR_SetTemperatureState                 (knx.getGroupObject(AIR_KoSetTemperatureState))
// Solltemperatur verringern/erhöhen
#define KoAIR_SetTemperatureUpDown                (knx.getGroupObject(AIR_KoSetTemperatureUpDown))
// Raumtemperatur Status
#define KoAIR_RoomTemperatureState                (knx.getGroupObject(AIR_KoRoomTemperatureState))
// Außentemperatur Status
#define KoAIR_OutsideTemperatureState             (knx.getGroupObject(AIR_KoOutsideTemperatureState))
// Eingang Raumtemperatur
#define KoAIR_RoomTemperatureInput                (knx.getGroupObject(AIR_KoRoomTemperatureInput))
// Szene
#define KoAIR_Scene                               (knx.getGroupObject(AIR_KoScene))
// Wifi LED Schalten
#define KoAIR_WifiLED                             (knx.getGroupObject(AIR_KoWifiLED))
// Klimagerät Solltemperatur Ausgang
#define KoAIR_ClimateTargetTemperatur             (knx.getGroupObject(AIR_KoClimateTargetTemperatur))
// Klimagerät Stromausfall
#define KoAIR_PowerAlarm                          (knx.getGroupObject(AIR_KoPowerAlarm))
// Luftreinigung
#define KoAIR_AirPurification                     (knx.getGroupObject(AIR_KoAirPurification))
// Luftreinigung Status
#define KoAIR_AirPurificationState                (knx.getGroupObject(AIR_KoAirPurificationState))
// Leistungsbegrenzung
#define KoAIR_PowerLimit                          (knx.getGroupObject(AIR_KoPowerLimit))
// Leistungsbegrenzung Status
#define KoAIR_PowerLimitState                     (knx.getGroupObject(AIR_KoPowerLimitState))
// Gerätemodus
#define KoAIR_DeviceMode                          (knx.getGroupObject(AIR_KoDeviceMode))
// Gerätemodus Status
#define KoAIR_DeviceModeState                     (knx.getGroupObject(AIR_KoDeviceModeState))
// Gerätemodus Standard
#define KoAIR_DeviceModeStandard                  (knx.getGroupObject(AIR_KoDeviceModeStandard))
// Gerätemodus Standard Status
#define KoAIR_DeviceModeStandardState             (knx.getGroupObject(AIR_KoDeviceModeStandardState))
// Gerätemodus Hi-Power
#define KoAIR_DeviceModeHiPower                   (knx.getGroupObject(AIR_KoDeviceModeHiPower))
// Gerätemodus Hi-Power Status
#define KoAIR_DeviceModeHiPowerState              (knx.getGroupObject(AIR_KoDeviceModeHiPowerState))
// Gerätemodus ECO
#define KoAIR_DeviceModeEco                       (knx.getGroupObject(AIR_KoDeviceModeEco))
// Gerätemodus ECO Status
#define KoAIR_DeviceModeEcoState                  (knx.getGroupObject(AIR_KoDeviceModeEcoState))
// Gerätemodus Außengerät Geräuscharm 1
#define KoAIR_DeviceModeSilent1                   (knx.getGroupObject(AIR_KoDeviceModeSilent1))
// Gerätemodus Außengerät Geräuscharm 1 Status
#define KoAIR_DeviceModeSilent1State              (knx.getGroupObject(AIR_KoDeviceModeSilent1State))
// Gerätemodus Außengerät Geräuscharm 2
#define KoAIR_DeviceModeSilent2                   (knx.getGroupObject(AIR_KoDeviceModeSilent2))
// Gerätemodus Außengerät Geräuscharm 2 Status
#define KoAIR_DeviceModeSilent2State              (knx.getGroupObject(AIR_KoDeviceModeSilent2State))
// Luftfeuchtigkeit Status
#define KoAIR_HumidityState                       (knx.getGroupObject(AIR_KoHumidityState))
// Gerät Online Status
#define KoAIR_OnlineState                         (knx.getGroupObject(AIR_KoOnlineState))
// Luftfeuchtigkeitsmodus
#define KoAIR_HumidityModePercent                 (knx.getGroupObject(AIR_KoHumidityModePercent))
// Gesamtenergieverbrauch
#define KoAIR_TotalEnergy                         (knx.getGroupObject(AIR_KoTotalEnergy))

#define LOG_VisibleChannels                     625      // uint8_t
#define LOG_VacationKo                          626      // 1 Bit, Bit 7
#define     LOG_VacationKoMask 0x80
#define     LOG_VacationKoShift 7
#define LOG_HolidayKo                           626      // 1 Bit, Bit 6
#define     LOG_HolidayKoMask 0x40
#define     LOG_HolidayKoShift 6
#define LOG_VacationRead                        626      // 1 Bit, Bit 5
#define     LOG_VacationReadMask 0x20
#define     LOG_VacationReadShift 5
#define LOG_HolidaySend                         626      // 1 Bit, Bit 4
#define     LOG_HolidaySendMask 0x10
#define     LOG_HolidaySendShift 4
#define LOG_Neujahr                             627      // 1 Bit, Bit 7
#define     LOG_NeujahrMask 0x80
#define     LOG_NeujahrShift 7
#define LOG_DreiKoenige                         627      // 1 Bit, Bit 6
#define     LOG_DreiKoenigeMask 0x40
#define     LOG_DreiKoenigeShift 6
#define LOG_Weiberfastnacht                     627      // 1 Bit, Bit 5
#define     LOG_WeiberfastnachtMask 0x20
#define     LOG_WeiberfastnachtShift 5
#define LOG_Rosenmontag                         627      // 1 Bit, Bit 4
#define     LOG_RosenmontagMask 0x10
#define     LOG_RosenmontagShift 4
#define LOG_Fastnachtsdienstag                  627      // 1 Bit, Bit 3
#define     LOG_FastnachtsdienstagMask 0x08
#define     LOG_FastnachtsdienstagShift 3
#define LOG_Aschermittwoch                      627      // 1 Bit, Bit 2
#define     LOG_AschermittwochMask 0x04
#define     LOG_AschermittwochShift 2
#define LOG_Frauentag                           627      // 1 Bit, Bit 1
#define     LOG_FrauentagMask 0x02
#define     LOG_FrauentagShift 1
#define LOG_Gruendonnerstag                     627      // 1 Bit, Bit 0
#define     LOG_GruendonnerstagMask 0x01
#define     LOG_GruendonnerstagShift 0
#define LOG_Karfreitag                          628      // 1 Bit, Bit 7
#define     LOG_KarfreitagMask 0x80
#define     LOG_KarfreitagShift 7
#define LOG_Ostersonntag                        628      // 1 Bit, Bit 6
#define     LOG_OstersonntagMask 0x40
#define     LOG_OstersonntagShift 6
#define LOG_Ostermontag                         628      // 1 Bit, Bit 5
#define     LOG_OstermontagMask 0x20
#define     LOG_OstermontagShift 5
#define LOG_TagDerArbeit                        628      // 1 Bit, Bit 4
#define     LOG_TagDerArbeitMask 0x10
#define     LOG_TagDerArbeitShift 4
#define LOG_Himmelfahrt                         628      // 1 Bit, Bit 3
#define     LOG_HimmelfahrtMask 0x08
#define     LOG_HimmelfahrtShift 3
#define LOG_Pfingstsonntag                      628      // 1 Bit, Bit 2
#define     LOG_PfingstsonntagMask 0x04
#define     LOG_PfingstsonntagShift 2
#define LOG_Pfingstmontag                       628      // 1 Bit, Bit 1
#define     LOG_PfingstmontagMask 0x02
#define     LOG_PfingstmontagShift 1
#define LOG_Fronleichnam                        628      // 1 Bit, Bit 0
#define     LOG_FronleichnamMask 0x01
#define     LOG_FronleichnamShift 0
#define LOG_Friedensfest                        629      // 1 Bit, Bit 7
#define     LOG_FriedensfestMask 0x80
#define     LOG_FriedensfestShift 7
#define LOG_MariaHimmelfahrt                    629      // 1 Bit, Bit 6
#define     LOG_MariaHimmelfahrtMask 0x40
#define     LOG_MariaHimmelfahrtShift 6
#define LOG_DeutscheEinheit                     629      // 1 Bit, Bit 5
#define     LOG_DeutscheEinheitMask 0x20
#define     LOG_DeutscheEinheitShift 5
#define LOG_Reformationstag                     629      // 1 Bit, Bit 4
#define     LOG_ReformationstagMask 0x10
#define     LOG_ReformationstagShift 4
#define LOG_Allerheiligen                       629      // 1 Bit, Bit 3
#define     LOG_AllerheiligenMask 0x08
#define     LOG_AllerheiligenShift 3
#define LOG_BussBettag                          629      // 1 Bit, Bit 2
#define     LOG_BussBettagMask 0x04
#define     LOG_BussBettagShift 2
#define LOG_Advent1                             629      // 1 Bit, Bit 1
#define     LOG_Advent1Mask 0x02
#define     LOG_Advent1Shift 1
#define LOG_Advent2                             629      // 1 Bit, Bit 0
#define     LOG_Advent2Mask 0x01
#define     LOG_Advent2Shift 0
#define LOG_Advent3                             630      // 1 Bit, Bit 7
#define     LOG_Advent3Mask 0x80
#define     LOG_Advent3Shift 7
#define LOG_Advent4                             630      // 1 Bit, Bit 6
#define     LOG_Advent4Mask 0x40
#define     LOG_Advent4Shift 6
#define LOG_Heiligabend                         630      // 1 Bit, Bit 5
#define     LOG_HeiligabendMask 0x20
#define     LOG_HeiligabendShift 5
#define LOG_Weihnachtstag1                      630      // 1 Bit, Bit 4
#define     LOG_Weihnachtstag1Mask 0x10
#define     LOG_Weihnachtstag1Shift 4
#define LOG_Weihnachtstag2                      630      // 1 Bit, Bit 3
#define     LOG_Weihnachtstag2Mask 0x08
#define     LOG_Weihnachtstag2Shift 3
#define LOG_Silvester                           630      // 1 Bit, Bit 2
#define     LOG_SilvesterMask 0x04
#define     LOG_SilvesterShift 2
#define LOG_Nationalfeiertag                    630      // 1 Bit, Bit 1
#define     LOG_NationalfeiertagMask 0x02
#define     LOG_NationalfeiertagShift 1
#define LOG_MariaEmpfaengnis                    630      // 1 Bit, Bit 0
#define     LOG_MariaEmpfaengnisMask 0x01
#define     LOG_MariaEmpfaengnisShift 0
#define LOG_NationalfeiertagSchweiz             631      // 1 Bit, Bit 7
#define     LOG_NationalfeiertagSchweizMask 0x80
#define     LOG_NationalfeiertagSchweizShift 7
#define LOG_Totensonntag                        631      // 1 Bit, Bit 6
#define     LOG_TotensonntagMask 0x40
#define     LOG_TotensonntagShift 6
#define LOG_Weltkindertag                       631      // 1 Bit, Bit 5
#define     LOG_WeltkindertagMask 0x20
#define     LOG_WeltkindertagShift 5
#define LOG_UserFormula1                        632      // char*, 99 Byte
#define     LOG_UserFormula1Length 99
#define LOG_UserFormula1Active                  731      // 1 Bit, Bit 7
#define     LOG_UserFormula1ActiveMask 0x80
#define     LOG_UserFormula1ActiveShift 7
#define LOG_UserFormula2                        732      // char*, 99 Byte
#define     LOG_UserFormula2Length 99
#define LOG_UserFormula2Active                  831      // 1 Bit, Bit 7
#define     LOG_UserFormula2ActiveMask 0x80
#define     LOG_UserFormula2ActiveShift 7
#define LOG_UserFormula3                        832      // char*, 99 Byte
#define     LOG_UserFormula3Length 99
#define LOG_UserFormula3Active                  931      // 1 Bit, Bit 7
#define     LOG_UserFormula3ActiveMask 0x80
#define     LOG_UserFormula3ActiveShift 7
#define LOG_UserFormula4                        932      // char*, 99 Byte
#define     LOG_UserFormula4Length 99
#define LOG_UserFormula4Active                  1031      // 1 Bit, Bit 7
#define     LOG_UserFormula4ActiveMask 0x80
#define     LOG_UserFormula4ActiveShift 7
#define LOG_UserFormula5                        1032      // char*, 99 Byte
#define     LOG_UserFormula5Length 99
#define LOG_UserFormula5Active                  1131      // 1 Bit, Bit 7
#define     LOG_UserFormula5ActiveMask 0x80
#define     LOG_UserFormula5ActiveShift 7
#define LOG_UserFormula6                        1132      // char*, 99 Byte
#define     LOG_UserFormula6Length 99
#define LOG_UserFormula6Active                  1231      // 1 Bit, Bit 7
#define     LOG_UserFormula6ActiveMask 0x80
#define     LOG_UserFormula6ActiveShift 7
#define LOG_UserFormula7                        1232      // char*, 99 Byte
#define     LOG_UserFormula7Length 99
#define LOG_UserFormula7Active                  1331      // 1 Bit, Bit 7
#define     LOG_UserFormula7ActiveMask 0x80
#define     LOG_UserFormula7ActiveShift 7
#define LOG_UserFormula8                        1332      // char*, 99 Byte
#define     LOG_UserFormula8Length 99
#define LOG_UserFormula8Active                  1431      // 1 Bit, Bit 7
#define     LOG_UserFormula8ActiveMask 0x80
#define     LOG_UserFormula8ActiveShift 7
#define LOG_UserFormula9                        1432      // char*, 99 Byte
#define     LOG_UserFormula9Length 99
#define LOG_UserFormula9Active                  1531      // 1 Bit, Bit 7
#define     LOG_UserFormula9ActiveMask 0x80
#define     LOG_UserFormula9ActiveShift 7
#define LOG_UserFormula10                       1532      // char*, 99 Byte
#define     LOG_UserFormula10Length 99
#define LOG_UserFormula10Active                 1631      // 1 Bit, Bit 7
#define     LOG_UserFormula10ActiveMask 0x80
#define     LOG_UserFormula10ActiveShift 7
#define LOG_UserFormula11                       1632      // char*, 99 Byte
#define     LOG_UserFormula11Length 99
#define LOG_UserFormula11Active                 1731      // 1 Bit, Bit 7
#define     LOG_UserFormula11ActiveMask 0x80
#define     LOG_UserFormula11ActiveShift 7
#define LOG_UserFormula12                       1732      // char*, 99 Byte
#define     LOG_UserFormula12Length 99
#define LOG_UserFormula12Active                 1831      // 1 Bit, Bit 7
#define     LOG_UserFormula12ActiveMask 0x80
#define     LOG_UserFormula12ActiveShift 7
#define LOG_UserFormula13                       1832      // char*, 99 Byte
#define     LOG_UserFormula13Length 99
#define LOG_UserFormula13Active                 1931      // 1 Bit, Bit 7
#define     LOG_UserFormula13ActiveMask 0x80
#define     LOG_UserFormula13ActiveShift 7
#define LOG_UserFormula14                       1932      // char*, 99 Byte
#define     LOG_UserFormula14Length 99
#define LOG_UserFormula14Active                 2031      // 1 Bit, Bit 7
#define     LOG_UserFormula14ActiveMask 0x80
#define     LOG_UserFormula14ActiveShift 7
#define LOG_UserFormula15                       2032      // char*, 99 Byte
#define     LOG_UserFormula15Length 99
#define LOG_UserFormula15Active                 2131      // 1 Bit, Bit 7
#define     LOG_UserFormula15ActiveMask 0x80
#define     LOG_UserFormula15ActiveShift 7
#define LOG_UserFormula16                       2132      // char*, 99 Byte
#define     LOG_UserFormula16Length 99
#define LOG_UserFormula16Active                 2231      // 1 Bit, Bit 7
#define     LOG_UserFormula16ActiveMask 0x80
#define     LOG_UserFormula16ActiveShift 7
#define LOG_UserFormula17                       2232      // char*, 99 Byte
#define     LOG_UserFormula17Length 99
#define LOG_UserFormula17Active                 2331      // 1 Bit, Bit 7
#define     LOG_UserFormula17ActiveMask 0x80
#define     LOG_UserFormula17ActiveShift 7
#define LOG_UserFormula18                       2332      // char*, 99 Byte
#define     LOG_UserFormula18Length 99
#define LOG_UserFormula18Active                 2431      // 1 Bit, Bit 7
#define     LOG_UserFormula18ActiveMask 0x80
#define     LOG_UserFormula18ActiveShift 7
#define LOG_UserFormula19                       2432      // char*, 99 Byte
#define     LOG_UserFormula19Length 99
#define LOG_UserFormula19Active                 2531      // 1 Bit, Bit 7
#define     LOG_UserFormula19ActiveMask 0x80
#define     LOG_UserFormula19ActiveShift 7
#define LOG_UserFormula20                       2532      // char*, 99 Byte
#define     LOG_UserFormula20Length 99
#define LOG_UserFormula20Active                 2631      // 1 Bit, Bit 7
#define     LOG_UserFormula20ActiveMask 0x80
#define     LOG_UserFormula20ActiveShift 7
#define LOG_UserFormula21                       2632      // char*, 99 Byte
#define     LOG_UserFormula21Length 99
#define LOG_UserFormula21Active                 2731      // 1 Bit, Bit 7
#define     LOG_UserFormula21ActiveMask 0x80
#define     LOG_UserFormula21ActiveShift 7
#define LOG_UserFormula22                       2732      // char*, 99 Byte
#define     LOG_UserFormula22Length 99
#define LOG_UserFormula22Active                 2831      // 1 Bit, Bit 7
#define     LOG_UserFormula22ActiveMask 0x80
#define     LOG_UserFormula22ActiveShift 7
#define LOG_UserFormula23                       2832      // char*, 99 Byte
#define     LOG_UserFormula23Length 99
#define LOG_UserFormula23Active                 2931      // 1 Bit, Bit 7
#define     LOG_UserFormula23ActiveMask 0x80
#define     LOG_UserFormula23ActiveShift 7
#define LOG_UserFormula24                       2932      // char*, 99 Byte
#define     LOG_UserFormula24Length 99
#define LOG_UserFormula24Active                 3031      // 1 Bit, Bit 7
#define     LOG_UserFormula24ActiveMask 0x80
#define     LOG_UserFormula24ActiveShift 7
#define LOG_UserFormula25                       3032      // char*, 99 Byte
#define     LOG_UserFormula25Length 99
#define LOG_UserFormula25Active                 3131      // 1 Bit, Bit 7
#define     LOG_UserFormula25ActiveMask 0x80
#define     LOG_UserFormula25ActiveShift 7
#define LOG_UserFormula26                       3132      // char*, 99 Byte
#define     LOG_UserFormula26Length 99
#define LOG_UserFormula26Active                 3231      // 1 Bit, Bit 7
#define     LOG_UserFormula26ActiveMask 0x80
#define     LOG_UserFormula26ActiveShift 7
#define LOG_UserFormula27                       3232      // char*, 99 Byte
#define     LOG_UserFormula27Length 99
#define LOG_UserFormula27Active                 3331      // 1 Bit, Bit 7
#define     LOG_UserFormula27ActiveMask 0x80
#define     LOG_UserFormula27ActiveShift 7
#define LOG_UserFormula28                       3332      // char*, 99 Byte
#define     LOG_UserFormula28Length 99
#define LOG_UserFormula28Active                 3431      // 1 Bit, Bit 7
#define     LOG_UserFormula28ActiveMask 0x80
#define     LOG_UserFormula28ActiveShift 7
#define LOG_UserFormula29                       3432      // char*, 99 Byte
#define     LOG_UserFormula29Length 99
#define LOG_UserFormula29Active                 3531      // 1 Bit, Bit 7
#define     LOG_UserFormula29ActiveMask 0x80
#define     LOG_UserFormula29ActiveShift 7
#define LOG_UserFormula30                       3532      // char*, 99 Byte
#define     LOG_UserFormula30Length 99
#define LOG_UserFormula30Active                 3631      // 1 Bit, Bit 7
#define     LOG_UserFormula30ActiveMask 0x80
#define     LOG_UserFormula30ActiveShift 7

// Verfügbare Kanäle
#define ParamLOG_VisibleChannels                     (knx.paramByte(LOG_VisibleChannels))
// Urlaubsbehandlung aktivieren?
#define ParamLOG_VacationKo                          ((bool)(knx.paramByte(LOG_VacationKo) & LOG_VacationKoMask))
// Feiertage auf dem Bus verfügbar machen?
#define ParamLOG_HolidayKo                           ((bool)(knx.paramByte(LOG_HolidayKo) & LOG_HolidayKoMask))
// Nach Neustart Urlaubsinfo lesen?
#define ParamLOG_VacationRead                        ((bool)(knx.paramByte(LOG_VacationRead) & LOG_VacationReadMask))
// Nach Neuberechnung Feiertagsinfo senden?
#define ParamLOG_HolidaySend                         ((bool)(knx.paramByte(LOG_HolidaySend) & LOG_HolidaySendMask))
// 1. Neujahr
#define ParamLOG_Neujahr                             ((bool)(knx.paramByte(LOG_Neujahr) & LOG_NeujahrMask))
// 2. Heilige Drei Könige
#define ParamLOG_DreiKoenige                         ((bool)(knx.paramByte(LOG_DreiKoenige) & LOG_DreiKoenigeMask))
// 3. Weiberfastnacht
#define ParamLOG_Weiberfastnacht                     ((bool)(knx.paramByte(LOG_Weiberfastnacht) & LOG_WeiberfastnachtMask))
// 4. Rosenmontag
#define ParamLOG_Rosenmontag                         ((bool)(knx.paramByte(LOG_Rosenmontag) & LOG_RosenmontagMask))
// 5. Fastnachtsdienstag
#define ParamLOG_Fastnachtsdienstag                  ((bool)(knx.paramByte(LOG_Fastnachtsdienstag) & LOG_FastnachtsdienstagMask))
// 6. Aschermittwoch
#define ParamLOG_Aschermittwoch                      ((bool)(knx.paramByte(LOG_Aschermittwoch) & LOG_AschermittwochMask))
// 7. Frauentag
#define ParamLOG_Frauentag                           ((bool)(knx.paramByte(LOG_Frauentag) & LOG_FrauentagMask))
// 8. Gründonnerstag
#define ParamLOG_Gruendonnerstag                     ((bool)(knx.paramByte(LOG_Gruendonnerstag) & LOG_GruendonnerstagMask))
// 9. Karfreitag
#define ParamLOG_Karfreitag                          ((bool)(knx.paramByte(LOG_Karfreitag) & LOG_KarfreitagMask))
// 10. Ostersonntag
#define ParamLOG_Ostersonntag                        ((bool)(knx.paramByte(LOG_Ostersonntag) & LOG_OstersonntagMask))
// 11. Ostermontag
#define ParamLOG_Ostermontag                         ((bool)(knx.paramByte(LOG_Ostermontag) & LOG_OstermontagMask))
// 12. Tag der Arbeit
#define ParamLOG_TagDerArbeit                        ((bool)(knx.paramByte(LOG_TagDerArbeit) & LOG_TagDerArbeitMask))
// 13. Christi Himmelfahrt
#define ParamLOG_Himmelfahrt                         ((bool)(knx.paramByte(LOG_Himmelfahrt) & LOG_HimmelfahrtMask))
// 14. Pfingstsonntag
#define ParamLOG_Pfingstsonntag                      ((bool)(knx.paramByte(LOG_Pfingstsonntag) & LOG_PfingstsonntagMask))
// 15. Pfingstmontag
#define ParamLOG_Pfingstmontag                       ((bool)(knx.paramByte(LOG_Pfingstmontag) & LOG_PfingstmontagMask))
// 16. Fronleichnam
#define ParamLOG_Fronleichnam                        ((bool)(knx.paramByte(LOG_Fronleichnam) & LOG_FronleichnamMask))
// 17. Hohes Friedensfest
#define ParamLOG_Friedensfest                        ((bool)(knx.paramByte(LOG_Friedensfest) & LOG_FriedensfestMask))
// 18. Mariä Himmelfahrt
#define ParamLOG_MariaHimmelfahrt                    ((bool)(knx.paramByte(LOG_MariaHimmelfahrt) & LOG_MariaHimmelfahrtMask))
// 19. Tag der Deutschen Einheit
#define ParamLOG_DeutscheEinheit                     ((bool)(knx.paramByte(LOG_DeutscheEinheit) & LOG_DeutscheEinheitMask))
// 20. Reformationstag
#define ParamLOG_Reformationstag                     ((bool)(knx.paramByte(LOG_Reformationstag) & LOG_ReformationstagMask))
// 21. Allerheiligen
#define ParamLOG_Allerheiligen                       ((bool)(knx.paramByte(LOG_Allerheiligen) & LOG_AllerheiligenMask))
// 22. Buß- und Bettag
#define ParamLOG_BussBettag                          ((bool)(knx.paramByte(LOG_BussBettag) & LOG_BussBettagMask))
// 23. Erster Advent
#define ParamLOG_Advent1                             ((bool)(knx.paramByte(LOG_Advent1) & LOG_Advent1Mask))
// 24. Zweiter Advent
#define ParamLOG_Advent2                             ((bool)(knx.paramByte(LOG_Advent2) & LOG_Advent2Mask))
// 25. Dritter Advent
#define ParamLOG_Advent3                             ((bool)(knx.paramByte(LOG_Advent3) & LOG_Advent3Mask))
// 26. Vierter Advent
#define ParamLOG_Advent4                             ((bool)(knx.paramByte(LOG_Advent4) & LOG_Advent4Mask))
// 27. Heiligabend
#define ParamLOG_Heiligabend                         ((bool)(knx.paramByte(LOG_Heiligabend) & LOG_HeiligabendMask))
// 28. Erster Weihnachtstag
#define ParamLOG_Weihnachtstag1                      ((bool)(knx.paramByte(LOG_Weihnachtstag1) & LOG_Weihnachtstag1Mask))
// 29. Zweiter Weihnachtstag
#define ParamLOG_Weihnachtstag2                      ((bool)(knx.paramByte(LOG_Weihnachtstag2) & LOG_Weihnachtstag2Mask))
// 30. Silvester
#define ParamLOG_Silvester                           ((bool)(knx.paramByte(LOG_Silvester) & LOG_SilvesterMask))
// 31. Nationalfeiertag (AT)
#define ParamLOG_Nationalfeiertag                    ((bool)(knx.paramByte(LOG_Nationalfeiertag) & LOG_NationalfeiertagMask))
// 32. Maria Empfängnis (AT)
#define ParamLOG_MariaEmpfaengnis                    ((bool)(knx.paramByte(LOG_MariaEmpfaengnis) & LOG_MariaEmpfaengnisMask))
// 33. Nationalfeiertag (CH)
#define ParamLOG_NationalfeiertagSchweiz             ((bool)(knx.paramByte(LOG_NationalfeiertagSchweiz) & LOG_NationalfeiertagSchweizMask))
// 34. Totensonntag
#define ParamLOG_Totensonntag                        ((bool)(knx.paramByte(LOG_Totensonntag) & LOG_TotensonntagMask))
// 35. Weltkindertag
#define ParamLOG_Weltkindertag                       ((bool)(knx.paramByte(LOG_Weltkindertag) & LOG_WeltkindertagMask))
// Formeldefinition
#define ParamLOG_UserFormula1                        (knx.paramData(LOG_UserFormula1))
#define ParamLOG_UserFormula1Str                     (knx.paramString(LOG_UserFormula1, LOG_UserFormula1Length))
// Benutzerformel 1 aktiv
#define ParamLOG_UserFormula1Active                  ((bool)(knx.paramByte(LOG_UserFormula1Active) & LOG_UserFormula1ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula2                        (knx.paramData(LOG_UserFormula2))
#define ParamLOG_UserFormula2Str                     (knx.paramString(LOG_UserFormula2, LOG_UserFormula2Length))
// Benutzerformel 2 aktiv
#define ParamLOG_UserFormula2Active                  ((bool)(knx.paramByte(LOG_UserFormula2Active) & LOG_UserFormula2ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula3                        (knx.paramData(LOG_UserFormula3))
#define ParamLOG_UserFormula3Str                     (knx.paramString(LOG_UserFormula3, LOG_UserFormula3Length))
// Benutzerformel 3 aktiv
#define ParamLOG_UserFormula3Active                  ((bool)(knx.paramByte(LOG_UserFormula3Active) & LOG_UserFormula3ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula4                        (knx.paramData(LOG_UserFormula4))
#define ParamLOG_UserFormula4Str                     (knx.paramString(LOG_UserFormula4, LOG_UserFormula4Length))
// Benutzerformel 4 aktiv
#define ParamLOG_UserFormula4Active                  ((bool)(knx.paramByte(LOG_UserFormula4Active) & LOG_UserFormula4ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula5                        (knx.paramData(LOG_UserFormula5))
#define ParamLOG_UserFormula5Str                     (knx.paramString(LOG_UserFormula5, LOG_UserFormula5Length))
// Benutzerformel 5 aktiv
#define ParamLOG_UserFormula5Active                  ((bool)(knx.paramByte(LOG_UserFormula5Active) & LOG_UserFormula5ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula6                        (knx.paramData(LOG_UserFormula6))
#define ParamLOG_UserFormula6Str                     (knx.paramString(LOG_UserFormula6, LOG_UserFormula6Length))
// Benutzerformel 6 aktiv
#define ParamLOG_UserFormula6Active                  ((bool)(knx.paramByte(LOG_UserFormula6Active) & LOG_UserFormula6ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula7                        (knx.paramData(LOG_UserFormula7))
#define ParamLOG_UserFormula7Str                     (knx.paramString(LOG_UserFormula7, LOG_UserFormula7Length))
// Benutzerformel 7 aktiv
#define ParamLOG_UserFormula7Active                  ((bool)(knx.paramByte(LOG_UserFormula7Active) & LOG_UserFormula7ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula8                        (knx.paramData(LOG_UserFormula8))
#define ParamLOG_UserFormula8Str                     (knx.paramString(LOG_UserFormula8, LOG_UserFormula8Length))
// Benutzerformel 8 aktiv
#define ParamLOG_UserFormula8Active                  ((bool)(knx.paramByte(LOG_UserFormula8Active) & LOG_UserFormula8ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula9                        (knx.paramData(LOG_UserFormula9))
#define ParamLOG_UserFormula9Str                     (knx.paramString(LOG_UserFormula9, LOG_UserFormula9Length))
// Benutzerformel 9 aktiv
#define ParamLOG_UserFormula9Active                  ((bool)(knx.paramByte(LOG_UserFormula9Active) & LOG_UserFormula9ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula10                       (knx.paramData(LOG_UserFormula10))
#define ParamLOG_UserFormula10Str                    (knx.paramString(LOG_UserFormula10, LOG_UserFormula10Length))
// Benutzerformel 10 aktiv
#define ParamLOG_UserFormula10Active                 ((bool)(knx.paramByte(LOG_UserFormula10Active) & LOG_UserFormula10ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula11                       (knx.paramData(LOG_UserFormula11))
#define ParamLOG_UserFormula11Str                    (knx.paramString(LOG_UserFormula11, LOG_UserFormula11Length))
// Benutzerformel 11 aktiv
#define ParamLOG_UserFormula11Active                 ((bool)(knx.paramByte(LOG_UserFormula11Active) & LOG_UserFormula11ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula12                       (knx.paramData(LOG_UserFormula12))
#define ParamLOG_UserFormula12Str                    (knx.paramString(LOG_UserFormula12, LOG_UserFormula12Length))
// Benutzerformel 12 aktiv
#define ParamLOG_UserFormula12Active                 ((bool)(knx.paramByte(LOG_UserFormula12Active) & LOG_UserFormula12ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula13                       (knx.paramData(LOG_UserFormula13))
#define ParamLOG_UserFormula13Str                    (knx.paramString(LOG_UserFormula13, LOG_UserFormula13Length))
// Benutzerformel 13 aktiv
#define ParamLOG_UserFormula13Active                 ((bool)(knx.paramByte(LOG_UserFormula13Active) & LOG_UserFormula13ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula14                       (knx.paramData(LOG_UserFormula14))
#define ParamLOG_UserFormula14Str                    (knx.paramString(LOG_UserFormula14, LOG_UserFormula14Length))
// Benutzerformel 14 aktiv
#define ParamLOG_UserFormula14Active                 ((bool)(knx.paramByte(LOG_UserFormula14Active) & LOG_UserFormula14ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula15                       (knx.paramData(LOG_UserFormula15))
#define ParamLOG_UserFormula15Str                    (knx.paramString(LOG_UserFormula15, LOG_UserFormula15Length))
// Benutzerformel 15 aktiv
#define ParamLOG_UserFormula15Active                 ((bool)(knx.paramByte(LOG_UserFormula15Active) & LOG_UserFormula15ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula16                       (knx.paramData(LOG_UserFormula16))
#define ParamLOG_UserFormula16Str                    (knx.paramString(LOG_UserFormula16, LOG_UserFormula16Length))
// Benutzerformel 16 aktiv
#define ParamLOG_UserFormula16Active                 ((bool)(knx.paramByte(LOG_UserFormula16Active) & LOG_UserFormula16ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula17                       (knx.paramData(LOG_UserFormula17))
#define ParamLOG_UserFormula17Str                    (knx.paramString(LOG_UserFormula17, LOG_UserFormula17Length))
// Benutzerformel 17 aktiv
#define ParamLOG_UserFormula17Active                 ((bool)(knx.paramByte(LOG_UserFormula17Active) & LOG_UserFormula17ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula18                       (knx.paramData(LOG_UserFormula18))
#define ParamLOG_UserFormula18Str                    (knx.paramString(LOG_UserFormula18, LOG_UserFormula18Length))
// Benutzerformel 18 aktiv
#define ParamLOG_UserFormula18Active                 ((bool)(knx.paramByte(LOG_UserFormula18Active) & LOG_UserFormula18ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula19                       (knx.paramData(LOG_UserFormula19))
#define ParamLOG_UserFormula19Str                    (knx.paramString(LOG_UserFormula19, LOG_UserFormula19Length))
// Benutzerformel 19 aktiv
#define ParamLOG_UserFormula19Active                 ((bool)(knx.paramByte(LOG_UserFormula19Active) & LOG_UserFormula19ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula20                       (knx.paramData(LOG_UserFormula20))
#define ParamLOG_UserFormula20Str                    (knx.paramString(LOG_UserFormula20, LOG_UserFormula20Length))
// Benutzerformel 20 aktiv
#define ParamLOG_UserFormula20Active                 ((bool)(knx.paramByte(LOG_UserFormula20Active) & LOG_UserFormula20ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula21                       (knx.paramData(LOG_UserFormula21))
#define ParamLOG_UserFormula21Str                    (knx.paramString(LOG_UserFormula21, LOG_UserFormula21Length))
// Benutzerformel 21 aktiv
#define ParamLOG_UserFormula21Active                 ((bool)(knx.paramByte(LOG_UserFormula21Active) & LOG_UserFormula21ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula22                       (knx.paramData(LOG_UserFormula22))
#define ParamLOG_UserFormula22Str                    (knx.paramString(LOG_UserFormula22, LOG_UserFormula22Length))
// Benutzerformel 22 aktiv
#define ParamLOG_UserFormula22Active                 ((bool)(knx.paramByte(LOG_UserFormula22Active) & LOG_UserFormula22ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula23                       (knx.paramData(LOG_UserFormula23))
#define ParamLOG_UserFormula23Str                    (knx.paramString(LOG_UserFormula23, LOG_UserFormula23Length))
// Benutzerformel 23 aktiv
#define ParamLOG_UserFormula23Active                 ((bool)(knx.paramByte(LOG_UserFormula23Active) & LOG_UserFormula23ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula24                       (knx.paramData(LOG_UserFormula24))
#define ParamLOG_UserFormula24Str                    (knx.paramString(LOG_UserFormula24, LOG_UserFormula24Length))
// Benutzerformel 24 aktiv
#define ParamLOG_UserFormula24Active                 ((bool)(knx.paramByte(LOG_UserFormula24Active) & LOG_UserFormula24ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula25                       (knx.paramData(LOG_UserFormula25))
#define ParamLOG_UserFormula25Str                    (knx.paramString(LOG_UserFormula25, LOG_UserFormula25Length))
// Benutzerformel 25 aktiv
#define ParamLOG_UserFormula25Active                 ((bool)(knx.paramByte(LOG_UserFormula25Active) & LOG_UserFormula25ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula26                       (knx.paramData(LOG_UserFormula26))
#define ParamLOG_UserFormula26Str                    (knx.paramString(LOG_UserFormula26, LOG_UserFormula26Length))
// Benutzerformel 26 aktiv
#define ParamLOG_UserFormula26Active                 ((bool)(knx.paramByte(LOG_UserFormula26Active) & LOG_UserFormula26ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula27                       (knx.paramData(LOG_UserFormula27))
#define ParamLOG_UserFormula27Str                    (knx.paramString(LOG_UserFormula27, LOG_UserFormula27Length))
// Benutzerformel 27 aktiv
#define ParamLOG_UserFormula27Active                 ((bool)(knx.paramByte(LOG_UserFormula27Active) & LOG_UserFormula27ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula28                       (knx.paramData(LOG_UserFormula28))
#define ParamLOG_UserFormula28Str                    (knx.paramString(LOG_UserFormula28, LOG_UserFormula28Length))
// Benutzerformel 28 aktiv
#define ParamLOG_UserFormula28Active                 ((bool)(knx.paramByte(LOG_UserFormula28Active) & LOG_UserFormula28ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula29                       (knx.paramData(LOG_UserFormula29))
#define ParamLOG_UserFormula29Str                    (knx.paramString(LOG_UserFormula29, LOG_UserFormula29Length))
// Benutzerformel 29 aktiv
#define ParamLOG_UserFormula29Active                 ((bool)(knx.paramByte(LOG_UserFormula29Active) & LOG_UserFormula29ActiveMask))
// Formeldefinition
#define ParamLOG_UserFormula30                       (knx.paramData(LOG_UserFormula30))
#define ParamLOG_UserFormula30Str                    (knx.paramString(LOG_UserFormula30, LOG_UserFormula30Length))
// Benutzerformel 30 aktiv
#define ParamLOG_UserFormula30Active                 ((bool)(knx.paramByte(LOG_UserFormula30Active) & LOG_UserFormula30ActiveMask))

#define LOG_KoVacation 15
#define LOG_KoHoliday1 16
#define LOG_KoHoliday2 17

// Urlaub
#define KoLOG_Vacation                            (knx.getGroupObject(LOG_KoVacation))
// Welcher Feiertag ist heute?
#define KoLOG_Holiday1                            (knx.getGroupObject(LOG_KoHoliday1))
// Welcher Feiertag ist morgen?
#define KoLOG_Holiday2                            (knx.getGroupObject(LOG_KoHoliday2))

#define LOG_ChannelCount 50

// Parameter per channel
#define LOG_ParamBlockOffset 3632
#define LOG_ParamBlockSize 89
#define LOG_ParamCalcIndex(index) (index + LOG_ParamBlockOffset + _channelIndex * LOG_ParamBlockSize)

#define LOG_fChannelDelayBase                    0      // 2 Bits, Bit 7-6
#define     LOG_fChannelDelayBaseMask 0xC0
#define     LOG_fChannelDelayBaseShift 6
#define LOG_fChannelDelayTime                    0      // 14 Bits, Bit 13-0
#define     LOG_fChannelDelayTimeMask 0x3FFF
#define     LOG_fChannelDelayTimeShift 0
#define LOG_fLogic                               2      // 8 Bits, Bit 7-0
#define LOG_fCalculate                           3      // 2 Bits, Bit 1-0
#define     LOG_fCalculateMask 0x03
#define     LOG_fCalculateShift 0
#define LOG_fDisable                             3      // 1 Bit, Bit 2
#define     LOG_fDisableMask 0x04
#define     LOG_fDisableShift 2
#define LOG_fTGate                               3      // 1 Bit, Bit 4
#define     LOG_fTGateMask 0x10
#define     LOG_fTGateShift 4
#define LOG_fOInternalOn                         3      // 1 Bit, Bit 5
#define     LOG_fOInternalOnMask 0x20
#define     LOG_fOInternalOnShift 5
#define LOG_fOInternalOff                        3      // 1 Bit, Bit 6
#define     LOG_fOInternalOffMask 0x40
#define     LOG_fOInternalOffShift 6
#define LOG_fTrigger                             4      // 8 Bits, Bit 7-0
#define LOG_fTriggerE1                           4      // 1 Bit, Bit 0
#define     LOG_fTriggerE1Mask 0x01
#define     LOG_fTriggerE1Shift 0
#define LOG_fTriggerE2                           4      // 1 Bit, Bit 1
#define     LOG_fTriggerE2Mask 0x02
#define     LOG_fTriggerE2Shift 1
#define LOG_fTriggerI1                           4      // 1 Bit, Bit 2
#define     LOG_fTriggerI1Mask 0x04
#define     LOG_fTriggerI1Shift 2
#define LOG_fTriggerI2                           4      // 1 Bit, Bit 3
#define     LOG_fTriggerI2Mask 0x08
#define     LOG_fTriggerI2Shift 3
#define LOG_fTriggerTime                         4      // 8 Bits, Bit 7-0
#define LOG_fTriggerGateClose                    5      // 2 Bits, Bit 7-6
#define     LOG_fTriggerGateCloseMask 0xC0
#define     LOG_fTriggerGateCloseShift 6
#define LOG_fTriggerGateOpen                     5      // 2 Bits, Bit 5-4
#define     LOG_fTriggerGateOpenMask 0x30
#define     LOG_fTriggerGateOpenShift 4
#define LOG_fE1ConvertInt                        6      // 4 Bits, Bit 7-4
#define     LOG_fE1ConvertIntMask 0xF0
#define     LOG_fE1ConvertIntShift 4
#define LOG_fE1Convert                           6      // 4 Bits, Bit 7-4
#define     LOG_fE1ConvertMask 0xF0
#define     LOG_fE1ConvertShift 4
#define LOG_fE1ConvertFloat                      6      // 4 Bits, Bit 7-4
#define     LOG_fE1ConvertFloatMask 0xF0
#define     LOG_fE1ConvertFloatShift 4
#define LOG_fE1ConvertSpecial                    6      // 4 Bits, Bit 7-4
#define     LOG_fE1ConvertSpecialMask 0xF0
#define     LOG_fE1ConvertSpecialShift 4
#define LOG_fE1ConvertBool                       6      // 4 Bits, Bit 7-4
#define     LOG_fE1ConvertBoolMask 0xF0
#define     LOG_fE1ConvertBoolShift 4
#define LOG_fE1                                  6      // 2 Bits, Bit 1-0
#define     LOG_fE1Mask 0x03
#define     LOG_fE1Shift 0
#define LOG_fE1Dpt                               7      // 8 Bits, Bit 7-0
#define LOG_fE1RepeatBase                        8      // 2 Bits, Bit 7-6
#define     LOG_fE1RepeatBaseMask 0xC0
#define     LOG_fE1RepeatBaseShift 6
#define LOG_fE1RepeatTime                        8      // 14 Bits, Bit 13-0
#define     LOG_fE1RepeatTimeMask 0x3FFF
#define     LOG_fE1RepeatTimeShift 0
#define LOG_fE1OtherKO                          10      // uint16_t
#define LOG_fE1OtherKORel                       10      // int16_t
#define LOG_fE1Default                          12      // 2 Bits, Bit 1-0
#define     LOG_fE1DefaultMask 0x03
#define     LOG_fE1DefaultShift 0
#define LOG_fE1DefaultExt                       12      // 2 Bits, Bit 1-0
#define     LOG_fE1DefaultExtMask 0x03
#define     LOG_fE1DefaultExtShift 0
#define LOG_fE1DefaultEEPROM                    12      // 1 Bit, Bit 2
#define     LOG_fE1DefaultEEPROMMask 0x04
#define     LOG_fE1DefaultEEPROMShift 2
#define LOG_fE1DefaultRepeat                    12      // 1 Bit, Bit 3
#define     LOG_fE1DefaultRepeatMask 0x08
#define     LOG_fE1DefaultRepeatShift 3
#define LOG_fE1UseOtherKO                       12      // 2 Bits, Bit 5-4
#define     LOG_fE1UseOtherKOMask 0x30
#define     LOG_fE1UseOtherKOShift 4
#define LOG_fE1LowDelta                         13      // int32_t
#define LOG_fE1HighDelta                        17      // int32_t
#define LOG_fE1LowDeltaFloat                    13      // float (4 Byte)
#define LOG_fE1HighDeltaFloat                   17      // float (4 Byte)
#define LOG_fE1LowDeltaDouble                   13      // float (4 Byte)
#define LOG_fE1HighDeltaDouble                  17      // float (4 Byte)
#define LOG_fE1Low0Valid                        20      // 1 Bit, Bit 7
#define     LOG_fE1Low0ValidMask 0x80
#define     LOG_fE1Low0ValidShift 7
#define LOG_fE1Low1Valid                        20      // 1 Bit, Bit 6
#define     LOG_fE1Low1ValidMask 0x40
#define     LOG_fE1Low1ValidShift 6
#define LOG_fE1Low2Valid                        20      // 1 Bit, Bit 5
#define     LOG_fE1Low2ValidMask 0x20
#define     LOG_fE1Low2ValidShift 5
#define LOG_fE1Low3Valid                        20      // 1 Bit, Bit 4
#define     LOG_fE1Low3ValidMask 0x10
#define     LOG_fE1Low3ValidShift 4
#define LOG_fE1Low4Valid                        20      // 1 Bit, Bit 3
#define     LOG_fE1Low4ValidMask 0x08
#define     LOG_fE1Low4ValidShift 3
#define LOG_fE1Low5Valid                        20      // 1 Bit, Bit 2
#define     LOG_fE1Low5ValidMask 0x04
#define     LOG_fE1Low5ValidShift 2
#define LOG_fE1Low6Valid                        20      // 1 Bit, Bit 1
#define     LOG_fE1Low6ValidMask 0x02
#define     LOG_fE1Low6ValidShift 1
#define LOG_fE1Low0Dpt2                         13      // 8 Bits, Bit 7-0
#define LOG_fE1Low1Dpt2                         14      // 8 Bits, Bit 7-0
#define LOG_fE1Low2Dpt2                         15      // 8 Bits, Bit 7-0
#define LOG_fE1Low3Dpt2                         16      // 8 Bits, Bit 7-0
#define LOG_fE1LowDpt2Fix                       13      // 8 Bits, Bit 7-0
#define LOG_fE1Low0Dpt3Dir                      13      // 5 Bits, Bit 7-3
#define     LOG_fE1Low0Dpt3DirMask 0xF8
#define     LOG_fE1Low0Dpt3DirShift 3
#define LOG_fE1Low0Dpt3Dim                      13      // 3 Bits, Bit 2-0
#define     LOG_fE1Low0Dpt3DimMask 0x07
#define     LOG_fE1Low0Dpt3DimShift 0
#define LOG_fE1Low1Dpt3Dir                      14      // 5 Bits, Bit 7-3
#define     LOG_fE1Low1Dpt3DirMask 0xF8
#define     LOG_fE1Low1Dpt3DirShift 3
#define LOG_fE1Low1Dpt3Dim                      14      // 3 Bits, Bit 2-0
#define     LOG_fE1Low1Dpt3DimMask 0x07
#define     LOG_fE1Low1Dpt3DimShift 0
#define LOG_fE1Low2Dpt3Dir                      15      // 5 Bits, Bit 7-3
#define     LOG_fE1Low2Dpt3DirMask 0xF8
#define     LOG_fE1Low2Dpt3DirShift 3
#define LOG_fE1Low2Dpt3Dim                      15      // 3 Bits, Bit 2-0
#define     LOG_fE1Low2Dpt3DimMask 0x07
#define     LOG_fE1Low2Dpt3DimShift 0
#define LOG_fE1Low3Dpt3Dir                      16      // 5 Bits, Bit 7-3
#define     LOG_fE1Low3Dpt3DirMask 0xF8
#define     LOG_fE1Low3Dpt3DirShift 3
#define LOG_fE1Low3Dpt3Dim                      16      // 3 Bits, Bit 2-0
#define     LOG_fE1Low3Dpt3DimMask 0x07
#define     LOG_fE1Low3Dpt3DimShift 0
#define LOG_fE1LowDpt3FixDir                    13      // 5 Bits, Bit 7-3
#define     LOG_fE1LowDpt3FixDirMask 0xF8
#define     LOG_fE1LowDpt3FixDirShift 3
#define LOG_fE1LowDpt3FixDim                    13      // 3 Bits, Bit 2-0
#define     LOG_fE1LowDpt3FixDimMask 0x07
#define     LOG_fE1LowDpt3FixDimShift 0
#define LOG_fE1LowDpt5                          13      // uint8_t
#define LOG_fE1HighDpt5                         17      // uint8_t
#define LOG_fE1Low0Dpt5In                       13      // uint8_t
#define LOG_fE1Low1Dpt5In                       14      // uint8_t
#define LOG_fE1Low2Dpt5In                       15      // uint8_t
#define LOG_fE1Low3Dpt5In                       16      // uint8_t
#define LOG_fE1Low4Dpt5In                       17      // uint8_t
#define LOG_fE1Low5Dpt5In                       18      // uint8_t
#define LOG_fE1Low6Dpt5In                       19      // uint8_t
#define LOG_fE1LowDpt5Fix                       13      // uint8_t
#define LOG_fE1LowDpt5001                       13      // uint8_t
#define LOG_fE1HighDpt5001                      17      // uint8_t
#define LOG_fE1Low0Dpt5xIn                      13      // uint8_t
#define LOG_fE1Low1Dpt5xIn                      14      // uint8_t
#define LOG_fE1Low2Dpt5xIn                      15      // uint8_t
#define LOG_fE1Low3Dpt5xIn                      16      // uint8_t
#define LOG_fE1Low4Dpt5xIn                      17      // uint8_t
#define LOG_fE1Low5Dpt5xIn                      18      // uint8_t
#define LOG_fE1Low6Dpt5xIn                      19      // uint8_t
#define LOG_fE1LowDpt5xFix                      13      // uint8_t
#define LOG_fE1LowDpt6                          13      // int8_t
#define LOG_fE1HighDpt6                         17      // int8_t
#define LOG_fE1Low0Dpt6In                       13      // int8_t
#define LOG_fE1Low1Dpt6In                       14      // int8_t
#define LOG_fE1Low2Dpt6In                       15      // int8_t
#define LOG_fE1Low3Dpt6In                       16      // int8_t
#define LOG_fE1Low4Dpt6In                       17      // int8_t
#define LOG_fE1Low5Dpt6In                       18      // int8_t
#define LOG_fE1Low6Dpt6In                       19      // int8_t
#define LOG_fE1LowDpt6Fix                       13      // int8_t
#define LOG_fE1LowDpt7                          13      // uint16_t
#define LOG_fE1HighDpt7                         17      // uint16_t
#define LOG_fE1Low0Dpt7In                       13      // uint16_t
#define LOG_fE1Low1Dpt7In                       15      // uint16_t
#define LOG_fE1Low2Dpt7In                       17      // uint16_t
#define LOG_fE1LowDpt7Fix                       13      // uint16_t
#define LOG_fE1LowDpt8                          13      // int16_t
#define LOG_fE1HighDpt8                         17      // int16_t
#define LOG_fE1Low0Dpt8In                       13      // int16_t
#define LOG_fE1Low1Dpt8In                       15      // int16_t
#define LOG_fE1Low2Dpt8In                       17      // int16_t
#define LOG_fE1LowDpt8Fix                       13      // int16_t
#define LOG_fE1LowDpt9                          13      // float (4 Byte)
#define LOG_fE1HighDpt9                         17      // float (4 Byte)
#define LOG_fE1LowDpt9Fix                       13      // float (4 Byte)
#define LOG_fE1LowDpt12                         13      // uint32_t
#define LOG_fE1HighDpt12                        17      // uint32_t
#define LOG_fE1LowDpt12Fix                      13      // uint32_t
#define LOG_fE1LowDpt13                         13      // int32_t
#define LOG_fE1HighDpt13                        17      // int32_t
#define LOG_fE1LowDpt13Fix                      13      // int32_t
#define LOG_fE1LowDpt14                         13      // float (4 Byte)
#define LOG_fE1HighDpt14                        17      // float (4 Byte)
#define LOG_fE1LowDpt14Fix                      13      // float (4 Byte)
#define LOG_fE1Low0Dpt17                        13      // 8 Bits, Bit 7-0
#define LOG_fE1Low1Dpt17                        14      // 8 Bits, Bit 7-0
#define LOG_fE1Low2Dpt17                        15      // 8 Bits, Bit 7-0
#define LOG_fE1Low3Dpt17                        16      // 8 Bits, Bit 7-0
#define LOG_fE1Low4Dpt17                        17      // 8 Bits, Bit 7-0
#define LOG_fE1Low5Dpt17                        18      // 8 Bits, Bit 7-0
#define LOG_fE1Low6Dpt17                        19      // 8 Bits, Bit 7-0
#define LOG_fE1Low7Dpt17                        20      // 8 Bits, Bit 7-0
#define LOG_fE1LowDpt17Fix                      13      // 8 Bits, Bit 7-0
#define LOG_fE1LowDptRGB                        13      // int32_t
#define LOG_fE1HighDptRGB                       17      // int32_t
#define LOG_fE1LowDptRGBFix                     13      // int32_t
#define LOG_fE2ConvertInt                       21      // 4 Bits, Bit 7-4
#define     LOG_fE2ConvertIntMask 0xF0
#define     LOG_fE2ConvertIntShift 4
#define LOG_fE2Convert                          21      // 4 Bits, Bit 7-4
#define     LOG_fE2ConvertMask 0xF0
#define     LOG_fE2ConvertShift 4
#define LOG_fE2ConvertFloat                     21      // 4 Bits, Bit 7-4
#define     LOG_fE2ConvertFloatMask 0xF0
#define     LOG_fE2ConvertFloatShift 4
#define LOG_fE2ConvertSpecial                   21      // 4 Bits, Bit 7-4
#define     LOG_fE2ConvertSpecialMask 0xF0
#define     LOG_fE2ConvertSpecialShift 4
#define LOG_fE2ConvertBool                      21      // 4 Bits, Bit 7-4
#define     LOG_fE2ConvertBoolMask 0xF0
#define     LOG_fE2ConvertBoolShift 4
#define LOG_fE2                                 21      // 2 Bits, Bit 1-0
#define     LOG_fE2Mask 0x03
#define     LOG_fE2Shift 0
#define LOG_fE2Dpt                              22      // 8 Bits, Bit 7-0
#define LOG_fE2RepeatBase                       23      // 2 Bits, Bit 7-6
#define     LOG_fE2RepeatBaseMask 0xC0
#define     LOG_fE2RepeatBaseShift 6
#define LOG_fE2RepeatTime                       23      // 14 Bits, Bit 13-0
#define     LOG_fE2RepeatTimeMask 0x3FFF
#define     LOG_fE2RepeatTimeShift 0
#define LOG_fE2OtherKO                          25      // uint16_t
#define LOG_fE2OtherKORel                       25      // int16_t
#define LOG_fE2Default                          27      // 2 Bits, Bit 1-0
#define     LOG_fE2DefaultMask 0x03
#define     LOG_fE2DefaultShift 0
#define LOG_fE2DefaultExt                       27      // 2 Bits, Bit 1-0
#define     LOG_fE2DefaultExtMask 0x03
#define     LOG_fE2DefaultExtShift 0
#define LOG_fE2DefaultEEPROM                    27      // 1 Bit, Bit 2
#define     LOG_fE2DefaultEEPROMMask 0x04
#define     LOG_fE2DefaultEEPROMShift 2
#define LOG_fE2DefaultRepeat                    27      // 1 Bit, Bit 3
#define     LOG_fE2DefaultRepeatMask 0x08
#define     LOG_fE2DefaultRepeatShift 3
#define LOG_fE2UseOtherKO                       27      // 2 Bits, Bit 5-4
#define     LOG_fE2UseOtherKOMask 0x30
#define     LOG_fE2UseOtherKOShift 4
#define LOG_fE2LowDelta                         28      // int32_t
#define LOG_fE2HighDelta                        32      // int32_t
#define LOG_fE2LowDeltaFloat                    28      // float (4 Byte)
#define LOG_fE2HighDeltaFloat                   32      // float (4 Byte)
#define LOG_fE2LowDeltaDouble                   28      // float (4 Byte)
#define LOG_fE2HighDeltaDouble                  32      // float (4 Byte)
#define LOG_fE2Low0Valid                        35      // 1 Bit, Bit 7
#define     LOG_fE2Low0ValidMask 0x80
#define     LOG_fE2Low0ValidShift 7
#define LOG_fE2Low1Valid                        35      // 1 Bit, Bit 6
#define     LOG_fE2Low1ValidMask 0x40
#define     LOG_fE2Low1ValidShift 6
#define LOG_fE2Low2Valid                        35      // 1 Bit, Bit 5
#define     LOG_fE2Low2ValidMask 0x20
#define     LOG_fE2Low2ValidShift 5
#define LOG_fE2Low3Valid                        35      // 1 Bit, Bit 4
#define     LOG_fE2Low3ValidMask 0x10
#define     LOG_fE2Low3ValidShift 4
#define LOG_fE2Low4Valid                        35      // 1 Bit, Bit 3
#define     LOG_fE2Low4ValidMask 0x08
#define     LOG_fE2Low4ValidShift 3
#define LOG_fE2Low5Valid                        35      // 1 Bit, Bit 2
#define     LOG_fE2Low5ValidMask 0x04
#define     LOG_fE2Low5ValidShift 2
#define LOG_fE2Low6Valid                        35      // 1 Bit, Bit 1
#define     LOG_fE2Low6ValidMask 0x02
#define     LOG_fE2Low6ValidShift 1
#define LOG_fE2Low0Dpt2                         28      // 8 Bits, Bit 7-0
#define LOG_fE2Low1Dpt2                         29      // 8 Bits, Bit 7-0
#define LOG_fE2Low2Dpt2                         30      // 8 Bits, Bit 7-0
#define LOG_fE2Low3Dpt2                         31      // 8 Bits, Bit 7-0
#define LOG_fE2LowDpt2Fix                       28      // 8 Bits, Bit 7-0
#define LOG_fE2Low0Dpt3Dir                      28      // 5 Bits, Bit 7-3
#define     LOG_fE2Low0Dpt3DirMask 0xF8
#define     LOG_fE2Low0Dpt3DirShift 3
#define LOG_fE2Low0Dpt3Dim                      28      // 3 Bits, Bit 2-0
#define     LOG_fE2Low0Dpt3DimMask 0x07
#define     LOG_fE2Low0Dpt3DimShift 0
#define LOG_fE2Low1Dpt3Dir                      29      // 5 Bits, Bit 7-3
#define     LOG_fE2Low1Dpt3DirMask 0xF8
#define     LOG_fE2Low1Dpt3DirShift 3
#define LOG_fE2Low1Dpt3Dim                      29      // 3 Bits, Bit 2-0
#define     LOG_fE2Low1Dpt3DimMask 0x07
#define     LOG_fE2Low1Dpt3DimShift 0
#define LOG_fE2Low2Dpt3Dir                      30      // 5 Bits, Bit 7-3
#define     LOG_fE2Low2Dpt3DirMask 0xF8
#define     LOG_fE2Low2Dpt3DirShift 3
#define LOG_fE2Low2Dpt3Dim                      30      // 3 Bits, Bit 2-0
#define     LOG_fE2Low2Dpt3DimMask 0x07
#define     LOG_fE2Low2Dpt3DimShift 0
#define LOG_fE2Low3Dpt3Dir                      31      // 5 Bits, Bit 7-3
#define     LOG_fE2Low3Dpt3DirMask 0xF8
#define     LOG_fE2Low3Dpt3DirShift 3
#define LOG_fE2Low3Dpt3Dim                      31      // 3 Bits, Bit 2-0
#define     LOG_fE2Low3Dpt3DimMask 0x07
#define     LOG_fE2Low3Dpt3DimShift 0
#define LOG_fE2LowDpt3FixDir                    28      // 5 Bits, Bit 7-3
#define     LOG_fE2LowDpt3FixDirMask 0xF8
#define     LOG_fE2LowDpt3FixDirShift 3
#define LOG_fE2LowDpt3FixDim                    28      // 3 Bits, Bit 2-0
#define     LOG_fE2LowDpt3FixDimMask 0x07
#define     LOG_fE2LowDpt3FixDimShift 0
#define LOG_fE2LowDpt5                          28      // uint8_t
#define LOG_fE2HighDpt5                         32      // uint8_t
#define LOG_fE2Low0Dpt5In                       28      // uint8_t
#define LOG_fE2Low1Dpt5In                       29      // uint8_t
#define LOG_fE2Low2Dpt5In                       30      // uint8_t
#define LOG_fE2Low3Dpt5In                       31      // uint8_t
#define LOG_fE2Low4Dpt5In                       32      // uint8_t
#define LOG_fE2Low5Dpt5In                       33      // uint8_t
#define LOG_fE2Low6Dpt5In                       34      // uint8_t
#define LOG_fE2LowDpt5Fix                       28      // uint8_t
#define LOG_fE2LowDpt5001                       28      // uint8_t
#define LOG_fE2HighDpt5001                      32      // uint8_t
#define LOG_fE2Low0Dpt5xIn                      28      // uint8_t
#define LOG_fE2Low1Dpt5xIn                      29      // uint8_t
#define LOG_fE2Low2Dpt5xIn                      30      // uint8_t
#define LOG_fE2Low3Dpt5xIn                      31      // uint8_t
#define LOG_fE2Low4Dpt5xIn                      32      // uint8_t
#define LOG_fE2Low5Dpt5xIn                      33      // uint8_t
#define LOG_fE2Low6Dpt5xIn                      34      // uint8_t
#define LOG_fE2LowDpt5xFix                      28      // uint8_t
#define LOG_fE2LowDpt6                          28      // int8_t
#define LOG_fE2HighDpt6                         32      // int8_t
#define LOG_fE2Low0Dpt6In                       28      // int8_t
#define LOG_fE2Low1Dpt6In                       29      // int8_t
#define LOG_fE2Low2Dpt6In                       30      // int8_t
#define LOG_fE2Low3Dpt6In                       31      // int8_t
#define LOG_fE2Low4Dpt6In                       32      // int8_t
#define LOG_fE2Low5Dpt6In                       33      // int8_t
#define LOG_fE2Low6Dpt6In                       34      // int8_t
#define LOG_fE2LowDpt6Fix                       28      // int8_t
#define LOG_fE2LowDpt7                          28      // uint16_t
#define LOG_fE2HighDpt7                         32      // uint16_t
#define LOG_fE2Low0Dpt7In                       28      // uint16_t
#define LOG_fE2Low1Dpt7In                       30      // uint16_t
#define LOG_fE2Low2Dpt7In                       32      // uint16_t
#define LOG_fE2LowDpt7Fix                       28      // uint16_t
#define LOG_fE2LowDpt8                          28      // int16_t
#define LOG_fE2HighDpt8                         32      // int16_t
#define LOG_fE2Low0Dpt8In                       28      // int16_t
#define LOG_fE2Low1Dpt8In                       30      // int16_t
#define LOG_fE2Low2Dpt8In                       32      // int16_t
#define LOG_fE2LowDpt8Fix                       28      // int16_t
#define LOG_fE2LowDpt9                          28      // float (4 Byte)
#define LOG_fE2HighDpt9                         32      // float (4 Byte)
#define LOG_fE2LowDpt9Fix                       28      // float (4 Byte)
#define LOG_fE2LowDpt12                         28      // uint32_t
#define LOG_fE2HighDpt12                        32      // uint32_t
#define LOG_fE2LowDpt12Fix                      28      // uint32_t
#define LOG_fE2LowDpt13                         28      // int32_t
#define LOG_fE2HighDpt13                        32      // int32_t
#define LOG_fE2LowDpt13Fix                      28      // int32_t
#define LOG_fE2LowDpt14                         28      // float (4 Byte)
#define LOG_fE2HighDpt14                        32      // float (4 Byte)
#define LOG_fE2LowDpt14Fix                      28      // float (4 Byte)
#define LOG_fE2Low0Dpt17                        28      // 8 Bits, Bit 7-0
#define LOG_fE2Low1Dpt17                        29      // 8 Bits, Bit 7-0
#define LOG_fE2Low2Dpt17                        30      // 8 Bits, Bit 7-0
#define LOG_fE2Low3Dpt17                        31      // 8 Bits, Bit 7-0
#define LOG_fE2Low4Dpt17                        32      // 8 Bits, Bit 7-0
#define LOG_fE2Low5Dpt17                        33      // 8 Bits, Bit 7-0
#define LOG_fE2Low6Dpt17                        34      // 8 Bits, Bit 7-0
#define LOG_fE2Low7Dpt17                        35      // 8 Bits, Bit 7-0
#define LOG_fE2LowDpt17Fix                      28      // 8 Bits, Bit 7-0
#define LOG_fE2LowDptRGB                        28      // int32_t
#define LOG_fE2HighDptRGB                       32      // int32_t
#define LOG_fE2LowDptRGBFix                     28      // int32_t
#define LOG_fTd1DuskDawn                         6      // 4 Bits, Bit 7-4
#define     LOG_fTd1DuskDawnMask 0xF0
#define     LOG_fTd1DuskDawnShift 4
#define LOG_fTd2DuskDawn                         6      // 4 Bits, Bit 3-0
#define     LOG_fTd2DuskDawnMask 0x0F
#define     LOG_fTd2DuskDawnShift 0
#define LOG_fTd3DuskDawn                         7      // 4 Bits, Bit 7-4
#define     LOG_fTd3DuskDawnMask 0xF0
#define     LOG_fTd3DuskDawnShift 4
#define LOG_fTd4DuskDawn                         7      // 4 Bits, Bit 3-0
#define     LOG_fTd4DuskDawnMask 0x0F
#define     LOG_fTd4DuskDawnShift 0
#define LOG_fTd5DuskDawn                         8      // 4 Bits, Bit 7-4
#define     LOG_fTd5DuskDawnMask 0xF0
#define     LOG_fTd5DuskDawnShift 4
#define LOG_fTd6DuskDawn                         8      // 4 Bits, Bit 3-0
#define     LOG_fTd6DuskDawnMask 0x0F
#define     LOG_fTd6DuskDawnShift 0
#define LOG_fTd7DuskDawn                         9      // 4 Bits, Bit 7-4
#define     LOG_fTd7DuskDawnMask 0xF0
#define     LOG_fTd7DuskDawnShift 4
#define LOG_fTd8DuskDawn                         9      // 4 Bits, Bit 3-0
#define     LOG_fTd8DuskDawnMask 0x0F
#define     LOG_fTd8DuskDawnShift 0
#define LOG_fTYearDay                           10      // 2 Bits, Bit 7-6
#define     LOG_fTYearDayMask 0xC0
#define     LOG_fTYearDayShift 6
#define LOG_fTHoliday                           10      // 2 Bits, Bit 5-4
#define     LOG_fTHolidayMask 0x30
#define     LOG_fTHolidayShift 4
#define LOG_fTRestoreState                      10      // 2 Bits, Bit 3-2
#define     LOG_fTRestoreStateMask 0x0C
#define     LOG_fTRestoreStateShift 2
#define LOG_fTVacation                          10      // 2 Bits, Bit 1-0
#define     LOG_fTVacationMask 0x03
#define     LOG_fTVacationShift 0
#define LOG_fTd1ValueNum                        11      // uint8_t
#define LOG_fTd2ValueNum                        12      // uint8_t
#define LOG_fTd3ValueNum                        13      // uint8_t
#define LOG_fTd4ValueNum                        14      // uint8_t
#define LOG_fTd5ValueNum                        15      // uint8_t
#define LOG_fTd6ValueNum                        16      // uint8_t
#define LOG_fTd7ValueNum                        17      // uint8_t
#define LOG_fTd8ValueNum                        18      // uint8_t
#define LOG_fTd1Value                           20      // 1 Bit, Bit 7
#define     LOG_fTd1ValueMask 0x80
#define     LOG_fTd1ValueShift 7
#define LOG_fTd1Degree                          20      // 6 Bits, Bit 6-1
#define     LOG_fTd1DegreeMask 0x7E
#define     LOG_fTd1DegreeShift 1
#define LOG_fTd1HourAbs                         20      // 5 Bits, Bit 5-1
#define     LOG_fTd1HourAbsMask 0x3E
#define     LOG_fTd1HourAbsShift 1
#define LOG_fTd1HourRel                         20      // 5 Bits, Bit 5-1
#define     LOG_fTd1HourRelMask 0x3E
#define     LOG_fTd1HourRelShift 1
#define LOG_fTd1HourRelShort                    20      // 5 Bits, Bit 5-1
#define     LOG_fTd1HourRelShortMask 0x3E
#define     LOG_fTd1HourRelShortShift 1
#define LOG_fTd1MinuteAbs                       20      // 6 Bits, Bit 0--5
#define LOG_fTd1MinuteRel                       20      // 6 Bits, Bit 0--5
#define LOG_fTd1Weekday                         21      // 3 Bits, Bit 2-0
#define     LOG_fTd1WeekdayMask 0x07
#define     LOG_fTd1WeekdayShift 0
#define LOG_fTd2Value                           22      // 1 Bit, Bit 7
#define     LOG_fTd2ValueMask 0x80
#define     LOG_fTd2ValueShift 7
#define LOG_fTd2Degree                          22      // 6 Bits, Bit 6-1
#define     LOG_fTd2DegreeMask 0x7E
#define     LOG_fTd2DegreeShift 1
#define LOG_fTd2HourAbs                         22      // 5 Bits, Bit 5-1
#define     LOG_fTd2HourAbsMask 0x3E
#define     LOG_fTd2HourAbsShift 1
#define LOG_fTd2HourRel                         22      // 5 Bits, Bit 5-1
#define     LOG_fTd2HourRelMask 0x3E
#define     LOG_fTd2HourRelShift 1
#define LOG_fTd2HourRelShort                    22      // 5 Bits, Bit 5-1
#define     LOG_fTd2HourRelShortMask 0x3E
#define     LOG_fTd2HourRelShortShift 1
#define LOG_fTd2MinuteAbs                       22      // 6 Bits, Bit 0--5
#define LOG_fTd2MinuteRel                       22      // 6 Bits, Bit 0--5
#define LOG_fTd2Weekday                         23      // 3 Bits, Bit 2-0
#define     LOG_fTd2WeekdayMask 0x07
#define     LOG_fTd2WeekdayShift 0
#define LOG_fTd3Value                           24      // 1 Bit, Bit 7
#define     LOG_fTd3ValueMask 0x80
#define     LOG_fTd3ValueShift 7
#define LOG_fTd3Degree                          24      // 6 Bits, Bit 6-1
#define     LOG_fTd3DegreeMask 0x7E
#define     LOG_fTd3DegreeShift 1
#define LOG_fTd3HourAbs                         24      // 5 Bits, Bit 5-1
#define     LOG_fTd3HourAbsMask 0x3E
#define     LOG_fTd3HourAbsShift 1
#define LOG_fTd3HourRel                         24      // 5 Bits, Bit 5-1
#define     LOG_fTd3HourRelMask 0x3E
#define     LOG_fTd3HourRelShift 1
#define LOG_fTd3HourRelShort                    24      // 5 Bits, Bit 5-1
#define     LOG_fTd3HourRelShortMask 0x3E
#define     LOG_fTd3HourRelShortShift 1
#define LOG_fTd3MinuteAbs                       24      // 6 Bits, Bit 0--5
#define LOG_fTd3MinuteRel                       24      // 6 Bits, Bit 0--5
#define LOG_fTd3Weekday                         25      // 3 Bits, Bit 2-0
#define     LOG_fTd3WeekdayMask 0x07
#define     LOG_fTd3WeekdayShift 0
#define LOG_fTd4Value                           26      // 1 Bit, Bit 7
#define     LOG_fTd4ValueMask 0x80
#define     LOG_fTd4ValueShift 7
#define LOG_fTd4Degree                          26      // 6 Bits, Bit 6-1
#define     LOG_fTd4DegreeMask 0x7E
#define     LOG_fTd4DegreeShift 1
#define LOG_fTd4HourAbs                         26      // 5 Bits, Bit 5-1
#define     LOG_fTd4HourAbsMask 0x3E
#define     LOG_fTd4HourAbsShift 1
#define LOG_fTd4HourRel                         26      // 5 Bits, Bit 5-1
#define     LOG_fTd4HourRelMask 0x3E
#define     LOG_fTd4HourRelShift 1
#define LOG_fTd4HourRelShort                    26      // 5 Bits, Bit 5-1
#define     LOG_fTd4HourRelShortMask 0x3E
#define     LOG_fTd4HourRelShortShift 1
#define LOG_fTd4MinuteAbs                       26      // 6 Bits, Bit 0--5
#define LOG_fTd4MinuteRel                       26      // 6 Bits, Bit 0--5
#define LOG_fTd4Weekday                         27      // 3 Bits, Bit 2-0
#define     LOG_fTd4WeekdayMask 0x07
#define     LOG_fTd4WeekdayShift 0
#define LOG_fTd5Value                           28      // 1 Bit, Bit 7
#define     LOG_fTd5ValueMask 0x80
#define     LOG_fTd5ValueShift 7
#define LOG_fTd5Degree                          28      // 6 Bits, Bit 6-1
#define     LOG_fTd5DegreeMask 0x7E
#define     LOG_fTd5DegreeShift 1
#define LOG_fTd5HourAbs                         28      // 5 Bits, Bit 5-1
#define     LOG_fTd5HourAbsMask 0x3E
#define     LOG_fTd5HourAbsShift 1
#define LOG_fTd5HourRel                         28      // 5 Bits, Bit 5-1
#define     LOG_fTd5HourRelMask 0x3E
#define     LOG_fTd5HourRelShift 1
#define LOG_fTd5HourRelShort                    28      // 5 Bits, Bit 5-1
#define     LOG_fTd5HourRelShortMask 0x3E
#define     LOG_fTd5HourRelShortShift 1
#define LOG_fTd5MinuteAbs                       28      // 6 Bits, Bit 0--5
#define LOG_fTd5MinuteRel                       28      // 6 Bits, Bit 0--5
#define LOG_fTd5Weekday                         29      // 3 Bits, Bit 2-0
#define     LOG_fTd5WeekdayMask 0x07
#define     LOG_fTd5WeekdayShift 0
#define LOG_fTd6Value                           30      // 1 Bit, Bit 7
#define     LOG_fTd6ValueMask 0x80
#define     LOG_fTd6ValueShift 7
#define LOG_fTd6Degree                          30      // 6 Bits, Bit 6-1
#define     LOG_fTd6DegreeMask 0x7E
#define     LOG_fTd6DegreeShift 1
#define LOG_fTd6HourAbs                         30      // 5 Bits, Bit 5-1
#define     LOG_fTd6HourAbsMask 0x3E
#define     LOG_fTd6HourAbsShift 1
#define LOG_fTd6HourRel                         30      // 5 Bits, Bit 5-1
#define     LOG_fTd6HourRelMask 0x3E
#define     LOG_fTd6HourRelShift 1
#define LOG_fTd6HourRelShort                    30      // 5 Bits, Bit 5-1
#define     LOG_fTd6HourRelShortMask 0x3E
#define     LOG_fTd6HourRelShortShift 1
#define LOG_fTd6MinuteAbs                       30      // 6 Bits, Bit 0--5
#define LOG_fTd6MinuteRel                       30      // 6 Bits, Bit 0--5
#define LOG_fTd6Weekday                         31      // 3 Bits, Bit 2-0
#define     LOG_fTd6WeekdayMask 0x07
#define     LOG_fTd6WeekdayShift 0
#define LOG_fTd7Value                           32      // 1 Bit, Bit 7
#define     LOG_fTd7ValueMask 0x80
#define     LOG_fTd7ValueShift 7
#define LOG_fTd7Degree                          32      // 6 Bits, Bit 6-1
#define     LOG_fTd7DegreeMask 0x7E
#define     LOG_fTd7DegreeShift 1
#define LOG_fTd7HourAbs                         32      // 5 Bits, Bit 5-1
#define     LOG_fTd7HourAbsMask 0x3E
#define     LOG_fTd7HourAbsShift 1
#define LOG_fTd7HourRel                         32      // 5 Bits, Bit 5-1
#define     LOG_fTd7HourRelMask 0x3E
#define     LOG_fTd7HourRelShift 1
#define LOG_fTd7HourRelShort                    32      // 5 Bits, Bit 5-1
#define     LOG_fTd7HourRelShortMask 0x3E
#define     LOG_fTd7HourRelShortShift 1
#define LOG_fTd7MinuteAbs                       32      // 6 Bits, Bit 0--5
#define LOG_fTd7MinuteRel                       32      // 6 Bits, Bit 0--5
#define LOG_fTd7Weekday                         33      // 3 Bits, Bit 2-0
#define     LOG_fTd7WeekdayMask 0x07
#define     LOG_fTd7WeekdayShift 0
#define LOG_fTd8Value                           34      // 1 Bit, Bit 7
#define     LOG_fTd8ValueMask 0x80
#define     LOG_fTd8ValueShift 7
#define LOG_fTd8Degree                          34      // 6 Bits, Bit 6-1
#define     LOG_fTd8DegreeMask 0x7E
#define     LOG_fTd8DegreeShift 1
#define LOG_fTd8HourAbs                         34      // 5 Bits, Bit 5-1
#define     LOG_fTd8HourAbsMask 0x3E
#define     LOG_fTd8HourAbsShift 1
#define LOG_fTd8HourRel                         34      // 5 Bits, Bit 5-1
#define     LOG_fTd8HourRelMask 0x3E
#define     LOG_fTd8HourRelShift 1
#define LOG_fTd8HourRelShort                    34      // 5 Bits, Bit 5-1
#define     LOG_fTd8HourRelShortMask 0x3E
#define     LOG_fTd8HourRelShortShift 1
#define LOG_fTd8MinuteAbs                       34      // 6 Bits, Bit 0--5
#define LOG_fTd8MinuteRel                       34      // 6 Bits, Bit 0--5
#define LOG_fTd8Weekday                         35      // 3 Bits, Bit 2-0
#define     LOG_fTd8WeekdayMask 0x07
#define     LOG_fTd8WeekdayShift 0
#define LOG_fTy1Weekday1                        28      // 1 Bit, Bit 7
#define     LOG_fTy1Weekday1Mask 0x80
#define     LOG_fTy1Weekday1Shift 7
#define LOG_fTy1Weekday2                        28      // 1 Bit, Bit 6
#define     LOG_fTy1Weekday2Mask 0x40
#define     LOG_fTy1Weekday2Shift 6
#define LOG_fTy1Weekday3                        28      // 1 Bit, Bit 5
#define     LOG_fTy1Weekday3Mask 0x20
#define     LOG_fTy1Weekday3Shift 5
#define LOG_fTy1Weekday4                        28      // 1 Bit, Bit 4
#define     LOG_fTy1Weekday4Mask 0x10
#define     LOG_fTy1Weekday4Shift 4
#define LOG_fTy1Weekday5                        28      // 1 Bit, Bit 3
#define     LOG_fTy1Weekday5Mask 0x08
#define     LOG_fTy1Weekday5Shift 3
#define LOG_fTy1Weekday6                        28      // 1 Bit, Bit 2
#define     LOG_fTy1Weekday6Mask 0x04
#define     LOG_fTy1Weekday6Shift 2
#define LOG_fTy1Weekday7                        28      // 1 Bit, Bit 1
#define     LOG_fTy1Weekday7Mask 0x02
#define     LOG_fTy1Weekday7Shift 1
#define LOG_fTy1Day                             28      // 7 Bits, Bit 7-1
#define     LOG_fTy1DayMask 0xFE
#define     LOG_fTy1DayShift 1
#define LOG_fTy1IsWeekday                       28      // 1 Bit, Bit 0
#define     LOG_fTy1IsWeekdayMask 0x01
#define     LOG_fTy1IsWeekdayShift 0
#define LOG_fTy1Month                           29      // 4 Bits, Bit 7-4
#define     LOG_fTy1MonthMask 0xF0
#define     LOG_fTy1MonthShift 4
#define LOG_fTy2Weekday1                        30      // 1 Bit, Bit 7
#define     LOG_fTy2Weekday1Mask 0x80
#define     LOG_fTy2Weekday1Shift 7
#define LOG_fTy2Weekday2                        30      // 1 Bit, Bit 6
#define     LOG_fTy2Weekday2Mask 0x40
#define     LOG_fTy2Weekday2Shift 6
#define LOG_fTy2Weekday3                        30      // 1 Bit, Bit 5
#define     LOG_fTy2Weekday3Mask 0x20
#define     LOG_fTy2Weekday3Shift 5
#define LOG_fTy2Weekday4                        30      // 1 Bit, Bit 4
#define     LOG_fTy2Weekday4Mask 0x10
#define     LOG_fTy2Weekday4Shift 4
#define LOG_fTy2Weekday5                        30      // 1 Bit, Bit 3
#define     LOG_fTy2Weekday5Mask 0x08
#define     LOG_fTy2Weekday5Shift 3
#define LOG_fTy2Weekday6                        30      // 1 Bit, Bit 2
#define     LOG_fTy2Weekday6Mask 0x04
#define     LOG_fTy2Weekday6Shift 2
#define LOG_fTy2Weekday7                        30      // 1 Bit, Bit 1
#define     LOG_fTy2Weekday7Mask 0x02
#define     LOG_fTy2Weekday7Shift 1
#define LOG_fTy2Day                             30      // 7 Bits, Bit 7-1
#define     LOG_fTy2DayMask 0xFE
#define     LOG_fTy2DayShift 1
#define LOG_fTy2IsWeekday                       30      // 1 Bit, Bit 0
#define     LOG_fTy2IsWeekdayMask 0x01
#define     LOG_fTy2IsWeekdayShift 0
#define LOG_fTy2Month                           31      // 4 Bits, Bit 7-4
#define     LOG_fTy2MonthMask 0xF0
#define     LOG_fTy2MonthShift 4
#define LOG_fTy3Weekday1                        32      // 1 Bit, Bit 7
#define     LOG_fTy3Weekday1Mask 0x80
#define     LOG_fTy3Weekday1Shift 7
#define LOG_fTy3Weekday2                        32      // 1 Bit, Bit 6
#define     LOG_fTy3Weekday2Mask 0x40
#define     LOG_fTy3Weekday2Shift 6
#define LOG_fTy3Weekday3                        32      // 1 Bit, Bit 5
#define     LOG_fTy3Weekday3Mask 0x20
#define     LOG_fTy3Weekday3Shift 5
#define LOG_fTy3Weekday4                        32      // 1 Bit, Bit 4
#define     LOG_fTy3Weekday4Mask 0x10
#define     LOG_fTy3Weekday4Shift 4
#define LOG_fTy3Weekday5                        32      // 1 Bit, Bit 3
#define     LOG_fTy3Weekday5Mask 0x08
#define     LOG_fTy3Weekday5Shift 3
#define LOG_fTy3Weekday6                        32      // 1 Bit, Bit 2
#define     LOG_fTy3Weekday6Mask 0x04
#define     LOG_fTy3Weekday6Shift 2
#define LOG_fTy3Weekday7                        32      // 1 Bit, Bit 1
#define     LOG_fTy3Weekday7Mask 0x02
#define     LOG_fTy3Weekday7Shift 1
#define LOG_fTy3Day                             32      // 7 Bits, Bit 7-1
#define     LOG_fTy3DayMask 0xFE
#define     LOG_fTy3DayShift 1
#define LOG_fTy3IsWeekday                       32      // 1 Bit, Bit 0
#define     LOG_fTy3IsWeekdayMask 0x01
#define     LOG_fTy3IsWeekdayShift 0
#define LOG_fTy3Month                           33      // 4 Bits, Bit 7-4
#define     LOG_fTy3MonthMask 0xF0
#define     LOG_fTy3MonthShift 4
#define LOG_fTy4Weekday1                        34      // 1 Bit, Bit 7
#define     LOG_fTy4Weekday1Mask 0x80
#define     LOG_fTy4Weekday1Shift 7
#define LOG_fTy4Weekday2                        34      // 1 Bit, Bit 6
#define     LOG_fTy4Weekday2Mask 0x40
#define     LOG_fTy4Weekday2Shift 6
#define LOG_fTy4Weekday3                        34      // 1 Bit, Bit 5
#define     LOG_fTy4Weekday3Mask 0x20
#define     LOG_fTy4Weekday3Shift 5
#define LOG_fTy4Weekday4                        34      // 1 Bit, Bit 4
#define     LOG_fTy4Weekday4Mask 0x10
#define     LOG_fTy4Weekday4Shift 4
#define LOG_fTy4Weekday5                        34      // 1 Bit, Bit 3
#define     LOG_fTy4Weekday5Mask 0x08
#define     LOG_fTy4Weekday5Shift 3
#define LOG_fTy4Weekday6                        34      // 1 Bit, Bit 2
#define     LOG_fTy4Weekday6Mask 0x04
#define     LOG_fTy4Weekday6Shift 2
#define LOG_fTy4Weekday7                        34      // 1 Bit, Bit 1
#define     LOG_fTy4Weekday7Mask 0x02
#define     LOG_fTy4Weekday7Shift 1
#define LOG_fTy4Day                             34      // 7 Bits, Bit 7-1
#define     LOG_fTy4DayMask 0xFE
#define     LOG_fTy4DayShift 1
#define LOG_fTy4IsWeekday                       34      // 1 Bit, Bit 0
#define     LOG_fTy4IsWeekdayMask 0x01
#define     LOG_fTy4IsWeekdayShift 0
#define LOG_fTy4Month                           35      // 4 Bits, Bit 7-4
#define     LOG_fTy4MonthMask 0xF0
#define     LOG_fTy4MonthShift 4
#define LOG_fI1                                 36      // 2 Bits, Bit 7-6
#define     LOG_fI1Mask 0xC0
#define     LOG_fI1Shift 6
#define LOG_fI1Kind                             36      // 2 Bits, Bit 5-4
#define     LOG_fI1KindMask 0x30
#define     LOG_fI1KindShift 4
#define LOG_fI1AsTrigger                        36      // 1 Bit, Bit 3
#define     LOG_fI1AsTriggerMask 0x08
#define     LOG_fI1AsTriggerShift 3
#define LOG_fI1InternalInputType                36      // 1 Bit, Bit 2
#define     LOG_fI1InternalInputTypeMask 0x04
#define     LOG_fI1InternalInputTypeShift 2
#define LOG_fI1Function                         37      // uint8_t
#define LOG_fI1FunctionRel                      37      // int8_t
#define LOG_fI1StatusLed                        37      // 16 Bits, Bit 15-0
#define LOG_fI2                                 39      // 2 Bits, Bit 7-6
#define     LOG_fI2Mask 0xC0
#define     LOG_fI2Shift 6
#define LOG_fI2Kind                             39      // 2 Bits, Bit 5-4
#define     LOG_fI2KindMask 0x30
#define     LOG_fI2KindShift 4
#define LOG_fI2AsTrigger                        39      // 1 Bit, Bit 3
#define     LOG_fI2AsTriggerMask 0x08
#define     LOG_fI2AsTriggerShift 3
#define LOG_fI2InternalInputType                39      // 1 Bit, Bit 2
#define     LOG_fI2InternalInputTypeMask 0x04
#define     LOG_fI2InternalInputTypeShift 2
#define LOG_fI2Function                         40      // uint8_t
#define LOG_fI2FunctionRel                      40      // int8_t
#define LOG_fI2StatusLed                        40      // 16 Bits, Bit 15-0
#define LOG_fOStairtimeBase                     42      // 2 Bits, Bit 7-6
#define     LOG_fOStairtimeBaseMask 0xC0
#define     LOG_fOStairtimeBaseShift 6
#define LOG_fOStairtimeTime                     42      // 14 Bits, Bit 13-0
#define     LOG_fOStairtimeTimeMask 0x3FFF
#define     LOG_fOStairtimeTimeShift 0
#define LOG_fOBlinkBase                         44      // 2 Bits, Bit 7-6
#define     LOG_fOBlinkBaseMask 0xC0
#define     LOG_fOBlinkBaseShift 6
#define LOG_fOBlinkTime                         44      // 14 Bits, Bit 13-0
#define     LOG_fOBlinkTimeMask 0x3FFF
#define     LOG_fOBlinkTimeShift 0
#define LOG_fODelayOnBase                       46      // 2 Bits, Bit 7-6
#define     LOG_fODelayOnBaseMask 0xC0
#define     LOG_fODelayOnBaseShift 6
#define LOG_fODelayOnTime                       46      // 14 Bits, Bit 13-0
#define     LOG_fODelayOnTimeMask 0x3FFF
#define     LOG_fODelayOnTimeShift 0
#define LOG_fODelayOffBase                      48      // 2 Bits, Bit 7-6
#define     LOG_fODelayOffBaseMask 0xC0
#define     LOG_fODelayOffBaseShift 6
#define LOG_fODelayOffTime                      48      // 14 Bits, Bit 13-0
#define     LOG_fODelayOffTimeMask 0x3FFF
#define     LOG_fODelayOffTimeShift 0
#define LOG_fORepeatOnBase                      50      // 2 Bits, Bit 7-6
#define     LOG_fORepeatOnBaseMask 0xC0
#define     LOG_fORepeatOnBaseShift 6
#define LOG_fORepeatOnTime                      50      // 14 Bits, Bit 13-0
#define     LOG_fORepeatOnTimeMask 0x3FFF
#define     LOG_fORepeatOnTimeShift 0
#define LOG_fORepeatOffBase                     52      // 2 Bits, Bit 7-6
#define     LOG_fORepeatOffBaseMask 0xC0
#define     LOG_fORepeatOffBaseShift 6
#define LOG_fORepeatOffTime                     52      // 14 Bits, Bit 13-0
#define     LOG_fORepeatOffTimeMask 0x3FFF
#define     LOG_fORepeatOffTimeShift 0
#define LOG_fODelay                             54      // 1 Bit, Bit 7
#define     LOG_fODelayMask 0x80
#define     LOG_fODelayShift 7
#define LOG_fODelayOnRepeat                     54      // 2 Bits, Bit 6-5
#define     LOG_fODelayOnRepeatMask 0x60
#define     LOG_fODelayOnRepeatShift 5
#define LOG_fODelayOnReset                      54      // 1 Bit, Bit 4
#define     LOG_fODelayOnResetMask 0x10
#define     LOG_fODelayOnResetShift 4
#define LOG_fODelayOffRepeat                    54      // 2 Bits, Bit 3-2
#define     LOG_fODelayOffRepeatMask 0x0C
#define     LOG_fODelayOffRepeatShift 2
#define LOG_fODelayOffReset                     54      // 1 Bit, Bit 1
#define     LOG_fODelayOffResetMask 0x02
#define     LOG_fODelayOffResetShift 1
#define LOG_fOStair                             54      // 1 Bit, Bit 0
#define     LOG_fOStairMask 0x01
#define     LOG_fOStairShift 0
#define LOG_fORetrigger                         55      // 1 Bit, Bit 7
#define     LOG_fORetriggerMask 0x80
#define     LOG_fORetriggerShift 7
#define LOG_fOStairOff                          55      // 1 Bit, Bit 6
#define     LOG_fOStairOffMask 0x40
#define     LOG_fOStairOffShift 6
#define LOG_fORepeat                            55      // 1 Bit, Bit 5
#define     LOG_fORepeatMask 0x20
#define     LOG_fORepeatShift 5
#define LOG_fOOutputFilter                      55      // 2 Bits, Bit 4-3
#define     LOG_fOOutputFilterMask 0x18
#define     LOG_fOOutputFilterShift 3
#define LOG_fOSendOnChange                      55      // 1 Bit, Bit 2
#define     LOG_fOSendOnChangeMask 0x04
#define     LOG_fOSendOnChangeShift 2
#define LOG_fOLockEnabled                       55      // 1 Bit, Bit 1
#define     LOG_fOLockEnabledMask 0x02
#define     LOG_fOLockEnabledShift 1
#define LOG_fODpt                               56      // 8 Bits, Bit 7-0
#define LOG_fOLockTriggerLock                   57      // 2 Bits, Bit 7-6
#define     LOG_fOLockTriggerLockMask 0xC0
#define     LOG_fOLockTriggerLockShift 6
#define LOG_fOLockTriggerUnlock                 57      // 2 Bits, Bit 5-4
#define     LOG_fOLockTriggerUnlockMask 0x30
#define     LOG_fOLockTriggerUnlockShift 4
#define LOG_fOLockResetQueue                    57      // 2 Bits, Bit 3-2
#define     LOG_fOLockResetQueueMask 0x0C
#define     LOG_fOLockResetQueueShift 2
#define LOG_fOLockKind                          57      // 2 Bits, Bit 1-0
#define     LOG_fOLockKindMask 0x03
#define     LOG_fOLockKindShift 0
#define LOG_fOLockFunction                      58      // uint8_t
#define LOG_fOLockFunctionRel                   58      // int8_t
#define LOG_fOOnAll                             59      // 8 Bits, Bit 7-0
#define LOG_fOOnDpt1                            60      // 8 Bits, Bit 7-0
#define LOG_fOOnDpt2                            60      // 8 Bits, Bit 7-0
#define LOG_fOOnDpt3Dir                         60      // 5 Bits, Bit 7-3
#define     LOG_fOOnDpt3DirMask 0xF8
#define     LOG_fOOnDpt3DirShift 3
#define LOG_fOOnDpt3Dim                         60      // 3 Bits, Bit 2-0
#define     LOG_fOOnDpt3DimMask 0x07
#define     LOG_fOOnDpt3DimShift 0
#define LOG_fOOnDpt5                            60      // uint8_t
#define LOG_fOOnDpt5001                         60      // uint8_t
#define LOG_fOOnDpt6                            60      // int8_t
#define LOG_fOOnDpt7                            60      // uint16_t
#define LOG_fOOnDpt8                            60      // int16_t
#define LOG_fOOnDpt9                            60      // float (4 Byte)
#define LOG_fOOnDpt12                           60      // uint32_t
#define LOG_fOOnDpt13                           60      // int32_t
#define LOG_fOOnDpt14                           60      // float (4 Byte)
#define LOG_fOOnDpt16                           60      // char*, 14 Byte
#define     LOG_fOOnDpt16Length 14
#define LOG_fOOnDpt17                           60      // 8 Bits, Bit 7-0
#define LOG_fOOnRGB                             60      // 24 Bits, Bit 31-8
#define     LOG_fOOnRGBMask 0xFFFFFF00
#define     LOG_fOOnRGBShift 8
#define LOG_fOOnLedProvider                     64      // 3 Bits, Bit 2-0
#define     LOG_fOOnLedProviderMask 0x07
#define     LOG_fOOnLedProviderShift 0
#define LOG_fOOnLedEffect                       65      // 3 Bits, Bit 2-0
#define     LOG_fOOnLedEffectMask 0x07
#define     LOG_fOOnLedEffectShift 0
#define LOG_fOOnLedDuration                     66      // uint16_t
#define LOG_fOOnPAArea                          60      // 4 Bits, Bit 7-4
#define     LOG_fOOnPAAreaMask 0xF0
#define     LOG_fOOnPAAreaShift 4
#define LOG_fOOnPALine                          60      // 4 Bits, Bit 3-0
#define     LOG_fOOnPALineMask 0x0F
#define     LOG_fOOnPALineShift 0
#define LOG_fOOnPADevice                        61      // uint8_t
#define LOG_fOOnFunction                        60      // 8 Bits, Bit 7-0
#define LOG_fOOnKOKind                          65      // 2 Bits, Bit 7-6
#define     LOG_fOOnKOKindMask 0xC0
#define     LOG_fOOnKOKindShift 6
#define LOG_fOOnKONumber                        60      // uint16_t
#define LOG_fOOnKONumberRel                     60      // int16_t
#define LOG_fOOnKODpt                           62      // 8 Bits, Bit 7-0
#define LOG_fOOnKOSend                          65      // 2 Bits, Bit 5-4
#define     LOG_fOOnKOSendMask 0x30
#define     LOG_fOOnKOSendShift 4
#define LOG_fOOnKOSendNumber                    66      // uint16_t
#define LOG_fOOnKOSendNumberRel                 66      // int16_t
#define LOG_fOOffAll                            74      // 8 Bits, Bit 7-0
#define LOG_fOOffDpt1                           75      // 8 Bits, Bit 7-0
#define LOG_fOOffDpt2                           75      // 8 Bits, Bit 7-0
#define LOG_fOOffDpt3Dir                        75      // 5 Bits, Bit 7-3
#define     LOG_fOOffDpt3DirMask 0xF8
#define     LOG_fOOffDpt3DirShift 3
#define LOG_fOOffDpt3Dim                        75      // 3 Bits, Bit 2-0
#define     LOG_fOOffDpt3DimMask 0x07
#define     LOG_fOOffDpt3DimShift 0
#define LOG_fOOffDpt5                           75      // uint8_t
#define LOG_fOOffDpt5001                        75      // uint8_t
#define LOG_fOOffDpt6                           75      // int8_t
#define LOG_fOOffDpt7                           75      // uint16_t
#define LOG_fOOffDpt8                           75      // int16_t
#define LOG_fOOffDpt9                           75      // float (4 Byte)
#define LOG_fOOffDpt12                          75      // uint32_t
#define LOG_fOOffDpt13                          75      // int32_t
#define LOG_fOOffDpt14                          75      // float (4 Byte)
#define LOG_fOOffDpt16                          75      // char*, 14 Byte
#define     LOG_fOOffDpt16Length 14
#define LOG_fOOffDpt17                          75      // 8 Bits, Bit 7-0
#define LOG_fOOffRGB                            75      // 24 Bits, Bit 31-8
#define     LOG_fOOffRGBMask 0xFFFFFF00
#define     LOG_fOOffRGBShift 8
#define LOG_fOOffLedProvider                    79      // 3 Bits, Bit 2-0
#define     LOG_fOOffLedProviderMask 0x07
#define     LOG_fOOffLedProviderShift 0
#define LOG_fOOffLedEffect                      80      // 3 Bits, Bit 2-0
#define     LOG_fOOffLedEffectMask 0x07
#define     LOG_fOOffLedEffectShift 0
#define LOG_fOOffLedDuration                    81      // uint16_t
#define LOG_fOOffPAArea                         75      // 4 Bits, Bit 7-4
#define     LOG_fOOffPAAreaMask 0xF0
#define     LOG_fOOffPAAreaShift 4
#define LOG_fOOffPALine                         75      // 4 Bits, Bit 3-0
#define     LOG_fOOffPALineMask 0x0F
#define     LOG_fOOffPALineShift 0
#define LOG_fOOffPADevice                       76      // uint8_t
#define LOG_fOOffFunction                       75      // 8 Bits, Bit 7-0
#define LOG_fOOffKOKind                         80      // 2 Bits, Bit 7-6
#define     LOG_fOOffKOKindMask 0xC0
#define     LOG_fOOffKOKindShift 6
#define LOG_fOOffKONumber                       75      // uint16_t
#define LOG_fOOffKONumberRel                    75      // int16_t
#define LOG_fOOffKODpt                          77      // 8 Bits, Bit 7-0
#define LOG_fOOffKOSend                         80      // 2 Bits, Bit 5-4
#define     LOG_fOOffKOSendMask 0x30
#define     LOG_fOOffKOSendShift 4
#define LOG_fOOffKOSendNumber                   81      // uint16_t
#define LOG_fOOffKOSendNumberRel                81      // int16_t

// Zeit bis der Kanal nach einem Neustart aktiv wird
#define ParamLOG_fChannelDelayBase                   ((knx.paramByte(LOG_ParamCalcIndex(LOG_fChannelDelayBase)) & LOG_fChannelDelayBaseMask) >> LOG_fChannelDelayBaseShift)
// Zeit bis der Kanal nach einem Neustart aktiv wird
#define ParamLOG_fChannelDelayTime                   (knx.paramWord(LOG_ParamCalcIndex(LOG_fChannelDelayTime)) & LOG_fChannelDelayTimeMask)
// Zeit bis der Kanal nach einem Neustart aktiv wird (in Millisekunden)
#define ParamLOG_fChannelDelayTimeMS                 (paramDelay(knx.paramWord(LOG_ParamCalcIndex(LOG_fChannelDelayTime))))
// Logik-Operation
#define ParamLOG_fLogic                              (PT_Logic)(knx.paramByte(LOG_ParamCalcIndex(LOG_fLogic)))
// Logik auswerten
#define ParamLOG_fCalculate                          (PT_Calculate)(knx.paramByte(LOG_ParamCalcIndex(LOG_fCalculate)) & LOG_fCalculateMask)
// Kanal deaktivieren (zu Testzwecken)
#define ParamLOG_fDisable                            ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fDisable)) & LOG_fDisableMask))
// Tor geht sofort wieder zu
#define ParamLOG_fTGate                              ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTGate)) & LOG_fTGateMask))
// Wert EIN intern weiterleiten
#define ParamLOG_fOInternalOn                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOInternalOn)) & LOG_fOInternalOnMask))
// Wert AUS intern weiterleiten
#define ParamLOG_fOInternalOff                       ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOInternalOff)) & LOG_fOInternalOffMask))
// Logik sendet ihren Wert weiter
#define ParamLOG_fTrigger                            (knx.paramByte(LOG_ParamCalcIndex(LOG_fTrigger)))
//           Eingang 1
#define ParamLOG_fTriggerE1                          ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTriggerE1)) & LOG_fTriggerE1Mask))
//           Eingang 2
#define ParamLOG_fTriggerE2                          ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTriggerE2)) & LOG_fTriggerE2Mask))
//           Interner Eingang 3
#define ParamLOG_fTriggerI1                          ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTriggerI1)) & LOG_fTriggerI1Mask))
//           Interner Eingang 4
#define ParamLOG_fTriggerI2                          ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTriggerI2)) & LOG_fTriggerI2Mask))
// Logik sendet ihren Wert weiter
#define ParamLOG_fTriggerTime                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fTriggerTime)))
// Beim schließen vom Tor wird
#define ParamLOG_fTriggerGateClose                   (PT_GateTrigger)((knx.paramByte(LOG_ParamCalcIndex(LOG_fTriggerGateClose)) & LOG_fTriggerGateCloseMask) >> LOG_fTriggerGateCloseShift)
// Beim öffnen vom Tor wird
#define ParamLOG_fTriggerGateOpen                    (PT_GateTrigger)((knx.paramByte(LOG_ParamCalcIndex(LOG_fTriggerGateOpen)) & LOG_fTriggerGateOpenMask) >> LOG_fTriggerGateOpenShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE1ConvertInt                       (PT_InputConv)((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1ConvertInt)) & LOG_fE1ConvertIntMask) >> LOG_fE1ConvertIntShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE1Convert                          (PT_InputConv)((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Convert)) & LOG_fE1ConvertMask) >> LOG_fE1ConvertShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE1ConvertFloat                     (PT_InputConv)((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1ConvertFloat)) & LOG_fE1ConvertFloatMask) >> LOG_fE1ConvertFloatShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE1ConvertSpecial                   (PT_InputConv)((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1ConvertSpecial)) & LOG_fE1ConvertSpecialMask) >> LOG_fE1ConvertSpecialShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE1ConvertBool                      (PT_InputConv)((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1ConvertBool)) & LOG_fE1ConvertBoolMask) >> LOG_fE1ConvertBoolShift)
// Eingang 1
#define ParamLOG_fE1                                 (PT_InputEnable)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1)) & LOG_fE1Mask)
// DPT für Eingang
#define ParamLOG_fE1Dpt                              (PT_LogicDpt)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Dpt)))
// Eingang wird gelesen alle
#define ParamLOG_fE1RepeatBase                       ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1RepeatBase)) & LOG_fE1RepeatBaseMask) >> LOG_fE1RepeatBaseShift)
// Eingang wird gelesen alle
#define ParamLOG_fE1RepeatTime                       (knx.paramWord(LOG_ParamCalcIndex(LOG_fE1RepeatTime)) & LOG_fE1RepeatTimeMask)
// Eingang wird gelesen alle (in Millisekunden)
#define ParamLOG_fE1RepeatTimeMS                     (paramDelay(knx.paramWord(LOG_ParamCalcIndex(LOG_fE1RepeatTime))))
//     Nummer des Kommunikationsobjekts
#define ParamLOG_fE1OtherKO                          (knx.paramWord(LOG_ParamCalcIndex(LOG_fE1OtherKO)))
//     Nummer des Kommunikationsobjekts
#define ParamLOG_fE1OtherKORel                       ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE1OtherKORel)))
// Falls Vorbelegung aus dem Speicher nicht möglich oder nicht gewünscht, dann vorbelegen mit
#define ParamLOG_fE1Default                          (PT_InputDefault)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Default)) & LOG_fE1DefaultMask)
// Eingang vorbelegen mit
#define ParamLOG_fE1DefaultExt                       (PT_InputDefault)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1DefaultExt)) & LOG_fE1DefaultExtMask)
// Eingangswert speichern und beim nächsten Neustart als Vorbelegung nutzen?
#define ParamLOG_fE1DefaultEEPROM                    ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1DefaultEEPROM)) & LOG_fE1DefaultEEPROMMask))
// Nur so lange zyklisch lesen, bis erstes Telegramm eingeht
#define ParamLOG_fE1DefaultRepeat                    ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1DefaultRepeat)) & LOG_fE1DefaultRepeatMask))
// Kommunikationsobjekt für Eingang
#define ParamLOG_fE1UseOtherKO                       (PT_KORelInput)((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1UseOtherKO)) & LOG_fE1UseOtherKOMask) >> LOG_fE1UseOtherKOShift)
// Von-Wert
#define ParamLOG_fE1LowDelta                         ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE1LowDelta)))
// Bis-Wert
#define ParamLOG_fE1HighDelta                        ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE1HighDelta)))
// Von-Wert
#define ParamLOG_fE1LowDeltaFloat                    (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE1LowDeltaFloat), Float_Enc_IEEE754Single))
// Bis-Wert
#define ParamLOG_fE1HighDeltaFloat                   (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE1HighDeltaFloat), Float_Enc_IEEE754Single))
// Von-Wert
#define ParamLOG_fE1LowDeltaDouble                   (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE1LowDeltaDouble), Float_Enc_IEEE754Single))
// Bis-Wert
#define ParamLOG_fE1HighDeltaDouble                  (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE1HighDeltaDouble), Float_Enc_IEEE754Single))
// Nächste Zeile auswerten?
#define ParamLOG_fE1Low0Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low0Valid)) & LOG_fE1Low0ValidMask))
// Nächste Zeile auswerten?
#define ParamLOG_fE1Low1Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low1Valid)) & LOG_fE1Low1ValidMask))
// Nächste Zeile auswerten?
#define ParamLOG_fE1Low2Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low2Valid)) & LOG_fE1Low2ValidMask))
// Nächste Zeile auswerten?
#define ParamLOG_fE1Low3Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low3Valid)) & LOG_fE1Low3ValidMask))
// Nächste Zeile auswerten?
#define ParamLOG_fE1Low4Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low4Valid)) & LOG_fE1Low4ValidMask))
// Nächste Zeile auswerten?
#define ParamLOG_fE1Low5Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low5Valid)) & LOG_fE1Low5ValidMask))
// Nächste Zeile auswerten?
#define ParamLOG_fE1Low6Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low6Valid)) & LOG_fE1Low6ValidMask))
// Eingang ist EIN, wenn Wert gleich
#define ParamLOG_fE1Low0Dpt2                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low0Dpt2)))
// ... oder wenn Wert gleich 
#define ParamLOG_fE1Low1Dpt2                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low1Dpt2)))
// ... oder wenn Wert gleich 
#define ParamLOG_fE1Low2Dpt2                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low2Dpt2)))
// ... oder wenn Wert gleich 
#define ParamLOG_fE1Low3Dpt2                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low3Dpt2)))
// Eingang ist konstant
#define ParamLOG_fE1LowDpt2Fix                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1LowDpt2Fix)))
// Eingang ist EIN, wenn Wert gleich
#define ParamLOG_fE1Low0Dpt3Dir                      ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low0Dpt3Dir)) & LOG_fE1Low0Dpt3DirMask) >> LOG_fE1Low0Dpt3DirShift)
// 
#define ParamLOG_fE1Low0Dpt3Dim                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low0Dpt3Dim)) & LOG_fE1Low0Dpt3DimMask)
// ... oder wenn Wert gleich 
#define ParamLOG_fE1Low1Dpt3Dir                      ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low1Dpt3Dir)) & LOG_fE1Low1Dpt3DirMask) >> LOG_fE1Low1Dpt3DirShift)
// 
#define ParamLOG_fE1Low1Dpt3Dim                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low1Dpt3Dim)) & LOG_fE1Low1Dpt3DimMask)
// ... oder wenn Wert gleich 
#define ParamLOG_fE1Low2Dpt3Dir                      ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low2Dpt3Dir)) & LOG_fE1Low2Dpt3DirMask) >> LOG_fE1Low2Dpt3DirShift)
// 
#define ParamLOG_fE1Low2Dpt3Dim                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low2Dpt3Dim)) & LOG_fE1Low2Dpt3DimMask)
// ... oder wenn Wert gleich 
#define ParamLOG_fE1Low3Dpt3Dir                      ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low3Dpt3Dir)) & LOG_fE1Low3Dpt3DirMask) >> LOG_fE1Low3Dpt3DirShift)
// 
#define ParamLOG_fE1Low3Dpt3Dim                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low3Dpt3Dim)) & LOG_fE1Low3Dpt3DimMask)
// Eingang ist konstant
#define ParamLOG_fE1LowDpt3FixDir                    ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE1LowDpt3FixDir)) & LOG_fE1LowDpt3FixDirMask) >> LOG_fE1LowDpt3FixDirShift)
// 
#define ParamLOG_fE1LowDpt3FixDim                    (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1LowDpt3FixDim)) & LOG_fE1LowDpt3FixDimMask)
// Von-Wert
#define ParamLOG_fE1LowDpt5                          (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1LowDpt5)))
// Bis-Wert
#define ParamLOG_fE1HighDpt5                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1HighDpt5)))
// Eingang ist EIN bei Wert
#define ParamLOG_fE1Low0Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low0Dpt5In)))
// ... oder bei Wert
#define ParamLOG_fE1Low1Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low1Dpt5In)))
// ... oder bei Wert
#define ParamLOG_fE1Low2Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low2Dpt5In)))
// ... oder bei Wert
#define ParamLOG_fE1Low3Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low3Dpt5In)))
// ... oder bei Wert
#define ParamLOG_fE1Low4Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low4Dpt5In)))
// ... oder bei Wert
#define ParamLOG_fE1Low5Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low5Dpt5In)))
// ... oder bei Wert
#define ParamLOG_fE1Low6Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low6Dpt5In)))
// Eingang ist konstant
#define ParamLOG_fE1LowDpt5Fix                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1LowDpt5Fix)))
// Von-Wert
#define ParamLOG_fE1LowDpt5001                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1LowDpt5001)))
// Bis-Wert
#define ParamLOG_fE1HighDpt5001                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1HighDpt5001)))
// Eingang ist EIN bei Wert
#define ParamLOG_fE1Low0Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low0Dpt5xIn)))
// ... oder bei Wert
#define ParamLOG_fE1Low1Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low1Dpt5xIn)))
// ... oder bei Wert
#define ParamLOG_fE1Low2Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low2Dpt5xIn)))
// ... oder bei Wert
#define ParamLOG_fE1Low3Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low3Dpt5xIn)))
// ... oder bei Wert
#define ParamLOG_fE1Low4Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low4Dpt5xIn)))
// ... oder bei Wert
#define ParamLOG_fE1Low5Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low5Dpt5xIn)))
// ... oder bei Wert
#define ParamLOG_fE1Low6Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low6Dpt5xIn)))
// Eingang ist konstant
#define ParamLOG_fE1LowDpt5xFix                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1LowDpt5xFix)))
// Von-Wert
#define ParamLOG_fE1LowDpt6                          ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE1LowDpt6)))
// Bis-Wert
#define ParamLOG_fE1HighDpt6                         ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE1HighDpt6)))
// Eingang ist EIN bei Wert
#define ParamLOG_fE1Low0Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low0Dpt6In)))
// ... oder bei Wert
#define ParamLOG_fE1Low1Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low1Dpt6In)))
// ... oder bei Wert
#define ParamLOG_fE1Low2Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low2Dpt6In)))
// ... oder bei Wert
#define ParamLOG_fE1Low3Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low3Dpt6In)))
// ... oder bei Wert
#define ParamLOG_fE1Low4Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low4Dpt6In)))
// ... oder bei Wert
#define ParamLOG_fE1Low5Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low5Dpt6In)))
// ... oder bei Wert
#define ParamLOG_fE1Low6Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low6Dpt6In)))
// Eingang ist konstant
#define ParamLOG_fE1LowDpt6Fix                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE1LowDpt6Fix)))
// Von-Wert
#define ParamLOG_fE1LowDpt7                          (knx.paramWord(LOG_ParamCalcIndex(LOG_fE1LowDpt7)))
// Bis-Wert
#define ParamLOG_fE1HighDpt7                         (knx.paramWord(LOG_ParamCalcIndex(LOG_fE1HighDpt7)))
// Eingang ist EIN bei Wert
#define ParamLOG_fE1Low0Dpt7In                       (knx.paramWord(LOG_ParamCalcIndex(LOG_fE1Low0Dpt7In)))
// ... oder bei Wert
#define ParamLOG_fE1Low1Dpt7In                       (knx.paramWord(LOG_ParamCalcIndex(LOG_fE1Low1Dpt7In)))
// ... oder bei Wert
#define ParamLOG_fE1Low2Dpt7In                       (knx.paramWord(LOG_ParamCalcIndex(LOG_fE1Low2Dpt7In)))
// Eingang ist konstant
#define ParamLOG_fE1LowDpt7Fix                       (knx.paramWord(LOG_ParamCalcIndex(LOG_fE1LowDpt7Fix)))
// Von-Wert
#define ParamLOG_fE1LowDpt8                          ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE1LowDpt8)))
// Bis-Wert
#define ParamLOG_fE1HighDpt8                         ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE1HighDpt8)))
// Eingang ist EIN bei Wert
#define ParamLOG_fE1Low0Dpt8In                       ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE1Low0Dpt8In)))
// ... oder bei Wert
#define ParamLOG_fE1Low1Dpt8In                       ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE1Low1Dpt8In)))
// ... oder bei Wert
#define ParamLOG_fE1Low2Dpt8In                       ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE1Low2Dpt8In)))
// Eingang ist konstant
#define ParamLOG_fE1LowDpt8Fix                       ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE1LowDpt8Fix)))
// Von-Wert
#define ParamLOG_fE1LowDpt9                          (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE1LowDpt9), Float_Enc_IEEE754Single))
// Bis-Wert
#define ParamLOG_fE1HighDpt9                         (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE1HighDpt9), Float_Enc_IEEE754Single))
// Eingang ist konstant
#define ParamLOG_fE1LowDpt9Fix                       (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE1LowDpt9Fix), Float_Enc_IEEE754Single))
// Von-Wert
#define ParamLOG_fE1LowDpt12                         (knx.paramInt(LOG_ParamCalcIndex(LOG_fE1LowDpt12)))
// Bis-Wert
#define ParamLOG_fE1HighDpt12                        (knx.paramInt(LOG_ParamCalcIndex(LOG_fE1HighDpt12)))
// Eingang ist konstant
#define ParamLOG_fE1LowDpt12Fix                      (knx.paramInt(LOG_ParamCalcIndex(LOG_fE1LowDpt12Fix)))
// Von-Wert
#define ParamLOG_fE1LowDpt13                         ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE1LowDpt13)))
// Bis-Wert
#define ParamLOG_fE1HighDpt13                        ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE1HighDpt13)))
// Eingang ist konstant
#define ParamLOG_fE1LowDpt13Fix                      ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE1LowDpt13Fix)))
// Von-Wert
#define ParamLOG_fE1LowDpt14                         (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE1LowDpt14), Float_Enc_IEEE754Single))
// Bis-Wert
#define ParamLOG_fE1HighDpt14                        (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE1HighDpt14), Float_Enc_IEEE754Single))
// Eingang ist konstant
#define ParamLOG_fE1LowDpt14Fix                      (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE1LowDpt14Fix), Float_Enc_IEEE754Single))
// Eingang ist EIN bei Szene
#define ParamLOG_fE1Low0Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low0Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE1Low1Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low1Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE1Low2Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low2Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE1Low3Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low3Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE1Low4Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low4Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE1Low5Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low5Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE1Low6Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low6Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE1Low7Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1Low7Dpt17)))
// Eingang ist konstant
#define ParamLOG_fE1LowDpt17Fix                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE1LowDpt17Fix)))
// Von-Wert
#define ParamLOG_fE1LowDptRGB                        ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE1LowDptRGB)))
// Bis-Wert
#define ParamLOG_fE1HighDptRGB                       ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE1HighDptRGB)))
// Eingang ist konstant
#define ParamLOG_fE1LowDptRGBFix                     ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE1LowDptRGBFix)))
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE2ConvertInt                       (PT_InputConv)((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2ConvertInt)) & LOG_fE2ConvertIntMask) >> LOG_fE2ConvertIntShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE2Convert                          (PT_InputConv)((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Convert)) & LOG_fE2ConvertMask) >> LOG_fE2ConvertShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE2ConvertFloat                     (PT_InputConv)((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2ConvertFloat)) & LOG_fE2ConvertFloatMask) >> LOG_fE2ConvertFloatShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE2ConvertSpecial                   (PT_InputConv)((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2ConvertSpecial)) & LOG_fE2ConvertSpecialMask) >> LOG_fE2ConvertSpecialShift)
// Wert für Eingang wird ermittelt durch
#define ParamLOG_fE2ConvertBool                      (PT_InputConv)((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2ConvertBool)) & LOG_fE2ConvertBoolMask) >> LOG_fE2ConvertBoolShift)
// Eingang 2
#define ParamLOG_fE2                                 (PT_InputEnable)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2)) & LOG_fE2Mask)
// DPT für Eingang
#define ParamLOG_fE2Dpt                              (PT_LogicDpt)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Dpt)))
// Eingang wird gelesen alle
#define ParamLOG_fE2RepeatBase                       ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2RepeatBase)) & LOG_fE2RepeatBaseMask) >> LOG_fE2RepeatBaseShift)
// Eingang wird gelesen alle
#define ParamLOG_fE2RepeatTime                       (knx.paramWord(LOG_ParamCalcIndex(LOG_fE2RepeatTime)) & LOG_fE2RepeatTimeMask)
// Eingang wird gelesen alle (in Millisekunden)
#define ParamLOG_fE2RepeatTimeMS                     (paramDelay(knx.paramWord(LOG_ParamCalcIndex(LOG_fE2RepeatTime))))
//     Nummer des Kommunikationsobjekts
#define ParamLOG_fE2OtherKO                          (knx.paramWord(LOG_ParamCalcIndex(LOG_fE2OtherKO)))
//     Nummer des Kommunikationsobjekts
#define ParamLOG_fE2OtherKORel                       ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE2OtherKORel)))
// Falls Vorbelegung aus dem Speicher nicht möglich oder nicht gewünscht, dann vorbelegen mit
#define ParamLOG_fE2Default                          (PT_InputDefault)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Default)) & LOG_fE2DefaultMask)
// Eingang vorbelegen mit
#define ParamLOG_fE2DefaultExt                       (PT_InputDefault)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2DefaultExt)) & LOG_fE2DefaultExtMask)
// Eingangswert speichern und beim nächsten Neustart als Vorbelegung nutzen?
#define ParamLOG_fE2DefaultEEPROM                    ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2DefaultEEPROM)) & LOG_fE2DefaultEEPROMMask))
// Nur so lange zyklisch lesen, bis erstes Telegramm eingeht
#define ParamLOG_fE2DefaultRepeat                    ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2DefaultRepeat)) & LOG_fE2DefaultRepeatMask))
// Kommunikationsobjekt für Eingang
#define ParamLOG_fE2UseOtherKO                       (PT_KORelInput)((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2UseOtherKO)) & LOG_fE2UseOtherKOMask) >> LOG_fE2UseOtherKOShift)
// Von-Wert
#define ParamLOG_fE2LowDelta                         ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE2LowDelta)))
// Bis-Wert
#define ParamLOG_fE2HighDelta                        ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE2HighDelta)))
// Von-Wert
#define ParamLOG_fE2LowDeltaFloat                    (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE2LowDeltaFloat), Float_Enc_IEEE754Single))
// Bis-Wert
#define ParamLOG_fE2HighDeltaFloat                   (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE2HighDeltaFloat), Float_Enc_IEEE754Single))
// Von-Wert
#define ParamLOG_fE2LowDeltaDouble                   (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE2LowDeltaDouble), Float_Enc_IEEE754Single))
// Bis-Wert
#define ParamLOG_fE2HighDeltaDouble                  (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE2HighDeltaDouble), Float_Enc_IEEE754Single))
// Nächste Zeile auswerten?
#define ParamLOG_fE2Low0Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low0Valid)) & LOG_fE2Low0ValidMask))
// Nächste Zeile auswerten?
#define ParamLOG_fE2Low1Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low1Valid)) & LOG_fE2Low1ValidMask))
// Nächste Zeile auswerten?
#define ParamLOG_fE2Low2Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low2Valid)) & LOG_fE2Low2ValidMask))
// Nächste Zeile auswerten?
#define ParamLOG_fE2Low3Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low3Valid)) & LOG_fE2Low3ValidMask))
// Nächste Zeile auswerten?
#define ParamLOG_fE2Low4Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low4Valid)) & LOG_fE2Low4ValidMask))
// Nächste Zeile auswerten?
#define ParamLOG_fE2Low5Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low5Valid)) & LOG_fE2Low5ValidMask))
// Nächste Zeile auswerten?
#define ParamLOG_fE2Low6Valid                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low6Valid)) & LOG_fE2Low6ValidMask))
// Eingang ist EIN, wenn Wert gleich
#define ParamLOG_fE2Low0Dpt2                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low0Dpt2)))
// ... oder wenn Wert gleich 
#define ParamLOG_fE2Low1Dpt2                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low1Dpt2)))
// ... oder wenn Wert gleich 
#define ParamLOG_fE2Low2Dpt2                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low2Dpt2)))
// ... oder wenn Wert gleich 
#define ParamLOG_fE2Low3Dpt2                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low3Dpt2)))
// Eingang ist konstant
#define ParamLOG_fE2LowDpt2Fix                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2LowDpt2Fix)))
// Eingang ist EIN, wenn Wert gleich
#define ParamLOG_fE2Low0Dpt3Dir                      ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low0Dpt3Dir)) & LOG_fE2Low0Dpt3DirMask) >> LOG_fE2Low0Dpt3DirShift)
// 
#define ParamLOG_fE2Low0Dpt3Dim                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low0Dpt3Dim)) & LOG_fE2Low0Dpt3DimMask)
// ... oder wenn Wert gleich 
#define ParamLOG_fE2Low1Dpt3Dir                      ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low1Dpt3Dir)) & LOG_fE2Low1Dpt3DirMask) >> LOG_fE2Low1Dpt3DirShift)
// 
#define ParamLOG_fE2Low1Dpt3Dim                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low1Dpt3Dim)) & LOG_fE2Low1Dpt3DimMask)
// ... oder wenn Wert gleich 
#define ParamLOG_fE2Low2Dpt3Dir                      ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low2Dpt3Dir)) & LOG_fE2Low2Dpt3DirMask) >> LOG_fE2Low2Dpt3DirShift)
// 
#define ParamLOG_fE2Low2Dpt3Dim                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low2Dpt3Dim)) & LOG_fE2Low2Dpt3DimMask)
// ... oder wenn Wert gleich 
#define ParamLOG_fE2Low3Dpt3Dir                      ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low3Dpt3Dir)) & LOG_fE2Low3Dpt3DirMask) >> LOG_fE2Low3Dpt3DirShift)
// 
#define ParamLOG_fE2Low3Dpt3Dim                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low3Dpt3Dim)) & LOG_fE2Low3Dpt3DimMask)
// Eingang ist konstant
#define ParamLOG_fE2LowDpt3FixDir                    ((knx.paramByte(LOG_ParamCalcIndex(LOG_fE2LowDpt3FixDir)) & LOG_fE2LowDpt3FixDirMask) >> LOG_fE2LowDpt3FixDirShift)
// 
#define ParamLOG_fE2LowDpt3FixDim                    (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2LowDpt3FixDim)) & LOG_fE2LowDpt3FixDimMask)
// Von-Wert
#define ParamLOG_fE2LowDpt5                          (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2LowDpt5)))
// Bis-Wert
#define ParamLOG_fE2HighDpt5                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2HighDpt5)))
// Eingang ist EIN bei Wert
#define ParamLOG_fE2Low0Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low0Dpt5In)))
// ... oder bei Wert
#define ParamLOG_fE2Low1Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low1Dpt5In)))
// ... oder bei Wert
#define ParamLOG_fE2Low2Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low2Dpt5In)))
// ... oder bei Wert
#define ParamLOG_fE2Low3Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low3Dpt5In)))
// ... oder bei Wert
#define ParamLOG_fE2Low4Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low4Dpt5In)))
// ... oder bei Wert
#define ParamLOG_fE2Low5Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low5Dpt5In)))
// ... oder bei Wert
#define ParamLOG_fE2Low6Dpt5In                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low6Dpt5In)))
// Eingang ist konstant
#define ParamLOG_fE2LowDpt5Fix                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2LowDpt5Fix)))
// Von-Wert
#define ParamLOG_fE2LowDpt5001                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2LowDpt5001)))
// Bis-Wert
#define ParamLOG_fE2HighDpt5001                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2HighDpt5001)))
// Eingang ist EIN bei Wert
#define ParamLOG_fE2Low0Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low0Dpt5xIn)))
// ... oder bei Wert
#define ParamLOG_fE2Low1Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low1Dpt5xIn)))
// ... oder bei Wert
#define ParamLOG_fE2Low2Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low2Dpt5xIn)))
// ... oder bei Wert
#define ParamLOG_fE2Low3Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low3Dpt5xIn)))
// ... oder bei Wert
#define ParamLOG_fE2Low4Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low4Dpt5xIn)))
// ... oder bei Wert
#define ParamLOG_fE2Low5Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low5Dpt5xIn)))
// ... oder bei Wert
#define ParamLOG_fE2Low6Dpt5xIn                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low6Dpt5xIn)))
// Eingang ist konstant
#define ParamLOG_fE2LowDpt5xFix                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2LowDpt5xFix)))
// Von-Wert
#define ParamLOG_fE2LowDpt6                          ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE2LowDpt6)))
// Bis-Wert
#define ParamLOG_fE2HighDpt6                         ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE2HighDpt6)))
// Eingang ist EIN bei Wert
#define ParamLOG_fE2Low0Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low0Dpt6In)))
// ... oder bei Wert
#define ParamLOG_fE2Low1Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low1Dpt6In)))
// ... oder bei Wert
#define ParamLOG_fE2Low2Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low2Dpt6In)))
// ... oder bei Wert
#define ParamLOG_fE2Low3Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low3Dpt6In)))
// ... oder bei Wert
#define ParamLOG_fE2Low4Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low4Dpt6In)))
// ... oder bei Wert
#define ParamLOG_fE2Low5Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low5Dpt6In)))
// ... oder bei Wert
#define ParamLOG_fE2Low6Dpt6In                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low6Dpt6In)))
// Eingang ist konstant
#define ParamLOG_fE2LowDpt6Fix                       ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fE2LowDpt6Fix)))
// Von-Wert
#define ParamLOG_fE2LowDpt7                          (knx.paramWord(LOG_ParamCalcIndex(LOG_fE2LowDpt7)))
// Bis-Wert
#define ParamLOG_fE2HighDpt7                         (knx.paramWord(LOG_ParamCalcIndex(LOG_fE2HighDpt7)))
// Eingang ist EIN bei Wert
#define ParamLOG_fE2Low0Dpt7In                       (knx.paramWord(LOG_ParamCalcIndex(LOG_fE2Low0Dpt7In)))
// ... oder bei Wert
#define ParamLOG_fE2Low1Dpt7In                       (knx.paramWord(LOG_ParamCalcIndex(LOG_fE2Low1Dpt7In)))
// ... oder bei Wert
#define ParamLOG_fE2Low2Dpt7In                       (knx.paramWord(LOG_ParamCalcIndex(LOG_fE2Low2Dpt7In)))
// Eingang ist konstant
#define ParamLOG_fE2LowDpt7Fix                       (knx.paramWord(LOG_ParamCalcIndex(LOG_fE2LowDpt7Fix)))
// Von-Wert
#define ParamLOG_fE2LowDpt8                          ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE2LowDpt8)))
// Bis-Wert
#define ParamLOG_fE2HighDpt8                         ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE2HighDpt8)))
// Eingang ist EIN bei Wert
#define ParamLOG_fE2Low0Dpt8In                       ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE2Low0Dpt8In)))
// ... oder bei Wert
#define ParamLOG_fE2Low1Dpt8In                       ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE2Low1Dpt8In)))
// ... oder bei Wert
#define ParamLOG_fE2Low2Dpt8In                       ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE2Low2Dpt8In)))
// Eingang ist konstant
#define ParamLOG_fE2LowDpt8Fix                       ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fE2LowDpt8Fix)))
// Von-Wert
#define ParamLOG_fE2LowDpt9                          (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE2LowDpt9), Float_Enc_IEEE754Single))
// Bis-Wert
#define ParamLOG_fE2HighDpt9                         (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE2HighDpt9), Float_Enc_IEEE754Single))
// Eingang ist konstant
#define ParamLOG_fE2LowDpt9Fix                       (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE2LowDpt9Fix), Float_Enc_IEEE754Single))
// Von-Wert
#define ParamLOG_fE2LowDpt12                         (knx.paramInt(LOG_ParamCalcIndex(LOG_fE2LowDpt12)))
// Bis-Wert
#define ParamLOG_fE2HighDpt12                        (knx.paramInt(LOG_ParamCalcIndex(LOG_fE2HighDpt12)))
// Eingang ist konstant
#define ParamLOG_fE2LowDpt12Fix                      (knx.paramInt(LOG_ParamCalcIndex(LOG_fE2LowDpt12Fix)))
// Von-Wert
#define ParamLOG_fE2LowDpt13                         ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE2LowDpt13)))
// Bis-Wert
#define ParamLOG_fE2HighDpt13                        ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE2HighDpt13)))
// Eingang ist konstant
#define ParamLOG_fE2LowDpt13Fix                      ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE2LowDpt13Fix)))
// Von-Wert
#define ParamLOG_fE2LowDpt14                         (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE2LowDpt14), Float_Enc_IEEE754Single))
// Bis-Wert
#define ParamLOG_fE2HighDpt14                        (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE2HighDpt14), Float_Enc_IEEE754Single))
// Eingang ist konstant
#define ParamLOG_fE2LowDpt14Fix                      (knx.paramFloat(LOG_ParamCalcIndex(LOG_fE2LowDpt14Fix), Float_Enc_IEEE754Single))
// Eingang ist EIN bei Szene
#define ParamLOG_fE2Low0Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low0Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE2Low1Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low1Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE2Low2Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low2Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE2Low3Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low3Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE2Low4Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low4Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE2Low5Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low5Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE2Low6Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low6Dpt17)))
// ... oder bei Szene
#define ParamLOG_fE2Low7Dpt17                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2Low7Dpt17)))
// Eingang ist konstant
#define ParamLOG_fE2LowDpt17Fix                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fE2LowDpt17Fix)))
// Von-Wert
#define ParamLOG_fE2LowDptRGB                        ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE2LowDptRGB)))
// Bis-Wert
#define ParamLOG_fE2HighDptRGB                       ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE2HighDptRGB)))
// Eingang ist konstant
#define ParamLOG_fE2LowDptRGBFix                     ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fE2LowDptRGBFix)))
// Zeitbezug
#define ParamLOG_fTd1DuskDawn                        (PT_DuskDawn)((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd1DuskDawn)) & LOG_fTd1DuskDawnMask) >> LOG_fTd1DuskDawnShift)
// Zeitbezug
#define ParamLOG_fTd2DuskDawn                        (PT_DuskDawn)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTd2DuskDawn)) & LOG_fTd2DuskDawnMask)
// Zeitbezug
#define ParamLOG_fTd3DuskDawn                        (PT_DuskDawn)((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd3DuskDawn)) & LOG_fTd3DuskDawnMask) >> LOG_fTd3DuskDawnShift)
// Zeitbezug
#define ParamLOG_fTd4DuskDawn                        (PT_DuskDawn)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTd4DuskDawn)) & LOG_fTd4DuskDawnMask)
// Zeitbezug
#define ParamLOG_fTd5DuskDawn                        (PT_DuskDawn)((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd5DuskDawn)) & LOG_fTd5DuskDawnMask) >> LOG_fTd5DuskDawnShift)
// Zeitbezug
#define ParamLOG_fTd6DuskDawn                        (PT_DuskDawn)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTd6DuskDawn)) & LOG_fTd6DuskDawnMask)
// Zeitbezug
#define ParamLOG_fTd7DuskDawn                        (PT_DuskDawn)((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd7DuskDawn)) & LOG_fTd7DuskDawnMask) >> LOG_fTd7DuskDawnShift)
// Zeitbezug
#define ParamLOG_fTd8DuskDawn                        (PT_DuskDawn)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTd8DuskDawn)) & LOG_fTd8DuskDawnMask)
// Typ der Zeitschaltuhr
#define ParamLOG_fTYearDay                           (PT_YearDay)((knx.paramByte(LOG_ParamCalcIndex(LOG_fTYearDay)) & LOG_fTYearDayMask) >> LOG_fTYearDayShift)
// Feiertagsbehandlung
#define ParamLOG_fTHoliday                           (PT_Holiday)((knx.paramByte(LOG_ParamCalcIndex(LOG_fTHoliday)) & LOG_fTHolidayMask) >> LOG_fTHolidayShift)
// Bei Neustart letzte Schaltzeit nachholen
#define ParamLOG_fTRestoreState                      ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTRestoreState)) & LOG_fTRestoreStateMask) >> LOG_fTRestoreStateShift)
// Urlaubsbehandlung
#define ParamLOG_fTVacation                          (PT_Vacation)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTVacation)) & LOG_fTVacationMask)
// Zahlenwert
#define ParamLOG_fTd1ValueNum                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd1ValueNum)))
// Zahlenwert
#define ParamLOG_fTd2ValueNum                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd2ValueNum)))
// Zahlenwert
#define ParamLOG_fTd3ValueNum                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd3ValueNum)))
// Zahlenwert
#define ParamLOG_fTd4ValueNum                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd4ValueNum)))
// Zahlenwert
#define ParamLOG_fTd5ValueNum                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd5ValueNum)))
// Zahlenwert
#define ParamLOG_fTd6ValueNum                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd6ValueNum)))
// Zahlenwert
#define ParamLOG_fTd7ValueNum                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd7ValueNum)))
// Zahlenwert
#define ParamLOG_fTd8ValueNum                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd8ValueNum)))
// Schaltwert
#define ParamLOG_fTd1Value                           ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTd1Value)) & LOG_fTd1ValueMask))
// Grad
#define ParamLOG_fTd1Degree                          ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd1Degree)) & LOG_fTd1DegreeMask) >> LOG_fTd1DegreeShift)
// Stunde
#define ParamLOG_fTd1HourAbs                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd1HourAbs)) & LOG_fTd1HourAbsMask) >> LOG_fTd1HourAbsShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd1HourRel                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd1HourRel)) & LOG_fTd1HourRelMask) >> LOG_fTd1HourRelShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd1HourRelShort                    ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd1HourRelShort)) & LOG_fTd1HourRelShortMask) >> LOG_fTd1HourRelShortShift)
// Minute
#define ParamLOG_fTd1MinuteAbs                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd1MinuteAbs)))
// Minute
#define ParamLOG_fTd1MinuteRel                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd1MinuteRel)))
// Wochentag
#define ParamLOG_fTd1Weekday                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd1Weekday)) & LOG_fTd1WeekdayMask)
// Schaltwert
#define ParamLOG_fTd2Value                           ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTd2Value)) & LOG_fTd2ValueMask))
// Grad
#define ParamLOG_fTd2Degree                          ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd2Degree)) & LOG_fTd2DegreeMask) >> LOG_fTd2DegreeShift)
// Stunde
#define ParamLOG_fTd2HourAbs                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd2HourAbs)) & LOG_fTd2HourAbsMask) >> LOG_fTd2HourAbsShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd2HourRel                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd2HourRel)) & LOG_fTd2HourRelMask) >> LOG_fTd2HourRelShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd2HourRelShort                    ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd2HourRelShort)) & LOG_fTd2HourRelShortMask) >> LOG_fTd2HourRelShortShift)
// Minute
#define ParamLOG_fTd2MinuteAbs                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd2MinuteAbs)))
// Minute
#define ParamLOG_fTd2MinuteRel                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd2MinuteRel)))
// Wochentag
#define ParamLOG_fTd2Weekday                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd2Weekday)) & LOG_fTd2WeekdayMask)
// Schaltwert
#define ParamLOG_fTd3Value                           ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTd3Value)) & LOG_fTd3ValueMask))
// Grad
#define ParamLOG_fTd3Degree                          ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd3Degree)) & LOG_fTd3DegreeMask) >> LOG_fTd3DegreeShift)
// Stunde
#define ParamLOG_fTd3HourAbs                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd3HourAbs)) & LOG_fTd3HourAbsMask) >> LOG_fTd3HourAbsShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd3HourRel                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd3HourRel)) & LOG_fTd3HourRelMask) >> LOG_fTd3HourRelShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd3HourRelShort                    ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd3HourRelShort)) & LOG_fTd3HourRelShortMask) >> LOG_fTd3HourRelShortShift)
// Minute
#define ParamLOG_fTd3MinuteAbs                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd3MinuteAbs)))
// Minute
#define ParamLOG_fTd3MinuteRel                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd3MinuteRel)))
// Wochentag
#define ParamLOG_fTd3Weekday                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd3Weekday)) & LOG_fTd3WeekdayMask)
// Schaltwert
#define ParamLOG_fTd4Value                           ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTd4Value)) & LOG_fTd4ValueMask))
// Grad
#define ParamLOG_fTd4Degree                          ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd4Degree)) & LOG_fTd4DegreeMask) >> LOG_fTd4DegreeShift)
// Stunde
#define ParamLOG_fTd4HourAbs                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd4HourAbs)) & LOG_fTd4HourAbsMask) >> LOG_fTd4HourAbsShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd4HourRel                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd4HourRel)) & LOG_fTd4HourRelMask) >> LOG_fTd4HourRelShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd4HourRelShort                    ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd4HourRelShort)) & LOG_fTd4HourRelShortMask) >> LOG_fTd4HourRelShortShift)
// Minute
#define ParamLOG_fTd4MinuteAbs                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd4MinuteAbs)))
// Minute
#define ParamLOG_fTd4MinuteRel                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd4MinuteRel)))
// Wochentag
#define ParamLOG_fTd4Weekday                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd4Weekday)) & LOG_fTd4WeekdayMask)
// Schaltwert
#define ParamLOG_fTd5Value                           ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTd5Value)) & LOG_fTd5ValueMask))
// Grad
#define ParamLOG_fTd5Degree                          ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd5Degree)) & LOG_fTd5DegreeMask) >> LOG_fTd5DegreeShift)
// Stunde
#define ParamLOG_fTd5HourAbs                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd5HourAbs)) & LOG_fTd5HourAbsMask) >> LOG_fTd5HourAbsShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd5HourRel                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd5HourRel)) & LOG_fTd5HourRelMask) >> LOG_fTd5HourRelShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd5HourRelShort                    ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd5HourRelShort)) & LOG_fTd5HourRelShortMask) >> LOG_fTd5HourRelShortShift)
// Minute
#define ParamLOG_fTd5MinuteAbs                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd5MinuteAbs)))
// Minute
#define ParamLOG_fTd5MinuteRel                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd5MinuteRel)))
// Wochentag
#define ParamLOG_fTd5Weekday                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd5Weekday)) & LOG_fTd5WeekdayMask)
// Schaltwert
#define ParamLOG_fTd6Value                           ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTd6Value)) & LOG_fTd6ValueMask))
// Grad
#define ParamLOG_fTd6Degree                          ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd6Degree)) & LOG_fTd6DegreeMask) >> LOG_fTd6DegreeShift)
// Stunde
#define ParamLOG_fTd6HourAbs                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd6HourAbs)) & LOG_fTd6HourAbsMask) >> LOG_fTd6HourAbsShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd6HourRel                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd6HourRel)) & LOG_fTd6HourRelMask) >> LOG_fTd6HourRelShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd6HourRelShort                    ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd6HourRelShort)) & LOG_fTd6HourRelShortMask) >> LOG_fTd6HourRelShortShift)
// Minute
#define ParamLOG_fTd6MinuteAbs                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd6MinuteAbs)))
// Minute
#define ParamLOG_fTd6MinuteRel                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd6MinuteRel)))
// Wochentag
#define ParamLOG_fTd6Weekday                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd6Weekday)) & LOG_fTd6WeekdayMask)
// Schaltwert
#define ParamLOG_fTd7Value                           ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTd7Value)) & LOG_fTd7ValueMask))
// Grad
#define ParamLOG_fTd7Degree                          ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd7Degree)) & LOG_fTd7DegreeMask) >> LOG_fTd7DegreeShift)
// Stunde
#define ParamLOG_fTd7HourAbs                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd7HourAbs)) & LOG_fTd7HourAbsMask) >> LOG_fTd7HourAbsShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd7HourRel                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd7HourRel)) & LOG_fTd7HourRelMask) >> LOG_fTd7HourRelShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd7HourRelShort                    ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd7HourRelShort)) & LOG_fTd7HourRelShortMask) >> LOG_fTd7HourRelShortShift)
// Minute
#define ParamLOG_fTd7MinuteAbs                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd7MinuteAbs)))
// Minute
#define ParamLOG_fTd7MinuteRel                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd7MinuteRel)))
// Wochentag
#define ParamLOG_fTd7Weekday                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd7Weekday)) & LOG_fTd7WeekdayMask)
// Schaltwert
#define ParamLOG_fTd8Value                           ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTd8Value)) & LOG_fTd8ValueMask))
// Grad
#define ParamLOG_fTd8Degree                          ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd8Degree)) & LOG_fTd8DegreeMask) >> LOG_fTd8DegreeShift)
// Stunde
#define ParamLOG_fTd8HourAbs                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd8HourAbs)) & LOG_fTd8HourAbsMask) >> LOG_fTd8HourAbsShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd8HourRel                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd8HourRel)) & LOG_fTd8HourRelMask) >> LOG_fTd8HourRelShift)
// Sonnen auf-/untergang
#define ParamLOG_fTd8HourRelShort                    ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTd8HourRelShort)) & LOG_fTd8HourRelShortMask) >> LOG_fTd8HourRelShortShift)
// Minute
#define ParamLOG_fTd8MinuteAbs                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd8MinuteAbs)))
// Minute
#define ParamLOG_fTd8MinuteRel                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd8MinuteRel)))
// Wochentag
#define ParamLOG_fTd8Weekday                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fTd8Weekday)) & LOG_fTd8WeekdayMask)
// Mo
#define ParamLOG_fTy1Weekday1                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy1Weekday1)) & LOG_fTy1Weekday1Mask))
// Di
#define ParamLOG_fTy1Weekday2                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy1Weekday2)) & LOG_fTy1Weekday2Mask))
// Mi
#define ParamLOG_fTy1Weekday3                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy1Weekday3)) & LOG_fTy1Weekday3Mask))
// Do
#define ParamLOG_fTy1Weekday4                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy1Weekday4)) & LOG_fTy1Weekday4Mask))
// Fr
#define ParamLOG_fTy1Weekday5                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy1Weekday5)) & LOG_fTy1Weekday5Mask))
// Sa
#define ParamLOG_fTy1Weekday6                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy1Weekday6)) & LOG_fTy1Weekday6Mask))
// So
#define ParamLOG_fTy1Weekday7                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy1Weekday7)) & LOG_fTy1Weekday7Mask))
// Tag
#define ParamLOG_fTy1Day                             ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTy1Day)) & LOG_fTy1DayMask) >> LOG_fTy1DayShift)
// Wochentag
#define ParamLOG_fTy1IsWeekday                       ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy1IsWeekday)) & LOG_fTy1IsWeekdayMask))
// Monat
#define ParamLOG_fTy1Month                           ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTy1Month)) & LOG_fTy1MonthMask) >> LOG_fTy1MonthShift)
// Mo
#define ParamLOG_fTy2Weekday1                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy2Weekday1)) & LOG_fTy2Weekday1Mask))
// Di
#define ParamLOG_fTy2Weekday2                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy2Weekday2)) & LOG_fTy2Weekday2Mask))
// Mi
#define ParamLOG_fTy2Weekday3                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy2Weekday3)) & LOG_fTy2Weekday3Mask))
// Do
#define ParamLOG_fTy2Weekday4                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy2Weekday4)) & LOG_fTy2Weekday4Mask))
// Fr
#define ParamLOG_fTy2Weekday5                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy2Weekday5)) & LOG_fTy2Weekday5Mask))
// Sa
#define ParamLOG_fTy2Weekday6                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy2Weekday6)) & LOG_fTy2Weekday6Mask))
// So
#define ParamLOG_fTy2Weekday7                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy2Weekday7)) & LOG_fTy2Weekday7Mask))
// Tag
#define ParamLOG_fTy2Day                             ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTy2Day)) & LOG_fTy2DayMask) >> LOG_fTy2DayShift)
// Wochentag
#define ParamLOG_fTy2IsWeekday                       ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy2IsWeekday)) & LOG_fTy2IsWeekdayMask))
// Monat
#define ParamLOG_fTy2Month                           ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTy2Month)) & LOG_fTy2MonthMask) >> LOG_fTy2MonthShift)
// Mo
#define ParamLOG_fTy3Weekday1                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy3Weekday1)) & LOG_fTy3Weekday1Mask))
// Di
#define ParamLOG_fTy3Weekday2                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy3Weekday2)) & LOG_fTy3Weekday2Mask))
// Mi
#define ParamLOG_fTy3Weekday3                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy3Weekday3)) & LOG_fTy3Weekday3Mask))
// Do
#define ParamLOG_fTy3Weekday4                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy3Weekday4)) & LOG_fTy3Weekday4Mask))
// Fr
#define ParamLOG_fTy3Weekday5                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy3Weekday5)) & LOG_fTy3Weekday5Mask))
// Sa
#define ParamLOG_fTy3Weekday6                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy3Weekday6)) & LOG_fTy3Weekday6Mask))
// So
#define ParamLOG_fTy3Weekday7                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy3Weekday7)) & LOG_fTy3Weekday7Mask))
// Tag
#define ParamLOG_fTy3Day                             ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTy3Day)) & LOG_fTy3DayMask) >> LOG_fTy3DayShift)
// Wochentag
#define ParamLOG_fTy3IsWeekday                       ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy3IsWeekday)) & LOG_fTy3IsWeekdayMask))
// Monat
#define ParamLOG_fTy3Month                           ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTy3Month)) & LOG_fTy3MonthMask) >> LOG_fTy3MonthShift)
// Mo
#define ParamLOG_fTy4Weekday1                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy4Weekday1)) & LOG_fTy4Weekday1Mask))
// Di
#define ParamLOG_fTy4Weekday2                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy4Weekday2)) & LOG_fTy4Weekday2Mask))
// Mi
#define ParamLOG_fTy4Weekday3                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy4Weekday3)) & LOG_fTy4Weekday3Mask))
// Do
#define ParamLOG_fTy4Weekday4                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy4Weekday4)) & LOG_fTy4Weekday4Mask))
// Fr
#define ParamLOG_fTy4Weekday5                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy4Weekday5)) & LOG_fTy4Weekday5Mask))
// Sa
#define ParamLOG_fTy4Weekday6                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy4Weekday6)) & LOG_fTy4Weekday6Mask))
// So
#define ParamLOG_fTy4Weekday7                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy4Weekday7)) & LOG_fTy4Weekday7Mask))
// Tag
#define ParamLOG_fTy4Day                             ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTy4Day)) & LOG_fTy4DayMask) >> LOG_fTy4DayShift)
// Wochentag
#define ParamLOG_fTy4IsWeekday                       ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fTy4IsWeekday)) & LOG_fTy4IsWeekdayMask))
// Monat
#define ParamLOG_fTy4Month                           ((knx.paramByte(LOG_ParamCalcIndex(LOG_fTy4Month)) & LOG_fTy4MonthMask) >> LOG_fTy4MonthShift)
// Interner Eingang 3
#define ParamLOG_fI1                                 (PT_InputEnable)((knx.paramByte(LOG_ParamCalcIndex(LOG_fI1)) & LOG_fI1Mask) >> LOG_fI1Shift)
// Art der Verknüpfung
#define ParamLOG_fI1Kind                             (PT_KORelInput)((knx.paramByte(LOG_ParamCalcIndex(LOG_fI1Kind)) & LOG_fI1KindMask) >> LOG_fI1KindShift)
// Internen Eingang als Trigger nutzen(ist immer logisch EIN)
#define ParamLOG_fI1AsTrigger                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fI1AsTrigger)) & LOG_fI1AsTriggerMask))
// Interner Eingang wird versorgt vom
#define ParamLOG_fI1InternalInputType                (PT_InternalInputType)((knx.paramByte(LOG_ParamCalcIndex(LOG_fI1InternalInputType)) & LOG_fI1InternalInputTypeMask) >> LOG_fI1InternalInputTypeShift)
// Internen Eingang verbinden mit Kanal Nr.
#define ParamLOG_fI1Function                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fI1Function)))
// Internen Eingang verbinden mit Kanal Nr.
#define ParamLOG_fI1FunctionRel                      ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fI1FunctionRel)))
// Statuskanal
#define ParamLOG_fI1StatusLed                        (knx.paramWord(LOG_ParamCalcIndex(LOG_fI1StatusLed)))
// Interner Eingang 4
#define ParamLOG_fI2                                 (PT_InputEnable)((knx.paramByte(LOG_ParamCalcIndex(LOG_fI2)) & LOG_fI2Mask) >> LOG_fI2Shift)
// Art der Verknüpfung
#define ParamLOG_fI2Kind                             (PT_KORelInput)((knx.paramByte(LOG_ParamCalcIndex(LOG_fI2Kind)) & LOG_fI2KindMask) >> LOG_fI2KindShift)
// Internen Eingang als Trigger nutzen(ist immer logisch EIN)
#define ParamLOG_fI2AsTrigger                        ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fI2AsTrigger)) & LOG_fI2AsTriggerMask))
// Interner Eingang wird versorgt vom
#define ParamLOG_fI2InternalInputType                (PT_InternalInputType)((knx.paramByte(LOG_ParamCalcIndex(LOG_fI2InternalInputType)) & LOG_fI2InternalInputTypeMask) >> LOG_fI2InternalInputTypeShift)
// Internen Eingang verbinden mit Kanal Nr.
#define ParamLOG_fI2Function                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fI2Function)))
// Internen Eingang verbinden mit Kanal Nr.
#define ParamLOG_fI2FunctionRel                      ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fI2FunctionRel)))
// Statuskanal
#define ParamLOG_fI2StatusLed                        (knx.paramWord(LOG_ParamCalcIndex(LOG_fI2StatusLed)))
// Zeit für Treppenlicht
#define ParamLOG_fOStairtimeBase                     ((knx.paramByte(LOG_ParamCalcIndex(LOG_fOStairtimeBase)) & LOG_fOStairtimeBaseMask) >> LOG_fOStairtimeBaseShift)
// Zeit für Treppenlicht
#define ParamLOG_fOStairtimeTime                     (knx.paramWord(LOG_ParamCalcIndex(LOG_fOStairtimeTime)) & LOG_fOStairtimeTimeMask)
// Zeit für Treppenlicht (in Millisekunden)
#define ParamLOG_fOStairtimeTimeMS                   (paramDelay(knx.paramWord(LOG_ParamCalcIndex(LOG_fOStairtimeTime))))
// Treppenlicht blinkt im Rhythmus
#define ParamLOG_fOBlinkBase                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fOBlinkBase)) & LOG_fOBlinkBaseMask) >> LOG_fOBlinkBaseShift)
// Treppenlicht blinkt im Rhythmus
#define ParamLOG_fOBlinkTime                         (knx.paramWord(LOG_ParamCalcIndex(LOG_fOBlinkTime)) & LOG_fOBlinkTimeMask)
// Treppenlicht blinkt im Rhythmus (in Millisekunden)
#define ParamLOG_fOBlinkTimeMS                       (paramDelay(knx.paramWord(LOG_ParamCalcIndex(LOG_fOBlinkTime))))
// EINschalten wird verzögert um
#define ParamLOG_fODelayOnBase                       ((knx.paramByte(LOG_ParamCalcIndex(LOG_fODelayOnBase)) & LOG_fODelayOnBaseMask) >> LOG_fODelayOnBaseShift)
// EINschalten wird verzögert um
#define ParamLOG_fODelayOnTime                       (knx.paramWord(LOG_ParamCalcIndex(LOG_fODelayOnTime)) & LOG_fODelayOnTimeMask)
// EINschalten wird verzögert um (in Millisekunden)
#define ParamLOG_fODelayOnTimeMS                     (paramDelay(knx.paramWord(LOG_ParamCalcIndex(LOG_fODelayOnTime))))
// AUSschalten wird verzögert um
#define ParamLOG_fODelayOffBase                      ((knx.paramByte(LOG_ParamCalcIndex(LOG_fODelayOffBase)) & LOG_fODelayOffBaseMask) >> LOG_fODelayOffBaseShift)
// AUSschalten wird verzögert um
#define ParamLOG_fODelayOffTime                      (knx.paramWord(LOG_ParamCalcIndex(LOG_fODelayOffTime)) & LOG_fODelayOffTimeMask)
// AUSschalten wird verzögert um (in Millisekunden)
#define ParamLOG_fODelayOffTimeMS                    (paramDelay(knx.paramWord(LOG_ParamCalcIndex(LOG_fODelayOffTime))))
// EIN-Telegramm wird wiederholt alle
#define ParamLOG_fORepeatOnBase                      ((knx.paramByte(LOG_ParamCalcIndex(LOG_fORepeatOnBase)) & LOG_fORepeatOnBaseMask) >> LOG_fORepeatOnBaseShift)
// EIN-Telegramm wird wiederholt alle
#define ParamLOG_fORepeatOnTime                      (knx.paramWord(LOG_ParamCalcIndex(LOG_fORepeatOnTime)) & LOG_fORepeatOnTimeMask)
// EIN-Telegramm wird wiederholt alle (in Millisekunden)
#define ParamLOG_fORepeatOnTimeMS                    (paramDelay(knx.paramWord(LOG_ParamCalcIndex(LOG_fORepeatOnTime))))
// AUS-Telegramm wird wiederholt alle
#define ParamLOG_fORepeatOffBase                     ((knx.paramByte(LOG_ParamCalcIndex(LOG_fORepeatOffBase)) & LOG_fORepeatOffBaseMask) >> LOG_fORepeatOffBaseShift)
// AUS-Telegramm wird wiederholt alle
#define ParamLOG_fORepeatOffTime                     (knx.paramWord(LOG_ParamCalcIndex(LOG_fORepeatOffTime)) & LOG_fORepeatOffTimeMask)
// AUS-Telegramm wird wiederholt alle (in Millisekunden)
#define ParamLOG_fORepeatOffTimeMS                   (paramDelay(knx.paramWord(LOG_ParamCalcIndex(LOG_fORepeatOffTime))))
// Ausgang schaltet zeitverzögert
#define ParamLOG_fODelay                             ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fODelay)) & LOG_fODelayMask))
// Erneutes EIN führt zu
#define ParamLOG_fODelayOnRepeat                     (PT_OnOffRepeat)((knx.paramByte(LOG_ParamCalcIndex(LOG_fODelayOnRepeat)) & LOG_fODelayOnRepeatMask) >> LOG_fODelayOnRepeatShift)
// Darauffolgendes AUS führt zu
#define ParamLOG_fODelayOnReset                      (PT_OnOffReset)((knx.paramByte(LOG_ParamCalcIndex(LOG_fODelayOnReset)) & LOG_fODelayOnResetMask) >> LOG_fODelayOnResetShift)
// Erneutes AUS führt zu
#define ParamLOG_fODelayOffRepeat                    (PT_OnOffRepeat)((knx.paramByte(LOG_ParamCalcIndex(LOG_fODelayOffRepeat)) & LOG_fODelayOffRepeatMask) >> LOG_fODelayOffRepeatShift)
// Darauffolgendes EIN führt zu
#define ParamLOG_fODelayOffReset                     (PT_OnOffReset)((knx.paramByte(LOG_ParamCalcIndex(LOG_fODelayOffReset)) & LOG_fODelayOffResetMask) >> LOG_fODelayOffResetShift)
// Ausgang hat eine Treppenlichtfunktion
#define ParamLOG_fOStair                             ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOStair)) & LOG_fOStairMask))
// Treppenlicht kann verlängert werden
#define ParamLOG_fORetrigger                         ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fORetrigger)) & LOG_fORetriggerMask))
// Treppenlicht kann ausgeschaltet werden
#define ParamLOG_fOStairOff                          ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOStairOff)) & LOG_fOStairOffMask))
// Ausgang wiederholt zyklisch
#define ParamLOG_fORepeat                            ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fORepeat)) & LOG_fORepeatMask))
// Wiederholungsfilter
#define ParamLOG_fOOutputFilter                      (PT_OutputFilter)((knx.paramByte(LOG_ParamCalcIndex(LOG_fOOutputFilter)) & LOG_fOOutputFilterMask) >> LOG_fOOutputFilterShift)
// Sendeverhalten für Ausgang
#define ParamLOG_fOSendOnChange                      (PT_SendOnChange)((knx.paramByte(LOG_ParamCalcIndex(LOG_fOSendOnChange)) & LOG_fOSendOnChangeMask) >> LOG_fOSendOnChangeShift)
// Sperre aktivieren
#define ParamLOG_fOLockEnabled                       ((bool)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOLockEnabled)) & LOG_fOLockEnabledMask))
// DPT für Ausgang
#define ParamLOG_fODpt                               (PT_LogicDpt)(knx.paramByte(LOG_ParamCalcIndex(LOG_fODpt)))
// Beim Sperren
#define ParamLOG_fOLockTriggerLock                   (PT_LockTrigger)((knx.paramByte(LOG_ParamCalcIndex(LOG_fOLockTriggerLock)) & LOG_fOLockTriggerLockMask) >> LOG_fOLockTriggerLockShift)
// Beim Entsperren
#define ParamLOG_fOLockTriggerUnlock                 (PT_LockTrigger)((knx.paramByte(LOG_ParamCalcIndex(LOG_fOLockTriggerUnlock)) & LOG_fOLockTriggerUnlockMask) >> LOG_fOLockTriggerUnlockShift)
// Anschließend die Signalverarbeitung
#define ParamLOG_fOLockResetQueue                    (PT_LockResetQueue)((knx.paramByte(LOG_ParamCalcIndex(LOG_fOLockResetQueue)) & LOG_fOLockResetQueueMask) >> LOG_fOLockResetQueueShift)
// Art der Verknüpfung
#define ParamLOG_fOLockKind                          (PT_KORelInput)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOLockKind)) & LOG_fOLockKindMask)
// Sperre verbinden mit Kanal Nr.
#define ParamLOG_fOLockFunction                      (knx.paramByte(LOG_ParamCalcIndex(LOG_fOLockFunction)))
// Sperre verbinden mit Kanal Nr.
#define ParamLOG_fOLockFunctionRel                   ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fOLockFunctionRel)))
// Wert für EIN senden?
#define ParamLOG_fOOnAll                             (PT_OutputSend)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnAll)))
//     Wert für EIN senden als
#define ParamLOG_fOOnDpt1                            (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnDpt1)))
//     Wert für EIN senden als
#define ParamLOG_fOOnDpt2                            (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnDpt2)))
//     Wert für EIN senden als
#define ParamLOG_fOOnDpt3Dir                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnDpt3Dir)) & LOG_fOOnDpt3DirMask) >> LOG_fOOnDpt3DirShift)
// 
#define ParamLOG_fOOnDpt3Dim                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnDpt3Dim)) & LOG_fOOnDpt3DimMask)
//     Wert für EIN senden als 
#define ParamLOG_fOOnDpt5                            (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnDpt5)))
//     Wert für EIN senden als
#define ParamLOG_fOOnDpt5001                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnDpt5001)))
//     Wert für EIN senden als
#define ParamLOG_fOOnDpt6                            ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnDpt6)))
//     Wert für EIN senden als
#define ParamLOG_fOOnDpt7                            (knx.paramWord(LOG_ParamCalcIndex(LOG_fOOnDpt7)))
//     Wert für EIN senden als
#define ParamLOG_fOOnDpt8                            ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fOOnDpt8)))
//     Wert für EIN senden als
#define ParamLOG_fOOnDpt9                            (knx.paramFloat(LOG_ParamCalcIndex(LOG_fOOnDpt9), Float_Enc_IEEE754Single))
//     Wert für EIN senden als
#define ParamLOG_fOOnDpt12                           (knx.paramInt(LOG_ParamCalcIndex(LOG_fOOnDpt12)))
//     Wert für EIN senden als
#define ParamLOG_fOOnDpt13                           ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fOOnDpt13)))
//     Wert für EIN senden als
#define ParamLOG_fOOnDpt14                           (knx.paramFloat(LOG_ParamCalcIndex(LOG_fOOnDpt14), Float_Enc_IEEE754Single))
//     Wert für EIN senden als 
#define ParamLOG_fOOnDpt16                           (knx.paramData(LOG_ParamCalcIndex(LOG_fOOnDpt16)))
#define ParamLOG_fOOnDpt16Str                        (knx.paramString(LOG_ParamCalcIndex(LOG_fOOnDpt16), LOG_fOOnDpt16Length))
//     Wert für EIN senden als 
#define ParamLOG_fOOnDpt17                           (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnDpt17)))
//     Wert für EIN senden als (3-Byte-RGB)
#define ParamLOG_fOOnRGB                             ((knx.paramInt(LOG_ParamCalcIndex(LOG_fOOnRGB)) & LOG_fOOnRGBMask) >> LOG_fOOnRGBShift)
//     Status-LED Kanal
#define ParamLOG_fOOnLedProvider                     (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnLedProvider)) & LOG_fOOnLedProviderMask)
//     Status-LED Effekt
#define ParamLOG_fOOnLedEffect                       (PT_StatusLedEffect)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnLedEffect)) & LOG_fOOnLedEffectMask)
//     Status-LED Effektdauer
#define ParamLOG_fOOnLedDuration                     (knx.paramWord(LOG_ParamCalcIndex(LOG_fOOnLedDuration)))
// 
#define ParamLOG_fOOnPAArea                          ((knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnPAArea)) & LOG_fOOnPAAreaMask) >> LOG_fOOnPAAreaShift)
// 
#define ParamLOG_fOOnPALine                          (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnPALine)) & LOG_fOOnPALineMask)
// 
#define ParamLOG_fOOnPADevice                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnPADevice)))
//     Wert für EIN ermitteln als
#define ParamLOG_fOOnFunction                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnFunction)))
//     Nummer des Kommunikationsobjekts
#define ParamLOG_fOOnKOKind                          (PT_KORelInput)((knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnKOKind)) & LOG_fOOnKOKindMask) >> LOG_fOOnKOKindShift)
//     Nummer des Kommunikationsobjekts
#define ParamLOG_fOOnKONumber                        (knx.paramWord(LOG_ParamCalcIndex(LOG_fOOnKONumber)))
//     Nummer des Kommunikationsobjekts
#define ParamLOG_fOOnKONumberRel                     ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fOOnKONumberRel)))
//     DPT des Kommunikationsobjekts
#define ParamLOG_fOOnKODpt                           (PT_LogicDpt)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnKODpt)))
//     Wert für EIN an ein zusätzliches    KO senden?
#define ParamLOG_fOOnKOSend                          (PT_KORelInput)((knx.paramByte(LOG_ParamCalcIndex(LOG_fOOnKOSend)) & LOG_fOOnKOSendMask) >> LOG_fOOnKOSendShift)
//         Nummer des zusätzlichen KO
#define ParamLOG_fOOnKOSendNumber                    (knx.paramWord(LOG_ParamCalcIndex(LOG_fOOnKOSendNumber)))
//         Nummer des zusätzlichen KO
#define ParamLOG_fOOnKOSendNumberRel                 ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fOOnKOSendNumberRel)))
// Wert für AUS senden?
#define ParamLOG_fOOffAll                            (PT_OutputSend)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffAll)))
//     Wert für AUS senden als
#define ParamLOG_fOOffDpt1                           (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffDpt1)))
//     Wert für AUS senden als
#define ParamLOG_fOOffDpt2                           (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffDpt2)))
//     Wert für AUS senden als
#define ParamLOG_fOOffDpt3Dir                        ((knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffDpt3Dir)) & LOG_fOOffDpt3DirMask) >> LOG_fOOffDpt3DirShift)
// 
#define ParamLOG_fOOffDpt3Dim                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffDpt3Dim)) & LOG_fOOffDpt3DimMask)
//     Wert für AUS senden als
#define ParamLOG_fOOffDpt5                           (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffDpt5)))
//     Wert für AUS senden als
#define ParamLOG_fOOffDpt5001                        (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffDpt5001)))
//     Wert für AUS senden als
#define ParamLOG_fOOffDpt6                           ((int8_t)knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffDpt6)))
//     Wert für AUS senden als
#define ParamLOG_fOOffDpt7                           (knx.paramWord(LOG_ParamCalcIndex(LOG_fOOffDpt7)))
//     Wert für AUS senden als
#define ParamLOG_fOOffDpt8                           ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fOOffDpt8)))
//     Wert für AUS senden als
#define ParamLOG_fOOffDpt9                           (knx.paramFloat(LOG_ParamCalcIndex(LOG_fOOffDpt9), Float_Enc_IEEE754Single))
//     Wert für AUS senden als
#define ParamLOG_fOOffDpt12                          (knx.paramInt(LOG_ParamCalcIndex(LOG_fOOffDpt12)))
//     Wert für AUS senden als
#define ParamLOG_fOOffDpt13                          ((int32_t)knx.paramInt(LOG_ParamCalcIndex(LOG_fOOffDpt13)))
//     Wert für AUS senden als
#define ParamLOG_fOOffDpt14                          (knx.paramFloat(LOG_ParamCalcIndex(LOG_fOOffDpt14), Float_Enc_IEEE754Single))
//     Wert für AUS senden als
#define ParamLOG_fOOffDpt16                          (knx.paramData(LOG_ParamCalcIndex(LOG_fOOffDpt16)))
#define ParamLOG_fOOffDpt16Str                       (knx.paramString(LOG_ParamCalcIndex(LOG_fOOffDpt16), LOG_fOOffDpt16Length))
//     Wert für AUS senden als 
#define ParamLOG_fOOffDpt17                          (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffDpt17)))
//     Wert für AUS senden als (3-Byte-RGB)
#define ParamLOG_fOOffRGB                            ((knx.paramInt(LOG_ParamCalcIndex(LOG_fOOffRGB)) & LOG_fOOffRGBMask) >> LOG_fOOffRGBShift)
//     Status-LED-Kanal
#define ParamLOG_fOOffLedProvider                    (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffLedProvider)) & LOG_fOOffLedProviderMask)
//     Status-LED Effekt
#define ParamLOG_fOOffLedEffect                      (PT_StatusLedEffect)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffLedEffect)) & LOG_fOOffLedEffectMask)
//     Status-LED Effektdauer
#define ParamLOG_fOOffLedDuration                    (knx.paramWord(LOG_ParamCalcIndex(LOG_fOOffLedDuration)))
// 
#define ParamLOG_fOOffPAArea                         ((knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffPAArea)) & LOG_fOOffPAAreaMask) >> LOG_fOOffPAAreaShift)
// 
#define ParamLOG_fOOffPALine                         (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffPALine)) & LOG_fOOffPALineMask)
// 
#define ParamLOG_fOOffPADevice                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffPADevice)))
//     Wert für AUS ermitteln als
#define ParamLOG_fOOffFunction                       (knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffFunction)))
//     Nummer des Kommunikationsobjekts
#define ParamLOG_fOOffKOKind                         (PT_KORelInput)((knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffKOKind)) & LOG_fOOffKOKindMask) >> LOG_fOOffKOKindShift)
//     Nummer des Kommunikationsobjekts
#define ParamLOG_fOOffKONumber                       (knx.paramWord(LOG_ParamCalcIndex(LOG_fOOffKONumber)))
//     Nummer des Kommunikationsobjekts
#define ParamLOG_fOOffKONumberRel                    ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fOOffKONumberRel)))
//     DPT des Kommunikationsobjekts
#define ParamLOG_fOOffKODpt                          (PT_LogicDpt)(knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffKODpt)))
//     Wert für AUS an ein zusätzliches    KO senden?
#define ParamLOG_fOOffKOSend                         (PT_KORelInput)((knx.paramByte(LOG_ParamCalcIndex(LOG_fOOffKOSend)) & LOG_fOOffKOSendMask) >> LOG_fOOffKOSendShift)
//         Nummer des zusätzlichen KO
#define ParamLOG_fOOffKOSendNumber                   (knx.paramWord(LOG_ParamCalcIndex(LOG_fOOffKOSendNumber)))
//         Nummer des zusätzlichen KO
#define ParamLOG_fOOffKOSendNumberRel                ((int16_t)knx.paramWord(LOG_ParamCalcIndex(LOG_fOOffKOSendNumberRel)))

// deprecated
#define LOG_KoOffset 100

// Communication objects per channel (multiple occurrence)
#define LOG_KoBlockOffset 100
#define LOG_KoBlockSize 3

#define LOG_KoCalcNumber(index) (index + LOG_KoBlockOffset + _channelIndex * LOG_KoBlockSize)
#define LOG_KoCalcIndex(number) ((number >= LOG_KoCalcNumber(0) && number < LOG_KoCalcNumber(LOG_KoBlockSize)) ? (number - LOG_KoBlockOffset) % LOG_KoBlockSize : -1)
#define LOG_KoCalcChannel(number) ((number >= LOG_KoBlockOffset && number < LOG_KoBlockOffset + LOG_ChannelCount * LOG_KoBlockSize) ? (number - LOG_KoBlockOffset) / LOG_KoBlockSize : -1)

#define LOG_KoKOfE1 0
#define LOG_KoKOfE2 1
#define LOG_KoKOfO 2

// Eingang 1
#define KoLOG_KOfE1                               (knx.getGroupObject(LOG_KoCalcNumber(LOG_KoKOfE1)))
// Eingang 2
#define KoLOG_KOfE2                               (knx.getGroupObject(LOG_KoCalcNumber(LOG_KoKOfE2)))
// Ausgang
#define KoLOG_KOfO                                (knx.getGroupObject(LOG_KoCalcNumber(LOG_KoKOfO)))

#define FCB_VisibleChannels                     8082      // uint8_t

// Verfügbare Kanäle
#define ParamFCB_VisibleChannels                     (knx.paramByte(FCB_VisibleChannels))

#define FCB_ChannelCount 15

// Parameter per channel
#define FCB_ParamBlockOffset 8083
#define FCB_ParamBlockSize 81
#define FCB_ParamCalcIndex(index) (index + FCB_ParamBlockOffset + _channelIndex * FCB_ParamBlockSize)

#define FCB_CHChannelType                        0      // 8 Bits, Bit 7-0
#define FCB_CHChannelDisabled                    1      // 1 Bit, Bit 7
#define     FCB_CHChannelDisabledMask 0x80
#define     FCB_CHChannelDisabledShift 7
#define FCB_CHLogicKo0D                          2      // 2 Bits, Bit 7-6
#define     FCB_CHLogicKo0DMask 0xC0
#define     FCB_CHLogicKo0DShift 6
#define FCB_CHLogicKo1D                          2      // 2 Bits, Bit 5-4
#define     FCB_CHLogicKo1DMask 0x30
#define     FCB_CHLogicKo1DShift 4
#define FCB_CHLogicKo2D                          2      // 2 Bits, Bit 3-2
#define     FCB_CHLogicKo2DMask 0x0C
#define     FCB_CHLogicKo2DShift 2
#define FCB_CHLogicKo3D                          2      // 2 Bits, Bit 1-0
#define     FCB_CHLogicKo3DMask 0x03
#define     FCB_CHLogicKo3DShift 0
#define FCB_CHLogicKo4D                          3      // 2 Bits, Bit 7-6
#define     FCB_CHLogicKo4DMask 0xC0
#define     FCB_CHLogicKo4DShift 6
#define FCB_CHLogicKo5D                          3      // 2 Bits, Bit 5-4
#define     FCB_CHLogicKo5DMask 0x30
#define     FCB_CHLogicKo5DShift 4
#define FCB_CHLogicKo6D                          3      // 2 Bits, Bit 3-2
#define     FCB_CHLogicKo6DMask 0x0C
#define     FCB_CHLogicKo6DShift 2
#define FCB_CHLogicKo7D                          3      // 2 Bits, Bit 1-0
#define     FCB_CHLogicKo7DMask 0x03
#define     FCB_CHLogicKo7DShift 0
#define FCB_CHLogicKo8D                          4      // 2 Bits, Bit 7-6
#define     FCB_CHLogicKo8DMask 0xC0
#define     FCB_CHLogicKo8DShift 6
#define FCB_CHLogicOutInv                        4      // 1 Bit, Bit 4
#define     FCB_CHLogicOutInvMask 0x10
#define     FCB_CHLogicOutInvShift 4
#define FCB_CHLogicBehavOut                      4      // 1 Bit, Bit 3
#define     FCB_CHLogicBehavOutMask 0x08
#define     FCB_CHLogicBehavOutShift 3
#define FCB_CHLogicBehavKo0                      5      // 4 Bits, Bit 7-4
#define     FCB_CHLogicBehavKo0Mask 0xF0
#define     FCB_CHLogicBehavKo0Shift 4
#define FCB_CHLogicBehavKo1                      5      // 4 Bits, Bit 3-0
#define     FCB_CHLogicBehavKo1Mask 0x0F
#define     FCB_CHLogicBehavKo1Shift 0
#define FCB_CHLogicBehavKo2                      6      // 4 Bits, Bit 7-4
#define     FCB_CHLogicBehavKo2Mask 0xF0
#define     FCB_CHLogicBehavKo2Shift 4
#define FCB_CHLogicBehavKo3                      6      // 4 Bits, Bit 3-0
#define     FCB_CHLogicBehavKo3Mask 0x0F
#define     FCB_CHLogicBehavKo3Shift 0
#define FCB_CHLogicBehavKo4                      7      // 4 Bits, Bit 7-4
#define     FCB_CHLogicBehavKo4Mask 0xF0
#define     FCB_CHLogicBehavKo4Shift 4
#define FCB_CHLogicBehavKo5                      7      // 4 Bits, Bit 3-0
#define     FCB_CHLogicBehavKo5Mask 0x0F
#define     FCB_CHLogicBehavKo5Shift 0
#define FCB_CHLogicBehavKo6                      8      // 4 Bits, Bit 7-4
#define     FCB_CHLogicBehavKo6Mask 0xF0
#define     FCB_CHLogicBehavKo6Shift 4
#define FCB_CHLogicBehavKo7                      8      // 4 Bits, Bit 3-0
#define     FCB_CHLogicBehavKo7Mask 0x0F
#define     FCB_CHLogicBehavKo7Shift 0
#define FCB_CHLogicBehavKo8                      9      // 4 Bits, Bit 7-4
#define     FCB_CHLogicBehavKo8Mask 0xF0
#define     FCB_CHLogicBehavKo8Shift 4
#define FCB_CHBayesianPrior                     10      // uint8_t
#define FCB_CHBayesianThreshold                 11      // uint8_t
#define FCB_CHBayesianEnableProbOutput          12      // 1 Bit, Bit 7
#define     FCB_CHBayesianEnableProbOutputMask 0x80
#define     FCB_CHBayesianEnableProbOutputShift 7
#define FCB_CHLogicKo0BayesProbTrue             15      // uint8_t
#define FCB_CHLogicKo0BayesProbFalse            16      // uint8_t
#define FCB_CHLogicKo1BayesProbTrue             17      // uint8_t
#define FCB_CHLogicKo1BayesProbFalse            18      // uint8_t
#define FCB_CHLogicKo2BayesProbTrue             19      // uint8_t
#define FCB_CHLogicKo2BayesProbFalse            20      // uint8_t
#define FCB_CHLogicKo3BayesProbTrue             21      // uint8_t
#define FCB_CHLogicKo3BayesProbFalse            22      // uint8_t
#define FCB_CHLogicKo4BayesProbTrue             23      // uint8_t
#define FCB_CHLogicKo4BayesProbFalse            24      // uint8_t
#define FCB_CHLogicKo5BayesProbTrue             25      // uint8_t
#define FCB_CHLogicKo5BayesProbFalse            26      // uint8_t
#define FCB_CHLogicKo6BayesProbTrue             27      // uint8_t
#define FCB_CHLogicKo6BayesProbFalse            28      // uint8_t
#define FCB_CHLogicKo7BayesProbTrue             29      // uint8_t
#define FCB_CHLogicKo7BayesProbFalse            30      // uint8_t
#define FCB_CHLogicKo8BayesProbTrue             31      // uint8_t
#define FCB_CHLogicKo8BayesProbFalse            32      // uint8_t
#define FCB_CHPrioKo0D                           2      // 2 Bits, Bit 7-6
#define     FCB_CHPrioKo0DMask 0xC0
#define     FCB_CHPrioKo0DShift 6
#define FCB_CHPrioKo1D                           2      // 2 Bits, Bit 5-4
#define     FCB_CHPrioKo1DMask 0x30
#define     FCB_CHPrioKo1DShift 4
#define FCB_CHPrioKo2D                           2      // 2 Bits, Bit 3-2
#define     FCB_CHPrioKo2DMask 0x0C
#define     FCB_CHPrioKo2DShift 2
#define FCB_CHPrioKo3D                           2      // 2 Bits, Bit 1-0
#define     FCB_CHPrioKo3DMask 0x03
#define     FCB_CHPrioKo3DShift 0
#define FCB_CHPrioKo4D                           3      // 2 Bits, Bit 7-6
#define     FCB_CHPrioKo4DMask 0xC0
#define     FCB_CHPrioKo4DShift 6
#define FCB_CHPrioKo5D                           3      // 2 Bits, Bit 5-4
#define     FCB_CHPrioKo5DMask 0x30
#define     FCB_CHPrioKo5DShift 4
#define FCB_CHPrioKo6D                           3      // 2 Bits, Bit 3-2
#define     FCB_CHPrioKo6DMask 0x0C
#define     FCB_CHPrioKo6DShift 2
#define FCB_CHPrioKo7D                           3      // 2 Bits, Bit 1-0
#define     FCB_CHPrioKo7DMask 0x03
#define     FCB_CHPrioKo7DShift 0
#define FCB_CHPrioKo8D                           4      // 2 Bits, Bit 7-6
#define     FCB_CHPrioKo8DMask 0xC0
#define     FCB_CHPrioKo8DShift 6
#define FCB_CHPrioOutputType                     4      // 2 Bits, Bit 5-4
#define     FCB_CHPrioOutputTypeMask 0x30
#define     FCB_CHPrioOutputTypeShift 4
#define FCB_CHPrioOutPKo0                        5      // uint8_t
#define FCB_CHPrioOutByteKo0                     5      // uint8_t
#define FCB_CHPrioOutSceneKo0                    5      // uint8_t
#define FCB_CHPrioOutPKo1                        6      // uint8_t
#define FCB_CHPrioOutByteKo1                     6      // uint8_t
#define FCB_CHPrioOutSceneKo1                    6      // uint8_t
#define FCB_CHPrioOutPKo2                        7      // uint8_t
#define FCB_CHPrioOutByteKo2                     7      // uint8_t
#define FCB_CHPrioOutSceneKo2                    7      // uint8_t
#define FCB_CHPrioOutPKo3                        8      // uint8_t
#define FCB_CHPrioOutByteKo3                     8      // uint8_t
#define FCB_CHPrioOutSceneKo3                    8      // uint8_t
#define FCB_CHPrioOutPKo4                        9      // uint8_t
#define FCB_CHPrioOutByteKo4                     9      // uint8_t
#define FCB_CHPrioOutSceneKo4                    9      // uint8_t
#define FCB_CHPrioOutPKo5                       10      // uint8_t
#define FCB_CHPrioOutByteKo5                    10      // uint8_t
#define FCB_CHPrioOutSceneKo5                   10      // uint8_t
#define FCB_CHPrioOutPKo6                       11      // uint8_t
#define FCB_CHPrioOutByteKo6                    11      // uint8_t
#define FCB_CHPrioOutSceneKo6                   11      // uint8_t
#define FCB_CHPrioOutPKo7                       12      // uint8_t
#define FCB_CHPrioOutByteKo7                    12      // uint8_t
#define FCB_CHPrioOutSceneKo7                   12      // uint8_t
#define FCB_CHPrioOutPKo8                       13      // uint8_t
#define FCB_CHPrioOutByteKo8                    13      // uint8_t
#define FCB_CHPrioOutSceneKo8                   13      // uint8_t
#define FCB_CHPrioOutPDefault                   14      // uint8_t
#define FCB_CHPrioOutByteDefault                14      // uint8_t
#define FCB_CHPrioOutSceneDefault               14      // uint8_t
#define FCB_CHPrioBehavKo0                      15      // 4 Bits, Bit 7-4
#define     FCB_CHPrioBehavKo0Mask 0xF0
#define     FCB_CHPrioBehavKo0Shift 4
#define FCB_CHPrioBehavKo1                      15      // 4 Bits, Bit 3-0
#define     FCB_CHPrioBehavKo1Mask 0x0F
#define     FCB_CHPrioBehavKo1Shift 0
#define FCB_CHPrioBehavKo2                      16      // 4 Bits, Bit 7-4
#define     FCB_CHPrioBehavKo2Mask 0xF0
#define     FCB_CHPrioBehavKo2Shift 4
#define FCB_CHPrioBehavKo3                      16      // 4 Bits, Bit 3-0
#define     FCB_CHPrioBehavKo3Mask 0x0F
#define     FCB_CHPrioBehavKo3Shift 0
#define FCB_CHPrioBehavKo4                      17      // 4 Bits, Bit 7-4
#define     FCB_CHPrioBehavKo4Mask 0xF0
#define     FCB_CHPrioBehavKo4Shift 4
#define FCB_CHPrioBehavKo5                      17      // 4 Bits, Bit 3-0
#define     FCB_CHPrioBehavKo5Mask 0x0F
#define     FCB_CHPrioBehavKo5Shift 0
#define FCB_CHPrioBehavKo6                      18      // 4 Bits, Bit 7-4
#define     FCB_CHPrioBehavKo6Mask 0xF0
#define     FCB_CHPrioBehavKo6Shift 4
#define FCB_CHPrioBehavKo7                      18      // 4 Bits, Bit 3-0
#define     FCB_CHPrioBehavKo7Mask 0x0F
#define     FCB_CHPrioBehavKo7Shift 0
#define FCB_CHPrioBehavKo8                      19      // 4 Bits, Bit 7-4
#define     FCB_CHPrioBehavKo8Mask 0xF0
#define     FCB_CHPrioBehavKo8Shift 4
#define FCB_CHPrioBehavOut                      19      // 1 Bit, Bit 3
#define     FCB_CHPrioBehavOutMask 0x08
#define     FCB_CHPrioBehavOutShift 3
#define FCB_CHAggWeight                          2      // 1 Bit, Bit 7
#define     FCB_CHAggWeightMask 0x80
#define     FCB_CHAggWeightShift 7
#define FCB_CHAggType                            2      // 7 Bits, Bit 6-0
#define     FCB_CHAggTypeMask 0x7F
#define     FCB_CHAggTypeShift 0
#define FCB_CHAggKo0D                            3      // 2 Bits, Bit 7-6
#define     FCB_CHAggKo0DMask 0xC0
#define     FCB_CHAggKo0DShift 6
#define FCB_CHAggKo1D                            3      // 2 Bits, Bit 5-4
#define     FCB_CHAggKo1DMask 0x30
#define     FCB_CHAggKo1DShift 4
#define FCB_CHAggKo2D                            3      // 2 Bits, Bit 3-2
#define     FCB_CHAggKo2DMask 0x0C
#define     FCB_CHAggKo2DShift 2
#define FCB_CHAggKo3D                            3      // 2 Bits, Bit 1-0
#define     FCB_CHAggKo3DMask 0x03
#define     FCB_CHAggKo3DShift 0
#define FCB_CHAggKo4D                            4      // 2 Bits, Bit 7-6
#define     FCB_CHAggKo4DMask 0xC0
#define     FCB_CHAggKo4DShift 6
#define FCB_CHAggKo5D                            4      // 2 Bits, Bit 5-4
#define     FCB_CHAggKo5DMask 0x30
#define     FCB_CHAggKo5DShift 4
#define FCB_CHAggKo6D                            4      // 2 Bits, Bit 3-2
#define     FCB_CHAggKo6DMask 0x0C
#define     FCB_CHAggKo6DShift 2
#define FCB_CHAggKo7D                            4      // 2 Bits, Bit 1-0
#define     FCB_CHAggKo7DMask 0x03
#define     FCB_CHAggKo7DShift 0
#define FCB_CHAggKo8D                            5      // 2 Bits, Bit 7-6
#define     FCB_CHAggKo8DMask 0xC0
#define     FCB_CHAggKo8DShift 6
#define FCB_CHAggBehavOut                        5      // 1 Bit, Bit 5
#define     FCB_CHAggBehavOutMask 0x20
#define     FCB_CHAggBehavOutShift 5
#define FCB_CHAggOutputRounding                  5      // 1 Bit, Bit 3
#define     FCB_CHAggOutputRoundingMask 0x08
#define     FCB_CHAggOutputRoundingShift 3
#define FCB_CHAggOutputOverflow                  5      // 2 Bits, Bit 2-1
#define     FCB_CHAggOutputOverflowMask 0x06
#define     FCB_CHAggOutputOverflowShift 1
#define FCB_CHAggInputDpt                        6      // 8 Bits, Bit 7-0
#define FCB_CHAggOutputDptEff                    7      // 8 Bits, Bit 7-0
#define FCB_CHAggKo0W                            8      // int8_t
#define FCB_CHAggKo1W                            9      // int8_t
#define FCB_CHAggKo2W                           10      // int8_t
#define FCB_CHAggKo3W                           11      // int8_t
#define FCB_CHAggKo4W                           12      // int8_t
#define FCB_CHAggKo5W                           13      // int8_t
#define FCB_CHAggKo6W                           14      // int8_t
#define FCB_CHAggKo7W                           15      // int8_t
#define FCB_CHAggKo8W                           16      // int8_t
#define FCB_CHCountDownTimeStartKo               2      // 4 Bits, Bit 7-4
#define     FCB_CHCountDownTimeStartKoMask 0xF0
#define     FCB_CHCountDownTimeStartKoShift 4
#define FCB_CHCountDownDelayBase                 3      // 2 Bits, Bit 7-6
#define     FCB_CHCountDownDelayBaseMask 0xC0
#define     FCB_CHCountDownDelayBaseShift 6
#define FCB_CHCountDownDelayTime                 3      // 14 Bits, Bit 13-0
#define     FCB_CHCountDownDelayTimeMask 0x3FFF
#define     FCB_CHCountDownDelayTimeShift 0
#define FCB_CHCountDownTimeOffset                5      // 4 Bits, Bit 7-4
#define     FCB_CHCountDownTimeOffsetMask 0xF0
#define     FCB_CHCountDownTimeOffsetShift 4
#define FCB_CHCountDownTrigger                   5      // 4 Bits, Bit 3-0
#define     FCB_CHCountDownTriggerMask 0x0F
#define     FCB_CHCountDownTriggerShift 0
#define FCB_CHCountDownTemplate                  6      // char*, 14 Byte
#define     FCB_CHCountDownTemplateLength 14
#define FCB_CHCountDownTemplate1h               20      // char*, 14 Byte
#define     FCB_CHCountDownTemplate1hLength 14
#define FCB_CHCountDownTemplate1m               34      // char*, 14 Byte
#define     FCB_CHCountDownTemplate1mLength 14
#define FCB_CHCountDownTemplateEnd              48      // char*, 14 Byte
#define     FCB_CHCountDownTemplateEndLength 14
#define FCB_CHCountDownTextPause                62      // char*, 1 Byte
#define     FCB_CHCountDownTextPauseLength 1
#define FCB_CHCountDownTextRun                  63      // char*, 1 Byte
#define     FCB_CHCountDownTextRunLength 1
#define FCB_CHCountDownCounterKo                64      // 4 Bits, Bit 7-4
#define     FCB_CHCountDownCounterKoMask 0xF0
#define     FCB_CHCountDownCounterKoShift 4
#define FCB_CHCountDownTextKo                   64      // 2 Bits, Bit 3-2
#define     FCB_CHCountDownTextKoMask 0x0C
#define     FCB_CHCountDownTextKoShift 2
#define FCB_CHCountDownTemplateStopp            65      // char*, 14 Byte
#define     FCB_CHCountDownTemplateStoppLength 14
#define FCB_CHCountDownMaxDelayBase             79      // 2 Bits, Bit 7-6
#define     FCB_CHCountDownMaxDelayBaseMask 0xC0
#define     FCB_CHCountDownMaxDelayBaseShift 6
#define FCB_CHCountDownMaxDelayTime             79      // 14 Bits, Bit 13-0
#define     FCB_CHCountDownMaxDelayTimeMask 0x3FFF
#define     FCB_CHCountDownMaxDelayTimeShift 0
#define FCB_CHMonitoringValueType                2      // 8 Bits, Bit 7-0
#define FCB_CHMonitoringWDEnabled                3      // 1 Bit, Bit 7
#define     FCB_CHMonitoringWDEnabledMask 0x80
#define     FCB_CHMonitoringWDEnabledShift 7
#define FCB_CHMonitoringWDTTimeoutDelayBase      4      // 2 Bits, Bit 7-6
#define     FCB_CHMonitoringWDTTimeoutDelayBaseMask 0xC0
#define     FCB_CHMonitoringWDTTimeoutDelayBaseShift 6
#define FCB_CHMonitoringWDTTimeoutDelayTime      4      // 14 Bits, Bit 13-0
#define     FCB_CHMonitoringWDTTimeoutDelayTimeMask 0x3FFF
#define     FCB_CHMonitoringWDTTimeoutDelayTimeShift 0
#define FCB_CHMonitoringWDBehavior               6      // 4 Bits, Bit 7-4
#define     FCB_CHMonitoringWDBehaviorMask 0xF0
#define     FCB_CHMonitoringWDBehaviorShift 4
#define FCB_CHMonitoringStart                    6      // 2 Bits, Bit 3-2
#define     FCB_CHMonitoringStartMask 0x0C
#define     FCB_CHMonitoringStartShift 2
#define FCB_CHMonitoringWDDpt1                   7      // 8 Bits, Bit 7-0
#define FCB_CHMonitoringWDDpt5                   7      // uint8_t
#define FCB_CHMonitoringWDDpt5001                7      // uint8_t
#define FCB_CHMonitoringWDDpt6                   7      // int8_t
#define FCB_CHMonitoringWDDpt7                   7      // uint16_t
#define FCB_CHMonitoringWDDpt8                   7      // int16_t
#define FCB_CHMonitoringWDDpt9                   7      // float (4 Byte)
#define FCB_CHMonitoringWDDpt12                  7      // uint32_t
#define FCB_CHMonitoringWDDpt13                  7      // int32_t
#define FCB_CHMonitoringWDDpt14                  7      // float (4 Byte)
#define FCB_CHMonitoringWDDpt16                  7      // char*, 14 Byte
#define     FCB_CHMonitoringWDDpt16Length 14
#define FCB_CHMonitoringMin                     22      // 4 Bits, Bit 7-4
#define     FCB_CHMonitoringMinMask 0xF0
#define     FCB_CHMonitoringMinShift 4
#define FCB_CHMonitoringMinDpt1                 23      // 1 Bit, Bit 7
#define     FCB_CHMonitoringMinDpt1Mask 0x80
#define     FCB_CHMonitoringMinDpt1Shift 7
#define FCB_CHMonitoringMinDpt5                 23      // uint8_t
#define FCB_CHMonitoringMinDpt5001              23      // uint8_t
#define FCB_CHMonitoringMinDpt6                 23      // int8_t
#define FCB_CHMonitoringMinDpt7                 23      // uint16_t
#define FCB_CHMonitoringMinDpt8                 23      // int16_t
#define FCB_CHMonitoringMinDpt9                 23      // float (4 Byte)
#define FCB_CHMonitoringMinDpt12                23      // uint32_t
#define FCB_CHMonitoringMinDpt13                23      // int32_t
#define FCB_CHMonitoringMinDpt14                23      // float (4 Byte)
#define FCB_CHMonitoringMax                     27      // 4 Bits, Bit 7-4
#define     FCB_CHMonitoringMaxMask 0xF0
#define     FCB_CHMonitoringMaxShift 4
#define FCB_CHMonitoringMaxDpt1                 28      // 1 Bit, Bit 7
#define     FCB_CHMonitoringMaxDpt1Mask 0x80
#define     FCB_CHMonitoringMaxDpt1Shift 7
#define FCB_CHMonitoringMaxDpt5                 28      // uint8_t
#define FCB_CHMonitoringMaxDpt5001              28      // uint8_t
#define FCB_CHMonitoringMaxDpt6                 28      // int8_t
#define FCB_CHMonitoringMaxDpt7                 28      // uint16_t
#define FCB_CHMonitoringMaxDpt8                 28      // int16_t
#define FCB_CHMonitoringMaxDpt9                 28      // float (4 Byte)
#define FCB_CHMonitoringMaxDpt12                28      // uint32_t
#define FCB_CHMonitoringMaxDpt13                28      // int32_t
#define FCB_CHMonitoringMaxDpt14                28      // float (4 Byte)
#define FCB_CHMonitoringOutput                  32      // 4 Bits, Bit 7-4
#define     FCB_CHMonitoringOutputMask 0xF0
#define     FCB_CHMonitoringOutputShift 4
#define FCB_CHSelectionValueType                 2      // 8 Bits, Bit 7-0
#define FCB_CHSelectionType                      3      // 8 Bits, Bit 7-0
#define FCB_CHSelectionSwitching                 4      // 4 Bits, Bit 7-4
#define     FCB_CHSelectionSwitchingMask 0xF0
#define     FCB_CHSelectionSwitchingShift 4
#define FCB_CHSelectionStateOutput               4      // 1 Bit, Bit 3
#define     FCB_CHSelectionStateOutputMask 0x08
#define     FCB_CHSelectionStateOutputShift 3
#define FCB_CHBlinkerOnDelayBase                 4      // 2 Bits, Bit 7-6
#define     FCB_CHBlinkerOnDelayBaseMask 0xC0
#define     FCB_CHBlinkerOnDelayBaseShift 6
#define FCB_CHBlinkerOnDelayTime                 4      // 14 Bits, Bit 13-0
#define     FCB_CHBlinkerOnDelayTimeMask 0x3FFF
#define     FCB_CHBlinkerOnDelayTimeShift 0
#define FCB_CHBlinkerOffDelayBase                6      // 2 Bits, Bit 7-6
#define     FCB_CHBlinkerOffDelayBaseMask 0xC0
#define     FCB_CHBlinkerOffDelayBaseShift 6
#define FCB_CHBlinkerOffDelayTime                6      // 14 Bits, Bit 13-0
#define     FCB_CHBlinkerOffDelayTimeMask 0x3FFF
#define     FCB_CHBlinkerOffDelayTimeShift 0
#define FCB_CHBlinkerStart                       8      // 4 Bits, Bit 7-4
#define     FCB_CHBlinkerStartMask 0xF0
#define     FCB_CHBlinkerStartShift 4
#define FCB_CHBlinkerStop                        8      // 4 Bits, Bit 3-0
#define     FCB_CHBlinkerStopMask 0x0F
#define     FCB_CHBlinkerStopShift 0
#define FCB_CHBlinkerBreak                       9      // 4 Bits, Bit 7-4
#define     FCB_CHBlinkerBreakMask 0xF0
#define     FCB_CHBlinkerBreakShift 4
#define FCB_CHBlinkerBreakWithoutBreak           9      // 4 Bits, Bit 7-4
#define     FCB_CHBlinkerBreakWithoutBreakMask 0xF0
#define     FCB_CHBlinkerBreakWithoutBreakShift 4
#define FCB_CHBlinkerOutputDpt                  10      // 8 Bits, Bit 7-0
#define FCB_CHBlinkerOnPercentage               11      // uint8_t
#define FCB_CHBlinkerOffPercentage              12      // uint8_t
#define FCB_CHBlinkerCount                      13      // 8 Bits, Bit 7-0
#define FCB_CHBlinkerStartAnzahl                14      // 1 Bit, Bit 7
#define     FCB_CHBlinkerStartAnzahlMask 0x80
#define     FCB_CHBlinkerStartAnzahlShift 7
#define FCB_CHFormatString                       2      // char*, 28 Byte
#define     FCB_CHFormatStringLength 28
#define FCB_CHFormatOff                         30      // char*, 14 Byte
#define     FCB_CHFormatOffLength 14
#define FCB_CHFormatOn                          44      // char*, 14 Byte
#define     FCB_CHFormatOnLength 14
#define FCB_CHFormatThousand                    58      // char*, 1 Byte
#define     FCB_CHFormatThousandLength 1
#define FCB_CHFormatIn1                         59      // 8 Bits, Bit 7-0
#define FCB_CHFormatRoundFloat1                 60      // 2 Bits, Bit 7-6
#define     FCB_CHFormatRoundFloat1Mask 0xC0
#define     FCB_CHFormatRoundFloat1Shift 6
#define FCB_CHFormatRound1                      60      // 2 Bits, Bit 7-6
#define     FCB_CHFormatRound1Mask 0xC0
#define     FCB_CHFormatRound1Shift 6
#define FCB_CHFCBFormatRound5_1                 60      // 1 Bit, Bit 5
#define     FCB_CHFCBFormatRound5_1Mask 0x20
#define     FCB_CHFCBFormatRound5_1Shift 5
#define FCB_CHFormatDecimalPlaces1              60      // 4 Bits, Bit 3-0
#define     FCB_CHFormatDecimalPlaces1Mask 0x0F
#define     FCB_CHFormatDecimalPlaces1Shift 0
#define FCB_CHFormatSignificant1                60      // 4 Bits, Bit 3-0
#define     FCB_CHFormatSignificant1Mask 0x0F
#define     FCB_CHFormatSignificant1Shift 0
#define FCB_CHFormatFillupPrecomma1             61      // 4 Bits, Bit 7-4
#define     FCB_CHFormatFillupPrecomma1Mask 0xF0
#define     FCB_CHFormatFillupPrecomma1Shift 4
#define FCB_CHFormatFillupMode1                 61      // 4 Bits, Bit 7-4
#define     FCB_CHFormatFillupMode1Mask 0xF0
#define     FCB_CHFormatFillupMode1Shift 4
#define FCB_CHFormatFillupAfterComma1           61      // 4 Bits, Bit 3-0
#define     FCB_CHFormatFillupAfterComma1Mask 0x0F
#define     FCB_CHFormatFillupAfterComma1Shift 0
#define FCB_CHFCBFormatRoundType1               62      // 4 Bits, Bit 7-4
#define     FCB_CHFCBFormatRoundType1Mask 0xF0
#define     FCB_CHFCBFormatRoundType1Shift 4
#define FCB_CHFormatFillupLength1               62      // 4 Bits, Bit 3-0
#define     FCB_CHFormatFillupLength1Mask 0x0F
#define     FCB_CHFormatFillupLength1Shift 0
#define FCB_CHFormatBit1                        60      // 8 Bits, Bit 7-0
#define FCB_CHFormatIn2                         63      // 8 Bits, Bit 7-0
#define FCB_CHFormatRoundFloat2                 64      // 2 Bits, Bit 7-6
#define     FCB_CHFormatRoundFloat2Mask 0xC0
#define     FCB_CHFormatRoundFloat2Shift 6
#define FCB_CHFormatRound2                      64      // 2 Bits, Bit 7-6
#define     FCB_CHFormatRound2Mask 0xC0
#define     FCB_CHFormatRound2Shift 6
#define FCB_CHFCBFormatRound5_2                 64      // 1 Bit, Bit 5
#define     FCB_CHFCBFormatRound5_2Mask 0x20
#define     FCB_CHFCBFormatRound5_2Shift 5
#define FCB_CHFormatDecimalPlaces2              64      // 4 Bits, Bit 3-0
#define     FCB_CHFormatDecimalPlaces2Mask 0x0F
#define     FCB_CHFormatDecimalPlaces2Shift 0
#define FCB_CHFormatSignificant2                64      // 4 Bits, Bit 3-0
#define     FCB_CHFormatSignificant2Mask 0x0F
#define     FCB_CHFormatSignificant2Shift 0
#define FCB_CHFormatFillupPrecomma2             65      // 4 Bits, Bit 7-4
#define     FCB_CHFormatFillupPrecomma2Mask 0xF0
#define     FCB_CHFormatFillupPrecomma2Shift 4
#define FCB_CHFormatFillupMode2                 65      // 4 Bits, Bit 7-4
#define     FCB_CHFormatFillupMode2Mask 0xF0
#define     FCB_CHFormatFillupMode2Shift 4
#define FCB_CHFormatFillupAfterComma2           65      // 4 Bits, Bit 3-0
#define     FCB_CHFormatFillupAfterComma2Mask 0x0F
#define     FCB_CHFormatFillupAfterComma2Shift 0
#define FCB_CHFCBFormatRoundType2               66      // 4 Bits, Bit 7-4
#define     FCB_CHFCBFormatRoundType2Mask 0xF0
#define     FCB_CHFCBFormatRoundType2Shift 4
#define FCB_CHFormatFillupLength2               66      // 4 Bits, Bit 3-0
#define     FCB_CHFormatFillupLength2Mask 0x0F
#define     FCB_CHFormatFillupLength2Shift 0
#define FCB_CHFormatBit2                        64      // 8 Bits, Bit 7-0
#define FCB_CHFormatIn3                         67      // 8 Bits, Bit 7-0
#define FCB_CHFormatRoundFloat3                 68      // 2 Bits, Bit 7-6
#define     FCB_CHFormatRoundFloat3Mask 0xC0
#define     FCB_CHFormatRoundFloat3Shift 6
#define FCB_CHFormatRound3                      68      // 2 Bits, Bit 7-6
#define     FCB_CHFormatRound3Mask 0xC0
#define     FCB_CHFormatRound3Shift 6
#define FCB_CHFCBFormatRound5_3                 68      // 1 Bit, Bit 5
#define     FCB_CHFCBFormatRound5_3Mask 0x20
#define     FCB_CHFCBFormatRound5_3Shift 5
#define FCB_CHFormatDecimalPlaces3              68      // 4 Bits, Bit 3-0
#define     FCB_CHFormatDecimalPlaces3Mask 0x0F
#define     FCB_CHFormatDecimalPlaces3Shift 0
#define FCB_CHFormatSignificant3                68      // 4 Bits, Bit 3-0
#define     FCB_CHFormatSignificant3Mask 0x0F
#define     FCB_CHFormatSignificant3Shift 0
#define FCB_CHFormatFillupPrecomma3             69      // 4 Bits, Bit 7-4
#define     FCB_CHFormatFillupPrecomma3Mask 0xF0
#define     FCB_CHFormatFillupPrecomma3Shift 4
#define FCB_CHFormatFillupMode3                 69      // 4 Bits, Bit 7-4
#define     FCB_CHFormatFillupMode3Mask 0xF0
#define     FCB_CHFormatFillupMode3Shift 4
#define FCB_CHFormatFillupAfterComma3           69      // 4 Bits, Bit 3-0
#define     FCB_CHFormatFillupAfterComma3Mask 0x0F
#define     FCB_CHFormatFillupAfterComma3Shift 0
#define FCB_CHFCBFormatRoundType3               70      // 4 Bits, Bit 7-4
#define     FCB_CHFCBFormatRoundType3Mask 0xF0
#define     FCB_CHFCBFormatRoundType3Shift 4
#define FCB_CHFormatFillupLength3               70      // 4 Bits, Bit 3-0
#define     FCB_CHFormatFillupLength3Mask 0x0F
#define     FCB_CHFormatFillupLength3Shift 0
#define FCB_CHFormatBit3                        68      // 8 Bits, Bit 7-0
#define FCB_CHFormatIn4                         71      // 8 Bits, Bit 7-0
#define FCB_CHFormatRoundFloat4                 72      // 2 Bits, Bit 7-6
#define     FCB_CHFormatRoundFloat4Mask 0xC0
#define     FCB_CHFormatRoundFloat4Shift 6
#define FCB_CHFormatRound4                      72      // 2 Bits, Bit 7-6
#define     FCB_CHFormatRound4Mask 0xC0
#define     FCB_CHFormatRound4Shift 6
#define FCB_CHFCBFormatRound5_4                 72      // 1 Bit, Bit 5
#define     FCB_CHFCBFormatRound5_4Mask 0x20
#define     FCB_CHFCBFormatRound5_4Shift 5
#define FCB_CHFormatDecimalPlaces4              72      // 4 Bits, Bit 3-0
#define     FCB_CHFormatDecimalPlaces4Mask 0x0F
#define     FCB_CHFormatDecimalPlaces4Shift 0
#define FCB_CHFormatSignificant4                72      // 4 Bits, Bit 3-0
#define     FCB_CHFormatSignificant4Mask 0x0F
#define     FCB_CHFormatSignificant4Shift 0
#define FCB_CHFormatFillupPrecomma4             73      // 4 Bits, Bit 7-4
#define     FCB_CHFormatFillupPrecomma4Mask 0xF0
#define     FCB_CHFormatFillupPrecomma4Shift 4
#define FCB_CHFormatFillupMode4                 73      // 4 Bits, Bit 7-4
#define     FCB_CHFormatFillupMode4Mask 0xF0
#define     FCB_CHFormatFillupMode4Shift 4
#define FCB_CHFormatFillupAfterComma4           73      // 4 Bits, Bit 3-0
#define     FCB_CHFormatFillupAfterComma4Mask 0x0F
#define     FCB_CHFormatFillupAfterComma4Shift 0
#define FCB_CHFCBFormatRoundType4               74      // 4 Bits, Bit 7-4
#define     FCB_CHFCBFormatRoundType4Mask 0xF0
#define     FCB_CHFCBFormatRoundType4Shift 4
#define FCB_CHFormatFillupLength4               74      // 4 Bits, Bit 3-0
#define     FCB_CHFormatFillupLength4Mask 0x0F
#define     FCB_CHFormatFillupLength4Shift 0
#define FCB_CHFormatBit4                        72      // 8 Bits, Bit 7-0

// Type
#define ParamFCB_CHChannelType                       (knx.paramByte(FCB_ParamCalcIndex(FCB_CHChannelType)))
// Kanal deaktivieren (zu Testzwecken)
#define ParamFCB_CHChannelDisabled                   ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHChannelDisabled)) & FCB_CHChannelDisabledMask))
// Eingang 1
#define ParamFCB_CHLogicKo0D                         ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo0D)) & FCB_CHLogicKo0DMask) >> FCB_CHLogicKo0DShift)
// Eingang 2
#define ParamFCB_CHLogicKo1D                         ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo1D)) & FCB_CHLogicKo1DMask) >> FCB_CHLogicKo1DShift)
// Eingang 3
#define ParamFCB_CHLogicKo2D                         ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo2D)) & FCB_CHLogicKo2DMask) >> FCB_CHLogicKo2DShift)
// Eingang 4
#define ParamFCB_CHLogicKo3D                         (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo3D)) & FCB_CHLogicKo3DMask)
// Eingang 5
#define ParamFCB_CHLogicKo4D                         ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo4D)) & FCB_CHLogicKo4DMask) >> FCB_CHLogicKo4DShift)
// Eingang 6
#define ParamFCB_CHLogicKo5D                         ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo5D)) & FCB_CHLogicKo5DMask) >> FCB_CHLogicKo5DShift)
// Eingang 7
#define ParamFCB_CHLogicKo6D                         ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo6D)) & FCB_CHLogicKo6DMask) >> FCB_CHLogicKo6DShift)
// Eingang 8
#define ParamFCB_CHLogicKo7D                         (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo7D)) & FCB_CHLogicKo7DMask)
// Eingang 9
#define ParamFCB_CHLogicKo8D                         ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo8D)) & FCB_CHLogicKo8DMask) >> FCB_CHLogicKo8DShift)
// Invertiert
#define ParamFCB_CHLogicOutInv                       ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicOutInv)) & FCB_CHLogicOutInvMask))
// Sendeverhalten
#define ParamFCB_CHLogicBehavOut                     ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicBehavOut)) & FCB_CHLogicBehavOutMask))
// Initialisierung
#define ParamFCB_CHLogicBehavKo0                     ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicBehavKo0)) & FCB_CHLogicBehavKo0Mask) >> FCB_CHLogicBehavKo0Shift)
// Initialisierung
#define ParamFCB_CHLogicBehavKo1                     (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicBehavKo1)) & FCB_CHLogicBehavKo1Mask)
// Initialisierung
#define ParamFCB_CHLogicBehavKo2                     ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicBehavKo2)) & FCB_CHLogicBehavKo2Mask) >> FCB_CHLogicBehavKo2Shift)
// Initialisierung
#define ParamFCB_CHLogicBehavKo3                     (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicBehavKo3)) & FCB_CHLogicBehavKo3Mask)
// Initialisierung
#define ParamFCB_CHLogicBehavKo4                     ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicBehavKo4)) & FCB_CHLogicBehavKo4Mask) >> FCB_CHLogicBehavKo4Shift)
// Initialisierung
#define ParamFCB_CHLogicBehavKo5                     (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicBehavKo5)) & FCB_CHLogicBehavKo5Mask)
// Initialisierung
#define ParamFCB_CHLogicBehavKo6                     ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicBehavKo6)) & FCB_CHLogicBehavKo6Mask) >> FCB_CHLogicBehavKo6Shift)
// Initialisierung
#define ParamFCB_CHLogicBehavKo7                     (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicBehavKo7)) & FCB_CHLogicBehavKo7Mask)
// Initialisierung
#define ParamFCB_CHLogicBehavKo8                     ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicBehavKo8)) & FCB_CHLogicBehavKo8Mask) >> FCB_CHLogicBehavKo8Shift)
// Prior-Wahrscheinlichkeit
#define ParamFCB_CHBayesianPrior                     (knx.paramByte(FCB_ParamCalcIndex(FCB_CHBayesianPrior)))
// Schwellwert für binären Ausgang
#define ParamFCB_CHBayesianThreshold                 (knx.paramByte(FCB_ParamCalcIndex(FCB_CHBayesianThreshold)))
// Wahrscheinlichkeits-Ausgang aktivieren
#define ParamFCB_CHBayesianEnableProbOutput          ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHBayesianEnableProbOutput)) & FCB_CHBayesianEnableProbOutputMask))
// P(A|E_1)
#define ParamFCB_CHLogicKo0BayesProbTrue             (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo0BayesProbTrue)))
// P(A|!E_1)
#define ParamFCB_CHLogicKo0BayesProbFalse            (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo0BayesProbFalse)))
// P(A|E_2)
#define ParamFCB_CHLogicKo1BayesProbTrue             (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo1BayesProbTrue)))
// P(A|!E_2)
#define ParamFCB_CHLogicKo1BayesProbFalse            (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo1BayesProbFalse)))
// P(A|E_3)
#define ParamFCB_CHLogicKo2BayesProbTrue             (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo2BayesProbTrue)))
// P(A|!E_3)
#define ParamFCB_CHLogicKo2BayesProbFalse            (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo2BayesProbFalse)))
// P(A|E_4)
#define ParamFCB_CHLogicKo3BayesProbTrue             (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo3BayesProbTrue)))
// P(A|!E_4)
#define ParamFCB_CHLogicKo3BayesProbFalse            (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo3BayesProbFalse)))
// P(A|E_5)
#define ParamFCB_CHLogicKo4BayesProbTrue             (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo4BayesProbTrue)))
// P(A|!E_5)
#define ParamFCB_CHLogicKo4BayesProbFalse            (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo4BayesProbFalse)))
// P(A|E_6)
#define ParamFCB_CHLogicKo5BayesProbTrue             (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo5BayesProbTrue)))
// P(A|!E_6)
#define ParamFCB_CHLogicKo5BayesProbFalse            (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo5BayesProbFalse)))
// P(A|E_7)
#define ParamFCB_CHLogicKo6BayesProbTrue             (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo6BayesProbTrue)))
// P(A|!E_7)
#define ParamFCB_CHLogicKo6BayesProbFalse            (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo6BayesProbFalse)))
// P(A|E_8)
#define ParamFCB_CHLogicKo7BayesProbTrue             (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo7BayesProbTrue)))
// P(A|!E_8)
#define ParamFCB_CHLogicKo7BayesProbFalse            (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo7BayesProbFalse)))
// P(A|E_9)
#define ParamFCB_CHLogicKo8BayesProbTrue             (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo8BayesProbTrue)))
// P(A|!E_9)
#define ParamFCB_CHLogicKo8BayesProbFalse            (knx.paramByte(FCB_ParamCalcIndex(FCB_CHLogicKo8BayesProbFalse)))
// Eingang 1
#define ParamFCB_CHPrioKo0D                          ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioKo0D)) & FCB_CHPrioKo0DMask) >> FCB_CHPrioKo0DShift)
// Eingang 2
#define ParamFCB_CHPrioKo1D                          ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioKo1D)) & FCB_CHPrioKo1DMask) >> FCB_CHPrioKo1DShift)
// Eingang 3
#define ParamFCB_CHPrioKo2D                          ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioKo2D)) & FCB_CHPrioKo2DMask) >> FCB_CHPrioKo2DShift)
// Eingang 4
#define ParamFCB_CHPrioKo3D                          (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioKo3D)) & FCB_CHPrioKo3DMask)
// Eingang 5
#define ParamFCB_CHPrioKo4D                          ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioKo4D)) & FCB_CHPrioKo4DMask) >> FCB_CHPrioKo4DShift)
// Eingang 6
#define ParamFCB_CHPrioKo5D                          ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioKo5D)) & FCB_CHPrioKo5DMask) >> FCB_CHPrioKo5DShift)
// Eingang 7
#define ParamFCB_CHPrioKo6D                          ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioKo6D)) & FCB_CHPrioKo6DMask) >> FCB_CHPrioKo6DShift)
// Eingang 8
#define ParamFCB_CHPrioKo7D                          (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioKo7D)) & FCB_CHPrioKo7DMask)
// Eingang 9
#define ParamFCB_CHPrioKo8D                          ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioKo8D)) & FCB_CHPrioKo8DMask) >> FCB_CHPrioKo8DShift)
// Type
#define ParamFCB_CHPrioOutputType                    ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutputType)) & FCB_CHPrioOutputTypeMask) >> FCB_CHPrioOutputTypeShift)
// Ausgangswert
#define ParamFCB_CHPrioOutPKo0                       (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutPKo0)))
// Ausgangswert
#define ParamFCB_CHPrioOutByteKo0                    (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutByteKo0)))
// Ausgangswert Szenennummer
#define ParamFCB_CHPrioOutSceneKo0                   (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutSceneKo0)))
// Ausgangswert
#define ParamFCB_CHPrioOutPKo1                       (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutPKo1)))
// Ausgangswert
#define ParamFCB_CHPrioOutByteKo1                    (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutByteKo1)))
// Ausgangswert Szenennummer
#define ParamFCB_CHPrioOutSceneKo1                   (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutSceneKo1)))
// Ausgangswert
#define ParamFCB_CHPrioOutPKo2                       (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutPKo2)))
// Ausgangswert
#define ParamFCB_CHPrioOutByteKo2                    (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutByteKo2)))
// Ausgangswert Szenennummer
#define ParamFCB_CHPrioOutSceneKo2                   (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutSceneKo2)))
// Ausgangswert
#define ParamFCB_CHPrioOutPKo3                       (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutPKo3)))
// Ausgangswert
#define ParamFCB_CHPrioOutByteKo3                    (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutByteKo3)))
// Ausgangswert Szenennummer
#define ParamFCB_CHPrioOutSceneKo3                   (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutSceneKo3)))
// Ausgangswert
#define ParamFCB_CHPrioOutPKo4                       (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutPKo4)))
// Ausgangswert
#define ParamFCB_CHPrioOutByteKo4                    (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutByteKo4)))
// Ausgangswert Szenennummer
#define ParamFCB_CHPrioOutSceneKo4                   (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutSceneKo4)))
// Ausgangswert
#define ParamFCB_CHPrioOutPKo5                       (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutPKo5)))
// Ausgangswert
#define ParamFCB_CHPrioOutByteKo5                    (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutByteKo5)))
// Ausgangswert Szenennummer
#define ParamFCB_CHPrioOutSceneKo5                   (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutSceneKo5)))
// Ausgangswert
#define ParamFCB_CHPrioOutPKo6                       (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutPKo6)))
// Ausgangswert
#define ParamFCB_CHPrioOutByteKo6                    (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutByteKo6)))
// Ausgangswert Szenennummer
#define ParamFCB_CHPrioOutSceneKo6                   (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutSceneKo6)))
// Ausgangswert
#define ParamFCB_CHPrioOutPKo7                       (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutPKo7)))
// Ausgangswert
#define ParamFCB_CHPrioOutByteKo7                    (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutByteKo7)))
// Ausgangswert Szenennummer
#define ParamFCB_CHPrioOutSceneKo7                   (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutSceneKo7)))
// Ausgangswert
#define ParamFCB_CHPrioOutPKo8                       (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutPKo8)))
// Ausgangswert
#define ParamFCB_CHPrioOutByteKo8                    (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutByteKo8)))
// Ausgangswert Szenennummer
#define ParamFCB_CHPrioOutSceneKo8                   (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutSceneKo8)))
// Ausgangswert wenn alle Eingänge AUS
#define ParamFCB_CHPrioOutPDefault                   (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutPDefault)))
// Ausgangswert wenn alle Eingänge AUS
#define ParamFCB_CHPrioOutByteDefault                (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutByteDefault)))
// Ausgangswert Szenennummer wenn alle Eingänge AUS
#define ParamFCB_CHPrioOutSceneDefault               (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioOutSceneDefault)))
// Initialisierung
#define ParamFCB_CHPrioBehavKo0                      ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioBehavKo0)) & FCB_CHPrioBehavKo0Mask) >> FCB_CHPrioBehavKo0Shift)
// Initialisierung
#define ParamFCB_CHPrioBehavKo1                      (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioBehavKo1)) & FCB_CHPrioBehavKo1Mask)
// Initialisierung
#define ParamFCB_CHPrioBehavKo2                      ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioBehavKo2)) & FCB_CHPrioBehavKo2Mask) >> FCB_CHPrioBehavKo2Shift)
// Initialisierung
#define ParamFCB_CHPrioBehavKo3                      (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioBehavKo3)) & FCB_CHPrioBehavKo3Mask)
// Initialisierung
#define ParamFCB_CHPrioBehavKo4                      ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioBehavKo4)) & FCB_CHPrioBehavKo4Mask) >> FCB_CHPrioBehavKo4Shift)
// Initialisierung
#define ParamFCB_CHPrioBehavKo5                      (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioBehavKo5)) & FCB_CHPrioBehavKo5Mask)
// Initialisierung
#define ParamFCB_CHPrioBehavKo6                      ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioBehavKo6)) & FCB_CHPrioBehavKo6Mask) >> FCB_CHPrioBehavKo6Shift)
// Initialisierung
#define ParamFCB_CHPrioBehavKo7                      (knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioBehavKo7)) & FCB_CHPrioBehavKo7Mask)
// Initialisierung
#define ParamFCB_CHPrioBehavKo8                      ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioBehavKo8)) & FCB_CHPrioBehavKo8Mask) >> FCB_CHPrioBehavKo8Shift)
// Sendeverhalten
#define ParamFCB_CHPrioBehavOut                      ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHPrioBehavOut)) & FCB_CHPrioBehavOutMask))
// Gewichtung der Eingänge
#define ParamFCB_CHAggWeight                         ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggWeight)) & FCB_CHAggWeightMask))
// Funktion
#define ParamFCB_CHAggType                           (knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggType)) & FCB_CHAggTypeMask)
// Eingang 1
#define ParamFCB_CHAggKo0D                           ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo0D)) & FCB_CHAggKo0DMask) >> FCB_CHAggKo0DShift)
// Eingang 2
#define ParamFCB_CHAggKo1D                           ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo1D)) & FCB_CHAggKo1DMask) >> FCB_CHAggKo1DShift)
// Eingang 3
#define ParamFCB_CHAggKo2D                           ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo2D)) & FCB_CHAggKo2DMask) >> FCB_CHAggKo2DShift)
// Eingang 4
#define ParamFCB_CHAggKo3D                           (knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo3D)) & FCB_CHAggKo3DMask)
// Eingang 5
#define ParamFCB_CHAggKo4D                           ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo4D)) & FCB_CHAggKo4DMask) >> FCB_CHAggKo4DShift)
// Eingang 6
#define ParamFCB_CHAggKo5D                           ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo5D)) & FCB_CHAggKo5DMask) >> FCB_CHAggKo5DShift)
// Eingang 7
#define ParamFCB_CHAggKo6D                           ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo6D)) & FCB_CHAggKo6DMask) >> FCB_CHAggKo6DShift)
// Eingang 8
#define ParamFCB_CHAggKo7D                           (knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo7D)) & FCB_CHAggKo7DMask)
// Eingang 9
#define ParamFCB_CHAggKo8D                           ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo8D)) & FCB_CHAggKo8DMask) >> FCB_CHAggKo8DShift)
// Sendeverhalten
#define ParamFCB_CHAggBehavOut                       ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggBehavOut)) & FCB_CHAggBehavOutMask))
// Rundungsmodus
#define ParamFCB_CHAggOutputRounding                 ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggOutputRounding)) & FCB_CHAggOutputRoundingMask))
// Bei Überschreiten des Wertebereichs
#define ParamFCB_CHAggOutputOverflow                 ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggOutputOverflow)) & FCB_CHAggOutputOverflowMask) >> FCB_CHAggOutputOverflowShift)
// Wertetype / DPT
#define ParamFCB_CHAggInputDpt                       (knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggInputDpt)))
// DPT Ausgang
#define ParamFCB_CHAggOutputDptEff                   (knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggOutputDptEff)))
// Gewicht Eingang 1
#define ParamFCB_CHAggKo0W                           ((int8_t)knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo0W)))
// Gewicht Eingang 2
#define ParamFCB_CHAggKo1W                           ((int8_t)knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo1W)))
// Gewicht Eingang 3
#define ParamFCB_CHAggKo2W                           ((int8_t)knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo2W)))
// Gewicht Eingang 4
#define ParamFCB_CHAggKo3W                           ((int8_t)knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo3W)))
// Gewicht Eingang 5
#define ParamFCB_CHAggKo4W                           ((int8_t)knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo4W)))
// Gewicht Eingang 6
#define ParamFCB_CHAggKo5W                           ((int8_t)knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo5W)))
// Gewicht Eingang 7
#define ParamFCB_CHAggKo6W                           ((int8_t)knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo6W)))
// Gewicht Eingang 8
#define ParamFCB_CHAggKo7W                           ((int8_t)knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo7W)))
// Gewicht Eingang 9
#define ParamFCB_CHAggKo8W                           ((int8_t)knx.paramByte(FCB_ParamCalcIndex(FCB_CHAggKo8W)))
// Start mit Zeit
#define ParamFCB_CHCountDownTimeStartKo              ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHCountDownTimeStartKo)) & FCB_CHCountDownTimeStartKoMask) >> FCB_CHCountDownTimeStartKoShift)
// Ablaufzeit Einheit
#define ParamFCB_CHCountDownDelayBase                ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHCountDownDelayBase)) & FCB_CHCountDownDelayBaseMask) >> FCB_CHCountDownDelayBaseShift)
// Ablaufzeit
#define ParamFCB_CHCountDownDelayTime                (knx.paramWord(FCB_ParamCalcIndex(FCB_CHCountDownDelayTime)) & FCB_CHCountDownDelayTimeMask)
// Ablaufzeit (in Millisekunden)
#define ParamFCB_CHCountDownDelayTimeMS              (paramDelay(knx.paramWord(FCB_ParamCalcIndex(FCB_CHCountDownDelayTime))))
// Laufzeit Verringern / Erhöhen
#define ParamFCB_CHCountDownTimeOffset               ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHCountDownTimeOffset)) & FCB_CHCountDownTimeOffsetMask) >> FCB_CHCountDownTimeOffsetShift)
// Auslöser / Ende
#define ParamFCB_CHCountDownTrigger                  (knx.paramByte(FCB_ParamCalcIndex(FCB_CHCountDownTrigger)) & FCB_CHCountDownTriggerMask)
// Standard
#define ParamFCB_CHCountDownTemplate                 (knx.paramData(FCB_ParamCalcIndex(FCB_CHCountDownTemplate)))
#define ParamFCB_CHCountDownTemplateStr              (knx.paramString(FCB_ParamCalcIndex(FCB_CHCountDownTemplate), FCB_CHCountDownTemplateLength))
// kleiner eine Stunde
#define ParamFCB_CHCountDownTemplate1h               (knx.paramData(FCB_ParamCalcIndex(FCB_CHCountDownTemplate1h)))
#define ParamFCB_CHCountDownTemplate1hStr            (knx.paramString(FCB_ParamCalcIndex(FCB_CHCountDownTemplate1h), FCB_CHCountDownTemplate1hLength))
// kleiner eine Minute
#define ParamFCB_CHCountDownTemplate1m               (knx.paramData(FCB_ParamCalcIndex(FCB_CHCountDownTemplate1m)))
#define ParamFCB_CHCountDownTemplate1mStr            (knx.paramString(FCB_ParamCalcIndex(FCB_CHCountDownTemplate1m), FCB_CHCountDownTemplate1mLength))
// Ende
#define ParamFCB_CHCountDownTemplateEnd              (knx.paramData(FCB_ParamCalcIndex(FCB_CHCountDownTemplateEnd)))
#define ParamFCB_CHCountDownTemplateEndStr           (knx.paramString(FCB_ParamCalcIndex(FCB_CHCountDownTemplateEnd), FCB_CHCountDownTemplateEndLength))
// Pause
#define ParamFCB_CHCountDownTextPause                (knx.paramData(FCB_ParamCalcIndex(FCB_CHCountDownTextPause)))
#define ParamFCB_CHCountDownTextPauseStr             (knx.paramString(FCB_ParamCalcIndex(FCB_CHCountDownTextPause), FCB_CHCountDownTextPauseLength))
// Läuft
#define ParamFCB_CHCountDownTextRun                  (knx.paramData(FCB_ParamCalcIndex(FCB_CHCountDownTextRun)))
#define ParamFCB_CHCountDownTextRunStr               (knx.paramString(FCB_ParamCalcIndex(FCB_CHCountDownTextRun), FCB_CHCountDownTextRunLength))
// Zähler
#define ParamFCB_CHCountDownCounterKo                ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHCountDownCounterKo)) & FCB_CHCountDownCounterKoMask) >> FCB_CHCountDownCounterKoShift)
// Text
#define ParamFCB_CHCountDownTextKo                   ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHCountDownTextKo)) & FCB_CHCountDownTextKoMask) >> FCB_CHCountDownTextKoShift)
// Stopp
#define ParamFCB_CHCountDownTemplateStopp            (knx.paramData(FCB_ParamCalcIndex(FCB_CHCountDownTemplateStopp)))
#define ParamFCB_CHCountDownTemplateStoppStr         (knx.paramString(FCB_ParamCalcIndex(FCB_CHCountDownTemplateStopp), FCB_CHCountDownTemplateStoppLength))
// Maximalzeit Einheit
#define ParamFCB_CHCountDownMaxDelayBase             ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHCountDownMaxDelayBase)) & FCB_CHCountDownMaxDelayBaseMask) >> FCB_CHCountDownMaxDelayBaseShift)
// Maximalzeit
#define ParamFCB_CHCountDownMaxDelayTime             (knx.paramWord(FCB_ParamCalcIndex(FCB_CHCountDownMaxDelayTime)) & FCB_CHCountDownMaxDelayTimeMask)
// Maximalzeit (in Millisekunden)
#define ParamFCB_CHCountDownMaxDelayTimeMS           (paramDelay(knx.paramWord(FCB_ParamCalcIndex(FCB_CHCountDownMaxDelayTime))))
// Werttype
#define ParamFCB_CHMonitoringValueType               (knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringValueType)))
// Zeitüberwachung aktiv
#define ParamFCB_CHMonitoringWDEnabled               ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringWDEnabled)) & FCB_CHMonitoringWDEnabledMask))
// Watchdog Zeitbasis
#define ParamFCB_CHMonitoringWDTTimeoutDelayBase     ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringWDTTimeoutDelayBase)) & FCB_CHMonitoringWDTTimeoutDelayBaseMask) >> FCB_CHMonitoringWDTTimeoutDelayBaseShift)
// Watchdog Zeit
#define ParamFCB_CHMonitoringWDTTimeoutDelayTime     (knx.paramWord(FCB_ParamCalcIndex(FCB_CHMonitoringWDTTimeoutDelayTime)) & FCB_CHMonitoringWDTTimeoutDelayTimeMask)
// Watchdog Zeit (in Millisekunden)
#define ParamFCB_CHMonitoringWDTTimeoutDelayTimeMS   (paramDelay(knx.paramWord(FCB_ParamCalcIndex(FCB_CHMonitoringWDTTimeoutDelayTime))))
// Verhalten bei Zeitüberschreitung
#define ParamFCB_CHMonitoringWDBehavior              ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringWDBehavior)) & FCB_CHMonitoringWDBehaviorMask) >> FCB_CHMonitoringWDBehaviorShift)
// Verhalten beim Start
#define ParamFCB_CHMonitoringStart                   ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringStart)) & FCB_CHMonitoringStartMask) >> FCB_CHMonitoringStartShift)
// Ersatzwert
#define ParamFCB_CHMonitoringWDDpt1                  (knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringWDDpt1)))
// Ersatzwert
#define ParamFCB_CHMonitoringWDDpt5                  (knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringWDDpt5)))
// Ersatzwert
#define ParamFCB_CHMonitoringWDDpt5001               (knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringWDDpt5001)))
// Ersatzwert
#define ParamFCB_CHMonitoringWDDpt6                  ((int8_t)knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringWDDpt6)))
// Ersatzwert
#define ParamFCB_CHMonitoringWDDpt7                  (knx.paramWord(FCB_ParamCalcIndex(FCB_CHMonitoringWDDpt7)))
// Ersatzwert
#define ParamFCB_CHMonitoringWDDpt8                  ((int16_t)knx.paramWord(FCB_ParamCalcIndex(FCB_CHMonitoringWDDpt8)))
// Ersatzwert
#define ParamFCB_CHMonitoringWDDpt9                  (knx.paramFloat(FCB_ParamCalcIndex(FCB_CHMonitoringWDDpt9), Float_Enc_IEEE754Single))
// Ersatzwert
#define ParamFCB_CHMonitoringWDDpt12                 (knx.paramInt(FCB_ParamCalcIndex(FCB_CHMonitoringWDDpt12)))
// Ersatzwert
#define ParamFCB_CHMonitoringWDDpt13                 ((int32_t)knx.paramInt(FCB_ParamCalcIndex(FCB_CHMonitoringWDDpt13)))
// Ersatzwert
#define ParamFCB_CHMonitoringWDDpt14                 (knx.paramFloat(FCB_ParamCalcIndex(FCB_CHMonitoringWDDpt14), Float_Enc_IEEE754Single))
// Ersatzwert
#define ParamFCB_CHMonitoringWDDpt16                 (knx.paramData(FCB_ParamCalcIndex(FCB_CHMonitoringWDDpt16)))
#define ParamFCB_CHMonitoringWDDpt16Str              (knx.paramString(FCB_ParamCalcIndex(FCB_CHMonitoringWDDpt16), FCB_CHMonitoringWDDpt16Length))
// Verhalten bei Wertunterschreitung
#define ParamFCB_CHMonitoringMin                     ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringMin)) & FCB_CHMonitoringMinMask) >> FCB_CHMonitoringMinShift)
// Minimaler zulässiger Wert
#define ParamFCB_CHMonitoringMinDpt1                 ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringMinDpt1)) & FCB_CHMonitoringMinDpt1Mask))
// Minimaler zulässiger Wert
#define ParamFCB_CHMonitoringMinDpt5                 (knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringMinDpt5)))
// Minimaler zulässiger Wert
#define ParamFCB_CHMonitoringMinDpt5001              (knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringMinDpt5001)))
// Minimaler zulässiger Wert
#define ParamFCB_CHMonitoringMinDpt6                 ((int8_t)knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringMinDpt6)))
// Minimaler zulässiger Wert
#define ParamFCB_CHMonitoringMinDpt7                 (knx.paramWord(FCB_ParamCalcIndex(FCB_CHMonitoringMinDpt7)))
// Minimaler zulässiger Wert
#define ParamFCB_CHMonitoringMinDpt8                 ((int16_t)knx.paramWord(FCB_ParamCalcIndex(FCB_CHMonitoringMinDpt8)))
// Minimaler zulässiger Wert
#define ParamFCB_CHMonitoringMinDpt9                 (knx.paramFloat(FCB_ParamCalcIndex(FCB_CHMonitoringMinDpt9), Float_Enc_IEEE754Single))
// Minimaler zulässiger Wert
#define ParamFCB_CHMonitoringMinDpt12                (knx.paramInt(FCB_ParamCalcIndex(FCB_CHMonitoringMinDpt12)))
// Minimaler zulässiger Wert
#define ParamFCB_CHMonitoringMinDpt13                ((int32_t)knx.paramInt(FCB_ParamCalcIndex(FCB_CHMonitoringMinDpt13)))
// Minimaler zulässiger Wert
#define ParamFCB_CHMonitoringMinDpt14                (knx.paramFloat(FCB_ParamCalcIndex(FCB_CHMonitoringMinDpt14), Float_Enc_IEEE754Single))
// Verhalten bei Wertüberschreitung
#define ParamFCB_CHMonitoringMax                     ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringMax)) & FCB_CHMonitoringMaxMask) >> FCB_CHMonitoringMaxShift)
// Maximaler zulässiger Wert
#define ParamFCB_CHMonitoringMaxDpt1                 ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringMaxDpt1)) & FCB_CHMonitoringMaxDpt1Mask))
// Maximaler zulässiger Wert
#define ParamFCB_CHMonitoringMaxDpt5                 (knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringMaxDpt5)))
// Maximaler zulässiger Wert
#define ParamFCB_CHMonitoringMaxDpt5001              (knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringMaxDpt5001)))
// Maximaler zulässiger Wert
#define ParamFCB_CHMonitoringMaxDpt6                 ((int8_t)knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringMaxDpt6)))
// Maximaler zulässiger Wert
#define ParamFCB_CHMonitoringMaxDpt7                 (knx.paramWord(FCB_ParamCalcIndex(FCB_CHMonitoringMaxDpt7)))
// Maximaler zulässiger Wert
#define ParamFCB_CHMonitoringMaxDpt8                 ((int16_t)knx.paramWord(FCB_ParamCalcIndex(FCB_CHMonitoringMaxDpt8)))
// Maximaler zulässiger Wert
#define ParamFCB_CHMonitoringMaxDpt9                 (knx.paramFloat(FCB_ParamCalcIndex(FCB_CHMonitoringMaxDpt9), Float_Enc_IEEE754Single))
// Maximaler zulässiger Wert
#define ParamFCB_CHMonitoringMaxDpt12                (knx.paramInt(FCB_ParamCalcIndex(FCB_CHMonitoringMaxDpt12)))
// Maximaler zulässiger Wert
#define ParamFCB_CHMonitoringMaxDpt13                ((int32_t)knx.paramInt(FCB_ParamCalcIndex(FCB_CHMonitoringMaxDpt13)))
// Maximaler zulässiger Wert
#define ParamFCB_CHMonitoringMaxDpt14                (knx.paramFloat(FCB_ParamCalcIndex(FCB_CHMonitoringMaxDpt14), Float_Enc_IEEE754Single))
// Sendeverhalten
#define ParamFCB_CHMonitoringOutput                  ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHMonitoringOutput)) & FCB_CHMonitoringOutputMask) >> FCB_CHMonitoringOutputShift)
// Datentype
#define ParamFCB_CHSelectionValueType                (knx.paramByte(FCB_ParamCalcIndex(FCB_CHSelectionValueType)))
// Anzahl und Typ der Auswahlen (mit gemeinsamen Auswahl-Eingang)
#define ParamFCB_CHSelectionType                     (knx.paramByte(FCB_ParamCalcIndex(FCB_CHSelectionType)))
// Bei Umschaltung
#define ParamFCB_CHSelectionSwitching                ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHSelectionSwitching)) & FCB_CHSelectionSwitchingMask) >> FCB_CHSelectionSwitchingShift)
// Auswahl Status Objekt
#define ParamFCB_CHSelectionStateOutput              ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHSelectionStateOutput)) & FCB_CHSelectionStateOutputMask))
// Blinker EIN Zeitbasis
#define ParamFCB_CHBlinkerOnDelayBase                ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHBlinkerOnDelayBase)) & FCB_CHBlinkerOnDelayBaseMask) >> FCB_CHBlinkerOnDelayBaseShift)
// Blinker EIN Zeit
#define ParamFCB_CHBlinkerOnDelayTime                (knx.paramWord(FCB_ParamCalcIndex(FCB_CHBlinkerOnDelayTime)) & FCB_CHBlinkerOnDelayTimeMask)
// Blinker EIN Zeit (in Millisekunden)
#define ParamFCB_CHBlinkerOnDelayTimeMS              (paramDelay(knx.paramWord(FCB_ParamCalcIndex(FCB_CHBlinkerOnDelayTime))))
// Blinker AUS Zeitbasis
#define ParamFCB_CHBlinkerOffDelayBase               ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHBlinkerOffDelayBase)) & FCB_CHBlinkerOffDelayBaseMask) >> FCB_CHBlinkerOffDelayBaseShift)
// Blinker AUS Zeit
#define ParamFCB_CHBlinkerOffDelayTime               (knx.paramWord(FCB_ParamCalcIndex(FCB_CHBlinkerOffDelayTime)) & FCB_CHBlinkerOffDelayTimeMask)
// Blinker AUS Zeit (in Millisekunden)
#define ParamFCB_CHBlinkerOffDelayTimeMS             (paramDelay(knx.paramWord(FCB_ParamCalcIndex(FCB_CHBlinkerOffDelayTime))))
// Start
#define ParamFCB_CHBlinkerStart                      ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHBlinkerStart)) & FCB_CHBlinkerStartMask) >> FCB_CHBlinkerStartShift)
// Ende
#define ParamFCB_CHBlinkerStop                       (knx.paramByte(FCB_ParamCalcIndex(FCB_CHBlinkerStop)) & FCB_CHBlinkerStopMask)
// AUS Telegram am 'Start' Eingang
#define ParamFCB_CHBlinkerBreak                      ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHBlinkerBreak)) & FCB_CHBlinkerBreakMask) >> FCB_CHBlinkerBreakShift)
// AUS Telegram am 'Start' Eingang
#define ParamFCB_CHBlinkerBreakWithoutBreak          ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHBlinkerBreakWithoutBreak)) & FCB_CHBlinkerBreakWithoutBreakMask) >> FCB_CHBlinkerBreakWithoutBreakShift)
// Ausgang
#define ParamFCB_CHBlinkerOutputDpt                  (knx.paramByte(FCB_ParamCalcIndex(FCB_CHBlinkerOutputDpt)))
// Wert für EIN
#define ParamFCB_CHBlinkerOnPercentage               (knx.paramByte(FCB_ParamCalcIndex(FCB_CHBlinkerOnPercentage)))
// Wert für AUS
#define ParamFCB_CHBlinkerOffPercentage              (knx.paramByte(FCB_ParamCalcIndex(FCB_CHBlinkerOffPercentage)))
// Anzahl der Blinkvorgänge
#define ParamFCB_CHBlinkerCount                      (knx.paramByte(FCB_ParamCalcIndex(FCB_CHBlinkerCount)))
// Objekt zum Starten mit Anzahl
#define ParamFCB_CHBlinkerStartAnzahl                ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHBlinkerStartAnzahl)) & FCB_CHBlinkerStartAnzahlMask))
// Format
#define ParamFCB_CHFormatString                      (knx.paramData(FCB_ParamCalcIndex(FCB_CHFormatString)))
#define ParamFCB_CHFormatStringStr                   (knx.paramString(FCB_ParamCalcIndex(FCB_CHFormatString), FCB_CHFormatStringLength))
// Textbaustein Aus
#define ParamFCB_CHFormatOff                         (knx.paramData(FCB_ParamCalcIndex(FCB_CHFormatOff)))
#define ParamFCB_CHFormatOffStr                      (knx.paramString(FCB_ParamCalcIndex(FCB_CHFormatOff), FCB_CHFormatOffLength))
// Textbaustein Ein
#define ParamFCB_CHFormatOn                          (knx.paramData(FCB_ParamCalcIndex(FCB_CHFormatOn)))
#define ParamFCB_CHFormatOnStr                       (knx.paramString(FCB_ParamCalcIndex(FCB_CHFormatOn), FCB_CHFormatOnLength))
// Tausendertrennzeichen
#define ParamFCB_CHFormatThousand                    (knx.paramData(FCB_ParamCalcIndex(FCB_CHFormatThousand)))
#define ParamFCB_CHFormatThousandStr                 (knx.paramString(FCB_ParamCalcIndex(FCB_CHFormatThousand), FCB_CHFormatThousandLength))
// Datentype
#define ParamFCB_CHFormatIn1                         (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatIn1)))
// Runden
#define ParamFCB_CHFormatRoundFloat1                 ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatRoundFloat1)) & FCB_CHFormatRoundFloat1Mask) >> FCB_CHFormatRoundFloat1Shift)
// Runden
#define ParamFCB_CHFormatRound1                      ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatRound1)) & FCB_CHFormatRound1Mask) >> FCB_CHFormatRound1Shift)
// Auf 5 Runden
#define ParamFCB_CHFCBFormatRound5_1                 ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHFCBFormatRound5_1)) & FCB_CHFCBFormatRound5_1Mask))
// Stellen
#define ParamFCB_CHFormatDecimalPlaces1              (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatDecimalPlaces1)) & FCB_CHFormatDecimalPlaces1Mask)
// Stellenanzahl
#define ParamFCB_CHFormatSignificant1                (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatSignificant1)) & FCB_CHFormatSignificant1Mask)
// Auffüllen
#define ParamFCB_CHFormatFillupPrecomma1             ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupPrecomma1)) & FCB_CHFormatFillupPrecomma1Mask) >> FCB_CHFormatFillupPrecomma1Shift)
// Auffüllen
#define ParamFCB_CHFormatFillupMode1                 ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupMode1)) & FCB_CHFormatFillupMode1Mask) >> FCB_CHFormatFillupMode1Shift)
// Auffüllen nach Komma
#define ParamFCB_CHFormatFillupAfterComma1           (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupAfterComma1)) & FCB_CHFormatFillupAfterComma1Mask)
// Rundungsart
#define ParamFCB_CHFCBFormatRoundType1               ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFCBFormatRoundType1)) & FCB_CHFCBFormatRoundType1Mask) >> FCB_CHFCBFormatRoundType1Shift)
// Stellen
#define ParamFCB_CHFormatFillupLength1               (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupLength1)) & FCB_CHFormatFillupLength1Mask)
// Anzeige als
#define ParamFCB_CHFormatBit1                        (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatBit1)))
// Datentype
#define ParamFCB_CHFormatIn2                         (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatIn2)))
// Runden
#define ParamFCB_CHFormatRoundFloat2                 ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatRoundFloat2)) & FCB_CHFormatRoundFloat2Mask) >> FCB_CHFormatRoundFloat2Shift)
// Runden
#define ParamFCB_CHFormatRound2                      ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatRound2)) & FCB_CHFormatRound2Mask) >> FCB_CHFormatRound2Shift)
// Auf 5 Runden
#define ParamFCB_CHFCBFormatRound5_2                 ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHFCBFormatRound5_2)) & FCB_CHFCBFormatRound5_2Mask))
// Stellen
#define ParamFCB_CHFormatDecimalPlaces2              (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatDecimalPlaces2)) & FCB_CHFormatDecimalPlaces2Mask)
// Stellenanzahl
#define ParamFCB_CHFormatSignificant2                (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatSignificant2)) & FCB_CHFormatSignificant2Mask)
// Auffüllen
#define ParamFCB_CHFormatFillupPrecomma2             ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupPrecomma2)) & FCB_CHFormatFillupPrecomma2Mask) >> FCB_CHFormatFillupPrecomma2Shift)
// Auffüllen
#define ParamFCB_CHFormatFillupMode2                 ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupMode2)) & FCB_CHFormatFillupMode2Mask) >> FCB_CHFormatFillupMode2Shift)
// Auffüllen nach Komma
#define ParamFCB_CHFormatFillupAfterComma2           (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupAfterComma2)) & FCB_CHFormatFillupAfterComma2Mask)
// Rundungsart
#define ParamFCB_CHFCBFormatRoundType2               ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFCBFormatRoundType2)) & FCB_CHFCBFormatRoundType2Mask) >> FCB_CHFCBFormatRoundType2Shift)
// Stellen
#define ParamFCB_CHFormatFillupLength2               (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupLength2)) & FCB_CHFormatFillupLength2Mask)
// Anzeige als
#define ParamFCB_CHFormatBit2                        (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatBit2)))
// Datentype
#define ParamFCB_CHFormatIn3                         (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatIn3)))
// Runden
#define ParamFCB_CHFormatRoundFloat3                 ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatRoundFloat3)) & FCB_CHFormatRoundFloat3Mask) >> FCB_CHFormatRoundFloat3Shift)
// Runden
#define ParamFCB_CHFormatRound3                      ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatRound3)) & FCB_CHFormatRound3Mask) >> FCB_CHFormatRound3Shift)
// Auf 5 Runden
#define ParamFCB_CHFCBFormatRound5_3                 ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHFCBFormatRound5_3)) & FCB_CHFCBFormatRound5_3Mask))
// Stellen
#define ParamFCB_CHFormatDecimalPlaces3              (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatDecimalPlaces3)) & FCB_CHFormatDecimalPlaces3Mask)
// Stellenanzahl
#define ParamFCB_CHFormatSignificant3                (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatSignificant3)) & FCB_CHFormatSignificant3Mask)
// Auffüllen
#define ParamFCB_CHFormatFillupPrecomma3             ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupPrecomma3)) & FCB_CHFormatFillupPrecomma3Mask) >> FCB_CHFormatFillupPrecomma3Shift)
// Auffüllen
#define ParamFCB_CHFormatFillupMode3                 ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupMode3)) & FCB_CHFormatFillupMode3Mask) >> FCB_CHFormatFillupMode3Shift)
// Auffüllen nach Komma
#define ParamFCB_CHFormatFillupAfterComma3           (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupAfterComma3)) & FCB_CHFormatFillupAfterComma3Mask)
// Rundungsart
#define ParamFCB_CHFCBFormatRoundType3               ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFCBFormatRoundType3)) & FCB_CHFCBFormatRoundType3Mask) >> FCB_CHFCBFormatRoundType3Shift)
// Stellen
#define ParamFCB_CHFormatFillupLength3               (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupLength3)) & FCB_CHFormatFillupLength3Mask)
// Anzeige als
#define ParamFCB_CHFormatBit3                        (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatBit3)))
// Datentype
#define ParamFCB_CHFormatIn4                         (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatIn4)))
// Runden
#define ParamFCB_CHFormatRoundFloat4                 ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatRoundFloat4)) & FCB_CHFormatRoundFloat4Mask) >> FCB_CHFormatRoundFloat4Shift)
// Runden
#define ParamFCB_CHFormatRound4                      ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatRound4)) & FCB_CHFormatRound4Mask) >> FCB_CHFormatRound4Shift)
// Auf 5 Runden
#define ParamFCB_CHFCBFormatRound5_4                 ((bool)(knx.paramByte(FCB_ParamCalcIndex(FCB_CHFCBFormatRound5_4)) & FCB_CHFCBFormatRound5_4Mask))
// Stellen
#define ParamFCB_CHFormatDecimalPlaces4              (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatDecimalPlaces4)) & FCB_CHFormatDecimalPlaces4Mask)
// Stellenanzahl
#define ParamFCB_CHFormatSignificant4                (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatSignificant4)) & FCB_CHFormatSignificant4Mask)
// Auffüllen
#define ParamFCB_CHFormatFillupPrecomma4             ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupPrecomma4)) & FCB_CHFormatFillupPrecomma4Mask) >> FCB_CHFormatFillupPrecomma4Shift)
// Auffüllen
#define ParamFCB_CHFormatFillupMode4                 ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupMode4)) & FCB_CHFormatFillupMode4Mask) >> FCB_CHFormatFillupMode4Shift)
// Auffüllen nach Komma
#define ParamFCB_CHFormatFillupAfterComma4           (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupAfterComma4)) & FCB_CHFormatFillupAfterComma4Mask)
// Rundungsart
#define ParamFCB_CHFCBFormatRoundType4               ((knx.paramByte(FCB_ParamCalcIndex(FCB_CHFCBFormatRoundType4)) & FCB_CHFCBFormatRoundType4Mask) >> FCB_CHFCBFormatRoundType4Shift)
// Stellen
#define ParamFCB_CHFormatFillupLength4               (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatFillupLength4)) & FCB_CHFormatFillupLength4Mask)
// Anzeige als
#define ParamFCB_CHFormatBit4                        (knx.paramByte(FCB_ParamCalcIndex(FCB_CHFormatBit4)))

// deprecated
#define FCB_KoOffset 250

// Communication objects per channel (multiple occurrence)
#define FCB_KoBlockOffset 250
#define FCB_KoBlockSize 10

#define FCB_KoCalcNumber(index) (index + FCB_KoBlockOffset + _channelIndex * FCB_KoBlockSize)
#define FCB_KoCalcIndex(number) ((number >= FCB_KoCalcNumber(0) && number < FCB_KoCalcNumber(FCB_KoBlockSize)) ? (number - FCB_KoBlockOffset) % FCB_KoBlockSize : -1)
#define FCB_KoCalcChannel(number) ((number >= FCB_KoBlockOffset && number < FCB_KoBlockOffset + FCB_ChannelCount * FCB_KoBlockSize) ? (number - FCB_KoBlockOffset) / FCB_KoBlockSize : -1)

#define FCB_KoCHKO0 0
#define FCB_KoCHKO1 1
#define FCB_KoCHKO2 2
#define FCB_KoCHKO3 3
#define FCB_KoCHKO4 4
#define FCB_KoCHKO5 5
#define FCB_KoCHKO6 6
#define FCB_KoCHKO7 7
#define FCB_KoCHKO8 8
#define FCB_KoCHKO9 9

// 
#define KoFCB_CHKO0                               (knx.getGroupObject(FCB_KoCalcNumber(FCB_KoCHKO0)))
// 
#define KoFCB_CHKO1                               (knx.getGroupObject(FCB_KoCalcNumber(FCB_KoCHKO1)))
// 
#define KoFCB_CHKO2                               (knx.getGroupObject(FCB_KoCalcNumber(FCB_KoCHKO2)))
// 
#define KoFCB_CHKO3                               (knx.getGroupObject(FCB_KoCalcNumber(FCB_KoCHKO3)))
// 
#define KoFCB_CHKO4                               (knx.getGroupObject(FCB_KoCalcNumber(FCB_KoCHKO4)))
// 
#define KoFCB_CHKO5                               (knx.getGroupObject(FCB_KoCalcNumber(FCB_KoCHKO5)))
// 
#define KoFCB_CHKO6                               (knx.getGroupObject(FCB_KoCalcNumber(FCB_KoCHKO6)))
// 
#define KoFCB_CHKO7                               (knx.getGroupObject(FCB_KoCalcNumber(FCB_KoCHKO7)))
// 
#define KoFCB_CHKO8                               (knx.getGroupObject(FCB_KoCalcNumber(FCB_KoCHKO8)))
// 
#define KoFCB_CHKO9                               (knx.getGroupObject(FCB_KoCalcNumber(FCB_KoCHKO9)))



// Header generation for Module 'BASE_KommentarModule'

#define BASE_KommentarModuleCount 0
#define BASE_KommentarModuleModuleParamSize 0
#define BASE_KommentarModuleSubmodulesParamSize 0
#define BASE_KommentarModuleParamSize 0
#define BASE_KommentarModuleParamOffset 9298
#define BASE_KommentarModuleCalcIndex(index, m1) (index + BASE_KommentarModuleParamOffset + _channelIndex * BASE_KommentarModuleCount * BASE_KommentarModuleParamSize + m1 * BASE_KommentarModuleParamSize)




// enumeration types
enum class PT_AIRDeviceType
{
    None = 0,
    Daikin = 1,
    Mitsubishi = 3,
    Toshiba = 4
};

enum class PT_AIRStartBehavior
{
    None = 0,
    RestoreSettings = 1
};

enum class PT_AIRLockReleaseKo
{
    None = 0,
    Release = 1,
    Lock = 2
};

enum class PT_AIRLockReleaseBehavior
{
    None = 0,
    On = 1,
    Off = 2
};

enum class PT_AIRWifiLED
{
    WLANStatus = 0,
    AlwaysOff = 1,
    AlwaysOn = 2,
    SwitchableViaGroupObject = 3
};

enum class PT_AIRExternTemperatureWatchdogBehavior
{
    Disabled = 0,
    YesReadThenIgnore = 1,
    YesIgnore = 2
};

enum class PT_AIRSceneOnOff
{
    NoChange = 255,
    Off = 0,
    On = 1
};

enum class PT_Logic
{
    AUS = 0,
    UND = 1,
    ODER = 2,
    EXOR = 3,
    TOR = 4,
    SCHALTER = 6,
    ZEITSCHALTUHR = 5
};

enum class PT_Calculate
{
    Invalid = 0,
    Valid = 1
};

enum class PT_GateTrigger
{
    None = 0,
    Off = 1,
    On = 2,
    Input = 3
};

enum class PT_LockTrigger
{
    None = 0,
    Off = 1,
    On = 2,
    Value = 3
};

enum class PT_LockResetQueue
{
    None = 0,
    ResetAfterLock = 1,
    ResetAfterUnlock = 2
};

enum class PT_InputEnable
{
    Inactive = 0,
    ActiveNormal = 1,
    ActiveInverted = 2
};

enum class PT_InputConv
{
    Wertintervall = 0,
    Differenzintervall = 1,
    Hysterese = 2,
    Differenzhysterese = 3,
    Einzelwerte = 4,
    Konstante = 5,
    Eingangswert = 6,
    Trigger = 7
};

enum class PT_LogicDpt
{
    DPT_1 = 0,
    DPT_2 = 1,
    DPT_3 = 17,
    DPT_5 = 2,
    DPT_5001 = 3,
    DPT_6 = 4,
    DPT_7 = 5,
    DPT_8 = 6,
    DPT_9 = 7,
    DPT_12 = 13,
    DPT_13 = 14,
    DPT_14 = 15,
    DPT_16 = 8,
    DPT_17 = 9,
    DPT_232 = 10
};

enum class PT_InputDefault
{
    None = 0,
    Bus = 1,
    Off = 2,
    On = 3
};

enum class PT_OnOffRepeat
{
    Verzoegerung_bleibt_bestehen = 0,
    Verzoegerung_wird_verlaengert = 1,
    Sofort_schalten_ohne_Verzoegerung = 2
};

enum class PT_OnOffReset
{
    Verzoegerung_bleibt_bestehen = 0,
    Verzoegerung_beenden_ohne_zu_schalten = 1
};

enum class PT_OutputFilter
{
    Alle_Wiederholungen_durchlassen = 0,
    Nur_EIN_Wiederholungen_durchlassen = 1,
    Nur_AUS_Wiederholungen_durchlassen = 2,
    Keine_Wiederholungen_durchlassen = 3
};

enum class PT_SendOnChange
{
    Alle_Werte_senden = 0,
    Nur_geaenderte_Werte_senden = 1
};

enum class PT_OutputSend
{
    None = 0,
    Constant = 1,
    ValueInput1 = 2,
    ValueInput2 = 3,
    OtherKo = 9,
    Function = 8,
    ReadRequest = 4,
    RestartDevice = 5,
    StatusLed = 7
};

enum class PT_YearDay
{
    Tagesschaltuhr = 0,
    Jahresschaltuhr = 1,
    Tagesschaltuhr_verbunden = 2,
    Jahresschaltuhr_verbunden = 3
};

enum class PT_Holiday
{
    Feiertage_nicht_beachten = 0,
    An_Feiertagen_nicht_schalten = 1,
    Nur_an_Feiertagen_schalten = 2,
    Feiertage_wie_Sonntage_behandeln = 3
};

enum class PT_Vacation
{
    Urlaub_nicht_beachten = 0,
    Bei_Urlaub_nicht_schalten = 1,
    Nur_bei_Urlaub_schalten = 2,
    Urlaub_wie_Sonntag_behandeln = 3
};

enum class PT_DuskDawn
{
    Inactive = 0,
    PointInTime = 1,
    Sunrise_Plus = 4,
    Sunrise_Minus = 5,
    Sunrise_Earliest = 6,
    Sunrise_Latest = 7,
    Sunrise_DegreeUp = 12,
    Sunrise_DegreeDown = 14,
    Sunset_Plus = 8,
    Sunset_Minus = 9,
    Sunset_Earliest = 10,
    Sunset_Latest = 11,
    Sunset_DegreeUp = 13,
    Sunset_DegreeDown = 15
};

enum class PT_KORelInput
{
    None = 0,
    Absolute = 1,
    Relative = 2,
    Bitmask = 3
};

enum class PT_StatusLedEffect
{
    Aus = 0,
    Ein = 1,
    Blinken = 2,
    Pulsieren = 3,
    Aufblitzen = 4
};

enum class PT_InternalInputType
{
    Anderen_Logikkanal = 0,
    Statuskanal = 1
};



#ifdef MAIN_FirmwareRevision
#ifndef FIRMWARE_REVISION
#define FIRMWARE_REVISION MAIN_FirmwareRevision
#endif
#endif
#ifdef MAIN_FirmwareName
#ifndef FIRMWARE_NAME
#define FIRMWARE_NAME MAIN_FirmwareName
#endif
#endif
