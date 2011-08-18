PKGCONFIG = pkg-config
CONFIG = config.h
OUTFILE = systeminfo
FEATURES = BATTERY CPU DATE LOAD MEM NET ROOT SWAP TEMPERATURE UP

ifdef PRESET
	override CONFIG = config.$(PRESET).h
	override OUTFILE = systeminfo_$(PRESET)
	include $(wildcard $(PRESET).mk)
endif
ifdef DELAY
	override CFLAGS += -DDELAY='$(DELAY)'
endif
ifdef USLEEP
	override CFLAGS += -DUSLEEP='$(USLEEP)'
endif
ifdef ALARM
	override CFLAGS += -DALARM='$(ALARM)'
endif
ifdef XSETROOT
	override CFLAGS += -DXSETROOT='$(XSETROOT)' $(shell $(PKGCONFIG) --cflags x11)
	override LDFLAGS += $(shell $(PKGCONFIG) --libs x11)
endif
ifdef LUA
	override CFLAGS += -DLUA='"$(LUA)"' $(shell $(PKGCONFIG) --cflags lua)
	override LDFLAGS += $(shell $(PKGCONFIG) --libs lua)
endif
ifdef STATUS
	override CFLAGS += -DSTATUS='$(STATUS)'
	override CPPFLAGS += -DSTATUS='$(STATUS)'
endif

override CFLAGS += $(addprefix -DWITH_,$(FEATURES)) -DCONFIG='"$(CONFIG)"'

.PHONY:
all: $(OUTFILE)

.PHONY:
rebuild: clean $(OUTFILE)

$(OUTFILE): systeminfo.c $(CONFIG)
	$(CC) -std=c99 -Wall $< -o $@ $(CFLAGS) $(LDFLAGS)

config.h:
	cp config.def.h $@

.PHONY:
clean:
	$(RM) $(OUTFILE)

