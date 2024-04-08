# Object files to create for the executable
OBJS = obj/MainWindow.o obj/AboutDialog.o obj/Resource.o obj/WinMain.o obj/Config.o obj/Audio.o

# Warnings to be raised by the C compiler
WARNS = -Wall

CC = /usr/bin/i686-w64-mingw32-gcc
RC = /usr/bin/i686-w64-mingw32-windres

EXE = tr.exe

# Compiler flags. Compile ANSI build only if CHARSET=ANSI.
ifeq (${CHARSET}, ANSI)
  CFLAGS = -O2 -D _WIN32_IE=0x0500 -D _USER32_ -D WINVER=0x0500 ${WARNS} \
  -I /home/ubuntu/x86_64/include -Iinclude -Wno-incompatible-pointer-types -Wno-unused-variable
else
  CFLAGS = -O2 -D UNICODE -D _UNICODE -D _USER32_ -D _WIN32_IE=0x0500 -D WINVER=0x0500 ${WARNS} \
  -I /home/ubuntu/x86_64/include -Iinclude -Wno-incompatible-pointer-types -Wno-unused-variable
endif

# LDFLAGS = -L /home/ubuntu/x86_64/lib -lsox -lwinmm -fopenmp -static
LDFLAGS = -L /home/ubuntu/x86_64/lib -s -lsox -lcomctl32 -lole32 -luser32 -lgdi32 -luuid \
  -Wl,--subsystem,windows -static -luuid -lwinmm -fopenmp

.PHONY: all clean

# Build the executable by default
all: bin/$(EXE)

# Create build output directories if they don't exist
bin obj:
	if ! [ -d bin ]; then mkdir bin ; fi
	if ! [ -d obj ]; then mkdir obj ; fi

SRC = $(wildcard src/*.c)

# Compile object files for executable
obj/%.o: src/%.c | obj
	$(CC) $(CFLAGS) -c "$<" -o "$@"

# Build the resources
obj/Resource.o: res/Resource.rc res/Application.manifest res/Pictogrammers-Material-Microphone-variant.ico include/Resource.h | obj
	$(RC) -I./include -I./res -i "$<" -o "$@"

# Build the executable
bin/$(EXE): $(OBJS) | bin
	/usr/bin/printf "Now building %s\n" $(EXE)
	$(CC) -o "$@" $(OBJS) $(LDFLAGS)

clean:
	/usr/bin/printf "Now removing %s\n" $(EXE)
	if [ -f bin/$(EXE) ]; then rm bin/$(EXE) ; fi
	/usr/bin/printf "Now removing %s\n" $(OBJS)
	if [ -f obj/AboutDialog.o ]; then rm $(OBJS) ; fi


obj/AboutDialog.o: include/AboutDialog.h include/Globals.h include/Resource.h
obj/MainWindow.o:  include/AboutDialog.h include/Globals.h include/MainWindow.h include/Resource.h
obj/WinMain.o:     include/Globals.h include/MainWindow.h include/Resource.h
obj/Config.o:      include/Config.h include/Globals.h
