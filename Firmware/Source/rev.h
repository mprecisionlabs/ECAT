#define  REV_MAJOR   "5"
#define  REV_MINOR   "33"

#ifdef   BETA_RELEASE
#define  REV_BETA    "a     \n\r    <internal use only - do not ship>"
#else
#define  REV_BETA    ""
#endif

#ifndef  HACK_400Hz  /* For 400 Hz hack, display "400 Hz" instead of "Pre-release" */
#define  REV_400Hz   "\n\r                          \n\r"
#else
#define  REV_400Hz   "\n\r                          Special 400 Hz\n\r"
#endif

#define  REV_MENU    "ECAT Control Center    v%s.%s%s%s", REV_MAJOR, REV_MINOR, REV_BETA, REV_400Hz

