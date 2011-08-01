# print order
STATS = UPTIME CPU LOAD DISK MEMORY BATTERY TEMPERATURE NETWORK TIME
# delay
DELAY = 2

#CFLAGS = -ggdb -O0 -Wall
#CFLAGS = -Os -Wall

.PHONY:
all: systeminfo

systeminfo: systeminfo.c
	$(CC) $(CFLAGS) \
		$(addprefix -DPRINT_, $(STATS)) -DDELAY=$(DELAY) \
		-DPRINT_LABELS="$(addprefix LABEL_, $(STATS))" \
		-DPRINT_VALUES="$(addprefix VALUE_, $(STATS))" \
		-std=c99 -Wall $< -o $@
 
.PHONY:
clean:
	$(RM) systeminfo

