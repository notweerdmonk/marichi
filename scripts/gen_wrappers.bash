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


#
# Generate GNU ld --wrap wrappers from C function declarations.
#
# Usage:
#   ./gen_wrappers.bash input.h output.c
#
# Optional:
#   TABWIDTH=4 ./gen_wrappers.bash input.h output.c
#
# The default indentation width is two spaces.
#
# Supported wrapper prologue syntax:
#
#   /* function_wrapper_prologue: enter_trace(); */
#   void foo(int value);
#
#   /**
#   function_wrapper_prologue:
#     enter_trace();
#     increment_counter();
#   */
#   int bar(const char *text);
#
# The generated wrappers require linker options such as:
#
#   -Wl,--wrap=foo -Wl,--wrap=bar
#
# Important:
#   This generator expects one function declaration per logical line and
#   assumes that each parameter has a usable identifier from which a call
#   argument can be derived. Fully general C declarations require an AST-based
#   parser such as Clang.

set -eu

if [ "$#" -lt 2 ]; then
    printf 'Usage: TABWIDTH=N %s input.h output.c\n' "$0" >&2
    exit 2
fi

HEADER=$1
OUTPUT=$2
WRAPLIST="$3"

# Configurable tab width (default to 2 spaces)
TABWIDTH=${TABWIDTH:-2}

case "$TABWIDTH" in
    ''|*[!0-9]*|0)
        printf 'TABWIDTH must be a positive integer\n' >&2
        exit 2
        ;;
esac

INCLUDE_HEADERS=""

