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
 * This program will recieve and send data to and from /dev/ttyUSB0 or specified
 * serial device.
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <sys/select.h>
#include <termios.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>

#define F_CPU 16000000L

#include <config.h>
#include <uart_config.h>
#include <loge/loge.hpp>

typedef enum _uart_parity {
  UART_PARITY_NONE,
  UART_PARITY_DISABLED,
  UART_PARITY_EVEN,
  UART_PARITY_ODD,
  UART_PARITY_MARK,
  UART_PARITY_SPACE
} uart_parity_t;

#define UART_PARITY UART_PARITY_NONE

#define STRINGIFY(s) #s
#define CONCAT(a, b) a ## b

#define SERDEV STRINGIFY(/dev/ttyUSB0)

#define N2BAUDRATE(baud) CONCAT(B, baud)

#define E2BAUDRATE(baud) \
  ({                     \
    speed_t speed;       \
    switch ((int)baud) { \
      case 50:           \
        speed = B50;     \
        break;           \
      case 75:           \
        speed = B75;     \
        break;           \
      case 110:          \
        speed = B110;    \
        break;           \
      case 134:          \
        speed = B134;    \
        break;           \
      case 150:          \
        speed = B150;    \
        break;           \
      case 200:          \
        speed = B200;    \
        break;           \
      case 300:          \
        speed = B300;    \
        break;           \
      case 600:          \
        speed = B600;    \
        break;           \
      case 1200:         \
        speed = B1200;   \
        break;           \
      case 1800:         \
        speed = B1800;   \
        break;           \
      case 2400:         \
        speed = B2400;   \
        break;           \
      case 4800:         \
        speed = B4800;   \
        break;           \
      case 9600:         \
        speed = B9600;   \
        break;           \
      case 19200:        \
        speed = B19200;  \
        break;           \
      case 38400:        \
        speed = B38400;  \
        break;           \
      case 0:            \
      default:           \
        speed = B0;      \
    }                    \
    speed;               \
  })

#define BUFLEN 256

static struct loge logger;

static int __stop = 0;

void sigalarm_handler(int sig __attribute__ ((unused))) {
  __stop = 1;
}

int setup_alarm(struct sigaction* p_old_sa) {
  if (!p_old_sa) {
    return -1;
  }

  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = sigalarm_handler;
  sa.sa_flags = 0;
  if (sigaction(SIGALRM, &sa, p_old_sa) == -1) {
    LOGE_COLOR(
        &logger,
        LOGE_ERROR,
        "Sigaction: setting handler for SIGALARM failed: %s",
        strerror(errno)
      );
    return -1;
  }

  return 0;
}

int cleanup_alarm(struct sigaction* p_old_sa) {
  if (!p_old_sa) {
    return -1;
  }

  if (sigaction(SIGALRM, p_old_sa, 0) == -1) {
    LOGE_COLOR(
        &logger,
        LOGE_ERROR,
        "Sigaction: restoring handler for SIGALARM failed: %s",
        strerror(errno)
      );
    return -1;
  }
  return 0;
}

/* Returns malloced string */
char* strtohex(const char *str) {
  if (!str) {
    return NULL;
  }

  int len = strlen(str);
  char *hexstr = (char*)malloc((2 + len * 2 + 2) * sizeof(char));

  sprintf(hexstr, "0x");
  for (int i = 0, j = 0; i < len; ++i, j += 2) {
    sprintf(hexstr + 2 + j, "%02x", str[i] & 0xff);
  }

  return hexstr;
}

char* parity_type_to_str(int parity) {

  if (parity == UART_PARITY_NONE) {
    return "None";

  } else if (parity == UART_PARITY_EVEN) {
    return "Even";

  } else if (parity == UART_PARITY_ODD) {
    return "Odd";

  } else if (parity == UART_PARITY_MARK) {
    return "Mark";

  } else if (parity == UART_PARITY_SPACE) {
    return "Space";
  }

  return "None";
}

