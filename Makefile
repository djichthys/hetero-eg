SHELL :=/bin/bash

ifeq ($(VERBOSE),1)
 QUIET :=
else
 QUIET:=@
endif

CFLAGS :=
ifeq ($(DEBUG),1)
 CFLAGS += -g
endif 



CC := gcc
ROOT:=$(strip $(shell pwd))
SRC_DIR:=$(ROOT)/src
COMMON_SRC:=$(SRC_DIR)/common
ADDV_SRC:=$(SRC_DIR)/add_vector

SRC_SEARCH_DIRECTORIES := $(ADDV_SRC)     \
                          $(COMMON_SRC) 
                          

#add source paths to search 
vpath %.c $(SRC_SEARCH_DIRECTORIES)

OBJ_DIR:=$(ROOT)/obj
BIN_DIR:=$(ROOT)/bin
INCLUDE:= $(ROOT)/include


ADDV_GPU_OBJFILES  := $(OBJ_DIR)/add_v_entry.o     \
                      $(OBJ_DIR)/add_v_options.o   \
                      $(OBJ_DIR)/cmdline_utils.o   \
                      $(OBJ_DIR)/parser.o          \


TARGETS := add_vector


all : $(TARGETS)


.PHONY: create_objdir 
create_objdir : 
	$(QUIET)if [[ ! -d $(ROOT)/obj ]]; then \
            echo "Creating obj directory" $(ROOT)/obj ;\
            mkdir -p $(ROOT)/obj ;\
        fi


.PHONY: create_bindir
create_bindir : 
	$(QUIET)if [[ ! -d $(ROOT)/bin ]]; then \
            echo "Creating bin directory" $(ROOT)/bin ;\
            mkdir -p $(ROOT)/bin ;\
        fi


#GPU Add Vector
.PHONY: add_vector
add_vector : create_objdir create_bindir add_vector.elf
	
.PHONY: add_vector.elf
add_vector.elf : $(ADDV_GPU_OBJFILES) 
	$(QUIET)$(CC) $(CFLAGS) $? -o $(BIN_DIR)/$@
	

#compilation target - the source directories have been added in the 
# vpath directive at the start of the Makefile
$(OBJ_DIR)/%.o : %.c
	$(QUIET) echo "======================================================"
	$(QUIET) echo compiling $< 
	$(QUIET)$(CC) -c $(CFLAGS)  $(foreach inc,$(INCLUDE),-I$(inc)) $< -o $@



clean : 
	$(QUIET)echo "Removing" $(OBJ_DIR) $(BIN_DIR)
	$(QUIET)rm -rf $(OBJ_DIR)
	$(QUIET)rm -rf $(BIN_DIR)
	

.PHONY: help 
help:
	$(QUIET)echo "========================================================================="
	$(QUIET)echo "Targets to compile" 
	$(QUIET)echo "1) all.............. compile all targets"
	$(QUIET)echo "2) add_vector....... compile add_vector GPU program"
	$(QUIET)echo "========================================================================="
	$(QUIET)echo "VERBOSE=1  ......... to see individual commands executed"
	$(QUIET)echo "DEBUG=1  ........... to add -g option to compilation"
	$(QUIET)echo "========================================================================="

