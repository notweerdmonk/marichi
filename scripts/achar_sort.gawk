#!/usr/bin/env -S gawk -f

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


BEGIN {
   FS=","

   # C/C++ identifier.
   ident = "[_[:alpha:]][_[:alnum:]]*"

   # A permissive GNU/Clang attribute expression.
   #
   # This handles common forms such as:
   #   __attribute__((progmem))
   #   __attribute__((section(".foo")))
   #   __attribute__((aligned(4)))
   #
   # It is intentionally permissive because gawk cannot balance
   # arbitrarily nested parentheses.
   attr = "__attribute__[[:space:]]*\\(\\([^;{}]*\\)\\)"

   # Common qualifiers and compiler-specific qualifiers.
   qualifier = "(const|volatile|restrict|static|extern|register|mutable|"
   qualifier = qualifier "inline|virtual|explicit|friend|constexpr|"
   qualifier = qualifier "consteval|constinit|PROGMEM|__flash|__far|"
   qualifier = qualifier "__near|__memx|__persistent)"

   # Common declaration/type tokens.
   #
   # This is deliberately broad so that it accepts:
   #   uint8_t
   #   unsigned long
   #   Foo::Bar
   #   std::array<int, 4>
   #   char *
   type_token = "([_[:alnum:]:<>.,*&~]+)"

   # An array declarator. This is the important part: detect the actual
   # identifier followed by brackets, independently of preceding attributes
   # and qualifiers.
   array_declarator = ident "[[:space:]]*\\[[^][]*\\]"

   # A complete array definition/declaration before its initializer.
   #
   # Use this against the text before '=' rather than trying to parse the
   # entire declaration prefix.
   array_decl = array_declarator

   # Optional simple template arguments after a function/object name.
   template_args = "(<[[:space:]]*[_[:alnum:]:<>,*&[:space:]]+>)?"

   # One component of a qualified callable expression:
   #
   #   foo
   #   ns::foo
   #   object.foo
   #   object->foo
   #   ns::Vector<int>::push_back
   callable_part = ident template_args

   # Function-call expression.
   #
   # Matches:
   #   foo(...)
   #   ns::foo(...)
   #   object.method(...)
   #   ptr->method(...)
   #   Vector<int>::push_back(...)
   call = callable_part \
          "([[:space:]]*(::|\\.|->)[[:space:]]*" callable_part ")*" \
          "[[:space:]]*\\([^;{}]*\\)"

   # Keywords that look like calls but are control statements.
   control = "^(if|for|while|switch|catch|sizeof|decltype|alignof)$"

   # Closing brace
   braces = "^[[:space:]]*}[[:space:]]*;"

   array_decl_line = ""
   array_decl_closing_line = ""
   count = 1
}

$0 ~ array_decl {
    array_decl_line = $0
    next
}

$0 ~ braces {
    array_decl_closing_line = $0
    next
}

{
    for (i = 1; i <= NF; ++i) {
        if (!length($i))
            continue
        spaces[count] = gensub(/[^[:space:]]+/, "", "g", $i)
        gsub(/[[:space:]]+/, "", $i)
        fields[count] = $i
        ++count
    }
}

END {
    printf "%s\n", array_decl_line

    width = 0
    OFS = ""
    ORS = ","
    n = asort(fields, fields, "@val_str_asc")
    for (i = 1; i <= n; ++i) {
        width += length(spaces[i]) + length(fields[i]) + 1
        if (width >= 80) {
            printf "\n"
            width = 0
        }
        print spaces[i], fields[i]
    }
    printf "\n"

    printf "%s\n", array_decl_closing_line
}
