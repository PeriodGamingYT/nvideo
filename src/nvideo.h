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
void nvideo_single_frame_free(struct nvideo_single_frame *);

// WARNING: nvideo_set and nvideo_get don't check if
// unsigned char * length is equal to NVIDEO_COLOR_AMOUNT!
void nvideo_set(
	struct nvideo_single_frame *, 
	int, 
	int, 
	unsigned char *
);

struct nvideo_color {
	unsigned char r : 8;
	unsigned char g : 8;
	unsigned char b : 8;
}

struct nvideo_color nvideo_get(
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

void nvideo_frame_free(struct nvideo_frame *);
struct nvideo_frame *nvideo_frame_make();
void nvideo_merge(struct nvideo_frame *);
void nvideo_add_child(
	struct nvideo_frame *, 
	struct nvideo_frame *
);

struct nvideo_output {
	void (set*)(int, int, unsigned char*);
	unsigned char *(get*)(int, int);

	// X, Y (32-Bit), color * color_length (8-Bit).
	unsigned char *queue;
	int queue_length;
	int color_length;
};

void nvideo_output_free(struct nvideo_output *);
struct nvideo_output *nvideo_output_make();
void nvideo_output_pixel(
	struct nvideo_output *, 
	int, 
	int, 
	unsigned char *
);

void nvideo_output_single_frame(
	struct nvideo_output *, 
	struct nvideo_single_frame *
);

void nvideo_output_frame(
	struct nvideo_output *, 
	struct nvideo_frame *
);

#endif
