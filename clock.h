#include <stdio.h>
#include <time.h>

struct clock {
  char command[200];
};

void clock_init(struct clock* clock) {
  snprintf(clock->command, 200, "");
}

void clock_update(struct clock* clock) {
  time_t t = time(NULL);
  struct tm* tm = localtime(&t);

  char time[100];
  strftime(time, 100, "%H:%M", tm);

  char date[100];
  strftime(date, 100, "%a %d. %b", tm);

  snprintf(clock->command, 200, "--set calendar icon=\"%s\" label=\"%s\"", date, time);
}
