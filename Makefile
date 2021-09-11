include config.mk

SRC = tty2midi.c
OBJ = ${SRC:.c=.o}

all: tty2midi

.c.o:
	${CC} -c ${CFLAGS} $<

${OBJ}:

tty2midi: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin/
	cp -f pcmprint $(DESTDIR)${PREFIX}/bin/
	chmod 755 ${DESTDIR}${PREFIX}/bin/pcmprint
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	cp -f pcmprint.1 ${DESTDIR}${MANPREFIX}/man1/pcmprint.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/pcmprint.1

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/pcmprint\
		${DESTDIR}${MANDIR}/pcmprint.1

clean:
	rm -f tty2midi ${OBJ}

.PHONY: all clean instal uninstall
