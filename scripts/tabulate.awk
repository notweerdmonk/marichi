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


# tabulate.awk - Tabulate text stream

BEGIN {
    FS = ":"
    h1 = (ARGC > 1 ? ARGV[1] : "file")
    h2 = (ARGC > 2 ? ARGV[2] : "line")
    h3 = (ARGC > 3 ? ARGV[3] : "text")

    delete ARGV[1]
    delete ARGV[2]
    delete ARGV[3]

    w1 = length(h1)
    w2 = length(h2)
    w3 = length(h3)
}

{
    line = $0

    sub(/^([^:]+):([0-9]+):[[:space:]]*\/?\*?[[:space:]]*TODO:[[:space:]]*/, "", line)
    sub(/[[:space:]]+\*?\/?[[:space:]]*$/, "", line)

    match($0, /^([^:]+):([0-9]+):/, m)
    file = m[1]
    num  = m[2]

    rows[++n] = file SUBSEP num SUBSEP line

    if (length(file) > w1) w1 = length(file)
    if (length(num)  > w2) w2 = length(num)
    if (length(line) > w3) w3 = length(line)
}

END {
    printf "| %-*s | %-*s | %-*s |\n", w1, h1, w2, h2, w3, h3

    printf "|"
    for (i = 0; i < w1 + 2; i++) printf "-"
    printf "|"
    for (i = 0; i < w2 + 2; i++) printf "-"
    printf "|"
    for (i = 0; i < w3 + 2; i++) printf "-"
    printf "|\n"

    for (i = 1; i <= n; i++) {
        split(rows[i], a, SUBSEP)
        printf "| %-*s | %-*s | %-*s |\n", w1, a[1], w2, a[2], w3, a[3]
    }
}
