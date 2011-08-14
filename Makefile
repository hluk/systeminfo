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
	override CFLAGS += -DXSETROOT='$(XSETROOT)'
	override LDFLAGS += -lX11
endif
ifdef STATUS
	override CFLAGS += -DSTATUS='$(STATUS)'
	override CPPFLAGS += -DSTATUS='$(STATUS)'
	CLEAN_FEATURES = 1
.PHONY: features.h
endif
ifdef CONFIG
	CLEAN_FEATURES = 1
.PHONY: features.h
else
	CONFIG = config.h
endif

OUTFILE = systeminfo

.PHONY:
all: $(OUTFILE)

.PHONY:
rebuild: clean systeminfo

$(OUTFILE): systeminfo.c $(CONFIG) features.h
	if [ -n '$(CONFIG)' ]; then \
		cp $(CONFIG) config.current.h; \
	else \
		> config.current.h; \
	fi
	$(CC) $(CFLAGS) $(LDFLAGS) -std=c99 -Wall $< -o $@
	[ -z '$(CLEAN_FEATURES)' ] || $(RM) features.h

features.h: $(CONFIG)
	(echo '#define IGNORE(x)'; \
		echo -e \
		'#define C(t,x)   )x IGNORE(""\n' \
		'#define IF(x)    C(,x)\n' \
		'#define ELIF(x)  C(,x)\n' \
		'#define ELSE(x)  C(,x)\n' \
		'#define WHILE(x) C(,x)\n' \
		'#define FOR(x)   C(,x)\n' \
		'#define NUM(x)   C(,x)\n' \
		'#define FLOAT(x) C(,x)\n' \
		'#define STR(x)   C(,x)\n' \
		'#define CHAR(x)  C(,x)\n' \
		'#define DO(x)\n' \
		'#define BEGIN FEATURES: IGNORE(""\n' \
		'#ifdef STATUS\n' \
		'BEGIN STATUS\n' \
		'#else\n' \
		'#define STATUS BEGIN\n' \
		'#include "$<"\n' \
		'#endif\n)' | cpp $(CPPFLAGS) \
		) | cpp $(CPPFLAGS) | \
		sed -nr '/^\s*FEATURES:/,$${s/[^A-Z]*((([A-Z]+)[A-Z_]*[0-9]?)[A-Z_]*)\S*/\n#define WITH_\3\n#define WITH_\2\n#define WITH_\1/g;p;}' | \
		sort -u > $@

config.h:
	cp config.def.h $@

.PHONY:
clean:
	$(RM) features.h systeminfo

