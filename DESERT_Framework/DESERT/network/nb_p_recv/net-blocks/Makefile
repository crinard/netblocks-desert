-include Makefile.inc
BASE_DIR=$(shell pwd)
SRC_DIR=$(BASE_DIR)/src
BUILD_DIR?=$(BASE_DIR)/build
RUNTIME_DIR?=$(BASE_DIR)/runtime
SCRATCH_DIR?=$(BASE_DIR)/scratch
TEST_DIR?=$(BASE_DIR)/test
INCLUDE_DIR=$(BASE_DIR)/include
BUILDIT_DIR?=$(BASE_DIR)/buildit

INCLUDES=$(wildcard $(INCLUDE_DIR)/*.h) $(wildcard $(INCLUDE_DIR)/*/*.h) $(wildcard $(BUILDIT_DIR)/include/*.h) $(wildcard $(BUILDIT_DIR)/include/*/*.h)
RUNTIME_INCLUDE=$(wildcard $(RUNTIME_DIR)/*.h) 
INCLUDE_FLAG=-I$(INCLUDE_DIR) -I$(BUILDIT_DIR)/include

SRCS=$(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*/*.cpp)
OBJS=$(subst $(SRC_DIR),$(BUILD_DIR),$(SRCS:.cpp=.o))

# DESERT_LIBS=-I$(BASE_DIR)/.. -I$(BASE_DIR)/../../../../.unpacked_folder/nsmiracle-1.1.2/nsmiracle/ -I$(BASE_DIR)/../../../../.unpacked_folder/ns-2.34/common -I$(BASE_DIR)/../../../../.unpacked_folder/ns-2.34 -I$(BASE_DIR)/../../../../.unpacked_folder/tclcl-1.20 -I$(BASE_DIR)/../../../../.unpacked_folder/tcl-8.4.19/generic -I$(BASE_DIR)/../../../../.unpacked_folder/otcl-1.14 -I$(BASE_DIR)/../../../../.unpacked_folder/ns-2.34/mobile -I$(BASE_DIR)/../../../../.unpacked_folder/ns-2.34/trace -I$(BASE_DIR)/../../../../.unpacked_folder/ns-2.34/tcp -I$(BASE_DIR)/../../../../.unpacked_folder/ns-2.34/apps

$(shell mkdir -p $(SCRATCH_DIR))
$(shell mkdir -p $(BUILD_DIR))
$(shell mkdir -p $(BUILD_DIR)/core)
$(shell mkdir -p $(BUILD_DIR)/impls)
$(shell mkdir -p $(BUILD_DIR)/modules)
$(shell mkdir -p $(BUILD_DIR)/runtime)
$(shell mkdir -p $(BUILD_DIR)/test)
$(shell mkdir -p $(BUILD_DIR)/test/simple_network_test)
$(shell mkdir -p $(BUILD_DIR)/test/simple_test)
$(shell mkdir -p $(BUILD_DIR)/test/simple_test_accept)
$(shell mkdir -p $(BUILD_DIR)/test/simple_test_inorder)
$(shell mkdir -p $(BUILD_DIR)/test/simple_test_reliable)
$(shell mkdir -p $(BUILD_DIR)/runtime/mlx5_impl)
$(shell mkdir -p $(BASE_DIR)/../.build)

BUILDIT_LIBRARY_NAME=buildit
BUILDIT_LIBRARY_PATH=$(BUILDIT_DIR)/build

LIBRARY_NAME=net_blocks
DEBUG ?= 0
ifeq ($(DEBUG),1)
CFLAGS=-g -std=c++11 -O0
LINKER_FLAGS= -rdynamic -g -L$(BUILDIT_LIBRARY_PATH) -L$(BUILD_DIR) -l$(LIBRARY_NAME) -l$(BUILDIT_LIBRARY_NAME)
else
CFLAGS=-std=c++11 -O3
LINKER_FLAGS=-rdynamic -L$(BUILDIT_LIBRARY_PATH) -L$(BUILD_DIR) -l$(LIBRARY_NAME) -l$(BUILDIT_LIBRARY_NAME)
endif


ifeq ($(DEBUG),1)
RCFLAGS=-g -fPIC
else
RCFLAGS=-O3 -fPIC
endif




LIBRARY=$(BUILD_DIR)/lib$(LIBRARY_NAME).a

CFLAGS+=-Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wmissing-declarations -Woverloaded-virtual -pedantic-errors -Wno-deprecated -Wdelete-non-virtual-dtor -Werror -fno-move-loop-invariants

all: executables $(LIBRARY)

.PHONY: dep
dep:
	make -C $(BUILDIT_DIR)

