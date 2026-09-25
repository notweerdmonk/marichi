/*
 * marichi - cooperative kernel for AVR (R) Mega microcontrollers
 * Copyright (C) 2026  notweerdmonk
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef PORT_H
#define PORT_H

/**
 * @file port.h
 * @author notweerdmonk
 * @brief Portability layer index header
 */

#if defined (__AVR_ATmega32__)

/* For absolute pin numbering see mega32/port_mega32.h */
#include "mega32/port_mega32.h"

#elif defined (__AVR_ATmega328P__)

/* For absolute pin numbering see mega328p/port_mega328p.h */
#include "mega328p/port_mega328p.h"

#endif

#endif /* PORT_H */
