/* Configuration for WMFS (http://wmfs.info/projects/wmfs) */
#ifndef DELAY
#define DELAY 4
#endif

extern int cpu_iowait;
#ifdef USLEEP
#define N (60000000)
#else
#define N (60)
#endif
#define L1 (N/DELAY)
#define L5 (N*5/DELAY)
#define L15 (N*15/DELAY)
static void iowaits(){
    static int p[] = {[0 ... L15-1] = 0};
    static int last = -1;
    static int w1 = 0, w5 = 0, w15 = 0;
    static int i = 0;

    int r = (last >= 0) ? (cpu_iowait-last) : 0;
    last = cpu_iowait;
    
    w1  = w1  + r - p[i<L1 ? (L15+i-L1) : (i-L1)];
    w5  = w5  + r - p[i<L5 ? (L15+i-L5) : (i-L5)];
    w15 = w15 + r - p[i];
    p[i] = r;
    i = (i+1)%L15;

    printf("%.2f %.2f %.2f", (double)w1/L1, (double)w5/L5, (double)w15/L15);
}

#define COL_WARN "\\#ff8866\\"
#define COL_NORM "\\#aaaaaa\\"
#define COL_TAG  "\\#55aaff\\"

STATUS
COL_TAG "  UP:" COL_NORM NUM(UP_HOURS) ":" NUM(UP_MINUTES)
COL_TAG "  CPU:" STR(CPU<0.5 ? COL_NORM : COL_WARN) FLOAT(100*CPU) "%"
COL_TAG "  LOAD:" COL_NORM FLOAT(LOAD_1) " " FLOAT(LOAD_2) " " FLOAT(LOAD_3)
COL_TAG "  IO:" COL_NORM DO(iowaits())
COL_TAG "  MEM:" COL_NORM NUM(MEM_USED) "MiB"
IF(SWAP_USED > 0)
    COL_TAG "  SWAP:\\#" C(02x,255*SWAP_USED/SWAP_SIZE) "aaaa\\" NUM(SWAP_USED) "MiB"
ENDIF
COL_TAG "  ROOT:" STR(ROOT_FREE>8 ? COL_NORM : COL_WARN) NUM(ROOT_FREE) "/" NUM(ROOT_SIZE) "GiB"
IF(BATTERY < 95)
    COL_TAG "  BAT:" STR(BATTERY>10 ? COL_NORM : COL_WARN) NUM(BATTERY) "%"
ENDIF
COL_TAG "  TEMP:" STR(TEMPERATURE<60 ? COL_NORM : COL_WARN) C(d,TEMPERATURE) "°"
COL_TAG "  " C(s,NET_1_NAME) ":" COL_NORM FLOAT(NET_1_DOWNSPEED) " " FLOAT(NET_1_UPSPEED)
COL_TAG "  " NUM(DATE_MONTHDAY) "/" NUM(DATE_MONTH) " " COL_NORM " " C(02d,DATE_HOURS) ":" C(02d,DATE_MINUTES)
" \n"

