### VARIABLES ###

UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
	CC = aarch64-linux-musl-cc
	RUN = docker run -it --rm -v .:/home alpine /home/
else
	CC = gcc 
	RUN = ./
endif

CFLAGS = -DSTM32F446xx -D_XOPEN_SOURCE -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast
INCLUDES = 	-I $(INC_DIR)/bms/ \
			-I $(INC_DIR)/bms/timebase \
			-I $(INC_DIR)/bms/errors \
			-I $(INC_DIR)/bms/drivers \
			-I $(INC_DIR)/common/ \
			-I $(INC_DIR) \
			-I $(MICRO_LIB_DIR)/bms-monitor/inc \
			-I $(LIB_DIR)/can/lib/bms/ \
			-I ../Drivers/STM32F4xx_HAL_Driver/Inc \
			-I ../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy \
			-I ../Drivers/CMSIS/Device/ST/STM32F4xx/Include \
			-I ../Drivers/CMSIS/Include \
			-I $(MICRO_LIB_DIR)/Unity/src/ \
			-I $(MICRO_LIB_DIR)/CMock/src/ \
			-I $(MICRO_LIB_DIR)/blinky/inc \
			-I $(MICRO_LIB_DIR)/ring-buffer/inc \
			-I $(MICRO_LIB_DIR)/min-heap/inc \
			-I $(MICRO_LIB_DIR)/bms-monitor/inc \
			-I $(LIB_DIR)/can/lib \
			-I $(LIB_DIR)/can/bms \
			-I $(LIB_DIR)/can/lib/primary \
			-I mocks

INC_DIR = ../Core/Inc
LIB_DIR = ../Core/Lib
MICRO_LIB_DIR = $(LIB_DIR)/micro-libs

BIN_DIR = bin

BMS_DIR = ../Core/Src/bms
BMS_SRCS = $(shell find $(BMS_DIR) -name "*.c")
BMS_TARGETS = $(patsubst $(BMS_DIR)/%.c, %, $(shell find $(BMS_DIR) -name "*.c"))

