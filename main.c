#include <nvideo/nvideo.h>
#include <nvideo/output/terminal.h>

int main() {
	struct nvideo_frame *frame = nvideo_frame_make(80, 24);
	nvideo_set(frame, 0, 0, nvideo_color_make(255, 255, 255));
	nvideo_swap(frame);
	nvideo_merge(frame);
	struct nvideo_output terminal = nvideo_terminal_make();
	nvideo_output_frame(terminal, frame);
	nvideo_frame_free(frame);
	return 0;
}
