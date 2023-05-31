;
;     MCC68K V4 Default Linker Command File
;
chip 68332
listmap  publics,internals,crossref
format      s
resmem      $0000,$9000                   ; Reserve stack area
sect     vars=$9010
sect     SETCS=$80000
order    vars,zerovars,ioports,heap       ; RAM sections
order    SETCS,code,literals,strings,const   ; ROM code
order    ??INITDATA                    ; ROM section for init values
extern   ENTRY
load     startup.obj
load     fixit.obj
load     csel.obj
load     lpray.obj
load     intface.obj
load     cmd_eft.obj
load     cmd_srg.obj
load     cmd_bay.obj
load     cmd_li.obj
load     cmd_root.obj
load     cmd_meas.obj
load     cmd_data.obj
load     fp_eft.obj
load     fp_main.obj
load     fp_srg.obj
load     krnl_emc.obj
load     krnl_eft.obj
load     krnl_srg.obj
load     krnl_pqf.obj
load     fp_pqf.obj
load     ad_trap.obj
load     ads_trap.obj
load     acrms.obj
load     ehv.obj
load     ecat.obj
load     tiocntl.obj
load     front.obj

; NOTE:  Libraries are loaded at the linker command line invocation

public   ????STACKTOP      = $8000
public   ????USPTOP        = $9000
public   _TPU_rami         = $FFFF00
public   _TPU_rams         = $FFFF00
public   _RSR           = $FFFA07
public   _TCR1_READ        = $FFFFEC
public   _display_control  = $052008
public   _display_data     = $05200A
public   _vectors       = $000000
public   _GPIB_INT_0       = $04C000
public   _GPIB_INT_1       = $04C002
public   _GPIB_ADDR_STAT      = $04C004
public   _GPIB_AUX         = $04C006
public   _GPIB_ADDR_REG    = $04C008
public   _GPIB_SER_POLL    = $04C00A
public   _GPIB_PAR_POLL    = $04C00C
public   _GPIB_DATA        = $04C00E
public   _PITR          = $FFFA24
public   _PICR          = $FFFA22
public   _QMCR          = $FFFC00                          
public   _QILR_QIVR        = $FFFC04
public   _QPDR          = $FFFC14
public   _QPAR_QDDR        = $FFFC16
public   _QSM_command      = $FFFD40
public   _QSM_tx           = $FFFD20
public   _QSM_rx           = $FFFD00
public   _SPCR0            = $FFFC18
public   _SPCR1            = $FFFC1A
public   _SPCR2            = $FFFC1C
public   _SPCR3            = $FFFC1E
public   _SPSR          = $FFFC1F
public   _SCDR          = $FFFC0E
public   _EEPROM           = $04E000
public   _SCCR0            = $FFFC08
public   _SCCR1            = $FFFC0A
public   _CSPDR            = $FFFA40
public   _SCSR          = $FFFC0C
public   _key_control      = $052002
public   _key_data         = $052000
public   _relays           = $058800
public   _interlocks       = $058802
public   _ac_line       = $05880A
public   _intf_sw1         = $058804
public   _intf_sw2         = $05880C
public   _DUART_MODEA      = $050000
public   _DUART_SRA        = $050002
public   _DUART_CSRA       = $050002
public   _DUART_CRA        = $050004
public   _DUART_BUFA       = $050006
public   _DUART_IPCR       = $050008
public   _DUART_ACR        = $050008
public   _DUART_ISR        = $05000A
public   _DUART_IMR        = $05000A
public   _DUART_CUR        = $05000C
public   _DUART_CTUR       = $05000C
public   _DUART_CLR        = $05000E
public   _DUART_CTLR       = $05000E
public   _DUART_MODEB      = $050010
public   _DUART_SRB        = $050012
public   _DUART_CSRB       = $050012
public   _DUART_CRB        = $050014
public   _DUART_BUFB       = $050016
public   _IVR           = $050018
public   _DUART_INP        = $05001A
public   _DUART_OPCR       = $05001A
public   _DUART_START_CNT  = $05001C
public   _DUART_STOP_CNT      = $05001E
public   _DUART_OPR_SET    = $05001C
public   _DUART_OPR_CLR    = $05001E
public   _DISABLE_DUART_LOC   = $05A000
public   _interlockData    = $05C000
public   _PEAK_LD5         = $05D000
public   _PEAK_RELAYS      = $054000
; NEW Publics for PQF software
public _AD_CH           = $0C0004
public _RMS_SUM         = $0C0008
public _PUB_RMS_SUM     = $0C000C
public _RMS_N           = $0C0010
public _PUB_RMS_N       = $0C0014
public _RMS_ZERO        = $0C0018
public _RMS_PEAKP       = $0C001C
public _RMS_PEAKM       = $0C0020
public _slope_cnt       = $0C0024
public _ADADD           = $0C0028  
public _ADCMPL          = $0C002C
public _IN_RMS_SUM         = $0C0030
public _LIN_RMS_SUM     = $0C0034
public _LIN_RMS_ZERO    = $0C0038
public _LASTM           = $0C003C
public _adtotal_cnt     = $0C0040
public _TPUCISR         = $FFFE20
public   _RAMMCR           = $FFFB00
public   _RAMBAR           = $FFFB04
public _RMSCNT          = $0C0044
public _MAXPEAK         = $0C0048
public _MINPEAK         = $0C004C
public _LIMITHIT        = $0C0050
public _VAL_LIMITHIT    = $0C0054
public _STEP_LIMIT         = $0C0058

; NEW Publics for RMS SUM  software
public _RMS_L1          = $0C0060
public _RMS_L2          = $0C0080
public _RMS_L3          = $0C00A0
public _ACTIVE_RMS         = $0C00C0
public _SYNC_RISING_EDGE   = $0C00C4
public _OFF_ADCH        = $0C00C8
public _OFF_ZERO        = $0C00CA
public _OFF_RMSN        = $0C00CC
public _OFF_RMSSUM         = $0C00CE
public _OFF_PEAKP       = $0C00D0
public _OFF_PEAKM       = $0C00D2
public _OFF_PEAKMAX     = $0C00D4
public _OFF_PEAKMIN     = $0C00D6
