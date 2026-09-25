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

export PROJECT_ROOT := $(CURDIR)

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
USER_OBJS :=
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
OUTPUT_IMAGE_PATH :=
OUTPUT_IMAGE_PATH_AS_ARGS :=
OUTPUT_EEPROM_PATH :=
OUTPUT_EEPROM_PATH_AS_ARGS :=
ARTIFACTS_PATH :=
ARTIFACTS_PATH_AS_ARGS :=
ADDITIONAL_DEPENDENCIES:=

#

-include $(PROJECT_ROOT)/Makefile.config
-include $(PROJECT_ROOT)/build_functions.mk
-include $(PROJECT_ROOT)/enable_modules.mk
-include $(PROJECT_ROOT)/tasks.mk

#

export SCRIPTS_DIR := $(PROJECT_ROOT)/scripts

export STACK_USAGE_TOOL := perl -X $(SCRIPTS_DIR)/avstack.pl

# OUTPUT_DIR is where the build output goes into
export OUTPUT_DIR := $(PROJECT_ROOT)/build

export AUTOGEN_DIRNAME := autogen

export LIB_DIRNAME := libs

export TARGET_DIRNAME := target

export ARTIFACTS_DIRNAME := artifacts

# Source subdirectory names

export INCLUDE_DIRNAME := include

export CONFIG_DIRNAME := config

export KERNEL_DIRNAME := kernel

export OPT_DIRNAME := opt

export TASKS_DIRNAME := tasks

export PERIPHERAL_DIRNAME := peripheral

export PORTABILITY_DIRNAME := port

# Configuration header files

CONFIG := config.h

export CONFIG += $(addsuffix config.h,$(PROJECT_ROOT)/$(CONFIG_DIRNAME)/)

#

EXTERNAL_DEPS_DIRNAME ?= deps

EXTERNAL_DEPS_DIR ?= $(PROJECT_ROOT)/$(EXTERNAL_DEPS_DIRNAME)

#

# Relative path to directory of this Makefile
PROJECT_REL_PATH := $(subst $(PROJECT_ROOT),,$(realpath .))

KERNEL_PATH := $(PROJECT_ROOT)/$(KERNEL_DIRNAME)

PERIPHERAL_DIR_PATH := $(PROJECT_ROOT)/$(PERIPHERAL_DIRNAME)

#

# TODO: add Makefile for subdirectories

# Every subdirectory with source files must be described here
SUBDIRS := \
$(CONFIG_DIRNAME) \
$(INCLUDE_DIRNAME) \
$(KERNEL_DIRNAME) \
$(PERIPHERAL_DIRNAME)

OUTPUT_SUBDIRS := \
$(KERNEL_DIRNAME) \
$(PERIPHERAL_DIRNAME) \
$(LIB_DIRNAME) \
$(TARGET_DIRNAME) \
$(AUTOGEN_DIRNAME) \
$(ARTIFACTS_DIRNAME)

$(eval $(call make_abspath,OUTPUT_SUBDIRS,$(OUTPUT_DIR)/))

# Add inputs and outputs from these tool invocations to the build variables

INCLUDE_SUBDIRS := \
$(CONFIG_DIRNAME) \
$(INCLUDE_DIRNAME) \
$(KERNEL_DIRNAME) \
$(PERIPHERAL_DIRNAME) \
$(PORTABILITY_DIRNAME)

INCLUDE_DIRS := $(addprefix $(PROJECT_ROOT)/,$(INCLUDE_SUBDIRS))

INCLUDE_DIRS += $(CURDIR)

INCLUDE_DIRS += $(EXTERNAL_DEPS_DIR)

CPPFLAGS := $(addprefix -I,$(INCLUDE_DIRS))

