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


.DEFAULT_GOAL := all

PROJECT_ROOT ?= $(realpath ..)

LIBS :=
PROJ :=

O_SRCS :=
C_SRCS :=
S_SRCS :=
S_UPPER_SRCS :=
OBJ_SRCS :=
ASM_SRCS :=
PREPROCESSOR_SRCS :=
OBJS :=
OBJS_AS_ARGS :=
C_DEPS :=
CONFIG :=
DEBUG_OPTIONS :=
STACK_USAGE_REPORTS :=
ASM_LISTINGS :=
PREPROCESSOR_OUTPUTS :=
EXECUTABLES :=
OUTPUT_FILE_PATH :=
OUTPUT_LINKER_MAP_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
ADDITIONAL_DEPENDENCIES:=

#

-include $(PROJECT_ROOT)/Makefile.config
-include $(PROJECT_ROOT)/build_functions.mk
-include $(PROJECT_ROOT)/enable_modules.mk
-include $(PROJECT_ROOT)/tasks.mk

#

SCRIPTS_DIRNAME ?= scripts

SCRIPTS_DIR ?= $(PROJECT_ROOT)/$(SCRIPTS_DIRNAME)

STACK_USAGE_TOOL ?= perl -X $(SCRIPTS_DIR)/avstack.pl

LIB_DIRNAME := libs

# Source subdirectory names

INCLUDE_DIRNAME ?= include

CONFIG_DIRNAME ?= config

KERNEL_DIRNAME ?= kernel

OPT_DIRNAME ?= opt

TASKS_DIRNAME ?= tasks

PERIPHERAL_DIRNAME ?= peripheral

PORTABILITY_DIRNAME ?= port

# Configuration header files

CONFIG := config.h

export CONFIG += $(addsuffix config.h,$(PROJECT_ROOT)/$(CONFIG_DIRNAME)/)

#

EXTERNAL_DEPS_DIRNAME ?= deps

EXTERNAL_DEPS_DIR ?= $(PROJECT_ROOT)/$(EXTERNAL_DEPS_DIRNAME)

# OUTPUT_DIR is where the build output goes into
# KERNEL_OUTPUT_DIR for this Makefile is where kernel build output goes into
ifneq ($(strip $(OUTPUT_DIR)),)
KERNEL_OUTPUT_DIR := $(OUTPUT_DIR)
else
KERNEL_OUTPUT_DIR := $(PROJECT_ROOT)/build
endif

# Every subdirectory with source files must be described here
SUBDIRS :=

OUTPUT_DIR := $(PROJECT_ROOT)/build/tasks

OUTPUT_SUBDIRS := \
libs \
artifacts

$(eval $(call make_abspath,OUTPUT_SUBDIRS,$(OUTPUT_DIR)/))

# Add inputs and outputs from these tool invocations to the build variables

PREPROCESSOR_SRCS +=

ASM_SRCS +=

C_SRCS += $(wildcard *.c)

OBJS += $(C_SRCS:.c=.o)

OBJS_AS_ARGS += $(C_SRCS:.c=.o)

C_DEPS += $(C_SRCS:.c=.d)

STACK_USAGE_OBJS += $(C_SRCS:.c=.o)

STACK_USAGE_REPORTS += $(C_SRCS:.c=.su)

ASM_LISTINGS += $(C_SRCS:.c=.s)

PREPROCESSOR_OUTPUTS += $(C_SRCS:.c=.i)

###

$(OUTPUT_DIR):
	mkdir -p $@

$(OUTPUT_SUBDIRS):
	mkdir -p $@

###

PERIPHERALS := \
lcd \
adc \
uart

$($(call enable_modules))

# GNU Make 4.3
$(eval $(call make_abspath,OBJS,$(OUTPUT_DIR)/))
$(eval $(call make_abspath,OBJS_AS_ARGS,$(OUTPUT_DIR)/))
$(eval $(call make_abspath,STACK_USAGE_OBJS,$(OUTPUT_DIR)/))
$(eval $(call make_abspath,C_DEPS,$(OUTPUT_DIR)/))
$(eval $(call make_abspath,STACK_USAGE_REPORTS,$(OUTPUT_DIR)/))
$(eval $(call make_abspath,ASM_LISTINGS,$(OUTPUT_DIR)/))
$(eval $(call make_abspath,PREPROCESSOR_OUTPUTS,$(OUTPUT_DIR)/))

LIBS := \
libtasks.a

LIBDIR := $(OUTPUT_DIR)/$(LIB_DIRNAME)

LIBS := $(addprefix $(LIBDIR)/,$(LIBS))

OUTPUT_LINKER_MAP_PATH += $(OUTPUT_DIR)/tasks.map

ARTIFACTS_PATH += $(OUTPUT_DIR)/artifacts

ARTIFACTS := \
$(ARTIFACTS_PATH)/libtasks.lst

