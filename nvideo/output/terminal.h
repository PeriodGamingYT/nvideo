#ifndef __OUTPUT_TERMINAL_H
#define __OUTPUT_TERMINAL_H
struct nvideo_output;
struct nvideo_output nvideo_terminal_make();
struct nvideo_size {
	int width;
	int height;
};

struct nvideo_size nvideo_terminal_size();
#endif
