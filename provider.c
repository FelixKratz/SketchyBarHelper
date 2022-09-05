#include <Carbon/Carbon.h>
#include "cpu.h"
#include "clock.h"
#include "sketchybar.h"

struct cpu g_cpu;
struct clock g_clock;

void handler() {
  cpu_update(&g_cpu);
  clock_update(&g_clock);

  if (strlen(g_cpu.command) > 0 && strlen(g_clock.command) > 0) {
    char command[300];
    snprintf(command, 300, "%s %s", g_cpu.command, g_clock.command);
    sketchybar(command);
  }
}

int main (int argc, char** argv) {
  cpu_init(&g_cpu);
  clock_init(&g_clock);

  CFRunLoopTimerRef timer = CFRunLoopTimerCreate(NULL,
                                                 CFAbsoluteTimeGetCurrent(),
                                                 2.,
                                                 0,
                                                 0,
                                                 handler,
                                                 NULL                        );

  CFRunLoopAddTimer(CFRunLoopGetMain(), timer, kCFRunLoopCommonModes);
  CFRunLoopRun();
  return 0;
}
