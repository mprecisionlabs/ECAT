/******************************************************************************

	SIMWAVES.H -- simulation waveform and other data.

	History:			Start with 4.0A version.
		5/18/95		Add E521 entry.		 				JFL
		8/30/95		Add CE50, like E501:  this may not be right (no spec.) JFL
		11/20/95		Fix CE50 definition:  no FRONT
		12/18/95		Fix E505 definition:  last waveform bad.
      07/24/97    Added E502B definition.
      09/18/97    Added E501B, E504B, and E509A definitions.
      09/19/97    Fix E505 definition:  increase 800V/200A delay from 18s to 24s.
      02/02/04    [aat] Added E518 definitions.
******************************************************************************/
const char *sim_waves[]=
{
"E501 ,3 1 1 1 1 6600 6600 4400 12 12 12  ,1.2/50, 8/20 Comb 2 Ohm",
"E501 ,3 1 1 1 0 6600 6600 0    12 12  0  ,1.2/50, 8/20 Comb 12 Ohm",
"E501 ,3 1 1 1 0 6600 6600 0    12 12  0  ,1.2/50, 8/20 Comb Auto 2/12 Ohm",

"CE50 ,3 0 1 1 1 6600 6600 4400 12 12 12  ,1.2/50, 8/20 Comb 2 Ohm",
"CE50 ,3 0 1 1 0 6600 6600 0    12 12  0  ,1.2/50, 8/20 Comb 12 Ohm",
"CE50 ,3 0 1 1 0 6600 6600 0    12 12  0  ,1.2/50, 8/20 Comb Auto 2/12 Ohm",

"E501A,3 1 1 1 1 6600 6600 4400 12 12 12  ,1.2/50, 8/20 Comb 2 Ohm",
"E501A,3 1 1 1 0 6600 6600 0    12 12  0  ,1.2/50, 8/20 Comb 12 Ohm",
"E501A,3 1 1 1 0 6600 6600 0    12 12  0  ,1.2/50, 8/20 Comb Auto 2/12",

"E501B,3 1 1 1 1 6600 6600 4400 12 12 12  ,1.2/50, 8/20 Comb 2 Ohm",
"E501B,3 1 1 1 0 6600 6600 0    12 12  0  ,1.2/50, 8/20 Comb 12 Ohm",
"E501B,3 1 1 1 0 6600 6600 0    12 12  0  ,1.2/50, 8/20 Comb Auto 2/12",

"E502 ,3 1 0 0 0 6600 0 0    18 0  0  ,6 kV, 0.5/700 Exponential",
"E502 ,3 1 0 0 0 6600 0 0    18 0  0  ,6 kV, 10/700 Exponential",
"E502 ,3 1 0 0 0 5500 0 0    18 0  0  ,5 kV, 100/700 Exponential",

"E502A,2 1 0 0 0 6600 0 0    18 0  0  ,6 kV, 0.5/700 Exponential",
"E502A,2 1 0 0 1 6600 0 4400 18 0  18 ,6 kV, 10/700 Exponential",

"E502B,2 1 0 0 0 7000 0 0    18 0  0  ,6 kV, 0.5/700 Exponential",
"E502B,2 1 0 0 1 7000 0 4400 18 0  18 ,6 kV, 10/700 Exponential",

"E502C,2 1 0 0 0 7000 0 0    18 0  0  ,6 kV, 0.5/700 Exponential",
"E502C,2 1 0 0 0 7000 0 0    18 0  0  ,6 kV, 10/700 Exponential",

"E502K,2 1 0 0 0 6600 0 0    6  0  0  ,6 kV, 1.2/50 Exponential",
"E502K,2 1 0 0 0 6600 0 0    18 0  0  ,6 kV, 10/700 Exponential",

"E502H,3 1 0 0 0 7350 0 0    24 0  0  ,7 kV,  0.5/700 Exponential",
"E502H,3 1 0 0 0 7350 0 0    24 0  0  ,7 kV,  10/700 Exponential",
"E502H,3 1 0 0 0 6300 0 0    24 0  0  ,6 kV, 100/700 Exponential",

"E503 ,2 1 1 1 0 6600 6600 0 9  9  0  ,100 kHz Ring Wave, 200A",
"E503 ,2 1 1 1 0 6600 6600 0 9  9  0  ,100 kHz Ring Wave, 500A",

"E503A,2 1 1 1 0 6600 6600 0 9  9  0  ,100 kHz Ring Wave, 200A",
"E503A,2 1 1 1 0 6600 6600 0 9  9  0  ,100 kHz Ring Wave, 500A",

"E504 ,2 1 1 1 0 6600 6600 0 12 12  0  ,6 kV, 1.2 us/50 us, 500A,",
"E504 ,2 1 1 1 0 6600 6600 0 12 12  0  ,6 kV, 1.2 us/50 us, 750A,",

"E504A,3 1 1 1 0 6600 6600 0 12 12  0  ,6 kV, 1.2 us/50 us 500A,",
"E504A,3 1 1 1 0 6600 6600 0 12 12  0  ,6 kV, 1.2 us/50 us 750A,",
"E504A,3 1 1 1 0 6600 6600 0 12 12  0  ,6 kV, 1.2 us/50 us 125A,",

"E504B,3 1 1 1 0 6600 6600 0 12 12  0  ,6 kV, 1.2 us/50 us 500A,",
"E504B,3 1 1 1 0 6600 6600 0 12 12  0  ,6 kV, 1.2 us/50 us 750A,",
"E504B,3 1 1 1 0 6600 6600 0 12 12  0  ,6 kV, 1.2 us/50 us 125A,",

"E505 ,4 1 0 0 0 1650 0 0    18 0  0  ,1.5 kV, 10 us/160 us, 200A",
"E505 ,4 1 0 0 0  880 0 0    18 0  0  ,800 V,  10 us/560 us, 100A",
"E505 ,4 1 0 0 0  880 0 0    24 0  0  ,800 V,  10 us/560 us, 200A",
"E505 ,4 1 1 1 0 2750 2750 0  24 24 0 ,2.5 kV, 2 us/10 us,  1kA",

"E505A,3 1 0 0 0 1500 0 0    20 0  0  ,1.5 kV, 10 us/160 us, 200A",
"E505A,3 1 0 0 0  800 0 0    20 0  0  ,800 V,  10 us/560 us, 100A",
"E505A,3 1 1 1 0 2500 2500 0  20 20 0 ,2.5 kV, 2 us/10 us,  1kA",

"E505B,3 1 1 1 0 2500 0 0    20 0  0  ,2.5 kV, 2 us/10 us,   1kA",
"E505B,3 1 0 0 0 1500 0 0    20 0  0  ,1.5 kV, 10 us/160 us, 200A",
"E505B,3 1 0 0 0  800 0 0    20 0  0  ,800 V,  10 us/560 us, 100A",

"E506 ,4 1 0 0 0  880 0     0 16 0   0  ,800V,  2 us/10 us, 100A",
"E506 ,4 1 0 0 0 1650 0     0 16 0   0  ,1.5kV, 2 us/10 us, 100A",
"E506 ,4 1 1 1 0 2750 27500 0 16 16  0  ,2.5kV, 2 us/10 us, 500A",
"E506 ,4 1 1 1 0 5500 5500  0 16 16  0  ,5.0kV, 2 us/10 us, 500A",

"506-4,4 1 0 0 0  800 0 0    16 0  0  ,800V,  2 us/10 us, 100A",
"506-4,4 1 0 0 0 1500 0 0    16 0  0  ,1.5kV, 2 us/10 us, 100A",
"506-4,4 1 0 0 0 2500 0 0    16 0  0  ,2.5kV, 2 us/10 us, 500A",
"506-4,4 1 0 0 0 5000 0 0    16 0  0  ,5.0kV, 2 us/10 us, 500A",

"E507 ,3 1 1 1 0 440 440 0   12 12  0 ,1.2/50, 8/20 Comb 2 Ohm",
"E507 ,3 1 1 1 0 440 440 0   12 12  0 ,1.2/50, 8/20 Comb 12 Ohm",
"E507 ,3 1 1 1 0 440 440 0   12 12  0 ,1.2/50, 8/20 Comb Auto 2/12",

"E508 ,2 1 0 0 1 1000 0 1000 40 0  40 ,1 kV, 10 us/360 us, 100A",
"E508 ,2 1 0 0 0 1000 0 0   130 0  0  ,1 kV, 10 us/360 us, 25A",	  //e508-12P

"E509 ,3 1 0 0 0  660 0 0    15 0  0  ,10/1000 Current 600V 100A",
"E509 ,3 1 0 0 0 1100 0 0    15 0  0  ,10/1000 Current 1000V 100A",
"E509 ,3 1 0 0 0 1650 0 0    15 0  0  ,10/1000 Current 1500V 100A",

"E509A,3 1 0 0 0  660 0 0    15 0  0  ,10/1000 Current 600V 100A",
"E509A,3 1 0 0 0 1100 0 0    15 0  0  ,10/1000 Current 1000V 100A",
"E509A,3 1 0 0 0 1650 0 0    15 0  0  ,10/1000 Current 1500V 100A",

"E510 ,3 1 1 1 1 10100 10100 4400  18 18  12 ,1.2/50, 8/20 Comb 2 Ohm",
"E510 ,3 1 1 1 0 10100 10100 0     18 18  0  ,1.2/50, 8/20 Comb 12 Ohm",
"E510 ,3 1 1 1 0 10100 10100 0     18 18  0  ,1.2/50, 8/20 Comb Auto 2/12",

"E510A,3 1 1 1 1 10100 10100 4400 12 12 12 ,1.2/50, 8/20 Comb 2 Ohm",
"E510A,3 1 1 1 0 10100 10100 0    12 12  0 ,1.2/50, 8/20 Comb 12 Ohm",
"E510A,3 1 1 1 0 10100 10100 0    12 12  0 ,1.2/50, 8/20 Comb Auto 2/12",

"E511 ,1 1 1 1 0 6600  6600 0  12 12  0  ,1.2/50 8/20 Comb,",

"E512 ,5 1 1 0 0 1100  0 0   15  0 0  ,8/20, 20A Short-Circuit",
"E512 ,5 1 1 0 0 1100  0 0   15  0 0  ,8/20, 50A Short-Circuit",
"E512 ,5 1 1 0 0 1100  0 0   15  0 0  ,8/20,100A Short-Circuit",
"E512 ,5 1 1 0 0 1100  0 0   15  0 0  ,8/20,200A Short-Circuit",
"E512 ,5 1 1 0 0 1100  0 0   15  0 0  ,8/20,500A Short-Circuit",

"E513 ,5 1 0 0 0 3000  0 0   9  0  0  , 0.1 kV / 1 usec @ 3 kV",
"E513 ,5 1 0 0 0 3000  0 0   9  0  0  , 0.5 kV / 1 usec @ 3 kV",
"E513 ,5 1 0 0 0 3000  0 0   9  0  0  , 1.0 kV / 1 usec @ 3 kV",
"E513 ,5 1 0 0 0 3000  0 0   9  0  0  , 5.0 kV / 1 usec @ 3 kV",
"E513 ,5 1 0 0 0 3000  0 0   9  0  0  ,10.0 kV / 1 usec @ 3 kV",

"E514 ,4 1 0 0 0 1650  0 0   20 0  0  ,1.5 kV, 10 us/1000us,   15A",
"E514 ,4 1 0 0 0 1650  0 0   20 0  0  ,1.5 kV,  1 kV/1us ramp, 60A",
"E514 ,4 1 0 0 0 1100  0 0   59 0  0  ,1.0 kV, 10 us/1000us,   100A",
"E514 ,4 1 0 0 0 1650  0 0   59 0  0  ,1.5kV,   1 kV/us  ramp, 250A",

"E515 ,2 1 0 0 0 2000  0 0   63 0  0  ,2.0 kV, 10 us/250 us",
"E515 ,2 1 0 0 0 4000  0 0  126 0  0  ,4.0 kV, 10 us/250 us",

"E518 ,3 1 0 0 0  660 0 0    15 0  0  ,10/1000 Current 600V 100A",
"E518 ,3 1 0 0 0 1100 0 0    15 0  0  ,10/1000 Current 1000V 100A",
"E518 ,3 1 0 0 0 2000 0 0    15 0  0  ,10/1000 Current 2000V 200A",

"E521 ,3 0 0 1 0 0 20200 0   0 60 0   ,1.2/50, 8/20 Comb 2 Ohm",
"E521 ,3 0 0 1 0 0 20200 0   0 60 0   ,1.2/50, 8/20 Comb 12 Ohm",
"E521 ,3 0 0 1 0 0 20200 0   0 60 0   ,1.2/50, 8/20 Comb Auto 2/12",

"E522 ,3 0 0 1 0 0 20200 0   0 60 0   ,1.2/50, 8/20 Comb 2 Ohm",
"E522 ,3 0 0 1 0 0 20200 0   0 60 0   ,1.2/50, 8/20 Comb 12 Ohm",
"E522 ,3 0 0 1 0 0 20200 0   0 60 0   ,1.2/50, 8/20 Comb Auto 2/12",


""};