###

DEVICE     = atmega328p
PROGRAMMER = arduino
PORT       = /dev/ttyACM*
FREQ       = 16000000UL

###

ifeq ($(INCLUDE_DIRS),)
INCLUDE_SUBDIRS := \
$(CONFIG_DIRNAME) \
$(INCLUDE_DIRNAME) \
$(KERNEL_DIRNAME) \
$(PERIPHERAL_DIRNAME) \
$(PORTABILITY_DIRNAME)

INCLUDE_DIRS := $(addprefix $(PROJECT_ROOT)/,$(INCLUDE_SUBDIRS))
endif

INCLUDE_DIRS += $(CURDIR)

INCLUDE_DIRS += $(EXTERNAL_DEPS_DIR)

CPPFLAGS += $(addprefix -I,$(INCLUDE_DIRS))

#INCLUDE_HEADERS = $(foreach dir, $(abspath $(INCLUDE_DIRS)), $(wildcard $(dir)/*.h))

INCLUDE_HEADERS := $(foreach dir,$(INCLUDE_DIRS),$(wildcard $(dir)/*.h))

OPTIMIZATION=-Os

ifneq ($(strip $(ENABLE_DEBUG)),1)
DEBUG_OPTIONS += -ggdb3 -save-temps=obj
endif

DEBUG_OPTIONS += \
-fstack-usage

CFLAGS +=  \
-fshort-enums \
-Wall \
-Wextra \
-Werror \
$(OPTIMIZATION) \
$(DEBUG_OPTIONS) \
-DF_CPU=$(FREQ) \
-mmcu=$(DEVICE)

###

LINKER_SCRIPTS :=

LINKER_SCRIPT_FLAGS :=

###

LDFLAGS := \
-L$(OUTPUT_DIR)/tasks \
-Wl,-Map=$(OUTPUT_LINKER_MAP_PATH)

LDLIBS :=

###

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

ifeq ($(filter clean% flash size stack-usage serial-monitor %-jtag todo test,$(MAKECMDGOALS)),)

PKG_CONFIG := $(shell command -v pkg-config)
ifeq ($(PKG_CONFIG),)
    $(error pkg-config is not installed or not found in the PATH. Please install it.)
else
    $(info pkg-config found: $(PKG_CONFIG))
		override CFLAGS += $(shell $(PKG_CONFIG) --cflags simavr-avr)
		$$(PROJECT_PREFIX)_INCLUDE += $(shell $(PKG_CONFIG) --cflags-only-I simavr-avr)
		LDFLAGS += $(shell $(PKG_CONFIG) --libs simavr-avr)
endif

endif

ifeq ($(strip $(SIMAVR)),)
SIMAVR := $(shell command -v simavr)
endif

# AVR/GNU C Compiler

CC = avr-gcc
OBJCOPY := avr-objcopy
AR := avr-ar
OBJDUMP := avr-objdump
SIZE := avr-size

GENERATE_DEPS = -MD -MP -MF "$(@:%.o=%.d)" 

COMPILE = $(CC) $(CFLAGS) $(CPPFLAGS) -c -std=gnu99 $(GENERATE_DEPS) -o "$@" "$<"

$(OUTPUT_DIR)/%.o: %.c | $(OUTPUT_DIR)
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler
	$(COMPILE)
	@echo Finished building: $<

# AVR/GNU Preprocessing Assembler

# AVR/GNU Assembler

# Add inputs and outputs from these tool invocations to the build variables

# All Target
all: $(LIBS) $(ARTIFACTS) stack-usage | $(OUTPUT_DIR)

$(LIBS): $(OBJS) | $(OUTPUT_DIR)/libs
	@echo Building target: $@
	@echo Creating archives
	$(AR) vrc $@ $(OBJS_AS_ARGS)
	@echo Finished building target: $@

$(ARTIFACTS): $(LIBS) | $(ARTIFACTS_PATH)
	@if echo $@ | grep '^.*lst$$'; then \
		echo Generating assembly listing; \
		$(OBJDUMP) -h -D -S $< > $@; \
	fi

# Other Targets
stack-usage: $(LIBS) | $(ARTIFACTS_PATH)
	@echo Calculating stack usage
	@$(STACK_USAGE_TOOL) $(STACK_USAGE_OBJS) > $(ARTIFACTS_PATH)/stack_usage.summary 2>/dev/null

cleandeps:
	@-$(RM) $(C_DEPS)

cleantemps:
	@-$(RM) $(ASM_LISTINGS) $(PREPROCESSOR_OUTPUTS) \
		$(STACK_USAGE_REPORTS) \
		$(OBJS_AS_ARGS) $(LIBS) $(EXECUTABLES)

clean: cleandeps cleantemps
	@-$(RM) -rf $(OUTPUT_LINKER_MAP_PATH)
