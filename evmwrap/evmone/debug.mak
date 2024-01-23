OBJS = \
   advanced_analysis.o \
   baseline.o \
   baseline_instruction_table.o \
   advanced_execution.o \
   instructions_storage.o \
   instructions_calls.o \
   advanced_instructions.o \
   tracing.o \
   eof.o \
   vm.o \
   cpu_check.o

all: ${OBJS}

%.o: %.cpp
	g++ -O0 -g -fPIC -std=c++20 -c -DEVMONE_X86_64_ARCH_LEVEL=2 -DPROJECT_VERSION='"11.0.0"' -I ./include -I ../evmc/include/ -I ../intx/include/ -I ../keccak/include -o $@ -c $<


