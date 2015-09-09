# dwmstatus - updates the status page
# See LICENSE file for copyright and license details.

include config.mk

SRC	= $(wildcard src/*.c)
MOD_SRC = ${MODULES:%=src/modules/%.c}
OBJECTS	= ${SRC:src/%.c=obj/%.o} ${MOD_SRC:src/modules/%.c=obj/%.o}

all: options ${BINARY}

options: config.mk
	@echo build options:
	@echo "MODULES  = ${MODULES}"
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

obj/%.o: src/%.c
	${CC} -c ${CFLAGS} -o $@ $<

obj/%.o: src/modules/%.c
	${CC} -c ${CFLAGS} -o $@ $<

${OBJECTS}: config.h modules.gen.h

${BINARY}: ${OBJECTS}
	${CC} -o $@ ${OBJECTS} ${LDFLAGS}

config.h:
	@echo creating $@ from config.def.h
	cp config.def.h $@

modules.gen.h: Makefile generate_modules_file.sh
	./generate_modules_file.sh ${MODULES} >| modules.gen.h

Makefile: config.mk obj/

obj/:
	mkdir obj

config.mk:
	cp config.def.mk $@

clean:
	${RM} ${BINARY} ${OBJECTS} modules.gen.h

.PHONY: all options clean

