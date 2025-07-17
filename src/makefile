ifeq ($(OS), Windows_NT)
	#suppress warning 4127 (constant in condition)
	CC := cl
	CFLAGS := /Z7 /W4 /std:c11 /wd 4127

	PLATFORM_FILES := rjd.c
	PLATFORM_CFLAGS := /D_CRT_SECURE_NO_WARNINGS /DWINVER=_WIN32_WINNT_WIN10 /D_WIN32_WINNT=_WIN32_WINNT_WIN10
	PLATFORM_LFLAGS := /link "kernel32.lib" "user32.lib"
	OUTPUT_FILE := /OUT:gen.exe
else
	#SHELL_NAME := $(shell uname -s)
	#ifeq($(SHELL_NAME), Darwin) #OSX
	#endif
	#ifeq($(SHELL_NAME), Linux) #Linux
	#endif

	CC := clang
	CFLAGS := --std=c11 -pedantic -Wall -Wextra -g -march=native -Wno-unused-local-typedefs -Wno-missing-braces
	PLATFORM_FILES := rjd.m
	PLATFORM_CFLAGS := -fsanitize=undefined -fsanitize=address  
	PLATFORM_LFLAGS := -framework Foundation -framework AppKit
	OUTPUT_FILE := -o gen
endif


all:
	@# -Wno-unused-local-typedefs to suppress locally defined typedefs coming from RJD_STATIC_ASSERT
	$(CC) $(CFLAGS) $(PLATFORM_CFLAGS) main.c $(PLATFORM_FILES) $(PLATFORM_LFLAGS) $(OUTPUT_FILE)

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
	rm -r test

test:
	mkdir test
	./$(OUTPUT_FILE) ../markdown test
