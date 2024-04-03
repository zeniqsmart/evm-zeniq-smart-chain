OBJS = host_context.o tx_ctrl.o

all : libevmwrap.a

libevmwrap.a : host_context.o tx_ctrl.o 
	ar rvs libevmwrap.a host_context.o tx_ctrl.o ../keccak/src/keccak.o \
	   ../evmone/advanced_analysis.o \
	   ../evmone/baseline.o \
	   ../evmone/baseline_instruction_table.o \
	   ../evmone/advanced_execution.o \
	   ../evmone/instructions_storage.o \
	   ../evmone/instructions_calls.o \
	   ../evmone/advanced_instructions.o \
	   ../evmone/tracing.o \
	   ../evmone/eof.o \
	   ../evmone/vm.o \
	   ../evmc/instructions/instruction_metrics.o  \
	   ../evmc/instructions/instruction_names.o \
	   ../sha256/sha256.o ../ripemd160/ripemd160.o ../ripemd160/memzero.o
 
%.o : %.cpp
	g++ -Wall -O3 -static -std=c++20 -c -I ../evmone/include/ -I ../evmc/include/ -I ../intx/include/ -I ../keccak/include -o $@ -c  $(realpath $<)

clean:
	rm *.o *.a *.so
