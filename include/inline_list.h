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

/**
 * @file inline_list.h
 * @author notweerdmonk
 * @brief header file for inline linked list
 */

#ifndef _INLINE_LIST_
#define _INLINE_LIST_

struct list_node { struct list_node *prev, *next; };

#define INLINE_LIST(name) struct list_node name

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
  struct list_node name = LIST_HEAD_INIT(name)

static inline void LIST_HEAD_DETACH(struct list_node *head) {
  head->next = head;
  head->prev = head;
}

static inline void _list_add(struct list_node *prev,
                             struct list_node *next,
                             struct list_node *new) {
  prev->next = new;
  new->prev = prev;
  new->next = next;
  next->prev = new;
}

static inline void _list_del(struct list_node *prev,
                             struct list_node *next) {
  prev->next = next;
  next->prev = prev;
}

static inline void list_append(struct list_node *head,
                               struct list_node *new) {
  _list_add(head, head->next, new);
}

static inline void list_prepend(struct list_node *head,
                                struct list_node *new) {
  _list_add(head->prev, head, new);
}

static inline void list_delete(struct list_node *head) {
  _list_del(head->prev, head->next);
}

static inline void list_replace(struct list_node *old, struct list_node *new) {
  new->prev = old->prev;
  new->prev->next = new;
  new->next = old->next;
  new->next->prev = new;
}

#endif /* _INLINE_LIST_ */