.PRECIOUS: $(BUILD_DIR)/core/%.o
.PRECIOUS: $(BUILD_DIR)/modules/%.o
.PRECIOUS: $(BUILD_DIR)/impls/%.o


.PHONY: $(BUILDIT_LIBRARY_PATH)/lib$(BUILDIT_LIBRARY_NAME).a


.PHONY: executables
executables: $(BUILD_DIR)/impls/simple


$(LIBRARY): $(OBJS)
	ar rv $(LIBRARY) $(OBJS)

$(BUILD_DIR)/core/%.o: $(SRC_DIR)/core/%.cpp $(INCLUDES)
	$(CXX) $(CFLAGS) $< -o $@ $(INCLUDE_FLAG) -c
$(BUILD_DIR)/modules/%.o: $(SRC_DIR)/modules/%.cpp $(INCLUDES)
	$(CXX) $(CFLAGS) $< -o $@ $(INCLUDE_FLAG) -c
$(BUILD_DIR)/impls/%.o: $(SRC_DIR)/impls/%.cpp $(INCLUDES)
	$(CXX) $(CFLAGS) $< -o $@ $(INCLUDE_FLAG) -c

$(BUILD_DIR)/impls/simple: $(BUILD_DIR)/impls/simple.o $(LIBRARY)
	$(CXX) -o $@ $< $(LINKER_FLAGS)

# Runtime objs
$(SCRATCH_DIR)/nbp_simple.c: $(BUILD_DIR)/impls/simple
	$(BUILD_DIR)/impls/simple $(SCRATCH_DIR)/nbp_gen_headers.h > $(SCRATCH_DIR)/nbp_simple.c

$(BUILD_DIR)/runtime/nbp_simple.o: $(SCRATCH_DIR)/nbp_simple.c $(RUNTIME_INCLUDES) $(SCRATCH_DIR)/nbp_gen_headers.h
	$(CC) $(RCFLAGS) $(SCRATCH_DIR)/nbp_simple.c -o $(BUILD_DIR)/runtime/nbp_simple.o -c -I $(RUNTIME_DIR) -I $(SCRATCH_DIR)


$(BUILD_DIR)/runtime/nbp_runtime_simple.o: $(RUNTIME_DIR)/nbp_runtime.c $(RUNTIME_INCLUDES) $(SCRATCH_DIR)/nbp_simple.c $(SCRATCH_DIR)/nbp_gen_headers.h
	sed -i 's/nb_/nbp_/g' $(SCRATCH_DIR)/'nbp_gen_headers.h'
	sed -i 's/nb_/nbp_/g' $(SCRATCH_DIR)/'nbp_simple.c'
	$(CC) $(RCFLAGS) -o $@ $< -c -I $(SCRATCH_DIR) -I $(RUNTIME_DIR)


$(BUILD_DIR)/runtime/nbp_timer.o: $(RUNTIME_DIR)/nbp_timer.c $(RUNTIME_INCLUDES)
	$(CC) $(RCFLAGS) -o $@ $< -c -I $(SCRATCH_DIR) -I $(RUNTIME_DIR)


$(BUILD_DIR)/runtime/nbp_mlx5_transport.o: $(RUNTIME_DIR)/nbp_mlx5_transport.cc $(RUNTIME_INCLUDES)
	$(CXX) $(RCFLAGS) -c $(RUNTIME_DIR)/nbp_mlx5_transport.cc -o $(BUILD_DIR)/runtime/nbp_mlx5_transport.o -I $(RUNTIME_DIR)/mlx5_impl/ -I $(SCRATCH_DIR) -I $(RUNTIME_DIR)

$(BUILD_DIR)/runtime/nbp_runtime.o: $(RUNTIME_DIR)/nbp_runtime.c $(RUNTIME_INCLUDES)
	$(CXX) $(RCFLAGS) -c $(RUNTIME_DIR)/nbp_runtime.c -o $(BUILD_DIR)/runtime/nbp_runtime.o -I $(RUNTIME_DIR)/desert_impl/ -I $(SCRATCH_DIR) -I $(RUNTIME_DIR)

