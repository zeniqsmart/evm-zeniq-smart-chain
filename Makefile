.PHONY: all
all:
	$(MAKE) -C evmwrap;

.PHONY: clean
clean:
	$(MAKE) -C evmwrap clean;

.PHONY: debug
debug:
	$(MAKE) -C evmwrap -f debug.mak;

