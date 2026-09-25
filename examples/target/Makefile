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

PROJECT_ROOT ?= $(realpath ../../)

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
OUTPUT_FILE_PATH_AS_ARGS :=
ADDITIONAL_DEPENDENCIES:=

-include $(PROJECT_ROOT)/Makefile.config
-include $(PROJECT_ROOT)/build_functions.mk
-include $(PROJECT_ROOT)/enable_modules.mk

SCRIPTS_DIRNAME ?= scripts

SCRIPTS_DIR ?= $(PROJECT_ROOT)/$(SCRIPTS_DIRNAME)

STACK_USAGE_TOOL ?= perl -X $(SCRIPTS_DIR)/avstack.pl

# Source subdirectory names

INCLUDE_DIRNAME ?= include

CONFIG_DIRNAME ?= config

KERNEL_DIRNAME ?= kernel

OPT_DIRNAME ?= opt

TASKS_DIRNAME ?= tasks

PERIPHERAL_DIRNAME ?= peripheral

PORTABILITY_DIRNAME ?= port

# Configuration header files

CONFIG ?= $(PROJECT_ROOT)/config/config.h

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

AUTOGEN_DIRNAME ?= autogen

TARGET_DIRNAME ?= target

ARTIFACTS_DIRNAME ?= artifacts

#

# Relative path to directory of this Makefile
PROJECT_REL_PATH := $(subst $(PROJECT_ROOT)/,,$(realpath .))

# Reassign OUTPUT_DIR for the current build environment
# Only required for Makefiles in subdirectories
OUTPUT_DIR := $(addprefix $(KERNEL_OUTPUT_DIR)/,$(PROJECT_REL_PATH))

# Every subdirectory with source files must be described here
SUBDIRS :=

OUTPUT_SUBDIRS := \
$(TARGET_DIRNAME) \
$(ARTIFACTS_DIRNAME) \

$(eval $(call make_abspath,OUTPUT_SUBDIRS,$(OUTPUT_DIR)/))

# Add inputs and outputs from these tool invocations to the build variables

PREPROCESSOR_SRCS +=

ASM_SRCS +=

C_SRCS += \
parser.c

OBJS += $(C_SRCS:.c=.o)

OBJS_AS_ARGS += $(C_SRCS:.c=.o)

C_DEPS += $(C_SRCS:.c=.d)

STACK_USAGE_OBJS += $(C_SRCS:.c=.o)

STACK_USAGE_REPORTS += $(C_SRCS:.c=.su)

ASM_LISTINGS += $(C_SRCS:.c=.s)

PREPROCESSOR_OUTPUTS += $(C_SRCS:.c=.i)

###

$(OUTPUT_DIR):
	@mkdir -p $@

$(OUTPUT_SUBDIRS):
	@mkdir -p $@

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

LIBS :=

OUTPUT_FILE_PATH := \
$(call make_abspath_pred,$(C_SRCS),$(OUTPUT_DIR)/,subst_suffix,elf)

OUTPUT_FILE_PATH_AS_ARGS := \
$(call make_abspath_pred,$(C_SRCS),$(OUTPUT_DIR)/,subst_suffix,elf)

OUTPUT_LINKER_MAP_PATH := \
$(call make_abspath_pred,$(C_SRCS),$(OUTPUT_DIR)/,subst_suffix,map)

OUTPUT_IMAGE_PATH := \
$(call make_abspath_pred,$(C_SRCS),$(OUTPUT_DIR)/,subst_suffix,hex)

OUTPUT_EEPROM_PATH := \
$(call make_abspath_pred,$(C_SRCS),$(OUTPUT_DIR)/,subst_suffix,eep)

ARTIFACTS := \
parser.lst

###

-include $(PROJECT_ROOT)/symbol_wrappers.mk
$(eval \
	$(call set_symbol_wrappers_vars,$(PROJECT_ROOT),$(KERNEL_OUTPUT_DIR),$(AUTOGEN_DIRNAME))\
)

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

CPPFLAGS := $(foreach i, $(INCLUDE_DIRS), -I$i)

