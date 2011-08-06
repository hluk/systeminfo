ifdef DELAY
	override CFLAGS += -DDELAY='$(DELAY)'
endif
ifdef USLEEP
	override CFLAGS += -DUSLEEP='$(USLEEP)'
endif
ifdef ALARM
	override CFLAGS += -DALARM='$(ALARM)'
endif
ifdef STATUS
	override CFLAGS += -DSTATUS='$(STATUS)'
	override CPPFLAGS += -DSTATUS='$(STATUS)'
.PHONY: features.h
endif

.PHONY:
all: systeminfo

.PHONY:
rebuild: clean systeminfo

systeminfo: systeminfo.c config.h features.h
	$(CC) $(CFLAGS) -std=c99 -Wall $< -o $@
	test -z '$(STATUS)' || $(RM) features.h

features.h: config.h
	(echo '#define IGNORE(x)'; \
		echo -e \
		'#ifndef STATUS\n' \
		'#include "$<"\n' \
		'#endif\n' \
		'#define C(t,x)   )x IGNORE(\n' \
		'#define NUM(x)   C(d,x)\n' \
		'#define FLOAT(x) C(g,x)\n' \
		'#define STR(x)   C(s,x)\n' \
		'#define CHAR(x)  C(c,x)\n' \
		'\nFEATURES: IGNORE(STATUS)' | cpp $(CPPFLAGS) \
		) | cpp $(CPPFLAGS) | \
		sed -nr '/^FEATURES: /{s/\S*:/#define FEATURES/;s/[^A-Z]*((([A-Z]+)[A-Z_]*[0-9]?)[A-Z_]*)\S*/\n#define WITH_\3\n#define WITH_\2\n#define WITH_\1/g;p;q}' | \
		sort -u > $@

config.h:
	cp config.def.h $@

.PHONY:
clean:
	$(RM) features.h systeminfo

