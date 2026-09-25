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

PROJECT_ROOT ?= $(realpath ..)

EXTERNAL_DEPS_DIRNAME ?= deps

EXTERNAL_DEPS_DIR ?= $(PROJECT_ROOT)/$(EXTERNAL_DEPS_DIRNAME)
export EXTERNAL_DEPS_DIR

SUBDIRS := $(filter-out Makefile run_tests %.md,$(wildcard *))

.PHONY: $(SUBDIRS)

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

help:
	$(MAKE) -C $(firstword $(SUBDIRS)) help

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
