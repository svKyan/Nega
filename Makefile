VERSION = 0.3
INSTALL_DIR = /usr/local
MANPREFIX = ${INSTALL_DIR}/share/man

nega: nega.c
	gcc -O2 -Wall -pedantic nega.c -o nega

install: nega
	mkdir -p ${INSTALL_DIR}/bin
	cp -f nega ${INSTALL_DIR}/bin
	chmod 755 ${INSTALL_DIR}/bin/nega
	mkdir -p ${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" < nega.1 > ${MANPREFIX}/man1/nega.1
	chmod 644 ${MANPREFIX}/man1/nega.1

uninstall:
	rm -f ${INSTALL_DIR}/bin/nega\
		${MANPREFIX}/man1/nega.1
