#include "nvideo.h"
#include <stdlib.h>

struct nvideo_single_frame *nvideo_single_frame_make(
	int width,
	int height
) {
	struct nvideo_single_frame *frame =
		(struct nvideo_single_frame *) malloc(
			sizeof(struct nvideo_single_frame)
		);

	frame->width = width;
	frame->height = height;
	int frame_size =
		sizeof(unsigned char) * 
		width *
		height *
		NVIDEO_COLOR_AMOUNT;
	
	frame->front = (unsigned char *) malloc(frame_size);
	frame->back = (unsigned char *) malloc(frame_size);
	frame->x = 0;
	frame->y = 0;
	return frame;
}

void nvideo_single_frame_free(
	struct nvideo_single_frame *frame
) {
	free(frame->front);
	free(frame->back);
	free(frame);
}

static int get_frame_index(
	struct nvideo_single_frame frame,
	int x,
	int y
) {
	return ((y * frame.width) + x) * NVIDEO_COLOR_AMOUNT;
}

void nvideo_set(
	struct nvideo_single_frame *frame,
	int x,
	int y,
	unsigned char *color
) {
	int index = get_frame_index(*frame, x, y);
	for(int i = 0; i < NVIDEO_COLOR_AMOUNT; i++) {
		frame->back[index + i] = color[i];
	}
}

struct nvideo_color nvideo_get(
	struct nvideo_single_frame *frame,
	int x,
	int y
) {
	unsigned char color[NVIDEO_COLOR_AMOUNT] = { 0 };
	int index = get_frame_index(*frame, x, y);
	for(int i = 0; i < NVIDEO_COLOR_AMOUNT; i++) {
		color[i] = frame->back[index + i];
	}

	struct nvideo_color result;
	result.r = color[0];
	result.g = color[1];
	result.b = color[2];
	return result;
}

void nvideo_frame_free(
	struct nvideo_frame *frame
) {
	nvideo_single_frame_free(frame->self);
	for(int i = 0; i < frame->children_length; i++) {
		nvideo_frame_free(frame->children[i]);
	}
	
	if(frame->merged_result != NULL) {
		nvideo_single_frame_free(frame->merged_result);
	}

	free(frame);
}


struct nvideo_frame *nvideo_frame_make(
	int width,
	int height
) {
	struct nvideo_frame *result = 
		malloc(sizeof(struct nvideo_frame));

	result->self = nvideo_single_frame_make(width, height);
	result->children = (struct nvideo_frame **)
		malloc(sizeof(struct nvideo_frame *));

	result->children_length = 0;
	result->merged_result = NULL;
	return result;
}

static void write_to_frame(
	struct nvideo_single_frame *dest,
	struct nvideo_single_frase *src
) {
	
}

void nvideo_merge(
	struct nvideo_frame *frame
) {
	if(frame->merged_result != NULL) {
		free(frame->merged_result);
		frame->merged_result = NULL;
	}
	
	write_to_frame(frae->merged_result, frame->self);
	
}

void nvideo_add_child(
	struct nvideo_frame *frame,
	struct nvideo_frame *child
) {
	frame->children_length++;
	frame->children = (struct nvideo_frame **) realloc(
		frame->children,
		sizeof(struct nvideo_frame *) * (
			frame->children_length
		)
	);

	frame->children[frame->children_length - 1] = child;
}

void nvideo_output_free(
	struct nvideo_output *output
) {
	free(output->queue);
	free(output);
}

struct nvideo_output *nvideo_output_make() {
	struct nvideo_output *result =(struct nvideo_output *)
		malloc(sizeof(struct nvideo_output));

	result->queue = (unsigned char*)
		malloc(sizeof(unsigned char));

	return result;
}

void nvideo_output_pixel(
	struct nvideo_output *output, 
	int y, 
	int x, 
	unsigned char *color
) {
	// TODO
}

void nvideo_output_single_frame(
	struct nvideo_output *output, 
	struct nvideo_single_frame *frame
) {
	// TODO
}

void nvideo_output_frame(
	struct nvideo_output *output, 
	struct nvideo_frame *frame
) {
	// TODO
}
