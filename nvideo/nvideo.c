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

void nvideo_swap(
	struct nvideo_single_frame *frame
) {
	unsigned char *temp = frame->back;
	frame->back = frame->front;
	frame->front = temp;
}

static int get_frame_index(
	struct nvideo_single_frame frame,
	int x,
	int y
) {
	return ((y * frame.width) + x) * NVIDEO_COLOR_AMOUNT;
}

struct nvideo_color nvideo_color_make(
	unsigned char r,
	unsigned char g,
	unsigned char b
) {
	struct nvideo_color result = NVIDEO_COLOR_DECL(r, g, b);
	return result;
}

void nvideo_single_set(
	struct nvideo_single_frame *frame,
	int x,
	int y,
	struct nvideo_color arg_color
) {
	int index = get_frame_index(*frame, x, y);
	unsigned char color[NVIDEO_COLOR_AMOUNT] = {
		arg_color.r,
		arg_color.g,
		arg_color.b
	};
	
	for(int i = 0; i < NVIDEO_COLOR_AMOUNT; i++) {
		frame->back[index + i] = color[i];
	}
}

static struct nvideo_color nvideo_single_get_either(
	struct nvideo_single_frame *frame,
	int x,
	int y,
	int is_back
) {
	unsigned char color[NVIDEO_COLOR_AMOUNT] = { 0 };
	int index = get_frame_index(*frame, x, y);
	for(int i = 0; i < NVIDEO_COLOR_AMOUNT; i++) {
		unsigned char *data = is_back
			? frame->back
			: frame->front;
		
		color[i] = data[index + i];
	}

	struct nvideo_color result;
	result.r = color[0];
	result.g = color[1];
	result.b = color[2];
	return result;
}

struct nvideo_color nvideo_single_get(
	struct nvideo_single_frame *frame,
	int x,
	int y
) {
	return nvideo_single_get_either(frame, x, y, 1);
}

void nvideo_set(
	struct nvideo_frame *frame,
	int x,
	int y,
	struct nvideo_color arg_color
) {
	nvideo_single_set(frame->self, x, y, arg_color);
}

struct nvideo_color nvideo_get(
	struct nvideo_frame *frame,
	int x,
	int y
) {
	return nvideo_single_get(frame->self, x, y);
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
	struct nvideo_single_frame *src
) {
	for(int y = 0; y < src->height; y++) {
		for(int x = 0; x < src->width; x++) {
			int src_x = x + src->x;
			int src_y = y + src->y;
			if(
				src_x < 0 ||
				src_y < 0 ||
				src_x > dest->width ||
				src_y > dest->height
			) {
				continue;
			}

			int dest_index = get_frame_index(*dest, src_x, src_y);
			int src_index = get_frame_index(*src, x, y);
			dest->back[dest_index] = src->back[src_index];
		}
	}
}

void nvideo_merge(
	struct nvideo_frame *frame
) {
	if(frame->merged_result != NULL) {
		free(frame->merged_result);
		frame->merged_result = NULL;
	}

	frame->merged_result = nvideo_single_frame_make(
		frame->self->width,
		frame->self->height	
	);
	
	write_to_frame(frame->merged_result, frame->self);
	for(int i = 0; i < frame->children_length; i++) {
		nvideo_merge(frame->children[i]);
		write_to_frame(
			frame->merged_result, 
			frame->children[i]->self
		);
	}

	nvideo_swap(frame->merged_result);
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
	struct nvideo_output *result = (struct nvideo_output *)
		malloc(sizeof(struct nvideo_output));

	result->queue = (struct nvideo_queue *)
		malloc(sizeof(struct nvideo_queue));

	return result;
}

struct nvideo_queue nvideo_queue_make(
	int x,
	int y,
	struct nvideo_color color
) {
	struct nvideo_queue queue = NVIDEO_QUEUE_DECL(x, y, color);
	return queue;
}

void nvideo_process(
	struct nvideo_output *output
) {
	for(int i = 0; i < output->queue_length; i++) {
		output->set(
			output->queue[i].x,
			output->queue[i].y,
			output->queue[i].color
		);
	}
}

void nvideo_add_pixel(
	struct nvideo_output *output, 
	int x, 
	int y, 
	struct nvideo_color color
) {
	output->queue_length++;
	output->queue = (struct nvideo_queue *) realloc(
		output->queue,
		sizeof(struct nvideo_queue) *
		output->queue_length
	);

	struct nvideo_queue queue = nvideo_queue_make(
		x,
		y,
		color
	);

	output->queue[output->queue_length - 1] = queue;
}

void nvideo_add_single_frame(
	struct nvideo_output *output, 
	struct nvideo_single_frame *frame
) {
	for(int y = 0; y < frame->height; y++) {
		for(int x = 0; x < frame->width; x++) {
			nvideo_add_pixel(
				output,
				x,
				y,
				nvideo_single_get_either(frame, x, y, 0)
			);
		}
	}
}

void nvideo_add_frame(
	struct nvideo_output *output, 
	struct nvideo_frame *frame
) {
	if(frame->merged_result == NULL) {
		nvideo_merge(frame);
	}

	nvideo_add_single_frame(
		output, 
		frame->merged_result
	);
}
