/* See LICENSE for licence details. */
void erase_cell(struct terminal *term, int y, int x);
void copy_cell(struct terminal *term, int dst_y, int dst_x, int src_y, int src_x);
int set_cell(struct terminal *term, int y, int x, const struct glyph_t *glyphp);
void scroll(struct terminal *term, int from, int to, int offset);

/* relative movement: cause scrolling */
void move_cursor(struct terminal *term, int y_offset, int x_offset);

/* absolute movement: never scroll */
void set_cursor(struct terminal *term, int y, int x);
const struct glyph_t *drcsch(struct terminal *term, uint32_t code);
void addch(struct terminal *term, uint32_t code);
void reset_esc(struct terminal *term);
bool push_esc(struct terminal *term, uint8_t ch);
void reset_charset(struct terminal *term);
void reset(struct terminal *term);
void redraw(struct terminal *term);
void term_init(struct terminal *term, int width, int height);
void term_die(struct terminal *term);