int get_serial_device(int argc, char *argv[], char *buffer, size_t len) {

  int c;
  while ((c = getopt(argc, argv, ":hd:")) != -1) {
    switch (c) {
      case 'd':
        strncpy(buffer, optarg, len);
        break;

      case 'h':
        printf("Usage: %s [OPTION] ...\n", argv[0]);
        printf("  -h             Display this message\n");
        printf("  -d [device]    Open device as serial port\n");
        return -1;

      case '?':
        fprintf(stderr, "Invalid option: %c\n", optopt);
        return -1;

      case ':':
        fprintf(stderr, "Invalid option: %c requires an argument\n", optopt);
        return -1;

      default:
        return -1;
    }
  }

  if (c != 'd') {
    char *device_path = getenv("UART_DEVICE");
    if (device_path) {
      strncpy(buffer, device_path, len);
    }
  }

  return 0;
}

int open_serial_device(const char *device) {
  if (!device || !*device) {
    return -1;
  }

  LOGE_COLOR(&logger, LOGE_INFO, "Opening device: %s", device);

  int serdev = open(device, O_RDWR|O_NOCTTY);
  if (serdev == -1) {
    LOGE_COLOR(&logger, LOGE_ERROR, "Could not open device %s: %s", device,
        strerror(errno));
    return -1;
  }

  return serdev;
}

/* TODO: pass uart_config as argument */
int setup_serial_device(int serdev) {

  struct termios settings;
  tcgetattr(serdev, &settings);

  cfsetispeed(&settings, E2BAUDRATE(UART_BAUD_DEFAULT));
  cfsetospeed(&settings, E2BAUDRATE(UART_BAUD_DEFAULT));

  settings.c_cflag &= ~CSIZE;

  if (UART_CHAR_SIZE == 8) {
    settings.c_cflag |= CS8;
  }
  else if (UART_CHAR_SIZE == 7) {
    settings.c_cflag |= CS7;
  }

  if (UART_STOP_BITS < 2) {
    settings.c_cflag &= ~CSTOPB;
  }

  if (UART_PARITY > UART_PARITY_NONE) {
    settings.c_cflag |= PARENB;

    if (UART_PARITY == UART_PARITY_ODD) {
      settings.c_cflag |= PARODD;
    }

  } else {
    settings.c_cflag &= ~PARENB;
  }

  settings.c_cflag &= ~CRTSCTS;
  settings.c_cflag |= CREAD | CLOCAL;

  // SET RTS/CTS flow control
  //settings.c_cflag |= CRTSCTS;

  settings.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHONL | ISIG);
  
  // This tells the kernel to report termios changes as packets to the master.
  //settings.c_lflag |= EXTPROC;

  //settings.c_iflag &= ~(IXON | IXOFF | IXANY);
  settings.c_iflag &= ~(IXOFF | IXANY);
  settings.c_iflag |= IXON;

  settings.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);

  settings.c_oflag &= ~(OPOST | ONLCR);

  settings.c_cc[VMIN] = 0;
  settings.c_cc[VTIME] = 0;

  if (tcsetattr(serdev, TCSANOW, &settings) == -1) {
    LOGE_COLOR(&logger, LOGE_ERROR, "Failed to set serial port attributes!");
    close(serdev);
    return -1;
  }
  else {
    LOGE_COLOR(&logger, LOGE_INFO, "Serial port settings:");
    LOGE_COLOR(&logger, LOGE_INFO, "\t Baud rate: %d", UART_BAUD_DEFAULT);
    LOGE_COLOR(&logger, LOGE_INFO, "\t Character size: %d-bits", UART_CHAR_SIZE);
    LOGE_COLOR(&logger, LOGE_INFO, "\t Stop bit(s): %d", UART_STOP_BITS);
    LOGE_COLOR(&logger, LOGE_INFO, "\t Parity: %s", parity_type_to_str(UART_PARITY));
  }

  return 0;;
}

/* Define a structure to hold our serial file descriptor as the "cookie" */
typedef struct {
  int fd;
} serial_cookie;
/* Set a 2-second timeout for reading data */
time_t read_timeout = 2;

static
ssize_t serial_read(void *cookie, char *buf, size_t size) {
    serial_cookie *ctx = (serial_cookie *)cookie;
    fd_set read_fds;
    struct timeval timeout;

    if (size == 0) return 0;

    FD_ZERO(&read_fds);
    FD_SET(ctx->fd, &read_fds);

    timeout.tv_sec = read_timeout;
    timeout.tv_usec = 0;

    int ready = select(ctx->fd + 1, &read_fds, NULL, NULL, &timeout);

    if (ready < 0) {
        return -1; // Select error
    } else if (ready == 0) {
        return 0;  // Timeout (EOF condition for the stream)
    }

    // Data is ready, read it from the serial hardware descriptor
    return read(ctx->fd, buf, size);
}

