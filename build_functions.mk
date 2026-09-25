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


define make_abspath
$$(eval $(strip $(1)) := $$(addprefix $(strip $(2)),$$($(strip $(1)))))
endef

define make_abspath_pred
$(foreach _,$(strip $(1)),$(addprefix $(strip $(2)),$(call $(strip $(3)),$(_),$(strip $(4)))))
endef

define subst_suffix
$(eval arg := $(strip $(1)))
$(eval suffix_arg := $(strip $(2)))
$(subst $(suffix $(arg)),.$(suffix_arg),$(arg))
endef

define module_files_pat
$(eval modpath := $(strip $(1)))
$(eval modname := $(strip $(2)))
$(eval pattern := $(strip $(3)))
$(eval suffix_arg := $(strip $(4)))
$(wildcard $(modpath)/$(modname)/$(pattern).$(suffix_arg))
endef

define module_files
$(strip $(call module_files_pat,$(1),$(2),*,$(3)))
endef

define add_module_headers
$(eval project_root := $(strip $(1)))
$(eval modpath := $(strip $(2)))
$(eval modname := $(strip $(3)))
$$(eval CPPFLAGS += -I$(project_root)/$(modpath)/$(modname))
$$(eval MOD_HEADERS := $$(call module_files,$(modpath),$(modname),h))
$$(eval INCLUDE_HEADERS += $$(MOD_HEADERS))
endef

define add_module_sources
$(eval $(call add_module_headers,$(1),$(2),$(3)))
$(eval project_root := $(strip $(1)))
$(eval modpath := $(strip $(2)))
$(eval modname := $(strip $(3)))
$(eval output_dir := $(strip $(4)))
$$(eval MOD_C_SRCS := $$(call module_files,$(modpath),$(modname),c))
$$(eval MOD_OBJS := $$(patsubst %.c,%.o,$$(MOD_C_SRCS)))
$$(eval C_SRCS += $$(MOD_C_SRCS))
$$(eval C_DEPS += $$(patsubst %.c,%.d,$$(MOD_C_SRCS)))
$$(eval OBJS += $$(MOD_OBJS))
$$(eval OBJS_AS_ARGS += $$(MOD_OBJS))
$$(eval STACK_USAGE_OBJS += $$(MOD_OBJS))
$$(eval STACK_USAGE_REPORTS += $$(patsubst %.c,%.su,$$(MOD_C_SRCS)))
$$(eval ASM_LISTINGS += $$(patsubst %.c,%.s,$$(MOD_C_SRCS)))
$$(eval PREPROCESSOR_OUTPUTS += $$(patsubst %.c,%.i,$$(MOD_C_SRCS)))
$(output_dir)/$(modpath)/$(modname):
	mkdir -p $$@
$(output_dir)/$(modpath)/$(modname)/%.o: $(modpath)/$(modname)/%.c | $(output_dir)/$(modpath)/$(modname)
	@echo Building file: $$<
	@echo Invoking: AVR/GNU C Compiler
	$$(COMPILE)
	@echo Finished building: $$<
endef

define add_module_stub_sources
$(eval modpath := $(strip $(1))/$(strip $(2)))
$(eval $(call add_module_sources,$(modpath),stub))
endef

define gen_symbol_wrappers =
$(eval script := $(strip $(1)))
$(eval source_file := $(strip $(2)))
$(eval source_file_name := $(basename $(notdir $(2))))
$(eval output_path := $(strip $(3)))
$(eval wrapper_source_path := $$(output_path)/$$(source_file_name)_wrappers.c)
$(eval wraplist_path := $$(output_path)/$$(source_file_name)_wraplist)
$$(shell bash $$(script) $$(source_file) $$(wrapper_source_path) $$(wraplist_path))
endef

define gen_wrappers
$(foreach header,$(strip $(2)),\
	$(eval \
		$(call \
			gen_symbol_wrappers,$(strip $(1))/gen_wrappers.bash,$(header),$(strip $(3))\
		)\
	)\
)
endef

define make_symbol_wrap_list
$$(eval SYMBOL_WRAP_LIST := $$(foreach file,$(strip $(1)),$$(shell cat $$(file))))
endef

define make_wrapper_headers_list
$(addprefix $(strip $(1))/,$(strip $(2)))
endef

