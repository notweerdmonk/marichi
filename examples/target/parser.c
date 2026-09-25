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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <util/delay.h>

#include <coroutine/coroutine.h>
#include <uart.h>

#define MAX_TOKEN_LEN 16

#define EOT 4

enum token_type {
  WORD,
  PUNCT
};

struct token {
  char tok[MAX_TOKEN_LEN];
  int len;
  enum token_type type;
} *ptoken = NULL;;

int tokidx = 0;

struct node {
  struct node *prev, *next;
  struct token *key;
} *head = NULL, *tail = NULL;

void add_to_token(int c) {
  if (!ptoken) {
    if ( !( ptoken = (struct token *)malloc(sizeof(struct token)) ) ) {
      return;
    }
  }

  if (tokidx >= MAX_TOKEN_LEN) {
    return;
  }

  ptoken->tok[tokidx++] = c;
}

void got_token(enum token_type type) {
  if (!ptoken) {
    return;
  }

  ptoken->type = type;
  ptoken->tok[tokidx] = '\0';
  ptoken->len = tokidx;
  tokidx = 0;

  struct node *new;
  if ( !( new = (struct node *)malloc(sizeof(struct node)) ) ) {
    return;
  }

  new->next = NULL;
  new->prev = tail;

  if (!head) {
    tail = head = new;
  } else {
    tail = tail->next = new;
  }

  tail->key = ptoken;

  ptoken = NULL;
}

void print_token_list(struct node *head) {
  while (head) {
    if (!head->key) {
      continue;
    }

    struct token *p = head->key;
    printf("Token type: %d Token: %s\n", p->type, p->tok);

    head = head->next;
  }
}

void destroy_token(struct token *ptoken) {
  if (!ptoken) {
    return;
  }

  free(ptoken);
}

void destroy_token_list(struct node *head) {
  while (head) {
    struct node *tmp = head;
    head = head->next;

    destroy_token(tmp->key);
    free(tmp);
  }
}

int decompressor(void) {
  static int repchar;
  static int replen;
  if (replen > 0) {
    replen--;
    return repchar;
  }
  int c = getchar();
  if (c == EOT)
    return EOT;
  if (c == 0xFF) {
    replen = getchar();
    repchar = getchar();
    replen--;
    return repchar;
  } else
    return c;
}

void parser(int c) {
  static enum {
    START, IN_WORD
  } state = START;
  switch (state) {
    case IN_WORD:
      if (isalpha(c)) {
        add_to_token(c);
        return;
      }
      got_token(WORD);
      state = START;
      /* fall through */

    case START:
      add_to_token(c);
      if (isalpha(c)) {
        state = IN_WORD;
      } else
        got_token(PUNCT);
      break;
  }
}

void* parser_coroutine(int c) {
    COROUTINE_BEGIN();
    while (1) {
        /* First char already in c */
        if (c == EOF)
            break;
        if (isalpha(c)) {
            do {
                add_to_token(c);
                COROUTINE_YIELD();
            } while (isalpha(c));
            got_token(WORD);
        }
        add_to_token(c);
        got_token(PUNCT);
        COROUTINE_YIELD();
    }
    COROUTINE_END();
}


int main() {

  PORT_ENABLE_GLOBAL_INTERRUPTS();

  uart_init(&(uart_config_t) {
   .baud_rate = 0,
   .char_size = 8,
   .stop_bits = 1,
   .parity = UART_PARITY_DISABLED
  });

  printf("parser\n");

  while (1) {
    int c = getchar();
    if (c == EOT) {
      break;
    }
    if (c == -1) {
      int len = getchar();
      c = getchar();
      while (len--)
        parser_coroutine(c);
    } else
      parser_coroutine(c);
  }

  print_token_list(head);

  _delay_ms(2000);

  destroy_token_list(head);

  return 0;
}
