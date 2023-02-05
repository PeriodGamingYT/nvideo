#include <nvideo/output/dump.h>
#include <stdio.h>
#include <nvideo/nvideo.h>

static void dump_set(
	int x,
	int y,
	struct nvideo_color color
) {
	printf(
		"X: %d, Y: %d, Color: (R: %d, G: %d, B: %d)\n",
		x,
		y,
		color.r,
		color.g,
		color.b
	);
}

struct nvideo_output nvideo_dump_make() {
	struct nvideo_output result;
	result.set = dump_set;
	result.get = NULL;
	return result;
}
