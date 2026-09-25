#!/usr/bin/env bash

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


# Modify GDB script to set ELF binary file path

[[ -n "$1" && "$1" =~ ^(-h|--help)$ ]] && echo "$(basename "${BASH_SOURCE[0]}") - modify GDB script to set ELF binary file path" && exit 0
[[ -z "$1" ]] && echo "Target ELF binary pattern not provided" && exit 1
[[ -z "$2" ]] && echo "GDB script not provided" && exit 1
[[ ! -f "$2" ]] && echo "GDB script not found" && exit 1

target_elf="$(find . -regex ".*""$1")"

[[ -z "$target_elf" ]] && echo "Target ELF not found" && exit 1

sed -i -r "s|(^file\s+)(.*)|\1$target_elf|" "$2"
