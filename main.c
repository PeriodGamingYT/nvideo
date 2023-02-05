// This code uses local headers, but in practice you should use
// brackets in nvideo include statements, if it's installed.
// This file is meant to act as a test for the whole software suite.
#include "nvideo/nvideo.h"
#include "nvideo/output/dump.h"

int main() {
	struct nvideo_frame *frame = nvideo_frame_make(2, 2);
	nvideo_set(frame, 0, 0, nvideo_color_make(255, 255, 255));
	nvideo_merge(frame);
	struct nvideo_output *dump = nvideo_dump_make();
	nvideo_add_frame(dump, frame);
	nvideo_process(dump);
	nvideo_output_free(dump);
	nvideo_frame_free(frame);
	return 0;
}
