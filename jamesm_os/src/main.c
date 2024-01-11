// main.c -- Defines the C-code kernel entry point, calls initialisation
// routines.
//           Made for JamesM's tutorials <www.jamesmolloy.co.uk>

#include "descriptor_tables.h"
#include "monitor.h"
#include "stdio.h"

char msg[] = "welcome to os world!";

int main() {
    // Initialise the screen (by clearing it)
    monitor_clear();
    // Write out a sample string
    printf("%s\n", msg);

    asm volatile("xchgw %bx, %bx");

    init_descriptor_tables();

    asm volatile("int $0x3");
    asm volatile("int $0x4");
    return 0;
}
