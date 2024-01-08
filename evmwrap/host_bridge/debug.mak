all: libevmwrap.so

libevmwrap.so: host_context.o tx_ctrl.o 
	gcc -o libevmwrap.so -shared host_context.o tx_ctrl.o ../keccak/src/keccak.o \
	   ../evmone_v1/advanced_analysis.o \
	   ../evmone_v1/baseline.o \
	   ../evmone_v1/baseline_instruction_table.o \
	   ../evmone_v1/advanced_execution.o \
	   ../evmone_v1/instructions_storage.o \
	   ../evmone_v1/instructions_calls.o \
	   ../evmone_v1/advanced_instructions.o \
	   ../evmone_v1/tracing.o \
	   ../evmone_v1/eof.o \
	   ../evmone_v1/vm.o \
	   ../evmone_v2/advanced_analysis.o \
	   ../evmone_v2/baseline.o \
	   ../evmone_v2/baseline_instruction_table.o \
	   ../evmone_v2/advanced_execution.o \
	   ../evmone_v2/instructions_storage.o \
	   ../evmone_v2/instructions_calls.o \
	   ../evmone_v2/advanced_instructions.o \
	   ../evmone_v2/tracing.o \
	   ../evmone_v2/eof.o \
	   ../evmone_v2/vm.o \
	   ../evmc/instructions/instruction_metrics.o  \
	   ../evmc/instructions/instruction_names.o \
	   ../sha256/sha256.o ../ripemd160/ripemd160.o ../ripemd160/memzero.o -lc -lstdc++

%.o: %.cpp
	g++ -Wall -O0 -g -fPIC -std=c++20 -c -I ../evmone_v1/include/  -I ../evmone_v2/include/ -I ../evmc/include/ -I ../intx/include/ -I ../keccak/include -o $@ -c $<

