ifeq ($(OS), Windows_NT)
	PLATFORM_FILES := rjd.c
	PLATFORM_CFLAGS := -D WINVER=_WIN32_WINNT_WIN10 -D _WIN32_WINNT=_WIN32_WINNT_WIN10 -D WDK_NTDDI_VERSION=NTDDI_WIN10_RS1
	PLATFORM_LFLAGS :=
	OUTPUT_FILE := gen.exe
else
	#SHELL_NAME := $(shell uname -s)
	#ifeq($(SHELL_NAME), Darwin) #OSX
	#endif
	#ifeq($(SHELL_NAME), Linux) #Linux
	#endif

	PLATFORM_FILES := rjd.m
	PLATFORM_CFLAGS := -fsanitize=undefined -fsanitize=address  
	PLATFORM_LFLAGS := -framework Foundation -framework AppKit -framework Metal -framework MetalKit

	OUTPUT_FILE := gen
endif

CFLAGS := --std=c11 -pedantic -Wall -Wextra -g -march=native -Wno-unused-local-typedefs -Wno-missing-braces

all:
	@# -Wno-unused-local-typedefs to suppress locally defined typedefs coming from RJD_STATIC_ASSERT
	gcc $(CFLAGS) $(PLATFORM_CFLAGS) main.c $(PLATFORM_FILES) -o $(OUTPUT_FILE) $(PLATFORM_LFLAGS) 

tags:
	ctags -f tags *

clean:
	rm *.exe*
	rm *.ilk
	rm *.obj
	rm *.pdb
	rm -r Debug
	rm *.stackdump
	rm main
	rm -r main.dSYM

test:
	./$(OUTPUT_FILE)