static
ssize_t serial_write(void *cookie, const char *buf, size_t size) {
    serial_cookie *ctx = (serial_cookie *)cookie;
    fd_set write_fds;
    struct timeval timeout;

    if (size == 0) return 0;

    FD_ZERO(&write_fds);
    FD_SET(ctx->fd, &write_fds);

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    int ready = select(ctx->fd + 1, NULL, &write_fds, NULL, &timeout);

    if (ready <= 0) {
        return -1; // Timeout or select error
    }

    return write(ctx->fd, buf, size);
}

static
int serial_close(void *cookie) {
    serial_cookie *ctx = (serial_cookie *)cookie;
    int res = 0;
    if (ctx->fd >= 0) {
        res = close(ctx->fd);
    }
    free(ctx);
    return res;
}

int main(int argc, char *argv[]) {

  int serdev = -1;
  char buffer[BUFLEN];
  struct sigaction old_sa;

  loge_setup(
    &logger,                  /* struct loge *ploge */
    2048,                     /* size_t max_log_size */
    -1,                       /* int linenumwidth */
    -1,                       /* int wdith */
    -1,                       /* int precision */
    LOGE_TYPE(1, LOGE_ALL),   /* int log_type */
    NULL,                     /* FILE *file */
    NULL                      /* log_fn fn */
  );

  /* Copy default serial device path to buffer */
  strncpy(buffer, SERDEV, BUFLEN);

  if (get_serial_device(argc, argv, buffer, BUFLEN) == -1) {
    return -1;
  }
 
  if ( (serdev = open_serial_device(buffer)) == -1 ) {
    return -1;
  }

  setup_serial_device(serdev);

  /* ---- SETUP WRAPPER COOKIE STREAM ---- */
  serial_cookie *cookie = malloc(sizeof(serial_cookie));
  if (!cookie) {
    close(serdev);
    return EXIT_FAILURE;
  }
  cookie->fd = serdev;

  cookie_io_functions_t serial_funcs = {
    .read  = serial_read,
    .write = serial_write,
    .seek  = NULL,
    .close = serial_close
  };

  FILE *serial_stream = fopencookie(cookie, "r+", serial_funcs);
  if (!serial_stream) {
    perror("fopencookie failed");
    free(cookie);
    close(serdev);
    return EXIT_FAILURE;
  }

  /* Disable desktop caching/buffering so communications are instant */
  setvbuf(serial_stream, NULL, _IONBF, 0);

  fflush(stdout);

  stdout = serial_stream;
  stdin  = serial_stream;

#ifndef __SIMTEST
  /* Discard any data received over serial port */
  tcflush(serdev, TCIFLUSH);
#endif

  if (setup_alarm(&old_sa) == -1) {
    return -1;
  }

  LOGE_COLOR(&logger, LOGE_INFO, "%s", "Waiting for MCU...");

  int status = 0;
  for (char *ptr = "parser\n", c; !status && fread(&c, 1, 1, stdin);) {
    if (*ptr == c) {
      if (!*(++ptr)) {
        status = 1;
      }
    }
  } 

  if (status) {
    LOGE_COLOR(&logger, LOGE_INFO, "MCU is ready to recieve data");
    read_timeout = 1;

    const char payload[] = "Not ge\xff\x02ting the nu\xff\x02get; Y\xff\016et!\x4";

    fwrite(payload, sizeof(payload) - 1, 1, stdout);

    char *buf = NULL;
    size_t len = 0;
    ssize_t nread = 0;
    while((nread = getline(&buf, &len, stdin)) != -1) {
      buf[--nread] == '\n' && (buf[nread] = '\0');
      LOGE_COLOR(&logger, LOGE_INFO, "%s", buf);
    }
    if (buf) {
      free(buf);
    }
  }

  cleanup_alarm(&old_sa);

  close(serdev);

  return 0;
}
