/* See LICENSE for licence details. */

/* ctr char/esc sequence/charset function */
void control_character(struct terminal *term, uint8_t ch);
void esc_sequence(struct terminal *term, uint8_t ch);
void csi_sequence(struct terminal *term, uint8_t ch);
int is_osc_parm(int c);
void omit_string_terminator(char *bp, uint8_t ch);
void osc_sequence(struct terminal *term, uint8_t ch);
void dcs_sequence(struct terminal *term, uint8_t ch);
void utf8_charset(struct terminal *term, uint8_t ch);
void parse(struct terminal *term, uint8_t *buf, int size);