INCLUDE_HEADERS = $(foreach dir, $(INCLUDE_DIRS), $(wildcard $(dir)/*.h))

$(foreach p,$(PERIPHERALS),$(eval $(call add_module_headers,$(PROJECT_ROOT),$(PERIPHERAL_DIRNAME),$(p))))

OPTIMIZATION := -Os

ifeq ($(ENABLE_DEBUG),1)
DEBUG_OPTIONS += -ggdb3 -save-temps=obj
endif

DEBUG_OPTIONS += \
-fstack-usage

CFLAGS +=  \
-fshort-enums \
-Wall \
-Wextra \
$(OPTIMIZATION) \
$(DEBUG_OPTIONS) \
-DF_CPU=$(FREQ) \
-mmcu=$(DEVICE)

###

LINKER_SCRIPTS :=

LINKER_SCRIPT_FLAGS :=

###

LIB_DIRS := $(KERNEL_OUTPUT_DIR)/libs

LIB_FLAGS := $(foreach i, $(LIB_DIRS), -L$i)

###

LDFLAGS := \
$(LIB_FLAGS) \
$(LINKER_SCRIPT_FLAGS) \
$(SYMBOL_WRAP_LIST) \
-Wl,-Map=$(OUTPUT_LINKER_MAP_PATH)

LDLIBS := -lmain

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

$(OUTPUT_DIR)/%.o: %.c $(INCLUDE_HEADERS) | $(OUTPUT_DIR)
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler
	$(COMPILE)
	@echo Finished building: $<

$(WRAPPER_OUTPUT_DIR)/%.o: $(WRAPPER_GEN_DIR)/%.c | $(WRAPPER_OUTPUT_DIR)
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler
	$(COMPILE)
	@echo Finished building: $<

# AVR/GNU Preprocessing Assembler

# AVR/GNU Assembler

# Add inputs and outputs from these tool invocations to the build variables

$(foreach artifact,$(ARTIFACTS),\
	$(eval $(call\
		make_artifacts_recipes,\
		$(artifact),\
		$(OUTPUT_DIR),\
		$(strip \
			$(call\
			  artifact_output_file,\
			  $(artifact),\
				$(OUTPUT_DIR),\
			)\
		)\
  ))\
)

# Special target to check for standalone kernel build; we need non-standalone
# build
$(KERNEL_OUTPUT_DIR)/nonstandalone:
	@$(MAKE) -C $(PROJECT_ROOT) cleankeepdirs
	@$(MAKE) -C $(PROJECT_ROOT)

# All Target
all: $(ADDITIONAL_DEPENDENCIES) $(OUTPUT_FILE_PATH) $(OUTPUT_IMAGE_PATH) $(OUTPUT_EEPROM_PATH) $(ARTIFACT_TARGETS) size stack-usage | $(OUTPUT_DIR) $(KERNEL_OUTPUT_DIR)/nonstandalone

$(OUTPUT_FILE_PATH): $(OBJS) $(LIBS) $(WRAPPER_OBJS) | $(OUTPUT_DIR) $(KERNEL_OUTPUT_DIR)/nonstandalone
	@echo Building target: $@
	$(foreach header,$(GEN_WRAPPER_HEADERS),\
		$(eval WRAP_LIST_FILES += $(WRAPPER_GEN_DIR)/$(basename $(notdir $(header)))_wraplist)\
	)
	$(eval $(call make_symbol_wrap_list,$(WRAP_LIST_FILES)))
	$(eval SYMBOL_WRAPPER_LDFLAGS := $(SYMBOL_WRAP_LIST))
	@echo Invoking AVR/GNU C Linker
	$(CC) -o $(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS) $(LDFLAGS) $(SYMBOL_WRAPPER_LDFLAGS) -Wl,--whole-archive $(LDLIBS) -Wl,--no-whole-archive -mmcu=$(DEVICE)
	@echo Finished building target: $@

$(OUTPUT_IMAGE_PATH): $(OUTPUT_FILE_PATH)
	@echo Building hex file
	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature $< $@
	@echo Finished building target: $@

$(OUTPUT_EEPROM_PATH): $(OUTPUT_FILE_PATH)
	@echo Building eep file
	$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 --no-change-warnings -O ihex $< $@ || exit 0
	@echo Finished building target: $@

# Other Targets
cleandeps:
	-$(RM) $(C_DEPS)

cleantemps:
	-$(RM) $(ASM_LISTINGS) $(PREPROCESSOR_OUTPUTS) \
		$(STACK_USAGE_REPORTS) \
		$(OBJS_AS_ARGS) $(LIBS) $(EXECUTABLES)
	
clean: cleandeps cleantemps
	@-$(RM) $(OUTPUT_FILE_PATH) $(OUTPUT_IMAGE_PATH) $(OUTPUT_EEPROM_PATH) $(OUTPUT_LINKER_MAP_PATH)

AVRDUDE := avrdude -v -c $(PROGRAMMER) -P $(PORT) -p $(DEVICE)

flash: all
	$(AVRDUDE) -U flash:w:$(OUTPUT_IMAGE_PATH):i

ifeq ($(strip $(ENABLE_STANDALONE)),)
SIZE_PREREQUISITES := $(OUTPUT_FILE_PATH)
SIZE_OPTS := -C
else
SIZE_PREREQUISITES := $(LIBS)
SIZE_OPTS := -B
endif
size: $(SIZE_PREREQUISITES)
	@echo Calculating size
	$(SIZE) $(SIZE_OPTS) --mcu=$(DEVICE) $<

ifeq ($(strip $(ENABLE_STANDALONE)),)
STACK_USAGE_PREREQUISITES := $(OUTPUT_FILE_PATH)
else
STACK_USAGE_PREREQUISITES := $(LIBS)
endif
STACK_USAGE_PATH := $(strip $(call artifact_dir,$(call get_artifact,$(STACK_USAGE_PREREQUISITES)),$(OUTPUT_DIR),libs,target))
stack-usage: $(STACK_USAGE_PREREQUISITES) | $(STACK_USAGE_PATH)
	@echo Calculating stack usage
	@$(STACK_USAGE_TOOL) $(STACK_USAGE_OBJS) > $(STACK_USAGE_PATH)/stack_usage.summary 2>/dev/null

serial-monitor: SERIAL_DEV ?= /dev/ttyACM0

serial-monitor:
	$(eval $(call set_serial_prog_cmd, $(SERIAL_DEV),$(SERIAL_PROG)))
	$(SERIAL_PROG_CMD)

sim:
	$(SIMAVR) -v -v -v -g -m $(DEVICE) -f $(FREQ) $(OUTPUT_FILE_PATH)

trace:
	$(SIMAVR) -v -v -v -t -m $(DEVICE) -f $(FREQ) $(OUTPUT_FILE_PATH)
