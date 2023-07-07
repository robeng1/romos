FILES = ./build/kernel/kernel.asm.o ./build/kernel/kernel.o ./build/isr80h/isr80h.o ./build/isr80h/process.o ./build/isr80h/memory.o ./build/isr80h/io.o ./build/disk/disk.o ./build/disk/stream.o ./build/task/process.o ./build/task/task.o ./build/task/task.asm.o ./build/task/tss.asm.o ./build/fs/parser.o ./build/fs/file.o ./build/fs/fat/fat16.o ./build/string/string.o ./build/idt/idt.asm.o ./build/idt/idt.o ./build/mm/memory.o ./build/io/io.asm.o ./build/gdt/gdt.o ./build/gdt/gdt.asm.o ./build/mm/heap/heap.o ./build/mm/heap/kernel_heap.o ./build/mm/paging/paging.o ./build/mm/paging/paging.asm.o
INCLUDES = -I./src
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: ./bin/boot.bin ./bin/kernel.bin
	rm -rf ./bin/os.bin 
	dd if=./bin/boot.bin >> ./bin/os.bin
	dd if=./bin/kernel.bin >> ./bin/os.bin
	dd if=/dev/zero bs=1048576 count=16 >> ./bin/os.bin

./bin/kernel.bin: $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o  ./build/romos.o
	i686-elf-gcc  -T ./src/linker.ld -o ./bin/kernel.bin  -ffreestanding -O0 -nostdlib ./build/romos.o

./bin/boot.bin: ./src/boot/boot.asm
	nasm -f bin ./src/boot/boot.asm -o ./bin/boot.bin

./build/kernel/kernel.asm.o: ./src/kernel/kernel.asm
	nasm -f elf -g ./src/kernel/kernel.asm -o ./build/kernel/kernel.asm.o

./build/kernel/kernel.o: ./src/kernel/kernel.c
	i686-elf-gcc $(INCLUDES) -I./src/kernel $(FLAGS) -std=gnu99 -c ./src/kernel/kernel.c -o ./build/kernel/kernel.o

./build/idt/idt.asm.o: ./src/idt/idt.asm
	nasm -f elf -g ./src/idt/idt.asm -o ./build/idt/idt.asm.o

./build/idt/idt.o: ./src/idt/idt.c
	i686-elf-gcc $(INCLUDES) -I./src/idt $(FLAGS) -std=gnu99 -c ./src/idt/idt.c -o ./build/idt/idt.o

./build/gdt/gdt.o: ./src/gdt/gdt.c
	i686-elf-gcc $(INCLUDES) -I./src/gdt $(FLAGS) -std=gnu99 -c ./src/gdt/gdt.c -o ./build/gdt/gdt.o

./build/gdt/gdt.asm.o: ./src/gdt/gdt.asm
	nasm -f elf -g ./src/gdt/gdt.asm -o ./build/gdt/gdt.asm.o

./build/isr80h/isr80h.o: ./src/isr80h/isr80h.c
	i686-elf-gcc $(INCLUDES) -I./src/isr80h $(FLAGS) -std=gnu99 -c ./src/isr80h/isr80h.c -o ./build/isr80h/isr80h.o

./build/isr80h/memory.o: ./src/isr80h/memory.c
	i686-elf-gcc $(INCLUDES) -I./src/isr80h $(FLAGS) -std=gnu99 -c ./src/isr80h/memory.c -o ./build/isr80h/memory.o


./build/isr80h/io.o: ./src/isr80h/io.c
	i686-elf-gcc $(INCLUDES) -I./src/isr80h $(FLAGS) -std=gnu99 -c ./src/isr80h/io.c -o ./build/isr80h/io.o

./build/isr80h/process.o: ./src/isr80h/process.c
	i686-elf-gcc $(INCLUDES) -I./src/isr80h $(FLAGS) -std=gnu99 -c ./src/isr80h/process.c -o ./build/isr80h/process.o


./build/idt/idt.o: ./src/idt/idt.c
	i686-elf-gcc $(INCLUDES) -I./src/idt $(FLAGS) -std=gnu99 -c ./src/idt/idt.c -o ./build/idt/idt.o