.PRECIOUS: $(BUILD_DIR)/runtime/mlx5_impl/%.o
$(BUILD_DIR)/runtime/mlx5_impl/%.o: $(RUNTIME_DIR)/mlx5_impl/%.cc $(wildcard $(RUNTIME_DIR)/mlx5_impl/*)
	$(CXX) $(RCFLAGS) $< -o $@ -c -I $(RUNTIME_DIR)/mlx5_impl/ -I $(SCRATCH_DIR)

mlx5_runtime: $(BUILD_DIR)/runtime/nbp_mlx5_transport.o $(BUILD_DIR)/runtime/mlx5_impl/transport.o $(BUILD_DIR)/runtime/mlx5_impl/halloc.o

SIMPLE_RUNTIME_OBJS=$(BUILD_DIR)/runtime/nbp_runtime_simple.o $(BUILD_DIR)/runtime/nbp_simple.o $(BUILD_DIR)/runtime/nbp_timer.o

	
.PHONY: simple_network_test
simple_network_test: mlx5_runtime $(SIMPLE_RUNTIME_OBJS)
	$(CC) $(RCFLAGS) -c $(TEST_DIR)/test_mlx5_simple/server.c -o $(BUILD_DIR)/test/simple_network_test/server.o -I $(RUNTIME_DIR) -I $(SCRATCH_DIR)
	$(CC) $(RCFLAGS) -c $(TEST_DIR)/test_mlx5_simple/client.c -o $(BUILD_DIR)/test/simple_network_test/client.o -I $(RUNTIME_DIR) -I $(SCRATCH_DIR)
	$(CXX) $(BUILD_DIR)/runtime/nbp_mlx5_transport.o $(BUILD_DIR)/runtime/mlx5_impl/halloc.o $(BUILD_DIR)/runtime/mlx5_impl/transport.o $(SIMPLE_RUNTIME_OBJS) $(BUILD_DIR)/test/simple_network_test/server.o -o $(BUILD_DIR)/test/network_simple_server -libverbs
	$(CXX) $(BUILD_DIR)/runtime/nbp_mlx5_transport.o $(BUILD_DIR)/runtime/mlx5_impl/halloc.o $(BUILD_DIR)/runtime/mlx5_impl/transport.o $(SIMPLE_RUNTIME_OBJS) $(BUILD_DIR)/test/simple_network_test/client.o -o $(BUILD_DIR)/test/network_simple_client -libverbs

	
.PHONY: simple_test
simple_test: executables $(SIMPLE_RUNTIME_OBJS)
# Links the simple stack -- src/nbp_runtime.c and src/nbp_simple.c
	$(CC) $(RCFLAGS) -c $(TEST_DIR)/test_simple/server.c -o $(BUILD_DIR)/test/simple_test/server.o -I $(RUNTIME_DIR) -I $(SCRATCH_DIR)
	$(CC) $(RCFLAGS) -c $(TEST_DIR)/test_simple/client.c -o $(BUILD_DIR)/test/simple_test/client.o -I $(RUNTIME_DIR) -I $(SCRATCH_DIR)
	$(CC) $(RCFLAGS) -c $(RUNTIME_DIR)/nbp_ipc_transport.c -o $(BUILD_DIR)/runtime/nbp_ipc_transport.o -I $(RUNTIME_DIR) -I $(SCRATCH_DIR)
	$(CC) $(SIMPLE_RUNTIME_OBJS) $(BUILD_DIR)/test/simple_test/server.o $(BUILD_DIR)/runtime/nbp_ipc_transport.o -o $(BUILD_DIR)/test/simple_server
	$(CC) $(SIMPLE_RUNTIME_OBJS) $(BUILD_DIR)/test/simple_test/client.o $(BUILD_DIR)/runtime/nbp_ipc_transport.o -o $(BUILD_DIR)/test/simple_client

.PHONY: simple_test_accept
simple_test_accept: executables $(SIMPLE_RUNTIME_OBJS)
	$(CC) $(RCFLAGS) -c $(TEST_DIR)/test_simple_accept/server.c -o $(BUILD_DIR)/test/simple_test_accept/server.o -I $(RUNTIME_DIR) -I $(SCRATCH_DIR)
	$(CC) $(RCFLAGS) -c $(TEST_DIR)/test_simple_accept/client.c -o $(BUILD_DIR)/test/simple_test_accept/client.o -I $(RUNTIME_DIR) -I $(SCRATCH_DIR)
	$(CC) $(RCFLAGS) -c $(RUNTIME_DIR)/nbp_ipc_transport.c -o $(BUILD_DIR)/runtime/nbp_ipc_transport.o -I $(RUNTIME_DIR) -I $(SCRATCH_DIR)
	$(CC) $(SIMPLE_RUNTIME_OBJS) $(BUILD_DIR)/test/simple_test_accept/server.o $(BUILD_DIR)/runtime/nbp_ipc_transport.o -o $(BUILD_DIR)/test/simple_accept_server
	$(CC) $(SIMPLE_RUNTIME_OBJS) $(BUILD_DIR)/test/simple_test_accept/client.o $(BUILD_DIR)/runtime/nbp_ipc_transport.o -o $(BUILD_DIR)/test/simple_accept_client

.PHONY: simple_test_inorder
simple_test_inorder: executables $(SIMPLE_RUNTIME_OBJS)
	$(CC) $(RCFLAGS) -c $(TEST_DIR)/test_simple_inorder/server.c -o $(BUILD_DIR)/test/simple_test_inorder/server.o -I $(RUNTIME_DIR) -I $(SCRATCH_DIR)
	$(CC) $(RCFLAGS) -c $(TEST_DIR)/test_simple_inorder/client.c -o $(BUILD_DIR)/test/simple_test_inorder/client.o -I $(RUNTIME_DIR) -I $(SCRATCH_DIR)
	$(CC) $(RCFLAGS) -c $(RUNTIME_DIR)/nbp_ipc_transport.c -o $(BUILD_DIR)/runtime/nbp_ipc_transport.o -I $(RUNTIME_DIR) -I $(SCRATCH_DIR)
	$(CC) $(SIMPLE_RUNTIME_OBJS) $(BUILD_DIR)/test/simple_test_inorder/server.o $(BUILD_DIR)/runtime/nbp_ipc_transport.o -o $(BUILD_DIR)/test/simple_inorder_server
	$(CC) $(SIMPLE_RUNTIME_OBJS) $(BUILD_DIR)/test/simple_test_inorder/client.o $(BUILD_DIR)/runtime/nbp_ipc_transport.o -o $(BUILD_DIR)/test/simple_inorder_client

.PHONY: simple_test_reliable
simple_test_reliable: executables $(SIMPLE_RUNTIME_OBJS)
	$(CC) $(RCFLAGS) -c $(TEST_DIR)/test_simple_reliable/server.c -o $(BUILD_DIR)/test/simple_test_reliable/server.o -I $(RUNTIME_DIR) -I $(SCRATCH_DIR)
	$(CC) $(RCFLAGS) -c $(TEST_DIR)/test_simple_reliable/client.c -o $(BUILD_DIR)/test/simple_test_reliable/client.o -I $(RUNTIME_DIR) -I $(SCRATCH_DIR)
	$(CC) $(RCFLAGS) -c $(RUNTIME_DIR)/nbp_ipc_transport.c -o $(BUILD_DIR)/runtime/nbp_ipc_transport.o -I $(RUNTIME_DIR) -I $(SCRATCH_DIR)
	$(CC) $(SIMPLE_RUNTIME_OBJS) $(BUILD_DIR)/test/simple_test_reliable/server.o $(BUILD_DIR)/runtime/nbp_ipc_transport.o -o $(BUILD_DIR)/test/simple_reliable_server
	$(CC) $(SIMPLE_RUNTIME_OBJS) $(BUILD_DIR)/test/simple_test_reliable/client.o $(BUILD_DIR)/runtime/nbp_ipc_transport.o -o $(BUILD_DIR)/test/simple_reliable_client

.PHONY: simple_desert_nbp

simple_desert_nbp: $(BASE_DIR)/../.build/libmynbp_p.a

$(BASE_DIR)/../.build/libmynbp_p.a: executables $(SIMPLE_RUNTIME_OBJS) $(RUNTIME_DIR)/nbp_desert_transport.cc
	$(CXX) $(RCFLAGS) -c $(RUNTIME_DIR)/nbp_desert_transport.cc -o $(BASE_DIR)/../.build/nbp_desert_transport.o -I $(RUNTIME_DIR) -I $(SCRATCH_DIR) -I $(BASE_DIR)/.. -I $(BASE_DIR)/../../../../.unpacked_folder/nsmiracle-1.1.2/nsmiracle/ -I $(BASE_DIR)/../../../../.unpacked_folder/ns-2.34/common -I $(BASE_DIR)/../../../../.unpacked_folder/ns-2.34 -I $(BASE_DIR)/../../../../.unpacked_folder/tclcl-1.20 -I $(BASE_DIR)/../../../../.unpacked_folder/tcl-8.4.19/generic -I $(BASE_DIR)/../../../../.unpacked_folder/otcl-1.14 -I $(BASE_DIR)/../../../../.unpacked_folder/ns-2.34/mobile -I $(BASE_DIR)/../../../../.unpacked_folder/ns-2.34/trace -I $(BASE_DIR)/../../../../.unpacked_folder/ns-2.34/tcp -I $(BASE_DIR)/../../../../.unpacked_folder/ns-2.34/apps
	ar crv $(BASE_DIR)/../.build/libmynbp_p.a $(SIMPLE_RUNTIME_OBJS)  $(BASE_DIR)/../.build/nbp_desert_transport.o

simple_test_run: simple_test
	$(BUILD_DIR)/test/simple_server &
	sleep 1
	$(BUILD_DIR)/test/simple_client
	pkill -9 simple_server || true

clean:
	rm -rf $(BUILD_DIR)
