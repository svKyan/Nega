INSTALL_DIR = /usr/local

nega: nega.c
	gcc -O2 -Wall -pedantic nega.c -o nega

install: nega
	mkdir -p ${INSTALL_DIR}/bin
	cp -f nega ${INSTALL_DIR}/bin
	chmod 755 ${INSTALL_DIR}/bin/nega
	rm nega
