.PHONY: all clean install uninstall

OUT_DIR = Release
SRC_FILES = $(wildcard *.cpp)
OBJECTS = $(addsuffix .o,$(basename $(SRC_FILES)))
PATH:=$(PATH):$(LINARO_PATH)/bin
CC=arm-linux-gnueabihf-gcc.exe
CFLAGS=-I./include
LIB_FLAGS=-Llib -lwiringPi -lwiringPiDev

all: prep DispKeyb
	
clean:
	rm -rf $(OUT_DIR)/DispKeyb bin/*.o

prep:
#	-mkdir bin

%.o: %.cpp
	$(CC) -c -o $(OUT_DIR)/$@ $(addsuffix .cpp,$(basename $@)) $(CFLAGS)

DispKeyb: $(OBJECTS)
	arm-linux-gnueabihf-gcc.exe -o $(OUT_DIR)/DispKeyb $(addprefix $(OUT_DIR)/,$(OBJECTS)) $(LIB_FLAGS)