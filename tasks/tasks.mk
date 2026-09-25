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


export DEP_LIBS := $(DEP_LIBS) tasks

ifeq ($(strip $(AVR_LCD_ROOT_DIR)),)
AVR_LCD_ROOT_DIR := $(PROJECT_ROOT)
endif

ifeq ($(strip $(AVR_LCD_LIB_DIR)),)
AVR_LCD_LIB_DIR := $(LIB_DIR)
endif

TASKS_INCLUDE_DIRS := $(CURDIR)

INCLUDE_DIRS += $(TASKS_INCLUDE_DIRS)

CPPFLAGS += $(addprefix -I,$(TASKS_INCLUDE_DIRS))

INCLUDE_HEADERS += $(foreach dir,$(TASK_INCLUDE_DIRS),$(wildcard $(dir)/*.h))

TASKS_DIRNAME := tasks

TASKS_PATH := $(PROJECT_ROOT)/$(TASKS_DIRNAME)

TASKS_OUTPUT_PATH := $(OUTPUT_DIR)/$(TASKS_DIRNAME)

TASKS_LIB_PATH := $(TASKS_OUTPUT_PATH)/$(LIB_DIRNAME)

TASK_ARTIFACTS_PATH := $(TASKS_OUTPUT_PATH)/$(ARTIFACTS_DIRNAME)

LDFLAGS += -L$(TASKS_OUTPUT_PATH)

TASKS_LIB_NAME := tasks

TASKS_LDLIBS := $(addprefix -l,$(TASKS_LIB_NAME))

# This variable requires $(TASKS_LDLIBS) to be appended to enable linker to
# choose strong symbols from libtasks.a over the weak symbols in the kernel
# archive
LDLIBS := $(TASKS_LDLIBS) $(LDLIBS)

LIBDIRS += $(TASKS_LIB_PATH)

TASKS_LIB := $(TASKS_LIB_PATH)/$(addprefix lib,$(addsuffix .a,$(TASKS_LIB_NAME)))

TASK_ARTIFACTS := $(addprefix, $(TASK_ARTIFACTS_PATH)/,$(TASKS_LIBS:.a=.lst))

cleantasks:
	$(MAKE) -C $(TASKS_PATH) clean

EXTERNAL_CLEAN_TARGTETS += cleantasks
