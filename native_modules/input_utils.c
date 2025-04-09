#include <bits/time.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "display.h"
#include "tablet.h"
#include "input_utils.h"

void print_position_buffer(struct position_buffer *pb) {
  printf("[");
  for (unsigned int i = 0; i < pb->capacity; i++) {
    printf("%d,", pb->items[i]);
  }
  printf("]\n");
}

void create_position_buffer(struct position_buffer *pb, unsigned int capacity) {
  pb->next_index = 0;
  pb->capacity = capacity;
  pb->items = calloc(capacity, sizeof(int));
}

void position_buffer_insert_val(struct position_buffer *pb, int val, unsigned int index_incr) {
  pb->items[pb->next_index % pb->capacity] = val;
  pb->next_index = (pb->next_index + index_incr) % pb->capacity;
}

void reset_position_buffer(struct position_buffer *pb) {
  for (unsigned int i = 0; i < pb->capacity; i++)
    pb->items[i] = -1;
}

void parse_tablet_buffer_avg(int buffer_fd, int tablet_fd, struct tablet_config tablet, struct display_config display) {
  int x = 0;
  int y = 0;
  double x_scaled = 0;
  double y_scaled = 0;

  ssize_t r;
  int active = 1;

  uint8_t buf[64];
  memset(buf, 0x0, sizeof(buf));

  struct position_buffer x_position_buf;
  struct position_buffer y_position_buf;

  unsigned int position_buffer_capacity = 8;
  unsigned int incr = 1;

  create_position_buffer(&x_position_buf, position_buffer_capacity);
  create_position_buffer(&y_position_buf, position_buffer_capacity);

  while (active) {
    r = read(buffer_fd, buf, 16);

    if (r < 0) {
      perror("\nread err");
      exit(EXIT_FAILURE);
    }

    DEBUG_REPORT(buf, r);

    if (buf[1] == 0x80) {
      reset_position_buffer(&x_position_buf);
      reset_position_buffer(&y_position_buf);
      // printf("out of range, setting position buffer values to -1\n");
      continue;
    }

    if (buf[0] <= 0x10) {
      x = (buf[tablet.xindex]) | ((buf[tablet.xindex + 1]) << 8);
      y = (buf[tablet.yindex]) | ((buf[tablet.yindex + 1]) << 8);

      x_scaled = (x - tablet.left) * tablet.xscale;
      y_scaled = (y - tablet.top) * tablet.yscale;

      position_buffer_insert_val(&x_position_buf, (int)x_scaled, incr);
      position_buffer_insert_val(&y_position_buf, (int)y_scaled, incr);

      print_position_buffer(&x_position_buf);

      int x_avg_position = 0;
      int y_avg_position = 0;
      int x_div = 0;
      int y_div = 0;

      for (unsigned int i = 0; i < position_buffer_capacity; i++) {
        if (x_position_buf.items[i] > 0) {
          x_avg_position += x_position_buf.items[i];
          x_div++;
        }

        if (y_position_buf.items[i] > 0) {
          y_avg_position += y_position_buf.items[i];
          y_div++;
        }
      }

      if (x_div > 0)
        x_avg_position /= x_div;
      if (y_div > 0)
        y_avg_position /= y_div;

      if (area_boundary_clamp(display.primary_width, display.primary_height, &x_scaled, &y_scaled)) {
        tabletbtn_input_event(tablet_fd, x_avg_position + display.offset_x, y_avg_position + display.offset_y, buf[tablet.bindex]);
      }
    }
  }
}

#define BASE_REPORT_RATE 133
#define MULTIPLIER 8
#define SLEEP_SCHED_ADJUSTMENT 2.0 / MULTIPLIER

void parse_tablet_buffer_interpolated_mult(int buffer_fd, int tablet_fd, struct tablet_config tablet, struct display_config display) {
  int x = 0;
  int y = 0;
  double x_scaled = 0;
  double y_scaled = 0;
  int x_prev = -1;
  int y_prev = -1;
  double x_interpolated_values[MULTIPLIER];
  double y_interpolated_values[MULTIPLIER];

  // clock_nanosleep with absolute time might avoid time drift when calling nanosleep very often
  double double_report_delay = (((1000.0 / BASE_REPORT_RATE) / (MULTIPLIER + 1.0)) - (SLEEP_SCHED_ADJUSTMENT)) * 1000000;
  printf("report delay: %f\n", double_report_delay);

  ssize_t r;
  int active = 1;

  uint8_t buf[64];
  memset(buf, 0x0, sizeof(buf));

  while (active) {
    // if using open without O_SYNC possibly busy wait/sleep here and then handle read ret value
    r = read(buffer_fd, buf, 16);

    if (r < 0) {
      perror("\nread err");
      exit(EXIT_FAILURE);
      // continue;
    }

    // hardcoded for 480 values
    if (buf[0] == 0xc0 || buf[1] <= 0x80)
      continue;

    DEBUG_REPORT(buf, r);

    if (buf[0] <= 0x10) {
      x = (buf[tablet.xindex]) | ((buf[tablet.xindex + 1]) << 8);
      y = (buf[tablet.yindex]) | ((buf[tablet.yindex + 1]) << 8);

      x_scaled = (x - tablet.left) * tablet.xscale;
      y_scaled = (y - tablet.top) * tablet.yscale;

      if (x_prev != -1) {

        double x_diff = x_scaled - x_prev;
        double x_split = x_diff / (MULTIPLIER + 1);

        double y_diff = y_scaled - y_prev;
        double y_split = y_diff / (MULTIPLIER + 1);

        for (int i = 0; i < MULTIPLIER; i++) {
          double x_inp = x_prev + (x_split * (i + 1));
          double y_inp = y_prev + (y_split * (i + 1));
          x_interpolated_values[i] = x_inp;
          y_interpolated_values[i] = y_inp;

          if (area_boundary_clamp(display.primary_width, display.primary_height, &x_interpolated_values[i], &y_interpolated_values[i]))
            tabletbtn_input_event(tablet_fd, (int)x_interpolated_values[i] + display.offset_x, (int)y_interpolated_values[i] + display.offset_y,
                                  buf[tablet.bindex]); // 0 for buttons for interpolated reports?

#ifdef ABSOLUTE_SLEEP_TIMERS
          struct timespec ts = {0};
          clock_gettime(CLOCK_MONOTONIC, &ts);
          ts.tv_nsec += (long)(double_report_delay);

          if (ts.tv_nsec >= 1000000000) {
            printf("normalized timespec");
            ts.tv_nsec -= 1000000000;
            ts.tv_sec++;
          }

          int n = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &ts, NULL);
#else
          struct timespec ts_delay = {.tv_sec = 0, .tv_nsec = (long)(double_report_delay)};
          nanosleep(&ts_delay, NULL);
#endif
        }

        if (area_boundary_clamp(display.primary_width, display.primary_height, &x_scaled, &y_scaled))
          tabletbtn_input_event(tablet_fd, (int)x_scaled + display.offset_x, (int)y_scaled + display.offset_y, buf[tablet.bindex]);

      } else {
        if (area_boundary_clamp(display.primary_width, display.primary_height, &x_scaled, &y_scaled))
          tabletbtn_input_event(tablet_fd, (int)x_scaled + display.offset_x, (int)y_scaled + display.offset_y, buf[tablet.bindex]);
      }
      x_prev = (int)x_scaled;
      y_prev = (int)y_scaled;
      continue;
    }

    x_prev = -1;
    y_prev = -1;
  }
}
