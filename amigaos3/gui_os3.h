/* 
AmigaOS 3 related prototypes
*/

#include "image/ico.h"

//static colour frame_colour;

char* usunhttp(char* s);
extern char *status_txt, *icon_file, *stitle;
const char *add_theme_path(const char* icon);
char *ReadClip( void );
int WriteClip(const char * );
extern int alt, ctrl, shift, x_pos, selected;

unsigned short szerokosc(char *text);
unsigned short zliczanie(char *text);

extern struct gui_download_table *amiga_download_table;

struct fbtk_bitmap *load_bitmap(const char *filename);
extern struct fbtk_bitmap *favicon_bitmap;

void get_video(struct browser_window *bw);  
void redraw_gui(void);
void rerun_greyhound(void);
void read_labels(void);

int fb_prefs_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_add_fav_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_redraw_bitmap(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_leftarrow_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_rightarrow_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_reload_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_stop_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_home_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_copy_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_paste_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_getvideo_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_sethome_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_screenmode_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_add_bookmark_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_search_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_bookmarks_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_getpage_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_openfile_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_url_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_searchbar_enter(void *pw, char *text);
int fb_fav1_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_fav2_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_fav3_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_fav4_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_fav5_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_fav6_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_fav7_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_fav8_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_fav9_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_fav10_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_fav11_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);
int fb_fav12_click(fbtk_widget_t *widget, fbtk_callback_info *cbi);

static fbtk_widget_t *fav1 = NULL;
static fbtk_widget_t *fav2 = NULL;
static fbtk_widget_t *fav3 = NULL;
static fbtk_widget_t *fav4 = NULL;
static fbtk_widget_t *fav5 = NULL;
static fbtk_widget_t *fav6 = NULL;
static fbtk_widget_t *fav7 = NULL;
static fbtk_widget_t *fav8 = NULL;
static fbtk_widget_t *fav9 = NULL;
static fbtk_widget_t *fav10 = NULL;
static fbtk_widget_t *fav11 = NULL;
static fbtk_widget_t *fav12 = NULL;

extern fbtk_widget_t *label1;
extern fbtk_widget_t *label2;
extern fbtk_widget_t *label3;
extern fbtk_widget_t *label4;
extern fbtk_widget_t *label5;
extern fbtk_widget_t *label6;
extern fbtk_widget_t *label7;	
extern fbtk_widget_t *label8;
extern fbtk_widget_t *label9;
extern fbtk_widget_t *label10;
extern fbtk_widget_t *label11;
extern fbtk_widget_t *label12;

extern fbtk_widget_t *toolbar;
extern fbtk_widget_t *favicon;
extern fbtk_widget_t *button;
extern fbtk_widget_t *url;

extern struct fbtk_bitmap throbber9;
extern struct fbtk_bitmap throbber10;
extern struct fbtk_bitmap throbber11;
extern struct fbtk_bitmap throbber12;
	
void gui_window_redraw_window(struct gui_window *g);
void gui_window_set_status(struct gui_window *g, const char *text);

fbtk_widget_t *create_fav_widget(int nr, int xpos, int text_w, fbtk_widget_t *toolbar, struct gui_window *gw);	
extern int text_w4, text_w5, text_w6, text_w7, text_w8, text_w9, text_w10, text_w11, text_w12;

void gui_window_redraw(struct gui_window *g, int x0, int y0, int x1, int y1);
void gui_window_hide_pointer(struct gui_window *g);
void *download_icon(void *argument);
void gui_launch_url(const char *url);

nserror gui_download_window_data(struct gui_download_window *dw, const char *data, unsigned int size);
void gui_download_window_error(struct gui_download_window *dw, const char *error_msg);
void gui_download_window_done(struct gui_download_window *dw);
