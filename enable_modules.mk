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


-include $(PROJECT_ROOT)/build_functions.mk

define enable_modules
$(if \
	$(strip $(LCD)),\
	$(eval CPPFLAGS += -D__ENABLE_LCD)\
	$(eval $(call add_module_sources,$(PROJECT_ROOT),$(PERIPHERAL_DIRNAME),lcd,$(OUTPUT_DIR))),\
	$(eval $(call add_module_stub_sources,$(PROJECT_ROOT),$(PERIPHERAL_DIRNAME),lcd,$(OUTPUT_DIR)))\
)

$(if \
  $(strip $(UART)),\
  $(eval CPPFLAGS += -D__ENABLE_UART)\
  $(eval $(call add_module_sources,$(PROJECT_ROOT),$(PERIPHERAL_DIRNAME),uart,$(OUTPUT_DIR))),\
  $(eval $(call add_module_stub_sources,$(PROJECT_ROOT),$(PERIPHERAL_DIRNAME),uart,$(OUTPUT_DIR)))\
)

$(if \
  $(strip $(ADC)),\
  $(eval CPPFLAGS += -D__ENABLE_ADC)\
  $(eval $(call add_module_sources,$(PROJECT_ROOT),$(PERIPHERAL_DIRNAME),adc,$(OUTPUT_DIR))),\
  $(eval $(call add_module_stub_sources,$(PROJECT_ROOT),$(PERIPHERAL_DIRNAME),adc,$(OUTPUT_DIR)))\
)

$(if \
  $(strip $(DEBUGGER)),\
  $(eval CPPFLAGS += -D__ENABLE_DEBUGGER)\
  $(eval $(call add_module_sources,$(PROJECT_ROOT),$(KERNEL_DIRNAME)/$(OPT_DIRNAME),debugger,$(OUTPUT_DIR)))\
)

$(if \
  $(strip $(DBG)),\
	$(eval ENABLE_DEBUG := 1)\
  $(eval CPPFLAGS += -D__ENABLE_DEBUG)\
)
$(if \
  $(strip $(RESMGMT)),\
  $(eval CPPFLAGS += -D__ENABLE_RESMGMT)\
)

$(if \
  $(strip $(SIM)),\
  $(eval CPPFLAGS += -D__ENABLE_SIMULATION)\
)

$(if \
  $(strip $(SA)),\
	$(eval ENABLE_STANDALONE := 1)\
  $(eval CPPFLAGS += -D__STANDALONE)\
)

$(eval VERBOSITY := $$(strip $(VV)))
$(eval VERBOSITY := $(strip \
	$(if \
		$(filter 2,$(VERBOSITY)),VERBOSE2,\
			$(if $(filter 1,$(VERBOSITY)),VERBOSE1,\
				VERBOSE0\
			)\
		)\
	)\
)
$(if \
  $(VERBOSITY),\
  $(eval CPPFLAGS += -DVERBOSITY=$(VERBOSITY))\
)
undefine VERBOSITY
endef
