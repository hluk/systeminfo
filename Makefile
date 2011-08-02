override S = \");printf(\"%$(3)$(1)\",$(2));printf(\"%s\",\"
override UP_HOURS = $(call S,d,hours,$(1))
override UP_MINUTES = $(call S,d,minutes,$(1))
override CPU = $(call S,d,cpu,$(1))
override LOAD1 = $(call S,f,s.loads[0]/LOADS_SCALE,$(1))
override LOAD2 = $(call S,f,s.loads[1]/LOADS_SCALE,$(1))
override LOAD3 = $(call S,f,s.loads[2]/LOADS_SCALE,$(1))
override ROOT_SIZE = $(call S,d,(int)((fs.f_blocks*fs.f_bsize) >> 30),$(1))
override ROOT_FREE = $(call S,d,(int)((fs.f_bavail*fs.f_bsize) >> 30),$(1))
override ROOT_USED = $(call S,d,(int)(((fs.f_blocks-fs.f_bavail)*fs.f_bsize) >> 30),$(1))
override MEM_SIZE = $(call S,d,(int)(s.totalram>>20),$(1))
override MEM_FREE = $(call S,d,(int)(s.freeram>>20),$(1))
override MEM_USED = $(call S,d,(int)((s.totalram - s.freeram)>>20),$(1))
override BATTERY = $(call S,d,100*bat/bat_full,$(1))
override TEMPERATURE = $(call S,d,temp/1000,$(1))
override ETH1_NAME = $(call S,s,eth_name,$(1))
override ETH1_DOWNSPEED = $(call S,f,downspeed,$(1))
override ETH1_UPSPEED = $(call S,f,upspeed,$(1))
override DATE_YEAR = $(call S,d,t->tm_year,$(1))
override DATE_MONTH = $(call S,d,t->tm_mon,$(1))
override DATE_MONTHDAY = $(call S,d,t->tm_mday,$(1))
override DATE_WEEKDAY = $(call S,d,t->tm_wday,$(1))
override DATE_YEARDAY = $(call S,d,t->tm_yday,$(1))
override DATE_HOURS = $(call S,d,t->tm_hour,$(1))
override DATE_MINUTES = $(call S,d,t->tm_min,$(1))
override DATE_SECONDS = $(call S,d,t->tm_sec,$(1))
override CUSTOM = $(call S,$(3),$(1),$(2))

include config_default.mk
include config.mk

STATUS_C = printf(\"%s\",\"$(STATUS)\\n\")

.PHONY:
all: systeminfo

systeminfo: systeminfo.c config.mk config_default.mk
	$(CC) $(CFLAGS) \
		$(addprefix -DPRINT_, $(FEATURES)) -DDELAY=$(DELAY) \
		-DSTATUS="$(STATUS_C)" \
		-std=c99 -Wall $< -o $@

config.mk:
	@echo "NOTE: Override default configuration with config.mk file."
	touch config.mk
 
.PHONY:
clean:
	$(RM) systeminfo

