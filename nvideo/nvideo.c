#include "nvideo.h"
#include <stdlib.h>
#include <stdio.h>

static void die(char *message) {
	fprintf(stderr, "FATAL: %s\n", message);
	exit(-1);
}

#define NVIDEO_ASSERT_NULL(x) \
	if( x == NULL) { \
		die("Can't malloc()/realloc()!"); \
	}

struct nvideo_single_frame *nvideo_single_frame_make(
	int width,
	int height
) {
	struct nvideo_single_frame *frame =
		(struct nvideo_single_frame *) malloc(
			sizeof(struct nvideo_single_frame)
		);

	NVIDEO_ASSERT_NULL(frame);
	frame->width = width;
	frame->height = height;
	int frame_size =
		sizeof(unsigned char) * 
		width *
		height *
		NVIDEO_COLOR_AMOUNT;
	
	frame->front = (unsigned char *) malloc(frame_size);
	frame->back = (unsigned char *) malloc(frame_size);
	NVIDEO_ASSERT_NULL(frame->front);
	NVIDEO_ASSERT_NULL(frame->back);
	frame->x = 0;
	frame->y = 0;
	return frame;
}

void nvideo_single_frame_resize(
	struct nvideo_single_frame *frame,
	int width,
	int height
) {
	free(frame->front);
	free(frame->back);
	frame->width = width;
	frame->height = height;
	int frame_size =
		sizeof(unsigned char) * 
		width *
		height *
		NVIDEO_COLOR_AMOUNT;
	
	frame->front = (unsigned char *) malloc(frame_size);
	frame->back = (unsigned char *) malloc(frame_size);
}

void nvideo_single_frame_free(
	struct nvideo_single_frame *frame
) {
	if(frame == NULL) {
		return;
	}
	
	free(frame->front);
	free(frame->back);
	free(frame);
	frame = NULL;
}

void nvideo_swap_single(
	struct nvideo_single_frame *frame
) {
	if(frame == NULL) {
		return;
	}
	
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
	if(frame == NULL) {
		return;
	}

	if(
		x < 0 ||
		y < 0 ||
		x > frame->width ||
		y > frame->height
	) {
		return;
	}
	
	int index = get_frame_index(*frame, x, y);
	frame->back[index] = arg_color.r;
	frame->back[index + 1] = arg_color.g;
	frame->back[index + 2] = arg_color.b;
}

static struct nvideo_color nvideo_single_get_either(
	struct nvideo_single_frame *frame,
	int x,
	int y,
	int is_back
) {
	struct nvideo_color error = nvideo_color_make(-1, -1, -1);
	if(frame == NULL) {
		return error;
	}

	if(
		x < 0 ||
		y < 0 ||
		x > frame->width ||
		y > frame->height
	) {
		return error;
	}

	// GCC gets mad if I use NVIDEO_COLOR_DECL.
	struct nvideo_color color = {
		.r = 0,
		.g = 0,
		.b = 0	
	};
	
	int index = get_frame_index(*frame, x, y);
	unsigned char *data = is_back
		? frame->back
		: frame->front;
		
	color.r = data[index];
	color.g = data[index + 1];
	color.b = data[index + 2];
	return color;
}

struct nvideo_color nvideo_single_get(
	struct nvideo_single_frame *frame,
	int x,
	int y
) {
	if(frame == NULL) {
		struct nvideo_color error = nvideo_color_make(-1, -1, -1);
		return error;
	}
	
	return nvideo_single_get_either(frame, x, y, 1);
}

void nvideo_swap(
	struct nvideo_frame *frame
) {
	if(frame == NULL) {
		return;
	}

	nvideo_swap_single(frame->self);
}

void nvideo_set(
	struct nvideo_frame *frame,
	int x,
	int y,
	struct nvideo_color arg_color
) {
	if(frame == NULL) {
		return;
	}
	
	nvideo_single_set(frame->self, x, y, arg_color);
}

struct nvideo_color nvideo_get(
	struct nvideo_frame *frame,
	int x,
	int y
) {
	if(frame == NULL) {
		struct nvideo_color error = nvideo_color_make(-1, -1, -1);
		return error;
	}
	
	return nvideo_single_get(frame->self, x, y);
}

void nvideo_frame_free(
	struct nvideo_frame *frame
) {
	if(frame == NULL) {
		return;
	}
	
	nvideo_single_frame_free(frame->self);
	for(int i = 0; i < frame->children_length; i++) {
		nvideo_frame_free(frame->children[i]);
	}

	free(frame->children);
	if(frame->merged_result != NULL) {
		nvideo_single_frame_free(frame->merged_result);
	}

	free(frame);
	frame = NULL;
}


struct nvideo_frame *nvideo_frame_make(
	int width,
	int height
) {
	struct nvideo_frame *frame = 
		malloc(sizeof(struct nvideo_frame));

	NVIDEO_ASSERT_NULL(frame);
	frame->self = nvideo_single_frame_make(width, height);
	frame->children = (struct nvideo_frame **)
		malloc(sizeof(struct nvideo_frame *));

	NVIDEO_ASSERT_NULL(frame->children);
	frame->children_length = 0;
	frame->merged_result = NULL;
	return frame;
}

static void write_to_frame(
	struct nvideo_single_frame *dest,
	struct nvideo_single_frame *src
) {
	if(dest == NULL || src == NULL) {
		return;
	}
	
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
			dest->front[dest_index] = src->front[src_index];
			dest->front[dest_index + 1] = src->front[src_index + 1];
			dest->front[dest_index + 2] = src->front[src_index + 2];
			nvideo_swap_single(dest);
		}
	}
}

void nvideo_merge(
	struct nvideo_frame *frame
) {
	if(frame == NULL) {
		return;
	}
	
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
		if(frame->children[i] == NULL) {
			continue;
		}
		
		nvideo_merge(frame->children[i]);
		write_to_frame(
			frame->merged_result, 
			frame->children[i]->self
		);
	}
}

void nvideo_add_child(
	struct nvideo_frame *frame,
	struct nvideo_frame *child
) {
	if(frame == NULL || child == NULL) {
		return;
	}
	
	frame->children_length++;
	frame->children = (struct nvideo_frame **) realloc(
		frame->children,
		sizeof(struct nvideo_frame *) *
		frame->children_length
	);

	NVIDEO_ASSERT_NULL(frame->children);
	frame->children[frame->children_length - 1] = child;
}

void nvideo_output_pixel(
	struct nvideo_output output, 
	int x, 
	int y, 
	struct nvideo_color color
) {
	output.set(x, y, color);
}

void nvideo_output_single_frame(
	struct nvideo_output output, 
	struct nvideo_single_frame *frame
) {
	if(frame == NULL) {
		return;
	}
	
	for(int y = 0; y < frame->height; y++) {
		for(int x = 0; x < frame->width; x++) {
			nvideo_output_pixel(
				output,
				x,
				y,
				nvideo_single_get_either(frame, x, y, 0)
			);
		}
	}
}

void nvideo_output_frame(
	struct nvideo_output output, 
	struct nvideo_frame *frame
) {
	if(frame == NULL) {
		return;
	}
	
	if(frame->merged_result == NULL) {
		nvideo_merge(frame);
	}

	nvideo_output_single_frame(
		output, 
		frame->merged_result
	);
}

#undef NVIDEO_ASSERT_NULL