#define artifact_output_file
#$$(shell artifact="$(1)"; \
#	artifact_basename="$$$${artifact##*/}"; \
#	if [ "$$$${artifact_basename#lib}" = "$$$$artifact_basename" ]; then \
#		echo "$(strip $(2))"/"$$$${artifact_basename%%.*}.elf"; \
#	else \
#		echo "$(strip $(3))"/"$$$${artifact_basename%%.*}.a"; \
#	fi \
#)
#endef

# let requires GNU Make 4.4+
#define artifact_output_file
#$(let abase,$(patsubst .%,,$(notdir $(1))),\
#	$(if $(filter $(subst lib,,$(abase)),$(abase)),\
#		$(OUTPUT_SUBDIR)/$(abase).elf,\
#		$(LIBDIR)/$(abase).a\
#	)\
#)
#endef
define artifact_output_file
$(eval abase := $(basename $(notdir $(1))))
$(eval bindir := $(strip $(2)))
$(eval libdir := $(strip $(3)))
$(if $(filter $(subst lib,,$(abase)),$(abase)),\
	$(bindir)/$(abase).elf,\
	$(libdir)/$(abase).a\
)
endef

define get_artifact
$(basename $(1)).lst
endef

#define artifact_dir
#$(let arg,$(firstword $(strip $(1))),\
#	$(let path,$(strip $(2)),\
#		$(if $(filter lib%.a,$(arg)),$(path)/$(strip $(3))/artifacts,\
#			$(if $(filter %.elf,$(arg)),$(path)/$(strip $(4))/artifacts,),\
#		)\
#	)\
#)
#endef

define artifact_dir
$(eval arg := $(firstword $(notdir $(strip $(1)))))\
$(eval path := $(strip $(2)))\
$(if $(filter lib%.lst,$(arg)),$(path)/$(strip $(3))/artifacts,\
	$(if $(filter %.lst,$(arg)),$(path)/$(strip $(4))/artifacts)\
)
endef

# This function requires four $ signs for the grep regex because eval function
# exapnds $$$$ to $$. The recipe contains $$ in the grep regex pattern. When
# Make parses the recipe invoking the shell to execute it, the $$ gets
# interpreted as an escaped $ and the regex pattern exapands correctly.
#define make_artifacts_recipes
#$$(strip $(1)): $(2) | $(3)
#	if echo $$@ | grep '^.*lst$$$$'; then \
#		echo Generating assembly listing for $((notder $(2)); \
#		$(OBJDUMP) -h -D -S $(2) > $$@; \
#	fi
#endef
# A better and portable alternative is the filter function that does not rely on
# the shell to filter the target of the artifact goals.
define make_artifacts_recipes
$(eval artifact := $(strip $(1)))
$(eval output_dir := $(strip $(2)))
$(eval artifact_path := $(strip $(call artifact_dir,$(artifact),$(output_dir),libs,target)))
$(artifact_path):
	mkdir -p $$@
$(artifact_path)/$(artifact): $$(strip $(3)) | $(artifact_path)
	$$(if \
		$$(filter %.lst,$$@),\
		$$(info Generating assembly listing for $$(notdir $(3)))\
		$(OBJDUMP) -h -D -S $(3) > $$@ \
	)
$(eval ARTIFACT_TARGETS += $(artifact_path)/$(artifact))
endef

define set_serial_prog_cmd
SERIAL_PROG_CMD := $$(shell \
	if [ -n "$(2)" ] && [ "$(2)" = "putty" ]; then \
		echo "putty -serial $(1) -sercfg \"8,1,19200,n,X\""; \
	elif [ -n "$(2)" ] && [ "$(2)" = "screen" ]; then \
		echo "stty -F $(1) 19200 cs8 -parenb -cstopb -crtscts -ixon -ixoff raw; screen $(1) 19200; stty -F $(1) hupcl"; \
	else \
		echo "minicom -c on -D $(1) -b 19200"; \
	fi \
)
endef

define parse_dep_pair
$(eval dep_pair_list := $(subst :, ,$(strip $(1))))
$(eval dep_path := $(firstword $(dep_pair_list)))
$(eval dep_name := $(wordlist 2,2,$(dep_pair_list)))
$(eval -include $(dep_path)/$(dep_name).mk)
endef

define include_deps_makfile
$(eval deps_pairs=$(strip $(1)))
$(foreach dep_pair,$(deps_pairs),$(call parse_dep_pair,$(dep_pair)))
endef
