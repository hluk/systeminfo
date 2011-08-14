status(
"  UP:",   NUM(UP_HOURS), ":", NUM(UP_MINUTES),
"  CPU:",  NUM(CPU_PERCENT), "%",
"  LOAD:", FLOAT(LOAD_1), " ", FLOAT(LOAD_2), " ", FLOAT(LOAD_3),
"  MEM:",  NUM(MEM_FREE), "MiB",
"  SWAP:", NUM(SWAP_FREE), "MiB",
"  ROOT:", NUM(ROOT_FREE), "/", NUM(ROOT_SIZE), "GiB",

BATTERY < 50 and
{"  BAT:", NUM(BATTERY), "%"},

"  TEMP:", TEMPERATURE, "°",
"  ", C('s',NET_1_NAME), ":", FLOAT(NET_1_DOWNSPEED), " ", FLOAT(NET_1_UPSPEED),
"  ", NUM(DATE_MONTHDAY), "/", NUM(DATE_MONTH), "  ", C('02d',DATE_HOURS), ":", C('02d',DATE_MINUTES),
" \r"
)

