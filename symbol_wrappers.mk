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


define set_symbol_wrappers_vars
$(eval project_root := $(strip $(1)))
$(eval output_dir := $(strip $(2)))
$(eval autogen_dirname := $(strip $(3)))
$(eval is_project_root := $(strip $(4)))


$(eval GEN_WRAPPER_HEADERS := \
	$(call make_wrapper_headers_list,$(project_root)/include,adc.h lcd.h uart.h)\
)

$(eval WRAPPER_GEN_SUBDIR := $(autogen_dirname))
$(eval WRAPPER_GEN_DIR := $(project_root)/$(WRAPPER_GEN_SUBDIR))

$(eval WRAPPER_SOURCES := \
	$(foreach header, $(GEN_WRAPPER_HEADERS), \
		$(WRAPPER_GEN_DIR)/$(basename $(notdir $(header)))_wrappers.c\
	)\
)

$(eval WRAPPER_OBJS := \
	$(foreach src, $(WRAPPER_SOURCES),\
		$(output_dir)/$(WRAPPER_GEN_SUBDIR)/$(patsubst %.c,%.o,$(notdir $(src)))\
	)\
)

$(eval WRAPPER_OBJS_AS_ARGS := $(WRAPPER_OBJS))

$(eval WRAPPER_OUTPUT_DIR := $(output_dir)/$(autogen_dirname))

ifeq ($(is_project_root),)
$(WRAPPER_SOURCES): $(WRAPPER_GEN_DIR)/autogen_symbol_wrappers
endif

$(WRAPPER_GEN_DIR):
	@mkdir -p $$@

$(WRAPPER_GEN_DIR)/autogen_symbol_wrappers:
	@$$(MAKE) -C $(project_root) gen-wrappers

ifeq ($(is_project_root),)
# This double-colon rule allows multiple independent recipes for the
# $(WRAPPER_OUTPUT_DIR) target.
$(WRAPPER_OUTPUT_DIR)::
	@$$(MAKE) -C $(project_root) $(WRAPPER_OUTPUT_DIR)
endif
endef
