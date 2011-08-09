#ifndef DELAY
#define DELAY 1
#endif

#ifndef STATUS
#define STATUS \
"  UP:"   NUM(UP_HOURS) ":" NUM(UP_MINUTES) \
"  CPU:"  NUM(CPU_PERCENT) "%" \
"  LOAD:" FLOAT(LOAD_1) " " FLOAT(LOAD_2) " " FLOAT(LOAD_3) \
"  MEM:"  NUM(MEM_FREE) "MiB" \
"  SWAP:" NUM(SWAP_FREE) "MiB" \
"  ROOT:" NUM(ROOT_FREE) "/" NUM(ROOT_SIZE) "GiB" \
IF(BATTERY < 50) \
    "  BAT:"  NUM(BATTERY) "%" \
ENDIF \
"  TEMP:" C(d,TEMPERATURE) "°" \
"  " C(s,NET_1_NAME) ":" FLOAT(NET_1_DOWNSPEED) " " FLOAT(NET_1_UPSPEED) \
"  " NUM(DATE_MONTHDAY) "/" NUM(DATE_MONTH) "  " C(02d,DATE_HOURS) ":" C(02d,DATE_MINUTES) \
" \r"
#endif

