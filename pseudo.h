/* See LICENSE for licence details. */
/* non real framebuffer: just emulates screen */
struct pseudobuffer {
	uint8_t *buf;        /* copy of framebuffer */
	int width, height;   /* display resolution */
	int line_length;     /* line length (byte) */
	int bytes_per_pixel; /* BYTES per pixel */
};

void refresh(struct pseudobuffer *pb, struct terminal *term);

