

#define BIT_Burst_HV     0x0040
#define BIT_Hazardous_V  0x0080
#define BIT_Warmed_Up    0x1000
#define BIT_Remote       0x0100

#define BIT_L1           0x0002
#define BIT_L2           0x0004
#define BIT_L3           0x0008
#define BIT_N            0x0010
#define BIT_GND          0x0020

#define BIT_CMODEON      0x4000

#define BIT_S1           BIT_L1 
#define BIT_S2           BIT_L2 
#define BIT_S3           BIT_L3 
#define BIT_S4           BIT_N  
#define BIT_G            BIT_GND

#define BITH_S1  		  (BIT_S1 << 8)
#define BITH_S2  		  (BIT_S2 << 8)
#define BITH_S3  		  (BIT_S3 << 8)
#define BITH_S4  		  (BIT_S4 << 8)
#define BITH_G   		  (BIT_G  << 8)


#define BIT_DATACPL      0x8000

#define CPLIST_START     0x8001
#define CPLIST_END       0x8080
