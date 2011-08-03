override CPU_TOTAL = $(call S,d,CPU_TOTAL,$(1))
override CPU_ACTIVE = $(call S,d,CPU_ACTIVE,$(1))
override CPU = $(call S,d,CPU,$(1))
override CPU_PERCENT = $(call S,d,CPU_PERCENT,$(1))
override LOAD_1 = $(call S,f,LOAD_1,$(1))
override LOAD_2 = $(call S,f,LOAD_2,$(1))
override LOAD_3 = $(call S,f,LOAD_3,$(1))
override ROOT_SIZE = $(call S,d,ROOT_SIZE,$(1))
override ROOT_FREE = $(call S,d,ROOT_FREE,$(1))
override ROOT_USED = $(call S,d,ROOT_USED,$(1))
override MEM_SIZE = $(call S,d,MEM_SIZE,$(1))
override MEM_FREE = $(call S,d,MEM_FREE,$(1))
override MEM_USED = $(call S,d,MEM_USED,$(1))
override BATTERY = $(call S,d,BATTERY,$(1))
override TEMPERATURE = $(call S,d,TEMPERATURE,$(1))
override NET_1_NAME = $(call S,s,NET_1_NAME,$(1))
override NET_1_DOWNSPEED = $(call S,f,NET_1_DOWNSPEED,$(1))
override NET_1_UPSPEED = $(call S,f,NET_1_UPSPEED,$(1))
override DATE_YEAR = $(call S,d,DATE_YEAR,$(1))
override DATE_MONTH = $(call S,d,DATE_MONTH,$(1))
override DATE_MONTHDAY = $(call S,d,DATE_MONTHDAY,$(1))
override DATE_WEEKDAY = $(call S,d,DATE_WEEKDAY,$(1))
override DATE_YEARDAY = $(call S,d,DATE_YEARDAY,$(1))
override DATE_HOURS = $(call S,d,DATE_HOURS,$(1))
override DATE_MINUTES = $(call S,d,DATE_MINUTES,$(1))
override DATE_SECONDS = $(call S,d,DATE_SECONDS,$(1))
override UP_HOURS = $(call S,d,UP_HOURS,$(1))
override UP_MINUTES = $(call S,d,UP_MINUTES,$(1))
override CUSTOM = $(call S,$(3),$(1),$(2))

include config_default.mk
include config.mk

override S = \") \"%$(3)$(1)%s\" IGNORE(\"
override PRINT_STR := \"%s\" IGNORE(\"$(STATUS)\") \"\"
override S = \", $(2), \"
override PRINT_ARGS := \"$(STATUS)\"
override S = \"} -DWITH_$2 -DWITH_$(firstword $(subst _, ,$(2))) $${IGNORE/\"
override FEATURES := $(FEATURES) $${IGNORE/\"$(STATUS)\"}

.PHONY:
all: systeminfo

.PHONY:
rebuild: clean systeminfo

systeminfo: systeminfo.c config.mk config_default.mk
	IGNORE="" $(CC) $(CFLAGS) \
		$(FEATURES) -DDELAY=$(DELAY) \
		-DSTATUS="$(STATUS_C)" \
		-DPRINT_STR="$(PRINT_STR)" \
		-DPRINT_ARGS="$(PRINT_ARGS)" \
		-std=c99 -Wall $< -o $@

config.mk:
	@echo "NOTE: Override default configuration with config.mk file."
	touch config.mk
 
.PHONY:
clean:
	$(RM) systeminfo

