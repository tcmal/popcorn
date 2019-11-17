#include <stdio.h>
#include <stdbool.h>

#include <kernel/tty.h>
#include <kernel/paging.h>
#include <string.h>

void kernel_main(void) {
	tty_init();
	setupPaging();

	// char buf[4];
	// for (unsigned int i = 0; i < 50; i++) {
	// 	int_to_str(i, buf, 4);
	// 	tty_writeString(buf);
	// 	tty_nextRow();
	// }

	while (true) {};
}
