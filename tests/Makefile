# Detect all assembly files in the tests directory.
ASSEMBLY_FILES := $(wildcard *.s)
CLANG_BIN_FILES := $(patsubst %.s, %-clang.bin, $(ASSEMBLY_FILES))
OBJ_FILES := $(patsubst %.s, %.o, $(ASSEMBLY_FILES))
BIN_FILES := $(patsubst %.s, %.bin, $(ASSEMBLY_FILES))

.PHONY: all clean test

all: $(CLANG_BIN_FILES) $(BIN_FILES)

test: all
	@for file in $(ASSEMBLY_FILES); do \
		echo "Comparing $$file output..."; \
		cmp $$(basename $$file .s)-clang.bin $$(basename $$file .s).bin; \
	done

%-clang.bin: %.o
	llvm-objcopy -O binary $< $@

%.o: %.s
	clang --target=riscv32-unknown-elf -O0 -march=rv32i -c $< -o $@

%.bin: %.s
	../build/asmkheiv $< -o $@

clean:
	rm -f $(BIN_FILES) $(OBJ_FILES)