CMOCK = $(MICRO_LIB_DIR)/CMock/lib/cmock.rb
MOCKS_CONF = conf.yaml
MOCKS_DST_DIR = mocks
MOCKS_INC_DIR = $(INC_DIR)
MOCKS_TARGETS = $(patsubst $(MOCKS_INC_DIR)/%.h, Mock%, $(wildcard $(MOCKS_INC_DIR)/*.h))

LIB_INCLUDES = $(foreach lib, $(LIBS_TARGETS), $(addprefix -I, $(MICRO_LIB_DIR)/$(lib)/inc)) \
			   $(foreach lib, $(LIBS_TARGETS), $(addprefix -I, $(MICRO_LIB_DIR)/$(lib)/src))
LIB_SRCS = $(foreach lib, $(LIBS_TARGETS), $(shell find $(MICRO_LIB_DIR)/$(lib)/src -name "*.c"))

PRIMARY_OBJS = $(patsubst $(LIB_DIR)/can/lib/primary/%.c, $(BIN_DIR)/%.o, $(shell find $(LIB_DIR)/can/lib/primary -name "*.c"))

LIBS_TARGETS = 	blinky \
		   		ring-buffer \
			    min-heap \
			    unity \
			    cmock \
			    bms_network \
			    canlib_device \

TARGETS = $(notdir $(BMS_TARGETS) $(MOCKS_TARGETS) $(LIBS_TARGETS))
TARGETS_OBJS = $(addprefix $(BIN_DIR)/, $(addsuffix .o, $(TARGETS))) $(PRIMARY_OBJS)

TESTS = $(patsubst test_%.c, test_%, $(wildcard test_*.c))

default: $(TESTS)

### FOLDERS CREATION ###

$(MOCKS_DST_DIR):
	mkdir $(MOCKS_DST_DIR)

$(BIN_DIR):
	mkdir $(BIN_DIR)

### MOCKS GENERATION ###

.PRECIOUS: $(MOCKS_DST_DIR)/Mock%.c
$(MOCKS_DST_DIR)/Mock%.c: $(MOCKS_INC_DIR)/%.h 
	ruby $(CMOCK) -o$(MOCKS_CONF) $<

### MOCKS COMPILATION ###

.PRECIOUS: $(BIN_DIR)/Mock%.o
$(BIN_DIR)/Mock%.o: $(MOCKS_DST_DIR)/Mock%.c $(BIN_DIR)/cmock.o | $(BIN_DIR)
	$(CC) -c $< -o $@ $(INCLUDES) -I$(MOCKS_DST_DIR) $(CFLAGS)

### BMS COMPILATION ###

.PRECIOUS: $(BIN_DIR)/%.o
$(BIN_DIR)/%.o: $(BMS_DIR)/%.c | $(BIN_DIR)
	$(CC) -c $< -o $@ $(INCLUDES) $(CFLAGS)

$(BIN_DIR)/%.o: $(BMS_DIR)/drivers/%.c | $(BIN_DIR)
	$(CC) -c $< -o $@ $(INCLUDES) $(CFLAGS)

$(BIN_DIR)/%.o: $(BMS_DIR)/timebase/%.c | $(BIN_DIR)
	$(CC) -c $< -o $@ $(INCLUDES) $(CFLAGS)

$(BIN_DIR)/%.o: $(BMS_DIR)/errors/%.c | $(BIN_DIR)
	$(CC) -c $< -o $@ $(INCLUDES) $(CFLAGS)

### LIBS COMPILATION ###

$(BIN_DIR)/unity.o: | $(BIN_DIR)
	$(CC) -c $(MICRO_LIB_DIR)/Unity/src/unity.c $(INCLUDES) -o $(BIN_DIR)/unity.o

$(BIN_DIR)/cmock.o: | $(BIN_DIR)
	$(CC) -c $(MICRO_LIB_DIR)/CMock/src/cmock.c $(INCLUDES) -o $(BIN_DIR)/cmock.o

$(BIN_DIR)/blinky.o: | $(BIN_DIR)
	$(CC) -c $(MICRO_LIB_DIR)/blinky/src/blinky.c $(INCLUDES) -o $(BIN_DIR)/blinky.o

$(BIN_DIR)/ring-buffer.o: | $(BIN_DIR)
	$(CC) -c $(MICRO_LIB_DIR)/ring-buffer/src/ring-buffer.c $(INCLUDES) -o $(BIN_DIR)/ring-buffer.o

$(BIN_DIR)/min-heap.o:| $(BIN_DIR)
	$(CC) -c $(MICRO_LIB_DIR)/min-heap/src/min-heap.c $(INCLUDES) -o $(BIN_DIR)/min-heap.o

$(BIN_DIR)/bms_network.o: | $(BIN_DIR)
	$(CC) -c $(LIB_DIR)/can/lib/bms/bms_network.c $(INCLUDES) -o $(BIN_DIR)/bms_network.o
	
$(BIN_DIR)/canlib_device.o: | $(BIN_DIR)
	$(CC) -c $(LIB_DIR)/can/lib/canlib_device.c $(INCLUDES) -o $(BIN_DIR)/canlib_device.o

.PRECIOUS: $(BIN_DIR)/primary_%.o
$(BIN_DIR)/primary_%.o: $(LIB_DIR)/can/lib/primary/primary_%.c | $(BIN_DIR)
	$(CC) -c $< -o $@ $(INCLUDES) $(CFLAGS)

### TEST COMPILATION ###

test_%: test_%.c $(TARGETS_OBJS) | $(BIN_DIR)
	$(CC) $< $(CFLAGS) $(TARGETS_OBJS) -o $@ $(INCLUDES)

### RUN ###

run_%: test_%
	$(RUN)$<

run_all: $(TESTS)
	$(foreach test, $(TESTS), $(RUN)$(test);)

### CLEAN ###

clean:
	rm -rf $(BIN_DIR) $(MOCKS_DST_DIR) $(TESTS)

