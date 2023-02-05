#include "terminal.h"
#include <stdio.h>
#include "../nvideo.h"

static void terminal_set(
	int x,
	int y,
	struct nvideo_color color
) {
	printf(
		"\x1b[%d;%df\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm#\x1b[0m", 
		y + 1,
		x + 1,
		color.r,
		color.g,
		color.b,
		color.r,
		color.g,
		color.b
	);
}

struct nvideo_output *nvideo_terminal_make() {
	struct nvideo_output *result = nvideo_output_make();
	result->set = terminal_set;
	result->get = NULL;
	result->color_length = 3;
}
