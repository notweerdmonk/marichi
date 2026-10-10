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

# Utility bash run commands file

script_path="$(realpath "${BASH_SOURCE[0]}")"
script_dir="$(dirname "$script_path")"
[[ "$(basename "$script_dir")" =~ ^scripts$ ]] && \
  {
    top_dir="${script_dir%/scripts}"
    find_result="$($(which find) "$top_dir" -maxdepth 2 -wholename "$script_path")"
    [[ -z "$find_result" ]] && top_dir="$script_dir"
    unset script_path
    unset script_dir
  }
export PROJECT_ROOT="$top_dir"
unset top_dir

# Actual programs and tools
[[ -z "$MYVIM" ]] && export MYVIM="$(which vim)"
[[ -z "$MYGREP" ]] && export MYGREP="$(which grep)"
[[ -z "$MYTREE" ]] && export MYTREE="$(which tree)"
[[ -z "$MYCSCOPE" ]] && export MYCSCOPE="$(which cscope)"

# Custom find function
function find() {
  local MYFIND="$(which find)"
  loc="$1"
  shift
  while [[ -n "$1" ]] && [[ "${1:0:1}" != "-" ]]
  do
    loc+=" $1"
    shift
  done
  [[ -z "$@" ]] && echo "No expressions provided to find" && return 255
  "$MYFIND" "$loc" -path "$PROJECT_ROOT"/build -prune -o \( "$@" \) -print
}

# Custom aliases
[[ -f "$PROJECT_ROOT""/.vimrc" ]] && \
alias vim="$MYVIM +\"source $PROJECT_ROOT/.vimrc\""
alias grep="$MYGREP --color=auto --exclude-dir=$PROJECT_ROOT/build/"
alias tree="$MYTREE -I build/"

# No cscope found
[[ -z "$MYCSCOPE" ]] && return 0

# Create cscope/ directory if it does not exist
[[ ! -d "$PROJECT_ROOT"/cscope ]] && \
  mkdir -p "$PROJECT_ROOT"/cscope && \
  echo Created "$PROJECT_ROOT"/cscope directory

# Cscope aliases
alias cscope-add="find $PROJECT_ROOT -name \"*.h\" -o -name \"*.c\" > $PROJECT_ROOT/cscope/cscope.files"
alias cscope-update="$MYCSCOPE -q -R -b -i $PROJECT_ROOT/cscope/cscope.files -f $PROJECT_ROOT/cscope/cscope.out"
alias cscope-browse="$MYCSCOPE -P$PROJECT_ROOT/ -d -f $PROJECT_ROOT/cscope/cscope.out"

# No vim found
[[ -z "$MYVIM" ]] && echo vim not found
