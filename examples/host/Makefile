#
# marichi - cooperative kernel for AVR (R) Mega microcontrollers
# Copyright (C) 2026  notweerdmonk
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#


.DEFAULT_GOAL = all

PROJECT_ROOT ?= $(realpath ../../)

EXTERNAL_DEPS_DIRNAME ?= deps

EXTERNAL_DEPS_DIR ?= $(PROJECT_ROOT)/$(EXTERNAL_DEPS_DIRNAME)

KERNEL_OUTPUT_DIR ?= $(PROJECT_ROOT)/build

OUTPUT_DIR ?= $(KERNEL_OUTPUT_DIR)/examples/host

PERIPHERAL_DIRNAME ?= peripheral

-include $(PROJECT_ROOT)/build_functions.mk

# Every subdirectory with source files must be described here
DIRS :=

# Add inputs and outputs from these tool invocations to the build variables

PREPROCESSOR_SRCS +=

ASM_SRCS +=

C_SRCS = $(wildcard *.c)

OBJS = $(C_SRCS:.c=.o)

OBJS_AS_ARGS += $(C_SRCS:.c=.o)

DEPS = $(C_SRCS:.c=.d)

STACK_USAGE_REPORTS = $(C_SRCS:.c=.su)

ASM_LISTINGS = $(C_SRCS:.c=.s)

PREPROCESSOR_OUTPUTS = $(C_SRCS:.c=.i)

###

$(OUTPUT_DIR):
	mkdir -p $@

###

PERIPHERALS := \
lcd \
adc \
uart

OBJS := $(foreach obj, $(OBJS), $(OUTPUT_DIR)/$(obj))
OBJS_AS_ARGS := $(foreach obj, $(OBJS_AS_ARGS), $(OUTPUT_DIR)/$(obj))
STACK_USAGE_OBJS := $(foreach obj, $(STACK_USAGE_OBJS), $(OUTPUT_DIR)/$(obj))
DEPS := $(foreach dep, $(DEPS), $(OUTPUT_DIR)/$(dep))
STACK_USAGE_REPORTS := $(foreach su, $(STACK_USAGE_REPORTS), $(OUTPUT_DIR)/$(su))
ASM_LISTINGS := $(foreach asm, $(ASM_LISTINGS), $(OUTPUT_DIR)/$(asm))
PREPROCESSOR_OUTPUTS := $(foreach pp, $(PREPROCESSOR_OUTPUTS), $(OUTPUT_DIR)/$(pp))

LIBS +=

LIBS :=

OUTPUT_FILES += $(foreach src,$(C_SRCS),$(OUTPUT_DIR)/$(src:.c=))

OUTPUT_FILES_AS_ARGS += $(foreach src,$(C_SRCS),$(OUTPUT_DIR)/$(src:.c=))

OUTPUT_LINKER_MAPS += $(foreach src,$(C_SRCS),$(OUTPUT_DIR)/$(src:.c=.map))

OUTPUT_IMAGES += $(foreach src,$(C_SRCS),$(OUTPUT_DIR)/$(src:.c=.hex))

OUTPUT_EEPROMS += $(foreach src,$(C_SRCS),$(OUTPUT_DIR)/$(src:.c=.eep))

ARTIFACTS_PATH += $(OUTPUT_DIR)/artifacts

###

ifeq ($(INCLUDE_DIRS),)
INCLUDE_SUBDIRS := config include peripheral port

INCLUDE_DIRS := $(addprefix $(PROJECT_ROOT)/,$(INCLUDE_SUBDIRS))
endif

INCLUDE_DIRS += $(CURDIR)

INCLUDE_DIRS += $(EXTERNAL_DEPS_DIR)

CPPFLAGS := $(foreach i, $(INCLUDE_DIRS), -I$i)

INCLUDE_HEADERS = $(foreach dir, $(abspath $(INCLUDE_DIRS)), $(wildcard $(dir)/*.h))

$(foreach p,$(PERIPHERALS),$(eval $(call add_module_headers,$(PROJECT_ROOT),$(PERIPHERAL_DIRNAME),$(p))))

OPTIMIZATION =

ifneq ($(strip $(DEBUG)),)
DEBUG_OPTIONS := -ggdb3 -save-temps=obj
else
DEBUG_OPTIONS :=
endif

CFLAGS := $(DEBUG_OPTIONS) $(OPTIMIZATION) -Wall -Wextra -Werror

###

LINKER_SCRIPTS :=

LINKER_SCRIPT_FLAGS :=

###

LIB_DIRS :=

LIB_FLAGS :=

LDFLAGS := 

LDLIBS := 

###

# GNU C Compiler

CC = gcc
OBJCOPY := objcopy
AR := ar
OBJDUMP := objdump
SIZE := size

GENERATE_DEPS = -MD -MP -MF "$(@:%.o=%.d)" 

COMPILE = $(CC) $(CFLAGS) $(CPPFLAGS) -std=gnu99 -c $(GENERATE_DEPS) -o "$@" "$<" $(LDFLAGS) $(LDLIBS)

$(OUTPUT_DIR)/%.o: %.c $(INCLUDE_HEADERS) | $(OUTPUT_DIR)
	$(info $(INCLUDE_DIRS))
	$(info $(EXTERNAL_DEPS_DIR))
	@echo Building file: $<
	@echo Invoking: $(CC)
	$(COMPILE)
	@echo Finished building: $<

$(OUTPUT_FILES): $(OUTPUT_DIR)/%: $(OUTPUT_DIR)/%.o
	@echo Building target: $@
	$(CC) -o $@ $< $(LDFLAGS) $(LDLIBS)
	@echo Finished building target: $@

$(OBJS): $(C_SRCS) $(INCLUDE_HEADERS)

all: $(OUTPUT_FILES) $(ARTIFACTS) | $(OUTPUT_DIR)

test-run: $(OUTPUT_FILES)
	for target in $(OUTPUT_FILES); \
	do \
		$(CURDIR)/$${target}; \
	done

cleandeps:
	-$(RM) $(DEPS)

cleantemps:
	-$(RM) $(ASM_LISTINGS) \
		$(PREPROCESSOR_OUTPUTS) \
		$(STACK_USAGE_REPORTS) \
		$(OBJS) \
		$(OUTPUT_FILES)
	
clean: cleandeps cleantemps
