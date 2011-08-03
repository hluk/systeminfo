# status message format
STATUS = UP:$(UP_HOURS):$(call UP_MINUTES,02)  CPU:$(CPU)  LOAD:$(call LOAD_1,2.2) $(call LOAD_2,2.2) $(call LOAD_3,2.2)  MEM:$(MEM_USED)MiB  ROOT:$(ROOT_FREE)/$(ROOT_SIZE)GiB  BAT:$(BATTERY)%  TEMP:$(TEMPERATURE)°  $(NET_1_NAME):$(call NET_1_DOWNSPEED,.2) $(call NET_1_UPSPEED,.2)  $(DATE_MONTHDAY)/$(DATE_MONTH) $(call DATE_HOURS,02):$(call DATE_MINUTES,02)\n
# update every second
DELAY = 1

#CFLAGS = -ggdb -O0 -Wall
#CFLAGS = -Os -Wall

