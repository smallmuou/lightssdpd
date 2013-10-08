#copyright(C) 2013 eVideo. All rights reversed.
#author xuwf

include Makefile.cfg

all:
	$(MAKE) -C src all
	$(MAKE) -C sample all

install:
	$(MAKE) -C src install

uninstall:
	$(MAKE) -C src uninstall

clean:
	$(MAKE) -C src clean
	$(MAKE) -C sample clean
	

