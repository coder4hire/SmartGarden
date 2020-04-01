.PHONY: all clean install uninstall

OUT_DIR = Release
SRC_FILES = $(wildcard *.cpp)
OBJECTS = $(addsuffix .o,$(basename $(SRC_FILES)))
PATH:=$(PATH):$(LINARO_PATH)/bin
CC=arm-linux-gnueabihf-gcc.exe
LD=arm-linux-gnueabihf-g++.exe
CFLAGS=-I./include
LIB_FLAGS=-Llib -LLinuxLibs -lwiringPi -lwiringPiDev -lpthread -lasound

all: prep DispKeyb
	
clean:
	erase /Q $(OUT_DIR)\*.*

prep:
#	-mkdir bin

%.o: %.cpp
	$(CC) -c -o $(OUT_DIR)/$@ $(addsuffix .cpp,$(basename $@)) $(CFLAGS)

DispKeyb: $(OBJECTS)
	$(LD) -o $(OUT_DIR)/DispKeyb $(addprefix $(OUT_DIR)/,$(OBJECTS)) $(LIB_FLAGS)