./build/idt/idt.asm.o: ./src/idt/idt.asm
	nasm -f elf -g ./src/idt/idt.asm -o ./build/idt/idt.asm.o

./build/mm/memory.o: ./src/mm/memory.c
	i686-elf-gcc $(INCLUDES) -I./src/mm $(FLAGS) -std=gnu99 -c ./src/mm/memory.c -o ./build/mm/memory.o

./build/task/process.o: ./src/task/process.c
	i686-elf-gcc $(INCLUDES) -I./src/task $(FLAGS) -std=gnu99 -c ./src/task/process.c -o ./build/task/process.o

./build/task/task.o: ./src/task/task.c
	i686-elf-gcc $(INCLUDES) -I./src/task $(FLAGS) -std=gnu99 -c ./src/task/task.c -o ./build/task/task.o

./build/task/task.asm.o: ./src/task/task.asm
	nasm -f elf -g ./src/task/task.asm -o ./build/task/task.asm.o

./build/task/tss.asm.o: ./src/task/tss.asm
	nasm -f elf -g ./src/task/tss.asm -o ./build/task/tss.asm.o

./build/io/io.asm.o: ./src/io/io.asm
	nasm -f elf -g ./src/io/io.asm -o ./build/io/io.asm.o

./build/mm/heap/heap.o: ./src/mm/heap/heap.c
	i686-elf-gcc $(INCLUDES) -I./src/mm/heap $(FLAGS) -std=gnu99 -c ./src/mm/heap/heap.c -o ./build/mm/heap/heap.o

./build/mm/heap/kernel_heap.o: ./src/mm/heap/kernel_heap.c
	i686-elf-gcc $(INCLUDES) -I./src/mm/heap $(FLAGS) -std=gnu99 -c ./src/mm/heap/kernel_heap.c -o ./build/mm/heap/kernel_heap.o

./build/mm/paging/paging.o: ./src/mm/paging/paging.c
	i686-elf-gcc $(INCLUDES) -I./src/mm/paging $(FLAGS) -std=gnu99 -c ./src/mm/paging/paging.c -o ./build/mm/paging/paging.o

./build/mm/paging/paging.asm.o: ./src/mm/paging/paging.asm
	nasm -f elf -g ./src/mm/paging/paging.asm -o ./build/mm/paging/paging.asm.o

./build/disk/disk.o: ./src/disk/disk.c
	i686-elf-gcc $(INCLUDES) -I./src/disk $(FLAGS) -std=gnu99 -c ./src/disk/disk.c -o ./build/disk/disk.o

./build/disk/stream.o: ./src/disk/stream.c
	i686-elf-gcc $(INCLUDES) -I./src/disk $(FLAGS) -std=gnu99 -c ./src/disk/stream.c -o ./build/disk/stream.o

./build/fs/fat/fat16.o: ./src/fs/fat/fat16.c
	i686-elf-gcc $(INCLUDES) -I./src/fs -I./src/fat $(FLAGS) -std=gnu99 -c ./src/fs/fat/fat16.c -o ./build/fs/fat/fat16.o

./build/fs/file.o: ./src/fs/file.c
	i686-elf-gcc $(INCLUDES) -I./src/fs $(FLAGS) -std=gnu99 -c ./src/fs/file.c -o ./build/fs/file.o

./build/fs/parser.o: ./src/fs/parser.c
	i686-elf-gcc $(INCLUDES) -I./src/fs $(FLAGS) -std=gnu99 -c ./src/fs/parser.c -o ./build/fs/parser.o

./build/string/string.o: ./src/string/string.c
	i686-elf-gcc $(INCLUDES) -I./src/string $(FLAGS) -std=gnu99 -c ./src/string/string.c -o ./build/string/string.o


clean:
	rm  -rf ./bin/boot.bin
	rm  -rf ./bin/kernel.bin
	rm  -rf ./bin/os.bin
	rm  -rf ${FILES}
	rm  -rf ./build/kernel.o
	rm  -rf ./build/romos.o
	