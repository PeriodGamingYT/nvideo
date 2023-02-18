#include <nvideo/output/terminal.h>
#include <stdio.h>
#include <nvideo/nvideo.h>
#include <sys/ioctl.h>
#include <unistd.h>

struct nvideo_size nvideo_terminal_size() {
	struct winsize size;
	struct nvideo_size result = {
		.width = -1,
		.height = -1
	};
	
	if(
		ioctl(
			STDOUT_FILENO,
			TIOCGWINSZ,
			&size
		) != -1 ||
		size.ws_col == 0
	) {
		result.width = size.ws_col;
		result.height = size.ws_row;
	}

	return result;
}

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

struct nvideo_output nvideo_terminal_make() {
	struct nvideo_output result;
	result.set = terminal_set;
	result.get = NULL;
	return result;
}
