.PHONY: all clean install uninstall

OUT_DIR = Release
SRC_FILES = $(wildcard *.cpp)
OBJECTS = $(addsuffix .o,$(basename $(SRC_FILES)))
PATH:=$(PATH):$(LINARO_PATH)/bin
CC=arm-linux-gnueabihf-gcc.exe
LD=arm-linux-gnueabihf-g++.exe
CFLAGS=-I./include -I../ESP32CamClient/ -std=c++11
LIB_FLAGS=-Llib -lpthread

all: prep CameraServer
	
clean:
	erase /Q $(OUT_DIR)\*.*

prep:
#	-mkdir bin

%.o: %.cpp
	$(CC) -c -o $(OUT_DIR)/$@ $(addsuffix .cpp,$(basename $@)) $(CFLAGS)

CameraServer: $(OBJECTS)
	$(LD) -o $(OUT_DIR)/CameraServer $(addprefix $(OUT_DIR)/,$(OBJECTS)) $(LIB_FLAGS)