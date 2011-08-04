override CPU_TOTAL = $(call S,d,CPU_TOTAL,$1)
override CPU_ACTIVE = $(call S,d,CPU_ACTIVE,$1)
override CPU = $(call S,d,CPU,$1)
override CPU_PERCENT = $(call S,d,CPU_PERCENT,$1)
override LOAD_1 = $(call S,f,LOAD_1,$1)
override LOAD_2 = $(call S,f,LOAD_2,$1)
override LOAD_3 = $(call S,f,LOAD_3,$1)
override ROOT_SIZE = $(call S,d,ROOT_SIZE,$1)
override ROOT_FREE = $(call S,d,ROOT_FREE,$1)
override ROOT_USED = $(call S,d,ROOT_USED,$1)
override MEM_SIZE = $(call S,d,MEM_SIZE,$1)
override MEM_FREE = $(call S,d,MEM_FREE,$1)
override MEM_USED = $(call S,d,MEM_USED,$1)
override BATTERY = $(call S,d,BATTERY,$1)
override TEMPERATURE = $(call S,d,TEMPERATURE,$1)
override NET_1_NAME = $(call S,s,NET_1_NAME,$1)
override NET_1_DOWNSPEED = $(call S,f,NET_1_DOWNSPEED,$1)
override NET_1_UPSPEED = $(call S,f,NET_1_UPSPEED,$1)
override NET_DOWNSPEED = $(call S,f,NET_DOWNSPEED,$2,($1))
override NET_UPSPEED = $(call S,f,NET_UPSPEED,$2,($1))
override DATE_YEAR = $(call S,d,DATE_YEAR,$1)
override DATE_MONTH = $(call S,d,DATE_MONTH,$1)
override DATE_MONTHDAY = $(call S,d,DATE_MONTHDAY,$1)
override DATE_WEEKDAY = $(call S,d,DATE_WEEKDAY,$1)
override DATE_YEARDAY = $(call S,d,DATE_YEARDAY,$1)
override DATE_HOURS = $(call S,d,DATE_HOURS,$1)
override DATE_MINUTES = $(call S,d,DATE_MINUTES,$1)
override DATE_SECONDS = $(call S,d,DATE_SECONDS,$1)
override UP_HOURS = $(call S,d,UP_HOURS,$1)
override UP_MINUTES = $(call S,d,UP_MINUTES,$1)
override C = $(call S,$2,$1,$3)

include config_default.mk
include config.mk

# replace quotes in STATUS with C macros for safer manipulation
override STATS = $(subst \0,\",$(subst \\,\" BACKSLASH \",$(subst $Q,\" QUOTE \",$(STATUS))))
override Q = "
# " # this should corrects syntax highlighting if it is messed with above line

override S = \0) \0%$3$1%s\0 IGNORE(\0
override PRINT_STR := \"%s\" IGNORE(\"$(STATS)\") \"\"
override S = \0, $(subst $Q,\0,$2)$4, \0
override PRINT_ARGS := \"$(STATS)\"
override S = \0} -DWITH_$(shell echo "$2"|grep -o '^[A-Z]*') -DWITH_$(shell echo "$2"|grep -o '^[A-Z_0-9]*') $${IGNORE/\0
override FEATURES := $(FEATURES) $${IGNORE/\"$(STATS)\"}

ifeq ($(USLEEP),1)
	override FEATURES := $(FEATURES) -DUSLEEP=1
endif

.PHONY:
all: systeminfo

.PHONY:
rebuild: clean systeminfo

systeminfo: systeminfo.c config.mk config_default.mk
	IGNORE="" $(CC) $(CFLAGS) \
		$(FEATURES) -DDELAY=$(DELAY) \
		-DPRINT_STR="$(PRINT_STR)" \
		-DPRINT_ARGS="$(PRINT_ARGS)" \
		-std=c99 -Wall $< -o $@

config.mk:
	@echo "NOTE: Override default configuration with config.mk file."
	touch config.mk
 
.PHONY:
clean:
	$(RM) systeminfo