# Extract included header files if now explicitly provided as arguments
shift 3
[[ $# -gt 0 && "$1" =~ ^(-i|--include)$ ]] && \
    {
        shift
        INCLUDE_HEADERS="$@"
    }

[[ -z "$INCLUDE_HEADERS" ]] && \
    INCLUDE_HEADERS="$(grep -Po "(?<=#include [<\"])(.*)(?=[>\"])" "$HEADER")"

# Use only the basename in the generated include. This avoids embedding
# an absolute path in the generated file.
HEADER_NAME=$(basename "$HEADER")

# Insert auto-generated file message
# Insert header file provided as argument
cat > "$OUTPUT" <<EOF
/*
 * Automatically generated wrapper source.
 * Do not edit manually.
 */

#include <$HEADER_NAME>
EOF

# Insert header files included in provided header file
[[ -n "$INCLUDE_HEADERS" ]] && \
    while read -r header
    do
        [[ "$header" != "$HEADER" ]] && \
            echo "#include <"$header">" >> "$OUTPUT"
    done <<< "$INCLUDE_HEADERS"

gawk -v tabwidth="$TABWIDTH" -v WRAPLIST="$WRAPLIST" '
###############################################################################
# Utility functions
###############################################################################

function spaces(n,    result) {
    result = ""

    while (n-- > 0)
        result = result " "

    return result
}

function trim(text) {
    sub(/^[ \t]*/, "", text)
    sub(/[ \t]*$/, "", text)
    return text
}

function indentation_width(text,    i, ch, width) {
    width = 0

    for (i = 1; i <= length(text); i++) {
        ch = substr(text, i, 1)

        if (ch == " ")
            width++
        else if (ch == "\t")
            width += tabwidth
        else
            break
    }

    return width
}

function remove_indentation(text, amount,    i, ch, width) {
    i = 1
    width = 0

    while (i <= length(text) && width < amount) {
        ch = substr(text, i, 1)

        if (ch == " ") {
            width++
            i++
        } else if (ch == "\t") {
            width += tabwidth
            i++
        } else {
            break
        }
    }

    return substr(text, i)
}

function clear_prologue(    i) {
    for (i = 1; i <= prologue_count; i++)
        delete prologue[i]

    prologue_count = 0
    prologue_indent = -1
}

function add_prologue_line(line,    width) {
    # Remove the closing comment delimiter, if present.
    sub(/[ \t]*\*\/[ \t]*$/, "", line)

    if (trim(line) == "")
        return

    width = indentation_width(line)

    if (prologue_indent < 0 || width < prologue_indent)
        prologue_indent = width

    prologue[++prologue_count] = line
}

function emit_prologue(    i, line) {
    for (i = 1; i <= prologue_count; i++) {
        line = remove_indentation(prologue[i], prologue_indent)
        printf "%s%s\n", spaces(tabwidth), line
    }

    clear_prologue()
}

###############################################################################
# C-text parsing helpers
###############################################################################

# Find the matching closing parenthesis for the opening parenthesis at pos.
# This handles nested parentheses in attributes and parameter declarations.
function matching_paren(text, pos,    i, ch, depth, quote, escaped) {
    depth = 0
    quote = ""
    escaped = 0

    for (i = pos; i <= length(text); i++) {
        ch = substr(text, i, 1)

        if (quote != "") {
            if (escaped) {
                escaped = 0
            } else if (ch == "\\") {
                escaped = 1
            } else if (ch == quote) {
                quote = ""
            }

            continue
        }

        if (ch == "\"" || ch == "\047") {
            quote = ch
        } else if (ch == "(") {
            depth++
        } else if (ch == ")") {
            depth--

            if (depth == 0)
                return i
        }
    }

    return 0
}

# Split a parameter list on top-level commas only.
function split_parameters(text, result,    i, ch, depth, start, count) {
    delete result

    text = trim(text)

    if (text == "" || text == "void")
        return 0

    depth = 0
    start = 1
    count = 0

    for (i = 1; i <= length(text); i++) {
        ch = substr(text, i, 1)

        if (ch == "(" || ch == "[" || ch == "{")
            depth++
        else if (ch == ")" || ch == "]" || ch == "}")
            depth--
        else if (ch == "," && depth == 0) {
            result[++count] = trim(substr(text, start, i - start))
            start = i + 1
        }
    }

    result[++count] = trim(substr(text, start))
    return count
}

# Derive a call argument from a simple parameter declaration.
#
# Examples:
#   int value              -> value
#   const char *text       -> text
#   unsigned long count    -> count
#   int values[4]          -> values
#
# Complex declarators, especially function-pointer parameters, cannot be
# reliably handled by regular expressions alone.
function parameter_name(parameter,    text, n, words, candidate) {
    text = parameter

    # Remove common GNU/MS attributes.
    gsub(/__attribute__[ \t]*\(\([^()]*\)\)/, "", text)
    gsub(/__declspec[ \t]*\([^()]*\)/, "", text)

    text = trim(text)

    # Remove an array suffix.
    sub(/[ \t]*\[[^]]*\][ \t]*$/, "", text)

    # Find the final identifier.
    n = split(text, words, /[^A-Za-z0-9_$]+/)
    candidate = words[n]

    if (candidate ~ /^[A-Za-z_][A-Za-z0-9_]*$/)
        return candidate

    return ""
}

###############################################################################
# Main processing
###############################################################################

BEGIN {
    clear_prologue()
    in_prologue = 0
}

{
    line = $0

    # Continue a multiline wrapper prologue.
    if (in_prologue) {
        if (index(line, "*/") != 0) {
            before_close = substr(line, 1, index(line, "*/") - 1)
            add_prologue_line(before_close)
            in_prologue = 0
        } else {
            add_prologue_line(line)
        }

        next
    }

    # Find the wrapper-prologue marker.
    marker = index(line, "function_wrapper_prologue:")

    if (marker != 0) {
        rest = substr(line, marker + length("function_wrapper_prologue:"))


        close_pos = index(rest, "*/")

        if (close_pos != 0) {
            add_prologue_line(substr(rest, 1, close_pos - 1))
        } else {
            add_prologue_line(rest)
            in_prologue = 1
        }

        next
    }

    # Ignore preprocessor lines, typedefs, and static functions.
    if (line ~ /^[ \t]*#/)
        next

    if (line ~ /^[ \t]*typedef([ \t]|$)/)
        next

    if (line ~ /^[ \t]*static([ \t]|$)/)
        next

    # Only process declarations ending in semicolon.
    if (line !~ /;[ \t]*$/)
        next

    declaration = line
    sub(/[ \t]*;[ \t]*$/, "", declaration)

    open_pos = index(declaration, "(")

    if (open_pos == 0)
        next

    close_pos = matching_paren(declaration, open_pos)

    if (close_pos == 0)
        next

    prefix = substr(declaration, 1, open_pos - 1)
    params = substr(declaration, open_pos + 1, close_pos - open_pos - 1)

    suffix = substr(declaration, close_pos + 1)

    # The function name is the final identifier before the opening paren.
    if (match(prefix, /[A-Za-z_][A-Za-z0-9_]*[ \t]*$/, matched) == 0)
        next

    func_name = matched[0]
    sub(/[ \t]*$/, "", func_name)

    name_pos = length(prefix) - length(matched[0]) + 1
    return_part = substr(prefix, 1, name_pos - 1)
    return_part = trim(return_part)

    # Do not accidentally process function-pointer typedef-like declarations.
    if (func_name == "")
        next

    # Skip function is prologue has not been set
    if (prologue_count == 0) {
        next
    }

    # Construct the call argument list.
    parameter_count = split_parameters(params, parameter)

    call_args = ""

    for (i = 1; i <= parameter_count; i++) {
        argument_name = parameter_name(parameter[i])

        if (argument_name == "") {
            printf "warning: cannot derive argument name for %s: %s\n", \
                func_name, parameter[i] > "/dev/stderr"

            call_args = ""
            invalid_arguments = 1
            break
        }

        if (i > 1)
            call_args = call_args ", "

        call_args = call_args argument_name
    }

    if (invalid_arguments) {
        invalid_arguments = 0
        clear_prologue()
        next
    }

    # A return type is void only when it contains the standalone token void.
    # This correctly treats void* as a non-void return type.
    is_void = (return_part ~ /(^|[ \t*])void([ \t*]|$)/ && \
        return_part !~ /void[ \t]*\*/)


    # Output linker flags to wrap symbol
    printf "-Wl,--wrap=%s\n", func_name > WRAPLIST

    print ""

    # Preserve the declaration suffix, including attributes.
    printf "extern %s __real_%s(%s)%s;\n", return_part, func_name, params, suffix

    # Output wrap definition
    printf "%s __wrap_%s(%s)%s {\n", return_part, func_name, params, suffix

    # Insert the captured prologue if it exists
    emit_prologue()

    if (is_void) {
        printf "%s__real_%s(%s);\n", spaces(tabwidth), func_name, call_args

    } else {
        printf "%sreturn __real_%s(%s);\n", spaces(tabwidth), func_name, call_args

    }

    print "}"
}

END {
    if (in_prologue)
        print "warning: unterminated function_wrapper_prologue comment" \
            > "/dev/stderr"

}
' "$HEADER" >> "$OUTPUT"