INCLUDE_HEADERS := $(foreach dir,$(INCLUDE_DIRS),$(wildcard $(dir)/*.h))

PREPROCESSOR_SRCS +=

ASM_SRCS +=

C_SRCS += $(wildcard $(KERNEL_DIRNAME)/*.c)

OBJS += $(patsubst %.c,%.o,$(C_SRCS))

OBJS_AS_ARGS += $(OBJS)

C_DEPS += $(patsubst %.c,%.d,$(C_SRCS))

STACK_USAGE_OBJS := $(filter-out %wdt.o,$(OBJS))

STACK_USAGE_REPORTS += $(patsubst %.o,%.su,$(STACK_USAGE_OBJS))

ASM_LISTINGS += $(patsubst %.c,%.s,$(C_SRCS))

PREPROCESSOR_OUTPUTS += $(patsubst %.c,%.i,$(C_SRCS))

$($(call enable_modules))

# GNU Make 4.3
$(eval $(call make_abspath,OBJS,$(OUTPUT_DIR)/))
$(eval $(call make_abspath,OBJS_AS_ARGS,$(OUTPUT_DIR)/))
$(eval $(call make_abspath,USER_OBJS,$(OUTPUT_DIR)/))
$(eval $(call make_abspath,STACK_USAGE_OBJS,$(OUTPUT_DIR)/))
$(eval $(call make_abspath,C_DEPS,$(OUTPUT_DIR)/))
$(eval $(call make_abspath,USER_C_DEPS,$(OUTPUT_DIR)/))
$(eval $(call make_abspath,STACK_USAGE_REPORTS,$(OUTPUT_DIR)/))
$(eval $(call make_abspath,USER_STACK_USAGE_REPORTS,$(OUTPUT_DIR)/))
$(eval $(call make_abspath,ASM_LISTINGS,$(OUTPUT_DIR)/))
$(eval $(call make_abspath,USER_ASM_LISTINGS,$(OUTPUT_DIR)/))
$(eval $(call make_abspath,PREPROCESSOR_OUTPUTS,$(OUTPUT_DIR)/))
$(eval $(call make_abspath,USER_PREPROCESSOR_OUTPUTS,$(OUTPUT_DIR)/))

###

-include $(PROJECT_ROOT)/symbol_wrappers.mk
$(eval \
	$(call set_symbol_wrappers_vars,$(PROJECT_ROOT),$(OUTPUT_DIR),$(AUTOGEN_DIRNAME),1)\
)

# This explicit target is not necessary because $(WRAPPER_OUTPUT_DIR)/%.c lists
# $(WRAPPER_GEN_DIR)/%.c as its requisite
#$(WRAPPER_OBJS): $(WRAPPER_SOURCES)

gen-wrappers: $(WRAPPER_SOURCES)
$(WRAPPER_SOURCES): | $(WRAPPER_GEN_DIR)
	@echo Generating symbol wrappers
	$(call gen_wrappers,$(SCRIPTS_DIR),$(GEN_WRAPPER_HEADERS),$(WRAPPER_GEN_DIR))
	@touch $(WRAPPER_GEN_DIR)/autogen_symbol_wrappers
	@echo Finished generating symbol wrappers

###

OUTPUT_LIBS += \
libmain.a

LIBDIR := $(OUTPUT_DIR)/$(LIB_DIRNAME)

OUTPUT_LIBS := $(addprefix $(LIBDIR)/,$(OUTPUT_LIBS))

# Will be modified by tasks.mk
TASKS_LIBS_AS_ARGS := $(TASKS_LIBS)

OUTPUT_SUBDIR := $(OUTPUT_DIR)/$(TARGET_DIRNAME)

TARGET_NAME := target

OUTPUT_FILE_PATH := \
$(call make_abspath_pred,$(TARGET_NAME),$(OUTPUT_SUBDIR)/,subst_suffix,elf)

OUTPUT_FILE_PATH_AS_ARGS := \
$(call make_abspath_pred,$(TARGET_NAME),$(OUTPUT_SUBDIR)/,subst_suffix,elf)

OUTPUT_LINKER_MAP_PATH := \
$(call make_abspath_pred,$(TARGET_NAME),$(OUTPUT_SUBDIR)/,subst_suffix,map)

OUTPUT_IMAGE_PATH := \
$(call make_abspath_pred,$(TARGET_NAME),$(OUTPUT_SUBDIR)/,subst_suffix,hex)

OUTPUT_EEPROM_PATH := \
$(call make_abspath_pred,$(TARGET_NAME),$(OUTPUT_SUBDIR)/,subst_suffix,eep)

#ARTIFACTS_PATH += $(OUTPUT_DIR)/artifacts

ARTIFACTS := libmain.lst
ifneq ($(strip $(ENABLE_STANDALONE)),)
ARTIFACTS += target.lst
endif

DEVICE     = atmega328p
PROGRAMMER = arduino
PORT       = /dev/ttyACM*
FREQ       = 16000000UL

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

LINKER_SCRIPTS := $(foreach dir, $(SUBDIRS), $(wildcard $(realpath $(dir))/*.ld))

LINKER_SCRIPT_FLAGS := $(foreach script, $(LINKER_SCRIPTS), -Wl,-T,$(script))

# Will be modified by tasks.mk
LIBDIRS := $(OUTPUT_DIR)/libs

LIB_FLAGS=$(foreach i, $(LIBDIRS), -L$i)

LDFLAGS += \
$(LIB_FLAGS) \
$(LINKER_SCRIPT_FLAGS) \
-Wl,-Map=$(OUTPUT_LINKER_MAP_PATH)

# Will be modified by tasks.mk
LDLIBS := -lmain

$(eval $(call include_deps_makfile,$(PROJECT_ROOT)/$(TASKS_DIRNAME):$(TASKS_DIRNAME)))
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(USER_C_DEPS)),)
-include $(USER_C_DEPS)
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

# AVR32/GNU C Compiler

CC := avr-gcc
OBJCOPY := avr-objcopy
AR := avr-ar
OBJDUMP := avr-objdump
SIZE := avr-size

GENERATE_DEPS = -MD -MP -MF "$(@:%.o=%.d)" 

COMPILE = $(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDE_FLAGS) -c -std=gnu11 -o "$@" "$<"

# TODO: add function to generate recipes for subdirectories

$(OUTPUT_DIR):
	@mkdir -p $@

# Unlike single-colon rules, Make executes ALL matching double-colon recipes
# sequentially rather than overriding previous definitions. This can be used to
# append cleanup tasks, logs, or modular hooks across different files.

# This double-colon rule allows multiple independent recipes for the
# $(WRAPPER_OUTPUT_DIR) target.
$(OUTPUT_SUBDIRS)::
	@mkdir -p $@ 

$(OUTPUT_DIR)/./%.o: ./%.c $(INCLUDE_HEADERS) | $(OUTPUT_DIR)
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler
	$(COMPILE)
	@echo Finished building: $<

$(OUTPUT_DIR)/kernel/%.o: ./kernel/%.c $(INCLUDE_HEADERS) | $(OUTPUT_DIR)/kernel
	@echo Building file: $<
	@echo 	Invoking: AVR/GNU C Compiler
	$(COMPILE)
	@echo Finished building: $<

$(OUTPUT_DIR)/kernel/wdt.o: CFLAGS := $(subst -fstack-usage,,$(CFLAGS))

$(OUTPUT_DIR)/kernel/wdt.o: ./kernel/wdt.c $(INCLUDE_HEADERS) | $(OUTPUT_DIR)/kernel
	@echo Building file: $<
	@echo 	Invoking: AVR/GNU C Compiler
	$(COMPILE)
	@echo Finished building: $<

$(OUTPUT_DIR)/peripheral/%.o: ./peripheral/%.c $(INCLUDE_HEADERS) | $(OUTPUT_DIR)/peripheral
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler
	$(COMPILE)
	@echo Finished building: $<

$(WRAPPER_OUTPUT_DIR)/%.o: $(WRAPPER_GEN_DIR)/%.c | $(WRAPPER_OUTPUT_DIR)
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler
	$(COMPILE)
	@echo Finished building: $<

# AVR32/GNU Preprocessing Assembler

# AVR32/GNU Assembler

# Add inputs and outputs from these tool invocations to the build variables

$(foreach artifact,$(ARTIFACTS),\
	$(eval \
		$(call\
			make_artifacts_recipes,\
			$(artifact),\
			$(OUTPUT_DIR),\
			$(strip \
				$(call\
				  artifact_output_file,\
				  $(artifact),\
					$(OUTPUT_SUBDIR),\
					$(LIBDIR)\
				)\
			)\
		)\
  )\
)

# All Target
ifneq ($(strip $(ENABLE_STANDALONE)),)
ifneq ($(wildcard $(OUTPUT_DIR)/nonstandalone),)
all:: cleankernel
endif
all:: $(ADDITIONAL_DEPENDENCIES) tasks $(OUTPUT_FILE_PATH) $(OUTPUT_IMAGE_PATH) $(OUTPUT_EEPROM_PATH) $(ARTIFACT_TARGETS) size stack-usage | $(OUTPUT_DIR)
	@-$(RM) $(OUTPUT_DIR)/nonstandalone
	@touch $(OUTPUT_DIR)/standalone
else
all: $(ADDITIONAL_DEPENDENCIES) $(OUTPUT_LIBS) $(ARTIFACT_TARGETS) size stack-usage | $(OUTPUT_DIR)
	@-$(RM) $(OUTPUT_DIR)/standalone
	@touch $(OUTPUT_DIR)/nonstandalone
endif

tasks: $(TASKS_LIBS)
	@echo Building target: $@
	@$(MAKE) -C $(TASKS_PATH) all
	@echo Finished building target: $@

$(TASKS_LIBS):
	@echo Building target: $@
	@$(MAKE) -C $(TASKS_PATH) $(TASKS_LIBS_AS_ARGS)
	@echo Finished building target: $@

$(TASK_ARTIFACTS):
	@echo Building target: $@
	@$(MAKE) -C $(TASKS_PATH) $(TASK_ARTIFACTS)
	@echo Finished building target: $@

$(OUTPUT_LIBS): $(OBJS) $(USER_OBJS) $(WRAPPER_OBJS) | $(OUTPUT_DIR)/libs
	@echo Building target: $@
	@echo Creating archives
	$(AR) vrc $@ $(OBJS_AS_ARGS) $(WRAPPER_OBJS_AS_ARGS)
	@echo Finished building target: $@

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_LIBS) $(WRAPPER_OBJS) | $(OUTPUT_SUBDIR)
	@echo Building target: $@
	$(foreach header,$(GEN_WRAPPER_HEADERS),\
		$(eval WRAP_LIST_FILES += $(WRAPPER_GEN_DIR)/$(basename $(notdir $(header)))_wraplist)\
	)
	$(eval $(call make_symbol_wrap_list,$(WRAP_LIST_FILES)))
	$(eval SYMBOL_WRAPPER_LDFLAGS := $(SYMBOL_WRAP_LIST))
	@echo Invoking AVR/GNU C Linker
	$(CC) -o $(OUTPUT_FILE_PATH_AS_ARGS) $(LDFLAGS) $(SYMBOL_WRAPPER_LDFLAGS) -Wl,--whole-archive $(LDLIBS) -Wl,--no-whole-archive -mmcu=$(DEVICE)
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

cleanlibs:
	@-$(RM) -r $(LIBDIRS)

cleankernel:
	@-$(RM) -r $(OUTPUT_DIR)/$(KERNEL_DIRNAME)

cleandeps:
	@-$(RM) $(C_DEPS) $(USER_C_DEPS)

cleantemps:
	@-$(RM) $(ASM_LISTINGS) $(USER_ASM_LISTINGS) $(PREPROCESSOR_OUTPUTS) \
		$(USER_PREPROCESSOR_OUTPUTS) $(STACK_USAGE_REPORTS) \
		$(USER_STACK_USAGE_REPORTS) $(OBJS_AS_ARGS) $(USER_OBJS) $(OUTPUT_LIBS)

cleanoutput:
	@-$(RM) \
    $(OUTPUT_FILE_PATH) \
    $(OUTPUT_IMAGE_PATH) \
    $(OUTPUT_EEPROM_PATH) \
    $(OUTPUT_LINKER_MAP_PATH) \
    $(OUTPUT_DIR)/*standalone

cleandirs:
	@-$(RM) -r\
		$(OUTPUT_DIR) \
		$(WRAPPER_GEN_DIR)

cleankeepdirs: $(EXTERNAL_CLEAN_TARGTETS) cleandeps cleantemps cleanoutput
clean: cleankeepdirs cleandirs

AVRDUDE := avrdude -v -c $(PROGRAMMER) -P $(PORT) -p $(DEVICE)

flash: $(OUTPUT_IMAGE_PATH) $(OUTPUT_EEPROM_PATH)
	@echo "Flash tool:\tavrdude"
	@echo "Programmer:\t$(PROGRAMMER)"
	@echo "Device:\t\t$(DEVICE)"
	@echo "Port:\t\t$(PORT)"
	@$(AVRDUDE) -U flash:w:$(OUTPUT_IMAGE_PATH):i

ifneq ($(strip $(ENABLE_STANDALONE)),)
SIZE_PREREQUISITES := $(OUTPUT_FILE_PATH)
SIZE_OPTS := -C
else
SIZE_PREREQUISITES := $(OUTPUT_LIBS)
SIZE_OPTS := -B
endif
size: $(SIZE_PREREQUISITES)
	@echo Calculating size
	$(SIZE) $(SIZE_OPTS) --mcu=$(DEVICE) $<

ifneq ($(strip $(ENABLE_STANDALONE)),)
STACK_USAGE_PREREQUISITES := $(OUTPUT_FILE_PATH)
else
STACK_USAGE_PREREQUISITES := $(OUTPUT_LIBS)
endif
STACK_USAGE_PATH := $(strip $(call artifact_dir,$(call get_artifact,$(STACK_USAGE_PREREQUISITES)),$(OUTPUT_DIR),libs,target))
stack-usage: $(STACK_USAGE_PREREQUISITES) | $(STACK_USAGE_PATH)
	@echo Calculating stack usage
	@$(STACK_USAGE_TOOL) $(STACK_USAGE_OBJS) > $(STACK_USAGE_PATH)/stack_usage.summary 2>/dev/null

serial-monitor: SERIAL_DEV ?= /dev/ttyACM0

serial-monitor:
	$(eval $(call set_serial_prog_cmd,$(SERIAL_DEV),$(SERIAL_PROG)))
	$(SERIAL_PROG_CMD)

sim:
	$(SIMAVR) -v -v -v -g -m $(DEVICE) -f $(FREQ) $(OUTPUT_FILE_PATH)

trace:
	$(SIMAVR) -v -v -v -t -m $(DEVICE) -f $(FREQ) $(OUTPUT_FILE_PATH)

JTAG_PORT = /dev/ttyUSB*
JTAG_BITRATE = 1000
JTAG_GDB_PORT = 4040
AVARICE:=avarice
flash-jtag: all
	$(AVARICE) --jtag $(JTAG_PORT) --jtag-bitrate $(JTAG_BITRATE) --erase --program --file $(OUTPUT_FILE_PATH)

debug-jtag:
	$(AVARICE) --jtag $(JTAG_PORT) --jtag-bitrate $(JTAG_BITRATE) localhost:$(JTAG_GDB_PORT)

todo:
	@grep -srn --include=*.h --include=*.c --exclude-dir=scratch/ TODO | awk -f $(SCRIPTS_DIR)/tabulate.awk File Line "Todo item" | tee TODO

.PHONY: tasks
