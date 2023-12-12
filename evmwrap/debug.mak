SUBDIRS = evmone sha256 ripemd160 keccak/src evmc/instructions host_bridge

.PHONY: all
all:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
		echo "Clean in $$subdir";\
		$(MAKE) -C $$subdir -f debug.mak;\
	done

