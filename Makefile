
INSTALL=install
INSTALL_PREFIX=/usr/local

all:
	$(MAKE) -C src

clean:
	$(MAKE) -C src clean

install:
	$(INSTALL) ipstats $(INSTALL_PREFIX)/bin/ipstats

uninstall:
	$(RM) $(INSTALL_PREFIX)/bin/ipstats

