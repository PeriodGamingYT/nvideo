#ifndef __NVIDEO_H
#define __NVIDEO_H
#define NVIDEO_COLOR_AMOUNT 3

// Show the front to the output.
// Process a new image in the back.
struct nvideo_single_frame {
	unsigned char *front;
	unsigned char *back;
	int x;
	int y;
	int width;
	int height;		
};

struct nvideo_single_frame *nvideo_single_frame_make(int, int);
void nvideo_single_frame_resize(struct nvideo_single_frame *, int, int);
void nvideo_single_frame_free(struct nvideo_single_frame *);
void nvideo_swap_single(struct nvideo_single_frame *);
struct nvideo_color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

#define NVIDEO_COLOR_DECL(r, g, b) \
	{ \
		.r = r, \
		.g = g, \
		.b = b \
	}

struct nvideo_color nvideo_color_make(
	unsigned char,
	unsigned char,
	unsigned char
);

void nvideo_single_set(
	struct nvideo_single_frame *, 
	int, 
	int, 
	struct nvideo_color
);

struct nvideo_color nvideo_single_get(
	struct nvideo_single_frame *, 
	int, 
	int
);

struct nvideo_frame {
	struct nvideo_single_frame *self;
	struct nvideo_frame **children;
	int children_length;
	struct nvideo_single_frame *merged_result;
};

void nvideo_swap(struct nvideo_frame *);
void nvideo_set(
	struct nvideo_frame *, 
	int, 
	int, 
	struct nvideo_color
);

struct nvideo_color nvideo_get(
	struct nvideo_frame *, 
	int, 
	int
);

void nvideo_frame_free(struct nvideo_frame *);
struct nvideo_frame *nvideo_frame_make(int, int);
void nvideo_merge(struct nvideo_frame *);
void nvideo_add_child(
	struct nvideo_frame *, 
	struct nvideo_frame *
);

struct nvideo_output {
	void (*set)(int, int, struct nvideo_color);
	struct nvideo_color *(*get)(int, int);
};

void nvideo_output_pixel(
	struct nvideo_output, 
	int, 
	int, 
	struct nvideo_color
);

void nvideo_output_single_frame(
	struct nvideo_output, 
	struct nvideo_single_frame *
);

void nvideo_output_frame(
	struct nvideo_output, 
	struct nvideo_frame *
);

#endif
