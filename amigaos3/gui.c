/*
 * Copyright 2008, 2014 Vincent Sanders <vince@greyhound-browser.org>
 *
 * This file is part of Greyhound, http://www.greyhound-browser.org/
 *
 * Greyhound is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * Greyhound is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <limits.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>


#include <SDL/SDL.h>
#include <libnsfb.h>
#include <libnsfb_plot.h>
#include <libnsfb_event.h>

#include "palette.h"
#include "desktop/browser_history.h"
#include "desktop/browser_private.h"
#include "desktop/gui.h"
#include "desktop/mouse.h"
#include "desktop/plotters.h"
#include "desktop/greyhound.h"
#include "desktop/textinput.h"
#include "desktop/download.h"

#include "utils/nsoption.h"
#include "utils/filepath.h"
#include "utils/log.h"
#include "utils/messages.h"
#include "utils/types.h"
#include "utils/utils.h"

#include "render/form.h"

#include "framebuffer/gui.h"
#include "framebuffer/fbtk.h"
#include "framebuffer/framebuffer.h"
#include "framebuffer/schedule.h"
#include "framebuffer/findfile.h"
#include "framebuffer/image_data.h"
#include "framebuffer/font.h"
#include "framebuffer/clipboard.h"
#include "framebuffer/fetch.h"

#include "content/urldb.h"
#include "content/fetch.h"
#include "content/hlcache.h"
#include "content/backing_store.h"

//#define FB_FRAME_COLOUR 0xFFD6F1F4
//#define FB_FRAME_COLOUR 0xFFF4F1D6
//#undef FB_FRAME_COLOUR
//#define FB_FRAME_COLOUR nsoption_colour(sys_colour_WindowFrame)	

#ifdef RTG
#	define NSFB_TOOLBAR_DEFAULT_LAYOUT "blfrhuvaqetk123456789xwzgdnmyop"
#else
#	define NSFB_TOOLBAR_DEFAULT_LAYOUT "blfrhcuvaqetk12345gdyop"
#endif

#include "amiga/utf8.h"

#include <exec/exec.h>
#include <libraries/asl.h>
#include <proto/asl.h>
#include <proto/openurl.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dostags.h>

#ifdef DT
#include <proto/datatypes.h>
#include <proto/pngalpha.h>
#include <datatypes/pictureclass.h>
#endif
#include "amigaos3/gui_os3.h"

int text_w4 = 0, text_w5 = 0, text_w6 = 0, text_w7 = 0, text_w8 = 0,
    text_w9 = 0, text_w10 = 0, text_w11 = 0, text_w12 = 0;

extern struct fbtk_bitmap caret_image2;

static uint8_t null_image_pixdata[] = {
	0xff, 0xff, 0xff, 0xff, 
};

struct fbtk_bitmap null_image = {
	.width		= 1,
	.height		= 1,
	.hot_x		= 0,
	.hot_y		= 0,
	.pixdata	= null_image_pixdata,
};

struct browser_window *bw_url;
struct gui_window *g_ui;
fbtk_callback_info *Cbi;
int mouse_2_click = 0;
char *get_url;

struct Library * AslBase;
struct Library * OpenURLBase;
#ifdef DT
struct Library * DataTypesBase = NULL;
struct Library * PNGAlphaBase = NULL;
#endif

#ifndef RTG
struct Library * KeymapBase;
struct Library * CxBase;
#endif

fbtk_widget_t *fbtk;

struct gui_window *input_window = NULL;
struct gui_window *search_current_window;
struct gui_window *window_list = NULL;

extern struct gui_utf8_table *amiga_utf8_table;

/* private data for browser user widget */
struct browser_widget_s {
	struct browser_window *bw; /**< The browser window connected to this gui window */
	int scrollx, scrolly; /**< scroll offsets. */

	/* Pending window redraw state. */
	bool redraw_required; /**< flag indicating the foreground loop
			       * needs to redraw the browser widget.
			       */
	bbox_t redraw_box; /**< Area requiring redraw. */
	bool pan_required; /**< flag indicating the foreground loop
			    * needs to pan the window.
			    */
	int panx, pany; /**< Panning required. */
};

static struct gui_drag {
	enum state {
		GUI_DRAG_NONE,
		GUI_DRAG_PRESSED,
		GUI_DRAG_DRAG
	} state;
	int button;
	int x;
	int y;
	bool grabbed_pointer;
} gui_drag;


/* queue a redraw operation, co-ordinates are relative to the window */
static void
fb_queue_redraw(struct fbtk_widget_s *widget, int x0, int y0, int x1, int y1)
{
	struct browser_widget_s *bwidget = fbtk_get_userpw(widget);

	bwidget->redraw_box.x0 = min(bwidget->redraw_box.x0, x0);
	bwidget->redraw_box.y0 = min(bwidget->redraw_box.y0, y0);
	bwidget->redraw_box.x1 = max(bwidget->redraw_box.x1, x1);
	bwidget->redraw_box.y1 = max(bwidget->redraw_box.y1, y1);

	if (fbtk_clip_to_widget(widget, &bwidget->redraw_box)) {
		bwidget->redraw_required = true;
		fbtk_request_redraw(widget);
	} else {
		bwidget->redraw_box.y0 = bwidget->redraw_box.x0 = INT_MAX;
		bwidget->redraw_box.y1 = bwidget->redraw_box.x1 = -(INT_MAX);
		bwidget->redraw_required = false;
	}
}

/* queue a window scroll */
static void
widget_scroll_y(struct gui_window *gw, int y, bool abs)
{
	struct browser_widget_s *bwidget = fbtk_get_userpw(gw->browser);
	int content_width, content_height;
	int height;


	LOG(("window scroll"));
	if (abs) {
		bwidget->pany = y - bwidget->scrolly;
	} else {
		bwidget->pany += y;
	}

	browser_window_get_extents(gw->bw, true,
			&content_width, &content_height);



	height = fbtk_get_height(gw->browser);

	/* dont pan off the top */
	if ((bwidget->scrolly + bwidget->pany) < 0)
		bwidget->pany = -bwidget->scrolly;

	/* do not pan off the bottom of the content */
	if ((bwidget->scrolly + bwidget->pany) > (content_height - height))
		bwidget->pany = (content_height - height) - bwidget->scrolly;

	if (bwidget->pany == 0)
		return;

	bwidget->pan_required = true;

	fbtk_request_redraw(gw->browser);

	fbtk_set_scroll_position(gw->vscroll, bwidget->scrolly + bwidget->pany);
}

/* queue a window scroll */
static void
widget_scroll_x(struct gui_window *gw, int x, bool abs)
{
	struct browser_widget_s *bwidget = fbtk_get_userpw(gw->browser);
	int content_width, content_height;
	int width;


	if (abs) {
		bwidget->panx = x - bwidget->scrollx;
	} else {
		bwidget->panx += x;
	}

	browser_window_get_extents(gw->bw, true,
			&content_width, &content_height);


	width = fbtk_get_width(gw->browser);

	/* dont pan off the left */
	if ((bwidget->scrollx + bwidget->panx) < 0)
		bwidget->panx = - bwidget->scrollx;

	/* do not pan off the right of the content */
	if ((bwidget->scrollx + bwidget->panx) > (content_width - width))
		bwidget->panx = (content_width - width) - bwidget->scrollx;

	if (bwidget->panx == 0)
		return;

	bwidget->pan_required = true;

	fbtk_request_redraw(gw->browser);

	fbtk_set_scroll_position(gw->hscroll, bwidget->scrollx + bwidget->panx);
}

static void
fb_pan(fbtk_widget_t *widget,
       struct browser_widget_s *bwidget,
       struct browser_window *bw)
{
	int x;
	int y;
	int width;
	int height;
	nsfb_bbox_t srcbox;
	nsfb_bbox_t dstbox;

	nsfb_t *nsfb = fbtk_get_nsfb(widget);

	height = fbtk_get_height(widget);
	width = fbtk_get_width(widget);

	LOG(("panning %d, %d", bwidget->panx, bwidget->pany));

	x = fbtk_get_absx(widget);
	y = fbtk_get_absy(widget);

	/* if the pan exceeds the viewport size just redraw the whole area */
	if (bwidget->pany >= height || bwidget->pany <= -height ||
	    bwidget->panx >= width || bwidget->panx <= -width) {

		bwidget->scrolly += bwidget->pany;
		bwidget->scrollx += bwidget->panx;
		fb_queue_redraw(widget, 0, 0, width, height);

		/* ensure we don't try to scroll again */
		bwidget->panx = 0;
		bwidget->pany = 0;
		bwidget->pan_required = false;
		return;
	}

	if (bwidget->pany < 0) {
		/* pan up by less then viewport height */
		srcbox.x0 = x;
		srcbox.y0 = y;
		srcbox.x1 = srcbox.x0 + width;
		srcbox.y1 = srcbox.y0 + height + bwidget->pany;

		dstbox.x0 = x;
		dstbox.y0 = y - bwidget->pany;
		dstbox.x1 = dstbox.x0 + width;
		dstbox.y1 = dstbox.y0 + height + bwidget->pany;

		/* move part that remains visible up */
		nsfb_plot_copy(nsfb, &srcbox, nsfb, &dstbox);

		/* redraw newly exposed area */
		bwidget->scrolly += bwidget->pany;
		fb_queue_redraw(widget, 0, 0, width, - bwidget->pany);


	} else if (bwidget->pany > 0) {
		/* pan down by less then viewport height */
		srcbox.x0 = x;
		srcbox.y0 = y + bwidget->pany;
		srcbox.x1 = srcbox.x0 + width;
		srcbox.y1 = srcbox.y0 + height - bwidget->pany;

		dstbox.x0 = x;
		dstbox.y0 = y;
		dstbox.x1 = dstbox.x0 + width;
		dstbox.y1 = dstbox.y0 + height - bwidget->pany;

		/* move part that remains visible down */
		nsfb_plot_copy(nsfb, &srcbox, nsfb, &dstbox);

		/* redraw newly exposed area */
		bwidget->scrolly += bwidget->pany;
		fb_queue_redraw(widget, 0, height - bwidget->pany,
				width, height);
	}

	if (bwidget->panx < 0) {
		/* pan left by less then viewport width */
		srcbox.x0 = x;
		srcbox.y0 = y;
		srcbox.x1 = srcbox.x0 + width + bwidget->panx;
		srcbox.y1 = srcbox.y0 + height;

		dstbox.x0 = x - bwidget->panx;
		dstbox.y0 = y;
		dstbox.x1 = dstbox.x0 + width + bwidget->panx;
		dstbox.y1 = dstbox.y0 + height;

		/* move part that remains visible left */
		nsfb_plot_copy(nsfb, &srcbox, nsfb, &dstbox);

		/* redraw newly exposed area */
		bwidget->scrollx += bwidget->panx;
		fb_queue_redraw(widget, 0, 0, -bwidget->panx, height);
	

	} else if (bwidget->panx > 0) {
		/* pan right by less then viewport width */
		srcbox.x0 = x + bwidget->panx;
		srcbox.y0 = y;
		srcbox.x1 = srcbox.x0 + width - bwidget->panx;
		srcbox.y1 = srcbox.y0 + height;

		dstbox.x0 = x;
		dstbox.y0 = y;
		dstbox.x1 = dstbox.x0 + width - bwidget->panx;
		dstbox.y1 = dstbox.y0 + height;

		/* move part that remains visible right */
		nsfb_plot_copy(nsfb, &srcbox, nsfb, &dstbox);

		/* redraw newly exposed area */
		bwidget->scrollx += bwidget->panx;
		fb_queue_redraw(widget, width - bwidget->panx, 0,
				width, height);
	}

	bwidget->pan_required = false;
	bwidget->panx = 0;
	bwidget->pany = 0;
}

static void
fb_redraw(fbtk_widget_t *widget,
	  struct browser_widget_s *bwidget,
	  struct browser_window *bw)
{
	int x;
	int y;
	int caret_x, caret_y, caret_h;
	struct rect clip;
	struct redraw_context ctx = {
		.interactive = true,
		.background_images = true,
		.plot = &fb_plotters
	};
	nsfb_t *nsfb = fbtk_get_nsfb(widget);
	float scale = browser_window_get_scale(bw);
	
	LOG(("%d,%d to %d,%d",
	     bwidget->redraw_box.x0,
	     bwidget->redraw_box.y0,
	     bwidget->redraw_box.x1,
	     bwidget->redraw_box.y1));

	x = fbtk_get_absx(widget);
	y = fbtk_get_absy(widget);

	/* adjust clipping co-ordinates according to window location */
	bwidget->redraw_box.y0 += y;
	bwidget->redraw_box.y1 += y;
	bwidget->redraw_box.x0 += x;
	bwidget->redraw_box.x1 += x;

	nsfb_claim(nsfb, &bwidget->redraw_box);

	/* redraw bounding box is relative to window */
	clip.x0 = bwidget->redraw_box.x0;
	clip.y0 = bwidget->redraw_box.y0;
	clip.x1 = bwidget->redraw_box.x1;
	clip.y1 = bwidget->redraw_box.y1;

	browser_window_redraw(bw,
			(x - bwidget->scrollx) / scale,
			(y - bwidget->scrolly) / scale,
			&clip, &ctx);

	if (fbtk_get_caret(widget, &caret_x, &caret_y, &caret_h)) {
		/* This widget has caret, so render it */
		nsfb_bbox_t line;
		nsfb_plot_pen_t pen;

		line.x0 = x - bwidget->scrollx + caret_x;
		line.y0 = y - bwidget->scrolly + caret_y;
		line.x1 = x - bwidget->scrollx + caret_x;
		line.y1 = y - bwidget->scrolly + caret_y + caret_h;

		pen.stroke_type = NFSB_PLOT_OPTYPE_SOLID;
		pen.stroke_width = 1;
		pen.stroke_colour = 0xFF0000FF;

		nsfb_plot_line(nsfb, &line, &pen);
	}
	nsfb_update(fbtk_get_nsfb(widget), &bwidget->redraw_box);

	bwidget->redraw_box.y0 = bwidget->redraw_box.x0 = INT_MAX;
	bwidget->redraw_box.y1 = bwidget->redraw_box.x1 = INT_MIN;
	bwidget->redraw_required = false;
}

static int
fb_browser_window_redraw(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct gui_window *gw = cbi->context;
	struct browser_widget_s *bwidget;

	bwidget = fbtk_get_userpw(widget);
	if (bwidget == NULL) {
		LOG(("browser widget from widget %p was null", widget));
		return -1;
	}

	if (bwidget->pan_required) {
		fb_pan(widget, bwidget, gw->bw);
	}

	if (bwidget->redraw_required) {
		fb_redraw(widget, bwidget, gw->bw);
	} else {
		bwidget->redraw_box.x0 = 0;
		bwidget->redraw_box.y0 = 0;
		bwidget->redraw_box.x1 = fbtk_get_width(widget);
		bwidget->redraw_box.y1 = fbtk_get_height(widget);
		fb_redraw(widget, bwidget, gw->bw);
	}
	return 0;
}



static int fb_browser_window_destroy(fbtk_widget_t *widget,
		fbtk_callback_info *cbi)
{
	struct browser_widget_s *browser_widget;

	if (widget == NULL) {
		return 0;
	}

	/* Free private data */
	browser_widget = fbtk_get_userpw(widget);
	free(browser_widget);

	return 0;
}
char *options;
static const char *fename;
static int febpp;
static int fewidth;
static int feheight;
static const char *feurl;

static bool
process_cmdline(int argc, char** argv)
{
	int opt;

	LOG(("argc %d, argv %p", argc, argv));
	
	fename = "sdl";
	
#ifdef RTG
	if (nsoption_int(window_depth) == 0) { nsoption_int(window_depth) = 32;
		nsoption_write(options, NULL, NULL);}
	febpp = nsoption_int(window_depth);
	fewidth = nsoption_int(window_width);
	feheight = nsoption_int(window_height);
#else	
	nsoption_int(window_depth) = 8;
	febpp = 8;
	fewidth = 640;
	feheight = 512;
#endif
	
	if ((nsoption_charp(homepage_url) != NULL) && 
	    (nsoption_charp(homepage_url)[0] != '\0')) {
		if (nsoption_charp(lastpage_url) == NULL)
			feurl = nsoption_charp(homepage_url); 
		else {
			feurl = nsoption_charp(lastpage_url);
			nsoption_charp(lastpage_url) = NULL;
			nsoption_write(options, NULL, NULL);
			}	
	}		


	while((opt = getopt(argc, argv, "f:b:w:h:")) != -1) {
		switch (opt) {
		case 'f':
			fename = optarg;
			break;

		case 'b':
			febpp = atoi(optarg);
			break;

		case 'w':
			fewidth = atoi(optarg);
			break;

		case 'h':
			feheight = atoi(optarg);
			break;

		default:
			fprintf(stderr,
				"Usage: %s [-f frontend] [-b bpp] url\n",
				argv[0]);
			return false;
		}
	}

	if (optind < argc) {
		feurl = argv[optind];
	}

	return true;
}

void redraw_gui(void)
{
	fbtk_request_redraw(toolbar);
	fbtk_request_redraw(url);
	if (label1)
		fbtk_request_redraw(label1);
	if (label2)
		fbtk_request_redraw(label2);
	if (label3)
		fbtk_request_redraw(label3);
	if (label4)
		fbtk_request_redraw(label4);
	if (label5)
		fbtk_request_redraw(label5);
	if (label6)
		fbtk_request_redraw(label6);
	if (label7)
		fbtk_request_redraw(label7);
	if (label8)
		fbtk_request_redraw(label8);
	if (label9)
		fbtk_request_redraw(label9);
	if (label10)		
		fbtk_request_redraw(label10);
	if (label11)		
		fbtk_request_redraw(label11);
	if (label12)		
		fbtk_request_redraw(label12);	

	gui_window_redraw_window(g_ui);
}
 
/**
 * Set option defaults for framebuffer frontend
 *
 * @param defaults The option table to update.
 * @return error status.
 */
static nserror set_defaults(struct nsoption_s *defaults)
{

	/* Set defaults for absent option strings */
	nsoption_setnull_charp(cookie_file, strdup("PROGDIR:Resources/Cookies"));
	nsoption_setnull_charp(cookie_jar, strdup("PROGDIR:Resources/Cookies"));
#ifdef RTG	
	nsoption_int(scale) = 100;
	nsoption_setnull_charp(fb_toolbar_layout, strdup("blfrhuvaqetk123456789xwzgdmyop"));	
#else
	nsoption_int(scale) = 75;
	nsoption_setnull_charp(fb_toolbar_layout, strdup("blfcuvaqetk12345gdyop"));	
	nsoption_int(window_depth) = 8;
#endif
#ifdef JS
	nsoption_bool(enable_javascript) = true;
#endif

	//if (nsoption_int(mobile_mode) == -1) nsoption_int(mobile_mode) = 0;
	//if (nsoption_int(gui_font_bold) == -1) nsoption_int(gui_font_bold) = 0;	
	nsoption_setnull_charp(net_player, strdup("C:ffplay"));	
	nsoption_setnull_charp(download_path, strdup("Downloads"));
	nsoption_setnull_charp(homepage_url, strdup("www.greyhound-browser.org/welcome"));
	nsoption_setnull_charp(theme, strdup("default"));	
	nsoption_setnull_charp(text_editor, strdup("C:ed"));
	nsoption_setnull_charp(download_manager, strdup("httpresume"));
	nsoption_setnull_charp(favicon_source,strdup("http://www.google.com/s2/favicons?domain=")); 
	
	if (nsoption_charp(cookie_file) == NULL ||
	    nsoption_charp(cookie_jar) == NULL) {
		LOG(("Failed initialising cookie options"));
		return NSERROR_BAD_PARAMETER;
	}

	return NSERROR_OK;
}

/**
 * Ensures output logging stream is correctly configured
 */
static bool nslog_stream_configure(FILE *fptr)
{
        /* set log stream to be non-buffering */
	setbuf(fptr, NULL);

	return true;
}

static void framebuffer_poll(bool active)
{
	nsfb_event_t event;
	int timeout; /* timeout in miliseconds */

	/* run the scheduler and discover how long to wait for the next event */
	timeout = schedule_run();
	
	/* if active do not wait for event, return immediately */
	if (active)
		timeout = 0;
		
	/* if redraws are pending do not wait for event, return immediately */
	if (fbtk_get_redraw_pending(fbtk))
		timeout = 0;
		
	if (fbtk_event(fbtk, &event, timeout)) {
		if ((event.type == NSFB_EVENT_CONTROL) &&
		    (event.value.controlcode ==  NSFB_CONTROL_QUIT))
			{
			greyhound_quit = true;
			}
	}
	
	fbtk_redraw(fbtk);

}

static void gui_quit(void)
{
	LOG(("gui_quit"));
	
	urldb_save_cookies(nsoption_charp(cookie_jar));
#ifdef DT		
	if (DataTypesBase)     CloseLibrary(DataTypesBase);	
	if (PNGAlphaBase)     CloseLibrary(PNGAlphaBase);	
	if (P96Base)     CloseLibrary(P96Base);	
#endif
#ifndef RTG
	if (KeymapBase)     CloseLibrary(KeymapBase);
		if (CxBase)     CloseLibrary(CxBase);	
#endif
	
	framebuffer_finalise();
}

/* called back when click in browser window */
static int
fb_browser_window_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct gui_window *gw = cbi->context;
	Cbi = cbi;
	struct browser_widget_s *bwidget = fbtk_get_userpw(widget);
	browser_mouse_state mouse;
	float scale = browser_window_get_scale(gw->bw);
	int x = (cbi->x + bwidget->scrollx) / scale;
	int y = (cbi->y + bwidget->scrolly) / scale;
	unsigned int time_now;
	static struct {
		enum { CLICK_SINGLE, CLICK_DOUBLE, CLICK_TRIPLE } type;
		unsigned int time;
	} last_click;

	if (cbi->event->type != NSFB_EVENT_KEY_DOWN &&
	    cbi->event->type != NSFB_EVENT_KEY_UP)
		return 0;

	LOG(("browser window clicked at %d,%d", cbi->x, cbi->y));

	switch (cbi->event->type) {
	case NSFB_EVENT_KEY_DOWN:
		switch (cbi->event->value.keycode) {
		case NSFB_KEY_MOUSE_1:
			browser_window_mouse_click(gw->bw,
					BROWSER_MOUSE_PRESS_1, x, y);
			gui_drag.state = GUI_DRAG_PRESSED;
			gui_drag.button = 1;
			gui_drag.x = x;
			gui_drag.y = y;
			break;

		case NSFB_KEY_MOUSE_3:
			browser_window_mouse_click(gw->bw,
					BROWSER_MOUSE_PRESS_2, x, y);
			gui_drag.state = GUI_DRAG_PRESSED;
			gui_drag.button = 2;
			gui_drag.x = x;
			gui_drag.y = y;
			
			mouse_2_click = 1;
			break;

		case NSFB_KEY_MOUSE_4:
			/* scroll up */
			if (browser_window_scroll_at_point(gw->bw, x, y,
					0, -100) == false)
				widget_scroll_y(gw, -100, false);
			break;

		case NSFB_KEY_MOUSE_5:
			/* scroll down */
			if (browser_window_scroll_at_point(gw->bw, x, y,
					0, 100) == false)
				widget_scroll_y(gw, 100, false);
			break;

		default:
			break;

		}

		break;
	case NSFB_EVENT_KEY_UP:

		mouse = 0;
		time_now = wallclock();

		switch (cbi->event->value.keycode) {
		case NSFB_KEY_MOUSE_1:
			if (gui_drag.state == GUI_DRAG_DRAG) {
				/* End of a drag, rather than click */

				if (gui_drag.grabbed_pointer) {
					/* need to ungrab pointer */
					fbtk_tgrab_pointer(widget);
					gui_drag.grabbed_pointer = false;
				}

				gui_drag.state = GUI_DRAG_NONE;

				/* Tell core */
				browser_window_mouse_track(gw->bw, 0, x, y);
				break;
			}
			/* This is a click;
			 * clear PRESSED state and pass to core */
			gui_drag.state = GUI_DRAG_NONE;
			mouse = BROWSER_MOUSE_CLICK_1;
			break;

		case NSFB_KEY_MOUSE_3:
			if (gui_drag.state == GUI_DRAG_DRAG) {
				/* End of a drag, rather than click */
				gui_drag.state = GUI_DRAG_NONE;

				if (gui_drag.grabbed_pointer) {
					/* need to ungrab pointer */
					fbtk_tgrab_pointer(widget);
					gui_drag.grabbed_pointer = false;
				}

				/* Tell core */
				browser_window_mouse_track(gw->bw, 0, x, y);
				break;
			}
			/* This is a click;
			 * clear PRESSED state and pass to core */
			gui_drag.state = GUI_DRAG_NONE;
			mouse = BROWSER_MOUSE_CLICK_2;
			break;

		default:
			break;

		}

		/* Determine if it's a double or triple click, allowing
		 * 0.5 seconds (50cs) between clicks */
		if (time_now < last_click.time + 50 &&
				cbi->event->value.keycode != NSFB_KEY_MOUSE_4 &&
				cbi->event->value.keycode != NSFB_KEY_MOUSE_5) {
			if (last_click.type == CLICK_SINGLE) {
				/* Set double click */
				mouse |= BROWSER_MOUSE_DOUBLE_CLICK;
				last_click.type = CLICK_DOUBLE;

			} else if (last_click.type == CLICK_DOUBLE) {
				/* Set triple click */
				mouse |= BROWSER_MOUSE_TRIPLE_CLICK;
				last_click.type = CLICK_TRIPLE;
			} else {
				/* Set normal click */
				last_click.type = CLICK_SINGLE;
			}
		} else {
			last_click.type = CLICK_SINGLE;
		}

		if (mouse)
			browser_window_mouse_click(gw->bw, mouse, x, y);

		last_click.time = time_now;

		break;
	default:
		break;

	}
	return 1;
}

/* called back when movement in browser window */
static int
fb_browser_window_move(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	browser_mouse_state mouse = 0;
	struct gui_window *gw = cbi->context;
	struct browser_widget_s *bwidget = fbtk_get_userpw(widget);
	float scale = browser_window_get_scale(gw->bw);
	int x = (cbi->x + bwidget->scrollx) / scale;
	int y = (cbi->y + bwidget->scrolly) / scale;

	if (gui_drag.state == GUI_DRAG_PRESSED &&
			(abs(x - gui_drag.x) > 5 ||
			 abs(y - gui_drag.y) > 5)) {
		/* Drag started */
		if (gui_drag.button == 1) {
			browser_window_mouse_click(gw->bw,
					BROWSER_MOUSE_DRAG_1,
					gui_drag.x, gui_drag.y);
		} else {
			browser_window_mouse_click(gw->bw,
					BROWSER_MOUSE_DRAG_2,
					gui_drag.x, gui_drag.y);
		}
		gui_drag.grabbed_pointer = fbtk_tgrab_pointer(widget);
		gui_drag.state = GUI_DRAG_DRAG;
	}

	if (gui_drag.state == GUI_DRAG_DRAG) {
		/* set up mouse state */
		mouse |= BROWSER_MOUSE_DRAG_ON;

		if (gui_drag.button == 1)
			mouse |= BROWSER_MOUSE_HOLDING_1;
		else
			mouse |= BROWSER_MOUSE_HOLDING_2;
	}

	browser_window_mouse_track(gw->bw, mouse, x, y);

	return 0;
}

int ucs4_pop = 0;
	
void rerun_greyhound(void)
{
	STRPTR curDir = AllocVec(1000, MEMF_CLEAR | MEMF_ANY);
	char run[1028];
	
	GetCurrentDirName(curDir,1024);
	
	strcpy(run, "run ");
	strcat(run, (const char*) curDir);
#ifdef RTG
#	ifndef NOTTF
	strcat(run, "/greyhound");
#	else
	strcat(run, "/greyhound-nottf");
#	endif	
#else
#	ifndef NOTTF
#		ifdef AGA685
		strcat(run, "/greyhoundaga");
#		else
		strcat(run, "/greyhoundagalow");
#		endif		
#	else
#		ifdef AGA685
		strcat(run, "/greyhoundaga-nottf");
#		else
		strcat(run, "/greyhoundagalow-nottf");
#		endif	
#	endif		
#endif	
	FreeVec(curDir);
	Execute(run, 0, 0);
	
	greyhound_quit = true;	
}

static void fb_update_back_forward(struct gui_window *gw);

int ctrl = 0;

static int
fb_browser_window_input(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct gui_window *gw = cbi->context;
	static fbtk_modifier_type modifier = FBTK_MOD_CLEAR;

	int ucs4 = -1;

	LOG(("got value %d", cbi->event->value.keycode));
	LOG(("ctrl %d", ctrl));


	switch (cbi->event->type) {
	case NSFB_EVENT_KEY_DOWN:

		switch (cbi->event->value.keycode) {
		case NSFB_KEY_PAGEUP:
			if (browser_window_key_press(gw->bw,
					KEY_PAGE_UP) == false)
				widget_scroll_y(gw, -fbtk_get_height(
						gw->browser), false);
			break;

		case NSFB_KEY_PAGEDOWN:
			if (browser_window_key_press(gw->bw,
					KEY_PAGE_DOWN) == false)
				widget_scroll_y(gw, fbtk_get_height(
						gw->browser), false);
			break;

		case NSFB_KEY_RIGHT:
			if (modifier & FBTK_MOD_RCTRL ||
					modifier & FBTK_MOD_LCTRL) {
				/* CTRL held */
				if (browser_window_key_press(gw->bw,
						KEY_LINE_END) == false)
					widget_scroll_x(gw, INT_MAX, true);

			} else if (modifier & FBTK_MOD_RSHIFT ||
					modifier & FBTK_MOD_LSHIFT) {
				/* SHIFT held */
				if (browser_window_key_press(gw->bw,
						KEY_WORD_RIGHT) == false)
					widget_scroll_x(gw, fbtk_get_width(
						gw->browser), false);
			} else {
				/* no modifier */
				if (browser_window_key_press(gw->bw,
						KEY_RIGHT) == false)
					widget_scroll_x(gw, 100, false);
			}
			break;
			 	
					
			case NSFB_KEY_LEFT:
				if (modifier & FBTK_MOD_RCTRL ||
						modifier & FBTK_MOD_LCTRL) {
					/* CTRL held */
					if (browser_window_key_press(gw->bw,
							KEY_LINE_START) == false)
						widget_scroll_x(gw, 0, true);

				} else 
					if (modifier & FBTK_MOD_RSHIFT ||
						modifier & FBTK_MOD_LSHIFT) {
					/* SHIFT held */
					if (browser_window_key_press(gw->bw,
							KEY_WORD_LEFT) == false)
						widget_scroll_x(gw, -fbtk_get_width(
							gw->browser), false);

				} else {
				/* no modifier */
				if (browser_window_key_press(gw->bw,
						KEY_LEFT) == false)
					widget_scroll_x(gw, -100, false);
			}
			break;

		case NSFB_KEY_UP:
			if (browser_window_key_press(gw->bw,
					KEY_UP) == false)
				widget_scroll_y(gw, -100, false);
			break;

		case NSFB_KEY_DOWN:
			if (browser_window_key_press(gw->bw,
					KEY_DOWN) == false)
				widget_scroll_y(gw, 100, false);
			break;

		case NSFB_KEY_RSHIFT:
			SDL_EnableKeyRepeat(0, 50);
			modifier |= FBTK_MOD_RSHIFT;
			break;

		case NSFB_KEY_LSHIFT:
			SDL_EnableKeyRepeat(0, 50);
			modifier |= FBTK_MOD_LSHIFT;
			break;
			
		case NSFB_KEY_RSUPER:
		case NSFB_KEY_RCTRL:
			SDL_EnableKeyRepeat(0, 50);
			SDL_EnableUNICODE(false);
			modifier |= FBTK_MOD_RCTRL;
			ctrl = 1;
			break;
			
		case NSFB_KEY_LSUPER:
		case NSFB_KEY_LCTRL:
			SDL_EnableKeyRepeat(0, 50);
			SDL_EnableUNICODE(false);
			modifier |= FBTK_MOD_LCTRL;
			ctrl = 1;
			break;
			
		case NSFB_KEY_F4:         			
			get_video(gw->bw);
			break;
			
		case NSFB_KEY_F5:
			browser_window_reload(gw->bw, true);     
			break;

		case NSFB_KEY_F6:
			nsoption_charp(lastpage_url) = strdup(nsurl_access(hlcache_handle_get_url(gw->bw->current_content)));
			rerun_greyhound();  
			break;
			
		case NSFB_KEY_F7:
			nsoption_charp(lastpage_url) = strdup(nsurl_access(hlcache_handle_get_url(gw->bw->current_content)));
			nsoption_write("PROGDIR:Resources/Options", NULL,NULL);
			rerun_greyhound(); 
			break;            										
		
		case NSFB_KEY_ESCAPE:
			greyhound_quit = true;
			break;				

		case NSFB_KEY_DELETE:
			{
			browser_window_key_press(gw->bw, KEY_DELETE_RIGHT);	
			char *se = strndup(status_txt, 8);
			if ( (strcmp(nsurl_access(hlcache_handle_get_url(gw->bw->current_content)),
                    "file:///PROGDIR:Resources/Bookmarks.htm") == 0) && (strcmp(se, "Document") != 0) )  
				{
				char *p, cmd[200] = "null";
				
				p = strtok(status_txt, "/"); 
				p = strtok(NULL, "/"); 
				
				strcpy(cmd, "c/sed -e /");
				strcat(cmd, p);  
				strcat(cmd, "/d resources/bookmarks.htm > resources/bookmarks2.htm");       				    				

				Execute(cmd, 0, 0);
						
				Execute("delete resources/bookmarks.htm", 0, 0);
				Execute("rename resources/bookmarks2.htm resources/bookmarks.htm", 0, 0);
						
				browser_window_reload(gw->bw, true);	
				}			
			free(se);
			}
			break;
			
		case NSFB_KEY_y:
		case NSFB_KEY_z:
			if (cbi->event->value.keycode == NSFB_KEY_z &&
					(modifier & FBTK_MOD_RCTRL ||
					 modifier & FBTK_MOD_LCTRL) &&
					(modifier & FBTK_MOD_RSHIFT ||
					 modifier & FBTK_MOD_LSHIFT)) {
				/* Z pressed with CTRL and SHIFT held */
				browser_window_key_press(gw->bw, KEY_REDO);
				break;

			} else if (cbi->event->value.keycode == NSFB_KEY_z &&
					(modifier & FBTK_MOD_RCTRL ||
					 modifier & FBTK_MOD_LCTRL)) {
				/* Z pressed with CTRL held */
				browser_window_key_press(gw->bw, KEY_UNDO);
				break;

			} else if (cbi->event->value.keycode == NSFB_KEY_y &&
					(modifier & FBTK_MOD_RCTRL ||
					 modifier & FBTK_MOD_LCTRL)) {
				/* Y pressed with CTRL held */
				browser_window_key_press(gw->bw, KEY_REDO);
				break;

			}			
			/* Z or Y pressed but not undo or redo;
			 * Fall through to default handling */			
		default:
			SDL_EnableUNICODE(true);
			if (strcmp(nsoption_charp(accept_charset), "AmigaPL") == 0) {
				switch (cbi->event->value.keycode) { 
					case 172: cbi->event->value.keycode = 175;			
						break;		
					case 177: cbi->event->value.keycode = 191;			
						break;	
					case 202: cbi->event->value.keycode = 198;
						break;
					case 203: cbi->event->value.keycode = 202;			
						break;
					case 234: cbi->event->value.keycode = 230;
						break;
					case 235: cbi->event->value.keycode = 234;			
						break;
					 default:
					 /* do nothing */
						break;
				}
			}
		
			ucs4 = fbtk_keycode_to_ucs4(cbi->event->value.keycode,
						    modifier);

			if (ucs4 != -1)
				browser_window_key_press(gw->bw, ucs4);
			break;
		}
		break;

		
	case NSFB_EVENT_KEY_UP:
		switch (cbi->event->value.keycode) {
		case NSFB_KEY_RSHIFT:
			modifier &= ~FBTK_MOD_RSHIFT;
			SDL_EnableKeyRepeat(300, 50);
			shift = 0;
			break;

		case NSFB_KEY_LSHIFT:
			modifier &= ~FBTK_MOD_LSHIFT;
			SDL_EnableKeyRepeat(300, 50);
			shift = 0;
			break;
		
		case NSFB_KEY_LALT:
		case NSFB_KEY_RALT:
			SDL_EnableKeyRepeat(300, 50);
			alt = 0;
			break;      							  		     

		case NSFB_KEY_BACKSPACE:
			if (ctrl==1)
			{
				struct browser_window *bw = gw->bw;
				
			if (browser_window_back_available(bw))
				browser_window_history_back(bw, false);
					
				fb_update_back_forward(gw);	
			}
			ctrl = 0;
			break;

		case NSFB_KEY_RSUPER:	
		case NSFB_KEY_RCTRL:
			SDL_EnableKeyRepeat(300, 50);
			SDL_EnableUNICODE(true);
			modifier &= ~FBTK_MOD_RCTRL;
			ctrl = 0;
			break;

		case NSFB_KEY_LSUPER:
		case NSFB_KEY_LCTRL:
			SDL_EnableKeyRepeat(300, 50);
			SDL_EnableUNICODE(true);
			modifier &= ~FBTK_MOD_LCTRL;
			ctrl = 0;
			break;

		default:		
			break;
		}
		
		break;		

	default:
		break;
	}

	return 0;
}

const char *
add_theme_path(const char *icon) 
{
	static char path[128];

	strcpy(path, "PROGDIR:Resources/theme/");
	strcat(path, nsoption_charp(theme));
	strcat(path, icon);

	return path;
}

static void
fb_update_back_forward(struct gui_window *gw)
{
	struct browser_window *bw = gw->bw;

	fbtk_set_bitmap(gw->back,
			(browser_window_back_available(bw)) ?			
			load_bitmap(add_theme_path("/back.png")) :  load_bitmap(add_theme_path("/back_g.png")));
		
	fbtk_set_bitmap(gw->forward,
			(browser_window_forward_available(bw)) ?			
			load_bitmap(add_theme_path("/forward.png")) :  load_bitmap(add_theme_path("/forward_g.png")));

}

/* left icon click routine */

int
fb_leftarrow_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct gui_window *gw = cbi->context;
	struct browser_window *bw = gw->bw;
	
 	if (cbi->event->type != NSFB_EVENT_KEY_UP)	
		return 0;
	
	if (browser_window_back_available(bw))
		browser_window_history_back(bw, false);
		
	fb_update_back_forward(gw);	
	
	return 1;
}

/* right arrow icon click routine */

int
fb_rightarrow_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct gui_window *gw = cbi->context;
	struct browser_window *bw = gw->bw;

	if (cbi->event->type == NSFB_EVENT_KEY_UP) 
		return 0;	
	
	if (browser_window_forward_available(bw))
		browser_window_history_forward(bw, false);

	fb_update_back_forward(gw);	
	
	return 1;

}

/* reload icon click routine */

int
fb_reload_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct browser_window *bw = cbi->context;

	if (cbi->event->type == NSFB_EVENT_KEY_UP) 
		return 0;	
		
	fbtk_set_bitmap(widget, load_bitmap(icon_file));

	if (cbi->event->value.keycode == NSFB_KEY_MOUSE_3) {
		nsoption_read(options, nsoptions);
		SDL_Delay(500);
		nsoption_charp(lastpage_url) = strdup(nsurl_access(hlcache_handle_get_url(bw->current_content)));
		nsoption_write(options, NULL, NULL);
		rerun_greyhound();  	
	}
	
	browser_window_reload(bw, true);

	return 1;
}

/* stop icon click routine */

int
fb_stop_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct browser_window *bw = cbi->context;
	
	if (cbi->event->type == NSFB_EVENT_KEY_UP) 
		return 0;	
	
	fbtk_set_bitmap(widget, load_bitmap(icon_file));
	
	browser_window_stop(bw);

	return 1;
}
#ifndef RTG	
/* close browser window icon click routine */
static int
fb_close_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	
	if (cbi->event->type != NSFB_EVENT_KEY_UP)
		return 0;
		
		greyhound_quit = true;
		return 0;

}
#endif
static int
fb_scroll_callback(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct gui_window *gw = cbi->context;

	switch (cbi->type) {
	case FBTK_CBT_SCROLLY:
		widget_scroll_y(gw, cbi->y, true);
		break;

	case FBTK_CBT_SCROLLX:
		widget_scroll_x(gw, cbi->x, true);
		break;

	default:
		break;
	}
	return 0;
}
static int
browser_window_go(struct browser_window *bw, char *text, int i, bool b)
{
	nsurl *url;
	nserror error;

	error = nsurl_create(text, &url);

	if (error != NSERROR_OK) {
		warn_user(messages_get_errorcode(error), 0);
	} else {
		browser_window_navigate(bw, url, NULL, BW_NAVIGATE_HISTORY,
				NULL, NULL, NULL);
		nsurl_unref(url);
	}

	return 0; 
}
	
static int
fb_url_enter(void *pw, char *text)
{
    struct browser_window *bw = pw;
	nsurl *url;
	nserror error;

	error = nsurl_create(text, &url);
	if (error != NSERROR_OK) {
		warn_user(messages_get_errorcode(error), 0);
	} else {
		browser_window_navigate(bw, url, NULL, BW_NAVIGATE_HISTORY,
				NULL, NULL, NULL);

		nsurl_unref(url);
	}

	return 0;
}

int
fb_url_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
#ifdef RTG	
	framebuffer_set_cursor(&null_image);	
	SDL_ShowCursor(SDL_ENABLE);		
#else
	framebuffer_set_cursor(&pointer_image);
	SDL_ShowCursor(SDL_DISABLE);
#endif	
	return 0;
}

/* home icon click routine */
int
fb_home_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct browser_window *bw = cbi->context;
	
	if (cbi->event->type == NSFB_EVENT_KEY_UP)	
		return 0;
		
	fbtk_set_bitmap(widget, load_bitmap(icon_file));			
			
	browser_window_go(bw, nsoption_charp(homepage_url), 0, true);
			
	return 1;
}

/* paste from clipboard click routine */
int
fb_paste_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct browser_window *bw = cbi->context;
	
	if (cbi->event->type == NSFB_EVENT_KEY_UP) 
		return 0;	
		
	fbtk_set_bitmap(widget, load_bitmap(icon_file));
		
	if (ReadClip())
		{
		
		//printf(ReadClip());

		 char str[100];
		 int len;
		 len = (int) strlen(ReadClip());
		 
		 strcpy(str,strdup(ReadClip()));
		 
		 //printf("str before [%s], len[%d]\n", str, len);
		 
		 len--;
		 while (isspace(*(str + len) )) {
			  len--;
		 }
		 
		 *(str + len + 1) = '\0';
		 
		// printf("str after [%s], len[%d]\n", str, (int) strlen(str));
		 
		browser_window_go(bw, str, 0, true);
		}
	
	return 1;
}

/* write to clipboard icon click routine */
int
fb_copy_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct browser_window *bw = cbi->context;
	
	if (cbi->event->type == NSFB_EVENT_KEY_UP) 
		return 0;	
		
	fbtk_set_bitmap(widget, load_bitmap(icon_file));			
			
	char *clip = strdup(nsurl_access(hlcache_handle_get_url(bw->current_content)));
	
	WriteClip(clip);
		
	return 1;
}

char* usunhttp(char* s)
{
     int p = 0, q = 0, i = 7;
     while (s[++p] != '\0') if (p >= i) s[q++] = s[p];
     s[q] = '\0';

    return s;
}
 
void 
get_video(struct browser_window *bw) 
{   	
	char *cmd = AllocVec(1000, MEMF_CLEAR | MEMF_ANY);
	char *url = strdup(nsurl_access(hlcache_handle_get_url(bw->current_content)));
	
	char addr[500];
	strcpy(addr, "http://www.getlinkinfo.com/info?link=");
	strcat(addr, url);	
	browser_window_go(bw, addr, 0, true);	
	
	strcpy(cmd, "run Sys:Rexxc/rx S:getvideo.rexx ");				 
	strcat(cmd, url);
	strcat(cmd, " play ");
	
	//Execute("Mount >NIL: TCP: from AmiTCP:devs/Inet-Mountlist",0,0);
	Execute(cmd, 0, 0);	
	SetTaskPri(FindTask(0), -1);
	
	FreeVec(cmd);	
}

/* get video icon click routine */
int
fb_getvideo_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct browser_window *bw = cbi->context;

	if (cbi->event->type == NSFB_EVENT_KEY_UP)
		return 0;
		
    get_video(bw);
    
	fbtk_set_bitmap(widget, load_bitmap(icon_file));
	
	return 1;
}


/* set current url as homepage icon click routine */
int
fb_sethome_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
 	struct browser_window *bw = cbi->context;
	
	if (cbi->event->type == NSFB_EVENT_KEY_UP) 
		return 0;	
		
	fbtk_set_bitmap(widget, load_bitmap(icon_file));
	
	nsoption_charp(homepage_url) = strdup(nsurl_access(hlcache_handle_get_url(bw->current_content)));
	nsoption_write(options, NULL, NULL);
	free(nsoption_charp(homepage_url));
	
	return 1;
}

/* add favourites icon click routine */
int
fb_add_fav_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct browser_window *bw = cbi->context;
	
	fbtk_widget_t *fav = NULL;
	fbtk_widget_t *label = NULL;
	char *bitmap = NULL;	
	
	if (cbi->event->type == NSFB_EVENT_KEY_UP) 
		return 0;	
		
	fbtk_set_bitmap(widget, load_bitmap(icon_file));
				
	BPTR fh;
	char *cmd;

	int inum = 0;	
	
	/* Show select window */
	
	cmd = strdup("RequestChoice > ENV:NSfav TITLE=\"Select favourite\" BODY=\"Choose which favourite slot you would like to use\" GADGETS=\"___1___|___2___|___3___|___4___|___5___|___6___|___7___|___8___|___9___|___10___|___11___|___12___|Cancel\"");

	Execute(cmd, 0, 0);
	fh = Open("ENV:NSfav",MODE_OLDFILE);

	char snum[3];
	
	Read(fh,snum,2);
	inum = atoi(snum);

	Close(fh);
	
	Execute("delete ENV:NSfav", 0, 0);
	
	/* Download favicon */		
	
	char *wget = AllocVec(1000, MEMF_CLEAR | MEMF_ANY);
	char *opt = strdup("?&format=png&width=16&height=16&canAudit=false -OResources/Icons/favicon");
	//char *opt = strdup("?&format=png&width=16&height=16 -OResources/Icons/favicon");

	strcpy(wget, "echo \" c/wget ");
	strcat(wget, "-PPROGDIR:Resources/Icons/ ");
	//strcat(wget, " 	http://g.fvicon.com/");
	//strcat(wget, " 	http://www.google.com/s2/favicons?domain=");	
	strcat(wget, nsoption_charp(favicon_source));
	
	if (inum > 0) {
		get_url = strdup(nsurl_access(hlcache_handle_get_url(bw->current_content)));		
		}
	
		
	if (inum == 1 ) {	
		   nsoption_charp(favourite_1_url) = strdup(get_url);
		   nsoption_charp(favourite_1_label) = strdup(stitle);	
		   fav = fav1;
		   label = label1;
		   }
	else if (inum == 2 ) {
		   nsoption_charp(favourite_2_url) = strdup(get_url);
		   nsoption_charp(favourite_2_label) = strdup(stitle);		   	   
		   fav = fav2;
		   label = label2;		   
		   } 
	else if (inum == 3 ) {
		   nsoption_charp(favourite_3_url) = strdup(get_url);
		   nsoption_charp(favourite_3_label) = strdup(stitle);		   
		   fav = fav3;
		   label = label3;	   
		   } 
	else if (inum == 4 ) {
		   nsoption_charp(favourite_4_url) = strdup(get_url);
		   nsoption_charp(favourite_4_label) = strdup(stitle);		   	   
		   fav = fav4;
		   label = label4;		   
		   } 	
	else if (inum == 5 )  {
		   nsoption_charp(favourite_5_url) = strdup(get_url);
		   nsoption_charp(favourite_5_label) = strdup(stitle);		   
		   fav = fav5;
		   label = label5;		   
		   } 
	else if (inum == 6  ) {
		   nsoption_charp(favourite_6_url) = strdup(get_url);
		   nsoption_charp(favourite_6_label) = strdup(stitle);		   
		   fav = fav6;
		   label = label6;		   
		   } 
	else if (inum == 7  ) {
		   nsoption_charp(favourite_7_url) = strdup(get_url);
		   nsoption_charp(favourite_7_label) = strdup(stitle);		   	   
		   fav = fav7;
		   label = label7;		   
		   } 	
	else if (inum == 8  ) {
		   nsoption_charp(favourite_8_url) = strdup(get_url);
		   strcat(wget, get_url);
		   nsoption_charp(favourite_8_label) = strdup(stitle);	   
		   strcat(wget, opt);
		   fav = fav8;		
		   label = label8;		   
		   }	 
	else if (inum == 9 ) {
		   nsoption_charp(favourite_9_url) = strdup(get_url);
		   nsoption_charp(favourite_9_label) = strdup(stitle);		      
		   fav = fav9;
		   label = label9;		   
		   } 
	else if (inum == 10 ) {
		   nsoption_charp(favourite_10_url) = strdup(get_url);
		   nsoption_charp(favourite_10_label) = strdup(stitle);		   	   		   
		   fav = fav10;
		   label = label10;		   
		   } 	
	else if (inum == 11 ) {
		   nsoption_charp(favourite_11_url) = strdup(get_url);
		   nsoption_charp(favourite_11_label) = strdup(stitle);		   
		   fav = fav11;
		   label = label11;		   
		   } 
	else if (inum == 12 ) {
		   nsoption_charp(favourite_12_url) = strdup(get_url);
		   nsoption_charp(favourite_12_label) = strdup(stitle);			   
		   fav = fav12;
		   label = label12;		   
		} 				
		
	if (inum != 0 ) 
	{
		sprintf(snum, "%d", inum);
		
		strcat(wget, get_url);
		strcat(wget, opt);		
		strcat(wget, snum);	
		strcat(wget, ".png ");	
		strcat(wget, " \" >script");
			
		Execute(wget, 0, 0);
		Execute("echo \"endcli\" >>script", 0, 0);
		Execute("execute script", 0, 0);
		Execute("delete script", 0, 0);
	
		nsoption_write(options, NULL, NULL);	
				
		if (fav != NULL)
			{	
			bitmap = strdup("PROGDIR:Resources/Icons/favicon");
			strcat(bitmap, snum);	
			strcat(bitmap, ".png");			
			fbtk_set_bitmap(fav, load_bitmap(bitmap));	
			fbtk_set_text(label, stitle);
			}
	}

	FreeVec(wget);			

	return 1;
}

/* add bookmark icon click routine */
int
fb_add_bookmark_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct browser_window *bw = cbi->context;
	
	if (cbi->event->type == NSFB_EVENT_KEY_UP) 
		return 0;		
			
	fbtk_set_bitmap(widget, load_bitmap(icon_file));
	
	get_url = strdup(nsurl_access(hlcache_handle_get_url(bw->current_content)));
	char *cmd = AllocVec(strlen("echo <li><a href=\"") + strlen(get_url) + strlen(stitle) + strlen("</a></li >> Resources/Bookmarks.htm") + 10, MEMF_CLEAR | MEMF_ANY);

	strcpy(cmd, "echo \"<li><a href=");
	strcat(cmd, get_url);
	strcat(cmd, ">");
	strcat(cmd, stitle );
	strcat(cmd, "</a></li>\" >> Resources/Bookmarks.htm");

	Execute(cmd, 0, 0);

	FreeVec(cmd);	
	
	return 1;
}

/* search icon click routine */
int
fb_search_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{

	if (cbi->event->type == NSFB_EVENT_KEY_UP) {		
      	BPTR fh;
      	char *cmd;
      	char buffer[1];	
		int buff = 0;
		
      	cmd = strdup("RequestChoice > ENV:NSsrcheng TITLE=\"Select search engine\" BODY=\"Select search engine\" GADGETS=\"Google|Yahoo|Bing|DuckDuckGo|YouTube|Ebay|Allegro|Aminet|Wikipedia|Cancel\"");

      	Execute(cmd, 0, 0);
      	fh = Open("ENV:NSsrcheng",MODE_OLDFILE);

      	Read(fh,buffer,1);
		buff = atoi(buffer);
		
      	Close(fh);

      	Execute("delete ENV:NSsrcheng", 0, 0);

      	  if (buff == 1) {
      		   nsoption_charp(def_search_bar) = strdup("http://www.google.com/search?q=");
      		   fbtk_set_bitmap(widget, load_bitmap("PROGDIR:Resources/Icons/google.png"));
				Execute("copy Resources/Icons/google.png Resources/Icons/search.png", 0, 0);
				}
      	  else if (buff == 2) {
      		   nsoption_charp(def_search_bar) = strdup("http://search.yahoo.com/search?p=");
      		   fbtk_set_bitmap(widget, load_bitmap("PROGDIR:Resources/Icons/yahoo.png"));     		   
				Execute("copy Resources/Icons/yahoo.png Resources/Icons/search.png", 0, 0);
			   }
      	  else if (buff == 3) {
      		   nsoption_charp(def_search_bar) = strdup("http://www.bing.com/search?q=");
      		   fbtk_set_bitmap(widget, load_bitmap("PROGDIR:Resources/Icons/bing.png"));     		   
				Execute("copy Resources/Icons/bing.png Resources/Icons/search.png", 0, 0);
			   }
      	  else if (buff == 4) {
      		   nsoption_charp(def_search_bar) = strdup("http://www.duckduckgo.com/html/?q=");
      		   fbtk_set_bitmap(widget, load_bitmap("PROGDIR:Resources/Icons/duckduckgo.png"));     		   
				Execute("copy Resources/Icons/duckduckgo.png Resources/Icons/search.png", 0, 0);
			   }			
      	  else if (buff == 5) {
      		   nsoption_charp(def_search_bar) = strdup("http://www.youtube.com/results?search_query=");
      		   fbtk_set_bitmap(widget, load_bitmap("PROGDIR:Resources/Icons/youtube.png"));
				Execute("copy Resources/Icons/youtube.png Resources/Icons/search.png", 0, 0);
			   }
      	  else if (buff == 6) {
      		   nsoption_charp(def_search_bar) = strdup("http://shop.ebay.com/items/");
      		   fbtk_set_bitmap(widget, load_bitmap("PROGDIR:Resources/Icons/ebay.png"));     		   
				Execute("copy Resources/Icons/ebay.png Resources/Icons/search.png", 0, 0);
			   }			
      	  else if (buff == 7) {
      		   nsoption_charp(def_search_bar) = strdup("http://allegro.pl/listing.php/search?string=");
      		   fbtk_set_bitmap(widget, load_bitmap("PROGDIR:Resources/Icons/allegro.png"));
				Execute("copy Resources/Icons/allegro.png Resources/Icons/search.png", 0, 0);
			   }			
      	  else if (buff == 8) {
      		   nsoption_charp(def_search_bar) = strdup("http://aminet.net/search?query=");
      		   fbtk_set_bitmap(widget,  load_bitmap("PROGDIR:Resources/Icons/aminet.png"));
				Execute("copy Resources/Icons/aminet.png Resources/Icons/search.png", 0, 0);
			   }
      	  else if (buff == 9) {
      		   nsoption_charp(def_search_bar) = strdup("http://en.wikipedia.org/w/index.php?title=Special:Search&search=");
      		   fbtk_set_bitmap(widget, load_bitmap("PROGDIR:Resources/Icons/wiki.png"));
				Execute("copy Resources/Icons/wiki.png Resources/Icons/search.png", 0, 0);
			   }

      	nsoption_write(options, NULL, NULL);
		free(cmd);
		
		return 0;
		}
	return 1;
}

int
fb_searchbar_enter(void *pw, char *text)
{
    struct browser_window *bw = pw;

	if (text)
		{
		char addr[500];

		strcpy(addr, nsoption_charp(def_search_bar));
		strcat(addr, text);
		browser_window_go(bw, addr, 0, true);	
		
		}
	return 0;
}

int
fb_bookmarks_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct browser_window *bw = cbi->context;
	
	if (cbi->event->type == NSFB_EVENT_KEY_UP) 
		return 0;		

	fbtk_set_bitmap(widget, load_bitmap(icon_file));
	
	char *cmd = strdup("file:///PROGDIR:Resources/Bookmarks.htm");
	
	browser_window_go(bw, cmd, 0, true);
	free(cmd);
	
	return 1;
}

void
read_labels(void)
{
fbtk_set_text(label1, nsoption_charp(favourite_1_label));
fbtk_set_text(label2, nsoption_charp(favourite_2_label));
fbtk_set_text(label3, nsoption_charp(favourite_3_label));
fbtk_set_text(label4, nsoption_charp(favourite_4_label));
fbtk_set_text(label5, nsoption_charp(favourite_5_label));
fbtk_set_text(label6, nsoption_charp(favourite_6_label));
fbtk_set_text(label7, nsoption_charp(favourite_7_label));
fbtk_set_text(label8, nsoption_charp(favourite_8_label));
fbtk_set_text(label9, nsoption_charp(favourite_9_label));
fbtk_set_text(label10, nsoption_charp(favourite_10_label));
fbtk_set_text(label11, nsoption_charp(favourite_11_label));
fbtk_set_text(label12, nsoption_charp(favourite_12_label));
}

int 
fb_prefs_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{	
	if (cbi->event->type == NSFB_EVENT_KEY_UP) 
		return 0;	
		
	fbtk_set_bitmap(widget, load_bitmap(icon_file));
	
	char *text_editor = AllocVec(200, MEMF_CLEAR | MEMF_ANY);

#ifndef RTG	
	strcpy(text_editor, "run ");
	strcat(text_editor, nsoption_charp(text_editor));	
#else
	strcpy(text_editor, nsoption_charp(text_editor));
#endif	
	strcat(text_editor, " greyhound:"); 	
	strcat(text_editor, "Resources/Options"); 

	Execute(text_editor,0,0);
#ifndef RTG	
	Execute("RequestChoice TITLE=\"Options\" BODY=\"Click ok after changing options \" GADGETS=\"OK\"",0,0);
#endif	
	FreeVec(text_editor);
	
	nsoption_read(options, nsoptions);

	read_labels();
	
	return 1;
} 
 
int
fb_getpage_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct browser_window *bw = cbi->context;
	
	if (cbi->event->type == NSFB_EVENT_KEY_UP) 
		return 0;

	fbtk_set_bitmap(widget, load_bitmap(icon_file));  

	BPTR fh;
	char result[1];
	int resint;

	char *cmd = strdup("RequestChoice > env:nstemp TITLE=\"Download\" BODY=\"Download \" GADGETS=\"using wget|as PDF|Cancel\"");
			   
	Execute(cmd, 0, 0);
	fh = Open("ENV:NStemp",MODE_OLDFILE);    
	Read(fh, result, 1);	
	resint = atoi(result);
	Close(fh); 
			
	if (resint > 0)  
		{
		if (strcmp(nsoption_charp(download_path),"") == 0)
			{
			AslBase = OpenLibrary("asl.library", 0);
			struct FileRequester *savereq;

			savereq = (struct FileRequester *)AllocAslRequestTags(ASL_FileRequest,
								ASLFR_DoSaveMode,TRUE,
								ASLFR_RejectIcons,TRUE,
								ASLFR_InitialDrawer,0,
								TAG_DONE);
			AslRequest(savereq,NULL);
			nsoption_charp(download_path) = strdup(savereq->fr_Drawer);
			nsoption_write(options, NULL, NULL);

			FreeAslRequest(savereq);
			CloseLibrary(AslBase);
			}
		 
		char *wget = AllocVec(2000, MEMF_CLEAR | MEMF_ANY);
		char *url = strdup(nsurl_access(hlcache_handle_get_url(bw->current_content)));
		
		if ( resint == 2 ) {
			usunhttp(url);
			char *wsk;
			wsk=strchr(url,'/');
			*wsk='-';
			strlcpy(url,url,strlen(url));
		}
		strcpy(wget, "run c/wget ");
		strcat(wget, "-P");
		strcat(wget, nsoption_charp(download_path));		
		strcat(wget, " ");
		
		if ( resint == 2 )	
			strcat(wget, "http://pdfmyurl.com?url=");	
			
		strcat(wget, url);
		strcat(wget, " -x -O ");		
		strcat(wget, nsoption_charp(download_path));
		
		if ( resint == 2 )			
			strcat(wget, "/");

		url = strdup(strrchr(url, '/'));

		strcat(wget, url);
				
		if ( resint == 2 ) 			
			strcat(wget, ".pdf");

		fh = Open("CON:", MODE_NEWFILE);	

		Execute(wget, fh, 0);	

		Close(fh);

		free(url);
		FreeVec(wget);			
		}  

			
	return 1;
}

int
fb_openfile_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct browser_window *bw = cbi->context;
	
	if (cbi->event->type == NSFB_EVENT_KEY_UP)
		return 0;
		
	fbtk_set_bitmap(widget, load_bitmap(icon_file));
	
	char *file = AllocVec(1024,MEMF_CLEAR | MEMF_ANY);

	AslBase = OpenLibrary("asl.library", 0);
	struct FileRequester *savereq;

	savereq = (struct FileRequester *)AllocAslRequestTags(ASL_FileRequest,
						ASLFR_DoSaveMode,TRUE,
						ASLFR_RejectIcons,TRUE,
						ASLFR_InitialDrawer,0,
						TAG_DONE);
						
	AslRequest(savereq,NULL);
	
	strcpy(file,"file:///");
	strcat(file, savereq->fr_Drawer);
	if (strcmp(savereq->fr_Drawer,"") == 0)
		strcat(file, "PROGDIR:");	
	else
		strcat(file, "/");		
	strcat(file, savereq->fr_File);

	FreeAslRequest(savereq);
	CloseLibrary(AslBase);

	if (strcmp(file, "file:///") != 0 )
		browser_window_go(bw, file, 0, true);

	FreeVec(file);
	
	return 1;
}
/* fav1 icon click routine */
int
fb_fav1_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct browser_window *bw = cbi->context;	
	
	if (cbi->event->value.keycode == NSFB_KEY_MOUSE_3)	{
		fbtk_set_bitmap(widget, NULL);
		nsoption_charp(favourite_1_url) = NULL;
		nsoption_charp(favourite_1_label) = NULL;
		fbtk_set_text(label1, NULL);
	    nsoption_write(options, NULL, NULL);
		Execute("delete /Greyhound/Resources/Icons/favicon1.png", 0, 0);
		return 0;
	}	

	if ((nsoption_charp(favourite_1_url) != NULL) && (nsoption_charp(favourite_1_url)[0] != '\0'))
		browser_window_go(bw, nsoption_charp(favourite_1_url), 0, true);	
	
	if (cbi->event->type == NSFB_EVENT_KEY_UP) {
		fbtk_set_bitmap(widget,  load_bitmap("PROGDIR:Resources/Icons/favicon1.png"));
		
		return 0;	
	}	
	return 1;
}
/* fav2 icon click routine */
int
fb_fav2_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct browser_window *bw = cbi->context;	
	
	if (cbi->event->value.keycode == NSFB_KEY_MOUSE_3)	{
		fbtk_set_bitmap(widget, NULL);
		nsoption_charp(favourite_2_url) = NULL;
		nsoption_charp(favourite_2_label) = NULL;
		fbtk_set_text(label2, NULL);
	    nsoption_write(options, NULL, NULL);
		Execute("delete /Greyhound/Resources/Icons/favicon2.png", 0, 0);
		return 0;
	}		

	if (nsoption_charp(favourite_2_url) != NULL && nsoption_charp(favourite_2_url)[0] != '\0')		
		browser_window_go(bw, nsoption_charp(favourite_2_url), 0, true);

	if (cbi->event->type == NSFB_EVENT_KEY_UP) {
		fbtk_set_bitmap(widget,  load_bitmap("PROGDIR:Resources/Icons/favicon2.png"));			
		return 0;	
	}	
	return 1;
}
/* fav3 icon click routine */
int
fb_fav3_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct browser_window *bw = cbi->context;	
	
	if (cbi->event->value.keycode == NSFB_KEY_MOUSE_3)	{
		fbtk_set_bitmap(widget, NULL);
		nsoption_charp(favourite_3_url) = NULL;
		nsoption_charp(favourite_3_label) = NULL;
		fbtk_set_text(label3, NULL);	
	    nsoption_write(options, NULL, NULL);
		Execute("delete /Greyhound/Resources/Icons/favicon3.png", 0, 0);
		return 0;
	}	
	
	if (nsoption_charp(favourite_3_url) != NULL && nsoption_charp(favourite_3_url)[0]!= '\0')		
		browser_window_go(bw, nsoption_charp(favourite_3_url), 0, true);

	if (cbi->event->type == NSFB_EVENT_KEY_UP) {
		fbtk_set_bitmap(widget,  load_bitmap("PROGDIR:Resources/Icons/favicon3.png"));			
		return 0;	
	}	
	return 1;
}
/* fav4 icon click routine */
int
fb_fav4_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct browser_window *bw = cbi->context;	
	
	if (cbi->event->value.keycode == NSFB_KEY_MOUSE_3)	{
		fbtk_set_bitmap(widget, NULL);
		nsoption_charp(favourite_4_url) = NULL;
		nsoption_charp(favourite_4_label) = NULL;
		fbtk_set_text(label4, NULL);
	    nsoption_write(options, NULL, NULL);
		Execute("delete /Greyhound/Resources/Icons/favicon4.png", 0, 0);
		return 0;
	}	
	
	if (nsoption_charp(favourite_4_url) != NULL && nsoption_charp(favourite_4_url)[0]!= '\0')		
		browser_window_go(bw, nsoption_charp(favourite_4_url), 0, true);

	if (cbi->event->type == NSFB_EVENT_KEY_UP) {
		fbtk_set_bitmap(widget,  load_bitmap("PROGDIR:Resources/Icons/favicon4.png"));			
		return 0;	
	}	
	return 1;
}
/* fav5 icon click routine */
int
fb_fav5_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct browser_window *bw = cbi->context;	
	
	if (cbi->event->value.keycode == NSFB_KEY_MOUSE_3)	{
		fbtk_set_bitmap(widget, NULL);
		nsoption_charp(favourite_5_url) = NULL;
		nsoption_charp(favourite_5_label) = NULL;
		fbtk_set_text(label5, NULL);	
	    nsoption_write(options, NULL, NULL);
		Execute("delete /Greyhound/Resources/Icons/favicon5.png", 0, 0);
		return 0;
	}	
	
	if (nsoption_charp(favourite_5_url) != NULL && nsoption_charp(favourite_5_url)[0]!= '\0')		
		browser_window_go(bw, nsoption_charp(favourite_5_url), 0, true);

	if (cbi->event->type == NSFB_EVENT_KEY_UP) {
		fbtk_set_bitmap(widget,  load_bitmap("PROGDIR:Resources/Icons/favicon5.png"));			
		return 0;	
	}	
	return 1;
}
/* fav6 icon click routine */
int
fb_fav6_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct browser_window *bw = cbi->context;	
	
	if (cbi->event->value.keycode == NSFB_KEY_MOUSE_3)	{
		fbtk_set_bitmap(widget, NULL);
		nsoption_charp(favourite_6_url) = NULL;
		nsoption_charp(favourite_6_label) = NULL;
		fbtk_set_text(label6, NULL);	
	    nsoption_write(options, NULL, NULL);
		Execute("delete /Greyhound/Resources/Icons/favicon6.png", 0, 0);
		return 0;
	}	
		
	if (nsoption_charp(favourite_6_url) != NULL && nsoption_charp(favourite_6_url)[0]!= '\0')		
		browser_window_go(bw, nsoption_charp(favourite_6_url), 0, true);

	if (cbi->event->type == NSFB_EVENT_KEY_UP) {
		fbtk_set_bitmap(widget,  load_bitmap("PROGDIR:Resources/Icons/favicon6.png"));			
		return 0;	
	}	
	return 1;
}
/* fav7 icon click routine */
int
fb_fav7_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct browser_window *bw = cbi->context;	
	
	if (cbi->event->value.keycode == NSFB_KEY_MOUSE_3)	{
		fbtk_set_bitmap(widget, NULL);
		nsoption_charp(favourite_7_url) = NULL;
		nsoption_charp(favourite_7_label) = NULL;
		fbtk_set_text(label7, NULL);		
	    nsoption_write(options, NULL, NULL);
		Execute("delete /Greyhound/Resources/Icons/favicon7.png", 0, 0);
		return 0;
	}	
	
	if (nsoption_charp(favourite_7_url) != NULL && nsoption_charp(favourite_7_url)[0]!= '\0')		
		browser_window_go(bw, nsoption_charp(favourite_7_url), 0, true);

	if (cbi->event->type == NSFB_EVENT_KEY_UP) {
		fbtk_set_bitmap(widget,  load_bitmap("PROGDIR:Resources/Icons/favicon7.png"));			
		return 0;	
	}	
	return 1;
}
/* fav8 icon click routine */
int
fb_fav8_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct browser_window *bw = cbi->context;	
	
	if (cbi->event->value.keycode == NSFB_KEY_MOUSE_3)	{
		fbtk_set_bitmap(widget, NULL);
		nsoption_charp(favourite_8_url) = NULL;
		nsoption_charp(favourite_8_label) = NULL;
		fbtk_set_text(label8, NULL);	
	    nsoption_write(options, NULL, NULL);
		Execute("delete /Greyhound/Resources/Icons/favicon8.png", 0, 0);
		return 0;
	}	
	
	if (nsoption_charp(favourite_8_url) != NULL && nsoption_charp(favourite_8_url)[0]!= '\0')		
		browser_window_go(bw, nsoption_charp(favourite_8_url), 0, true);

	if (cbi->event->type == NSFB_EVENT_KEY_UP) {
		fbtk_set_bitmap(widget,  load_bitmap("PROGDIR:Resources/Icons/favicon8.png"));			
		return 0;	
	}	
	return 1;
}
/* fav9 icon click routine */
int
fb_fav9_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct browser_window *bw = cbi->context;	
	
	if (cbi->event->value.keycode == NSFB_KEY_MOUSE_3)	{
		fbtk_set_bitmap(widget, NULL);
		nsoption_charp(favourite_9_url) = NULL;
		nsoption_charp(favourite_9_label) = NULL;
		fbtk_set_text(label9, NULL);	
	    nsoption_write(options, NULL, NULL);
		Execute("delete /Greyhound/Resources/Icons/favicon9.png", 0, 0);
		return 0;
	}	
	
	if (nsoption_charp(favourite_9_url) != NULL && nsoption_charp(favourite_9_url)[0]!= '\0')		
		browser_window_go(bw, nsoption_charp(favourite_9_url), 0, true);

	if (cbi->event->type == NSFB_EVENT_KEY_UP) {
		fbtk_set_bitmap(widget,  load_bitmap("PROGDIR:Resources/Icons/favicon9.png"));			
		return 0;	
	}	
	return 1;
}
/* fav10 icon click routine */
int
fb_fav10_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct browser_window *bw = cbi->context;	
	
	if (cbi->event->value.keycode == NSFB_KEY_MOUSE_3)	{
		fbtk_set_bitmap(widget, NULL);
		nsoption_charp(favourite_10_url) = NULL;
		nsoption_charp(favourite_10_label) = NULL;
		fbtk_set_text(label10, NULL);	
	    nsoption_write(options, NULL, NULL);
		Execute("delete /Greyhound/Resources/Icons/favicon10.png", 0, 0);
		return 0;
	}	
	
	if (nsoption_charp(favourite_10_url) != NULL && nsoption_charp(favourite_10_url)[0]!= '\0')		
		browser_window_go(bw, nsoption_charp(favourite_10_url), 0, true);

	if (cbi->event->type == NSFB_EVENT_KEY_UP) {
		fbtk_set_bitmap(widget,  load_bitmap("PROGDIR:Resources/Icons/favicon10.png"));			
		return 0;	
	}	
	return 1;
}
/* fav11 icon click routine */
int
fb_fav11_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct browser_window *bw = cbi->context;	
	
	if (cbi->event->value.keycode == NSFB_KEY_MOUSE_3)	{
		fbtk_set_bitmap(widget, NULL);
		nsoption_charp(favourite_11_url) = NULL;
		nsoption_charp(favourite_11_label) = NULL;
		fbtk_set_text(label11, NULL);	
	    nsoption_write(options, NULL, NULL);
		Execute("delete /Greyhound/Resources/Icons/favicon11.png", 0, 0);
		return 0;
	}	
	
	if (nsoption_charp(favourite_11_url) != NULL && nsoption_charp(favourite_11_url)[0]!= '\0')		
		browser_window_go(bw, nsoption_charp(favourite_11_url), 0, true);

	if (cbi->event->type == NSFB_EVENT_KEY_UP) {
		fbtk_set_bitmap(widget,  load_bitmap("PROGDIR:Resources/Icons/favicon11.png"));			
		return 0;	
	}	
	return 1;
}
/* fav12 icon click routine */
int
fb_fav12_click(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct browser_window *bw = cbi->context;	
	
	if (cbi->event->value.keycode == NSFB_KEY_MOUSE_3)	{
		fbtk_set_bitmap(widget, NULL);
		nsoption_charp(favourite_12_url) = NULL;
		nsoption_charp(favourite_12_label) = NULL;
		fbtk_set_text(label12, NULL);	
	    nsoption_write(options, NULL, NULL);
		Execute("delete /Greyhound/Resources/Icons/favicon12.png", 0, 0);
		return 0;
	}	
	
	if (nsoption_charp(favourite_12_url) != NULL && nsoption_charp(favourite_12_url)[0]!= '\0')		
		browser_window_go(bw, nsoption_charp(favourite_12_url), 0, true);

	if (cbi->event->type == NSFB_EVENT_KEY_UP) {
		fbtk_set_bitmap(widget,  load_bitmap("PROGDIR:Resources/Icons/favicon12.png"));			
		return 0;	
	}	
	return 1;
}

#include <proto/intuition.h>
#include <intuition/pointerclass.h>

static int
fb_url_move(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
    bw_url =  cbi->context;
	
	framebuffer_set_cursor(&caret_image);	
	SDL_ShowCursor(SDL_DISABLE);
	
	return 0;
}

static int
set_ptr_default_move(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
#ifdef  RTG
	framebuffer_set_cursor(&null_image); 	
    SDL_ShowCursor(SDL_ENABLE);	
#else
	framebuffer_set_cursor(&pointer_image); 	
    SDL_ShowCursor(SDL_DISABLE);	
#endif
	
	if (icon_file)
	   fbtk_set_bitmap(button,  load_bitmap(icon_file));
	
	return 0;
}

static int
fb_localhistory_btn_clik(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct gui_window *gw = cbi->context;

	fbtk_set_bitmap(widget,  load_bitmap(add_theme_path("/history_h.png")));	
	
	if (cbi->event->type == NSFB_EVENT_KEY_UP) {
		fbtk_set_bitmap(widget,  load_bitmap(add_theme_path("/history.png")));		
		
		fb_localhistory_map(gw->localhistory);
			
		return 0;
	}
	return 1;
}


static int
set_back_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct browser_window *bw = cbi->context;	
	if (browser_window_back_available(bw)) {
		button = widget;
		icon_file = strdup(add_theme_path("/back.png"));
		fbtk_set_bitmap(widget, load_bitmap(add_theme_path("/back_h.png")));
	}
	gui_window_set_status(g_ui, "Go to previous page");

	return 0;
}

static int
set_forward_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	struct browser_window *bw = cbi->context;
	if (browser_window_forward_available(bw)) {
		fbtk_set_bitmap(widget, load_bitmap(add_theme_path("/forward_h.png")));
		button = widget;
		icon_file = strdup(add_theme_path("/forward.png"));	
	}	
	gui_window_set_status(g_ui, "Go to next page");

	return 0;
}
#ifndef RTG
static int
set_close_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	gui_window_set_status(g_ui, "Close or restart(RMB) program");

	return 0;
}
#endif
static int
set_stop_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	button = widget;
	icon_file = strdup(add_theme_path("/stop.png"));	
	
	fbtk_set_bitmap(widget, load_bitmap(add_theme_path("/stop_h.png")));
	
	gui_window_set_status(g_ui, "Stop loading of current page");

	return 0;
}


static int
set_local_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	button = widget;
	icon_file = strdup(add_theme_path("/history.png"));	
	fbtk_set_bitmap(widget, load_bitmap(add_theme_path("/history_h.png")));
	
	gui_window_set_status(g_ui, "Show local history treeview");

	return 0;
}
#ifdef RTG	
static int
set_reload_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	button = widget;
	icon_file = strdup(add_theme_path("/reload.png"));	
	fbtk_set_bitmap(widget, load_bitmap(add_theme_path("/reload_h.png")));
	
	gui_window_set_status(g_ui, "Reload");

	return 0;
}

static int
set_home_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	button = widget;
	icon_file = strdup(add_theme_path("/home.png"));	
	fbtk_set_bitmap(widget, load_bitmap(add_theme_path("/home_h.png")));
	
	gui_window_set_status(g_ui, "Go to homepage");

	return 0;
}
#endif

static int
set_add_fav_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	button = widget;
	icon_file = strdup(add_theme_path("/add_fav.png"));	
	fbtk_set_bitmap(widget, load_bitmap(add_theme_path("/add_fav_h.png")));
	
	gui_window_set_status(g_ui, "Add current page to favourites");

	return 0;
}

static int
set_add_bookmark_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	button = widget;
	icon_file = strdup(add_theme_path("/add_bookmark.png"));	
	fbtk_set_bitmap(widget, load_bitmap(add_theme_path("/add_bookmark_h.png")));
	
	gui_window_set_status(g_ui, "Add current page to bookmarks");

	return 0;
}

static int
set_bookmarks_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	button = widget;
	icon_file = strdup(add_theme_path("/bookmarks.png"));	
	fbtk_set_bitmap(widget, load_bitmap(add_theme_path("/bookmarks_h.png")));
	
	gui_window_set_status(g_ui, "Go to bookmarks");

	return 0;
}

static int
set_search_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	gui_window_set_status(g_ui, "Select search engine");

	return 0;
}

static int
set_prefs_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	button = widget;
	icon_file = strdup(add_theme_path("/prefs.png"));	
	fbtk_set_bitmap(widget, load_bitmap(add_theme_path("/prefs_h.png")));
	
	gui_window_set_status(g_ui, "Open preferences file in editor");

	return 0;
}

static int
set_getpage_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	button = widget;
	icon_file = strdup(add_theme_path("/getpage.png"));	
	fbtk_set_bitmap(widget, load_bitmap(add_theme_path("/getpage_h.png")));
	
	gui_window_set_status(g_ui, "Save file using wget or as pdf");

	return 0;
}

static int
set_sethome_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	button = widget;
	icon_file = strdup(add_theme_path("/sethome.png"));	
	fbtk_set_bitmap(widget, load_bitmap(add_theme_path("/sethome_h.png")));
	
	gui_window_set_status(g_ui, "Set current site as homepage");

	return 0;
}

static int
set_getvideo_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	button = widget;
	icon_file = strdup(add_theme_path("/getvideo.png"));	
	fbtk_set_bitmap(widget, load_bitmap(add_theme_path("/getvideo_h.png")));
	
	gui_window_set_status(g_ui, "Play or save multimedia to disk using GetVideo plugin");

	return 0;
}

static int
set_paste_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	button = widget;
	icon_file = strdup(add_theme_path("/paste.png"));	
	fbtk_set_bitmap(widget, load_bitmap(add_theme_path("/paste_h.png")));
	
	gui_window_set_status(g_ui, "Paste and open URL from clipboard");

	return 0;
}

static int
set_copy_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	button = widget;
	icon_file = strdup(add_theme_path("/copy.png"));	
	fbtk_set_bitmap(widget, load_bitmap(add_theme_path("/copy_h.png")));
	
	gui_window_set_status(g_ui, "Copy current URL to clipboard");

	return 0;
}

static int
set_fav1_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	gui_window_set_status(g_ui, "Favourite #01");		
	return 0;
}
static int
set_fav2_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	gui_window_set_status(g_ui, "Favourite #02");			
	return 0;
}
static int
set_fav3_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	gui_window_set_status(g_ui, "Favourite #03");			
	return 0;
}
static int
set_fav4_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	gui_window_set_status(g_ui, "Favourite #04");			
	return 0;
}
static int
set_fav5_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	gui_window_set_status(g_ui, "Favourite #05");			
	return 0;
}
static int
set_fav6_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	gui_window_set_status(g_ui, "Favourite #06");			
	return 0;
}
static int
set_fav7_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	gui_window_set_status(g_ui, "Favourite #07");			
	return 0;
}
static int
set_fav8_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	gui_window_set_status(g_ui, "Favourite #08");			
	return 0;
}
static int
set_fav9_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	gui_window_set_status(g_ui, "Favourite #09");			
	return 0;
}
static int
set_fav10_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	gui_window_set_status(g_ui, "Favourite #10");			
	return 0;
}
static int
set_fav11_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	gui_window_set_status(g_ui, "Favourite #11");			
	return 0;
}
static int
set_fav12_status(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	gui_window_set_status(g_ui, "Favourite #12");			
	return 0;
}

unsigned short zliczanie(char *text)
{
  char znak;
  unsigned short i;
  int j=0;
  i=0;
  while((znak=text[i++])!='\0')
  {
    if ((znak=='l') || (znak=='i') || (znak=='.'))
		j++;
  }

  return j;
}

unsigned short szerokosc(char *text)
{
  unsigned short szer, dl; 
  dl = strlen(text);
  if (dl > 10)
	text = strndup(text, 10);
  dl = strlen(text);
  szer = (9 * dl) - (zliczanie(text) * 6);
  
#ifdef NOTTF
	szer += 10;
#endif
 
  return szer;
}

fbtk_widget_t *searchbar, *home, *addfav, *addbook, *quick, 
			  *prefs, *download, *sethome, *getvideo, *copy, *paste;

const char *label5txt,*label6txt,*label7txt,*label8txt,*label9txt,
			  *label10txt,*label11txt,*label12txt;

int x4,x5,x6,x7,x8,x9,x10,x11,x12 = 0;
int text_w = 0;
	
fbtk_widget_t *	
create_fav_widget(int nr, int xpos, int text_w, fbtk_widget_t *toolbar, struct gui_window *gw){
		
	fbtk_callback status = NULL, click = NULL; 
	fbtk_widget_t *fav = NULL, *label = NULL;
	char *image = NULL, *label_txt = NULL;

	switch (nr) {
		case 5:
			click = fb_fav5_click;
			image = strdup("PROGDIR:Resources/Icons/favicon5.png");
			status = set_fav5_status;
			label_txt = strndup(nsoption_charp(favourite_5_label), 10);	
			break;
		case 6:
			click = fb_fav6_click;
			image = strdup("PROGDIR:Resources/Icons/favicon6.png");
			status = set_fav6_status;
			label_txt = strndup(nsoption_charp(favourite_6_label), 10);	
			break;
		case 7:
			click = fb_fav7_click;
			image = strdup("PROGDIR:Resources/Icons/favicon7.png");
			status = set_fav7_status;
			label_txt = strndup(nsoption_charp(favourite_7_label), 10);	
			break;
		case 8:
			click = fb_fav8_click;
			image = strdup("PROGDIR:Resources/Icons/favicon8.png");
			status = set_fav8_status;
			label_txt = strndup(nsoption_charp(favourite_8_label), 10);	
			break;
		case 9:
			click = fb_fav9_click;
			image = strdup("PROGDIR:Resources/Icons/favicon9.png");
			status = set_fav9_status;
			label_txt = strndup(nsoption_charp(favourite_9_label), 10);	
			break;
		case 10:
			click = fb_fav10_click;
			image = strdup("PROGDIR:Resources/Icons/favicon10.png");
			status = set_fav10_status;
			label_txt = strndup(nsoption_charp(favourite_10_label), 10);	
			break;
		case 11:
			click = fb_fav11_click;
			image = strdup("PROGDIR:Resources/Icons/favicon11.png");
			status = set_fav11_status;
			label_txt = strndup(nsoption_charp(favourite_11_label), 10);	
			break;
		case 12:
			click = fb_fav12_click;
			image = strdup("PROGDIR:Resources/Icons/favicon12.png");
			status = set_fav12_status;
			label_txt = strndup(nsoption_charp(favourite_12_label), 10);	
			break;			
		
		}
		
		fav = fbtk_create_button(toolbar,
						    xpos,
						    30+2,
						    16,
						    16,
						    FB_FRAME_COLOUR,
						    load_bitmap(image),
						    click,
						    gw->bw);
							
			fbtk_set_handler(fav, FBTK_CBT_POINTERENTER, 
				 status, gw->bw);		
	
			label = fbtk_create_text(gw->window,
				      xpos+22,
				      28+2,
				      text_w, 20,
				      FB_FRAME_COLOUR, FB_COLOUR_BLACK,
				      false);
					 					
			fbtk_set_text(label, label_txt);	

			fbtk_set_handler(label, FBTK_CBT_CLICK, 
				 click, gw->bw);	
			fbtk_set_handler(label, FBTK_CBT_POINTERENTER, 
				 status, gw->bw);	

		switch (nr) {
		case 5:		
			fav5 = fav;		
			label5 = label;
			break;
		case 6:		
			fav6 = fav;		
			label6 = label;
			break;
		case 7:		
			fav7 = fav;		
			label7 = label;
			break;
		case 8:		
			fav8 = fav;		
			label8 = label;
			break;
		case 9:		
			fav9 = fav;		
			label9 = label;
			break;
		case 10:		
			fav10 = fav;		
			label10 = label;
			break;
		case 11:		
			fav11 = fav;		
			label11 = label;
			break;
		case 12:		
			fav12 = fav;		
			label12 = label;
			break;			
		}
	Bpp = nsoption_int(window_depth);
	
	return fav;
}

/** Create a toolbar window and populate it with buttons. 
 *
 * The toolbar layout uses a character to define buttons type and position:
 * b - back
 * l - local history
 * f - forward
 * s - stop 
 * r - refresh
 * u - url bar expands to fit remaining space
 * t - throbber/activity indicator
 * c - close the current window
 *
 * The default layout is "blfsrut" there should be no more than a
 * single url bar entry or behaviour will be undefined.
 *
 * @param gw Parent window 
 * @param toolbar_height The height in pixels of the toolbar
 * @param padding The padding in pixels round each element of the toolbar
 * @param frame_col Frame colour.
 * @param toolbar_layout A string defining which buttons and controls
 *                       should be added to the toolbar. May be empty
 *                       string to disable the bar..
 * 
 */

static fbtk_widget_t *
create_toolbar(struct gui_window *gw, 
	       int toolbar_height, 
	       int padding,
	       colour frame_col,
	       const char *toolbar_layout)
{
	fbtk_widget_t *toolbar;

	fbtk_widget_t *widget;

	int xpos; /* The position of the next widget. */
	int xlhs = 0; /* extent of the left hand side widgets */
	int xdir = 1; /* the direction of movement + or - 1 */
	int width = 0; /* width of widget */
	const char *itmtype; /* type of the next item */

	char *label = strdup("blfrhuvaqetk123456789xwzgdnmyop");
	
	if (toolbar_layout == NULL) {
		toolbar_layout = NSFB_TOOLBAR_DEFAULT_LAYOUT;
	}
		
	LOG(("Using toolbar layout %s", toolbar_layout));

	itmtype = toolbar_layout;

	if (*itmtype == 0) {
		return NULL;
	}

	toolbar = fbtk_create_window(gw->window, 0, 0, 0, 
				     toolbar_height, 
				     frame_col);

	if (toolbar == NULL) {
		return NULL;
	}

	fbtk_set_handler(toolbar, 
			 FBTK_CBT_POINTERENTER, 
			 set_ptr_default_move, 
			 NULL);


	xpos = padding;

	/* loop proceeds creating widget on the left hand side until
	 * it runs out of layout or encounters a url bar declaration
	 * wherupon it works backwards from the end of the layout
	 * untill the space left is for the url bar
	 */
	while ((itmtype >= toolbar_layout) && 
	       (*itmtype != 0) && 
	       (xdir !=0)) {

		LOG(("toolbar adding %c", *itmtype));

		switch (*itmtype) {

		case 'b': /* back */
			widget = fbtk_create_button(toolbar, 
						    (xdir == 1) ? xpos : 
						     xpos - left_arrow.width, 
						    padding, 
						    22, 
						    22, 
						    frame_col, 
						    load_bitmap(add_theme_path("/back_g.png")), 
						    fb_leftarrow_click, 
						    gw);
			gw->back = widget; /* keep reference */
			fbtk_set_handler(widget, FBTK_CBT_POINTERENTER, 
				 set_back_status, gw->bw);					
			break;

		case 'l': /* local history */
			widget = fbtk_create_button(toolbar,
						    (xdir == 1) ? xpos : 
						     xpos - left_arrow.width,
						    padding,
						    22,
						    22,
						    frame_col,
						    load_bitmap(add_theme_path("/history.png")), 
						    fb_localhistory_btn_clik,
						    gw);
			gw->history = widget;					
			fbtk_set_handler(widget, FBTK_CBT_POINTERENTER, 
				 set_local_status, gw->bw);							
			break;

		case 'f': /* forward */
			widget = fbtk_create_button(toolbar,
						    (xdir == 1)?xpos : 
						    xpos - right_arrow.width,
						    padding,
						    22,
						    22,
						    frame_col,
						    load_bitmap(add_theme_path("/forward_g.png")),
						    fb_rightarrow_click,
						    gw);
			gw->forward = widget;
			fbtk_set_handler(widget, FBTK_CBT_POINTERENTER, 
				 set_forward_status, gw->bw);					
			break;

		case 'c': /* close the current window */
#ifndef RTG	
			widget = fbtk_create_button(toolbar,
							73,
						    padding,
						    22,
						    22,
						    FB_FRAME_COLOUR,
						    load_bitmap(add_theme_path("/close.png")),
						    fb_close_click,
						    gw->bw);
			gw->close = widget;				
			fbtk_set_handler(widget, 
				 FBTK_CBT_POINTERENTER, set_close_status, gw->bw);
#endif			 
			break;

		case 's': /* stop  */
			widget = fbtk_create_button(toolbar,
						    (xdir == 1)?xpos : 
						    xpos - stop_image.width,
						    padding,
						    22,
						    22,
						    frame_col,
						    load_bitmap(add_theme_path("/stop.png")),
						    fb_stop_click,
						    gw->bw);
			gw->stop = widget;				
			fbtk_set_handler(widget, 
				 FBTK_CBT_POINTERENTER, set_stop_status, gw->bw);				 
			break;

		case 'r': /* reload */
#ifdef RTG	
			widget = fbtk_create_button(toolbar,
						    (xdir == 1)?xpos : 
						    xpos - 22,
						    padding,
						    22,
						    22,
						    frame_col,
						    load_bitmap(add_theme_path("/reload_g.png")),
						    fb_reload_click,
						    gw->bw);	 
			gw->reload = widget;				
			fbtk_set_handler(widget, 
				 FBTK_CBT_POINTERENTER, set_reload_status, gw->bw);			 
#endif	
			break;

		case 'h': /* home */
#ifdef RTG
			widget = fbtk_create_button(toolbar,
						    (xdir == 1)?xpos : 
						    xpos - stop_image.width,
						    padding,
						    22,
						    22,
						    frame_col,
						    load_bitmap(add_theme_path("/home.png")),
						    fb_home_click,
						    gw->bw);
			home = widget;				
			fbtk_set_handler(widget, FBTK_CBT_POINTERENTER, set_home_status, gw->bw);
#endif									
			break;				
			
		case 'u': /* url bar*/
			width = fbtk_get_width(gw->window) - xpos - 287;
#ifndef RTG
			width = width + 88;
			xpos = xpos - 47;
#endif			
			widget = fbtk_create_writable_text(toolbar,
						    xpos,
						    padding,
						    width,
						    23,
						    FB_COLOUR_WHITE,
							FB_COLOUR_BLACK,
							true,
						    fb_url_enter,
						    gw->bw);
							
			fbtk_set_handler(widget, 
					 FBTK_CBT_POINTERENTER, 
					 fb_url_move, gw->bw);
					 
			gw->url = widget;
			break;	

		case 'v': /* add to favourites button */
			widget = fbtk_create_button(toolbar,
						    fbtk_get_width(gw->window) - throbber0.width - 259
#ifndef RTG		
							+40
#endif							
							,
						    padding,
						    22,
						    22,
						    frame_col,
						    load_bitmap(add_theme_path("/add_fav.png")),
						    fb_add_fav_click,
						    gw->bw);
			addfav = widget;
			fbtk_set_handler(widget, FBTK_CBT_POINTERENTER, 
				 set_add_fav_status, gw->bw);								
			break;
			
		case 'a': /* add to bookmarks button */
			widget = fbtk_create_button(toolbar,
						    fbtk_get_width(gw->window) - throbber0.width - 237
#ifndef RTG		
							+40
#endif								
							,
						    padding,
						    22,
						    22,
						    frame_col,
						    load_bitmap(add_theme_path("/add_bookmark.png")),
						    fb_add_bookmark_click,
						    gw->bw);
			addbook = widget;				
			fbtk_set_handler(widget, FBTK_CBT_POINTERENTER, 
				 set_add_bookmark_status, gw->bw);	
				 
			break;
			
		case 'q': /* quick search button */
			widget = fbtk_create_button(toolbar,
						    fbtk_get_width(gw->window) - throbber0.width - 212
#ifndef RTG		
							+40
#endif								
							,
						    padding + 3,
						    16,
						    16,
						    frame_col,
						    load_bitmap("PROGDIR:Resources/Icons/search.png"),
						    fb_search_click,
						    gw->bw);
			quick = widget;
			fbtk_set_handler(widget, FBTK_CBT_POINTERENTER, 
				 set_search_status, gw->bw);						
			break;			

		case 'e': /* quick search bar*/
			widget = fbtk_create_writable_text(toolbar,
						    xpos+2,
						    padding,
						    185
#ifndef RTG		
							-40
#endif								
							,
						    23,
						    FB_COLOUR_WHITE,
							FB_COLOUR_BLACK,
							true,
						    fb_searchbar_enter,
						    gw->bw);						   
			searchbar = widget;
			fbtk_set_handler(widget, 
					 FBTK_CBT_POINTERENTER, 
					 fb_url_move, gw->bw);	
			break;
			
		case 't': /* throbber/activity indicator */
			widget = fbtk_create_bitmap(toolbar,
						    fbtk_get_width(gw->window) - throbber0.width - 3,
						    padding,
						    throbber0.width,
						    throbber0.height,
						    frame_col, 
						    &throbber0);
			gw->throbber = widget;
			break;			

		case 'k': /* open bookmarks button */
			xpos = 2;
			widget = fbtk_create_button(toolbar,
						    xpos,
						    padding + 25,
						    24,
						    24,
						    frame_col,
						    load_bitmap(add_theme_path("/bookmarks.png")),
						    fb_bookmarks_click,
						    gw->bw);
							
			fbtk_set_handler(widget, 
					 FBTK_CBT_POINTERENTER, 
					 set_bookmarks_status, gw->bw);								
			break;
			
		case '1': /* fav 1 button */
			text_w = szerokosc(nsoption_charp(favourite_1_label));
			xpos= xpos + 4;
			
			fav1 = fbtk_create_button(toolbar,
						    xpos,
						    padding + 30,
						    16,
						    16,
						    frame_col,
						    load_bitmap("PROGDIR:Resources/Icons/favicon1.png"),
						    fb_fav1_click,
						    gw->bw);
			fbtk_set_handler(fav1, FBTK_CBT_POINTERENTER, 
				 set_fav1_status, gw->bw);
				 
			widget = fbtk_create_text(gw->window,
				      xpos+22,
				      padding + 28,
				      text_w, 20,
				      FB_FRAME_COLOUR, FB_COLOUR_BLACK,
				      false);
					 			
			label = strndup(nsoption_charp(favourite_1_label), 10);			
			fbtk_set_text(widget, label);	
			
			fbtk_set_handler(widget, FBTK_CBT_CLICK, 
				 fb_fav1_click, gw->bw);	
			fbtk_set_handler(widget, FBTK_CBT_POINTERENTER, 
				 set_fav1_status, gw->bw);					 
			label1 = widget;
			break;	

		case '2': /* fav 2 button */
			xpos = xpos + 30;
			text_w = szerokosc(nsoption_charp(favourite_2_label));
			fav2 = fbtk_create_button(toolbar,
						    xpos,
						    padding + 30,
						    16,
						    16,
						    frame_col,
						    load_bitmap("PROGDIR:Resources/Icons/favicon2.png"),
						    fb_fav2_click,
						    gw->bw);
			fbtk_set_handler(fav2, FBTK_CBT_POINTERENTER, 
				 set_fav2_status, gw->bw);	
				 
			widget = fbtk_create_text(gw->window,
				      xpos+22,
				      padding + 28,
				      text_w, 20,
				      FB_FRAME_COLOUR, FB_COLOUR_BLACK,
				      false);
					 			
			label = strndup(nsoption_charp(favourite_2_label), 10);		
			fbtk_set_text(widget, label);	
			
			fbtk_set_handler(widget, FBTK_CBT_CLICK, 
				 fb_fav2_click, gw->bw);	
			fbtk_set_handler(widget, FBTK_CBT_POINTERENTER, 
				 set_fav2_status, gw->bw);
			label2 = widget;				 
			break;	
			
		case '3': /* fav 3 button */
			xpos = xpos + 30;
			text_w = szerokosc(nsoption_charp(favourite_3_label));
			fav3 = fbtk_create_button(toolbar,
						    xpos,
						    padding + 30,
						    16,
						    16,
						    frame_col,
						    load_bitmap("PROGDIR:Resources/Icons/favicon3.png"),
						    fb_fav3_click,
						    gw->bw);
			fbtk_set_handler(fav3, FBTK_CBT_POINTERENTER, 
				 set_fav3_status, gw->bw);		
				 
			widget = fbtk_create_text(gw->window,
				      xpos+22,
				      padding + 28,
				      text_w, 20,
				      FB_FRAME_COLOUR, FB_COLOUR_BLACK,
				      false);
					 			
			label = strndup(nsoption_charp(favourite_3_label), 10);			
			fbtk_set_text(widget, label);	
			
			fbtk_set_handler(widget, FBTK_CBT_CLICK, 
				 fb_fav3_click, gw->bw);
			fbtk_set_handler(widget, FBTK_CBT_POINTERENTER, 
				 set_fav3_status, gw->bw);	
			label3 = widget;				 
			 break;

		case '4': /* fav 4 button */
			xpos = x4 = xpos + 30;
			text_w = text_w4 = szerokosc(nsoption_charp(favourite_4_label));	
			fav4 = fbtk_create_button(toolbar,
						    xpos,
						    padding + 30,
						    16,
						    16,
						    frame_col,
						    load_bitmap("PROGDIR:Resources/Icons/favicon4.png"),
						    fb_fav4_click,
						    gw->bw);
			fbtk_set_handler(fav4, FBTK_CBT_POINTERENTER, 
				 set_fav4_status, gw->bw);		
				 
			widget = fbtk_create_text(gw->window,
				      xpos+22,
				      padding + 28,
				      text_w, 20,
				      FB_FRAME_COLOUR, FB_COLOUR_BLACK,
				      false);
					 			
			label = strndup(nsoption_charp(favourite_4_label), 10);			
			fbtk_set_text(widget, label);	
			
			fbtk_set_handler(widget, FBTK_CBT_CLICK, 
				 fb_fav4_click, gw->bw);	
			fbtk_set_handler(widget, FBTK_CBT_POINTERENTER, 
				 set_fav4_status, gw);					 
			label4 = widget;				 				 
			break;	

		case '5': /* fav 5 button */
			text_w5 = szerokosc(nsoption_charp(favourite_5_label));			
			x5 = x4 + text_w4 + 30;	
			if (nsoption_int(window_width) - 230 < (xpos)) break;
			
			fav5 = create_fav_widget(5, x5, text_w5, toolbar, gw);				 				 
			break;	

		case '6': /* fav 6 button */
#ifdef RTG
			text_w6 = szerokosc(nsoption_charp(favourite_6_label));			
			x6 = x5 + text_w5 + 30;	
			if (nsoption_int(window_width) - 250 < (xpos)) break;		
			fav6 = create_fav_widget(6, x6, text_w6, toolbar, gw);
#endif			
			break;	

		case '7': /* fav 7 button */	
#ifdef RTG		
			xpos = x7 = x6 + text_w6 + 30;
			text_w = text_w7 = szerokosc(nsoption_charp(favourite_7_label))-20;		
			if (nsoption_int(window_width) - 250 < (xpos)) break;
			fav7 = create_fav_widget(7, xpos, text_w, toolbar, gw);
#endif			
			break;	

		case '8': /* fav 8 button */
#ifdef RTG
			xpos = x8 = x7 + text_w7 + 30;
			text_w = text_w8 = szerokosc(nsoption_charp(favourite_8_label));		
			if (nsoption_int(window_width) - 250 < (xpos) ) break;
			fav8 = create_fav_widget(8, xpos, text_w, toolbar, gw);
#endif			
			break;	

		case '9': /* fav 9 button */
#ifdef RTG
			xpos = x9 = x8 + text_w8 + 30;
			text_w = text_w9 = szerokosc(nsoption_charp(favourite_9_label));		
			if (nsoption_int(window_width) - 250 < (xpos)) break;
			fav9 = create_fav_widget(9, xpos, text_w, toolbar, gw);
#endif			
			break;	

		case 'x': /* fav 10 button */
#ifdef RTG
			xpos = x10 = x9 + text_w9 + 30;
			text_w = text_w10 = szerokosc(nsoption_charp(favourite_10_label));	
			if (nsoption_int(window_width) - 250 < (xpos) ) break;
			fav10 = create_fav_widget(10, xpos, text_w, toolbar, gw);
#endif			
			break;			

		case 'w': /* fav 11 button */
#ifdef RTG
			xpos = x11 = x10 + text_w10 + 30;
			text_w = text_w11 = szerokosc(nsoption_charp(favourite_11_label));	
			if (nsoption_int(window_width) - 250 < (xpos) ) break;
			fav11 = create_fav_widget(11, xpos, text_w, toolbar, gw);
#endif			
			break;	

		case 'z': /* fav 12 button */
#ifdef RTG
			xpos = x12 = x11 + text_w11 + 30;	
			text_w = text_w12 = szerokosc(nsoption_charp(favourite_12_label));			
			if (nsoption_int(window_width) - 250 < (xpos)) break;
			fav12 = create_fav_widget(12, xpos, text_w, toolbar, gw);
#endif			
			break;	
		
		case 'g': /* edit preferences file button */
			xpos = fbtk_get_width(gw->window) - 138;	
#ifndef RTG		
			xpos = xpos	+ 20;
#endif				
			widget = fbtk_create_button(toolbar,
						    xpos,
						    padding + 35,
						    17,
						    17,
						    frame_col,
						    load_bitmap(add_theme_path("/prefs.png")),
						    fb_prefs_click,
						    gw->bw);
			prefs = widget;
			fbtk_set_handler(widget, FBTK_CBT_POINTERENTER, 
				 set_prefs_status, gw->bw);	
			break;	
			
		case 'd': /* download button */
			widget = fbtk_create_button(toolbar,
						    xpos,
						    padding + 30,
						    22,
						    22,
						    frame_col,
						    load_bitmap(add_theme_path("/getpage.png")),
						    fb_getpage_click,
						    gw->bw);
			download = widget;
			fbtk_set_handler(widget, FBTK_CBT_POINTERENTER, 
				 set_getpage_status, gw->bw);									
			break;	

		case 'm': /* set home button */
			widget = fbtk_create_button(toolbar,
						    xpos,
						    padding + 30,
						    22,
						    22,
						    frame_col,
						    load_bitmap(add_theme_path("/sethome.png")),
						    fb_sethome_click,
						    gw->bw);
			sethome = widget;
			fbtk_set_handler(widget, FBTK_CBT_POINTERENTER, 
				 set_sethome_status, gw->bw);									
			break;		

		case 'y': /* getvideo button */
			widget = fbtk_create_button(toolbar,
						    xpos,
						    padding + 30,
						    22,
						    22,
						    frame_col,
						    load_bitmap(add_theme_path("/getvideo.png")),
						    fb_getvideo_click,
						    gw->bw);
			getvideo = widget;
			fbtk_set_handler(widget, FBTK_CBT_POINTERENTER, 
				 set_getvideo_status, gw->bw);									
			break;				

		case 'o': /* copy text button */
			widget = fbtk_create_button(toolbar,
						    xpos,
						    padding + 30,
						    22,
						    22,
						    frame_col,
						    load_bitmap(add_theme_path("/copy.png")),
						    fb_copy_click,
						    gw->bw);
			copy = widget;
			fbtk_set_handler(widget, FBTK_CBT_POINTERENTER, 
				 set_copy_status, gw->bw);			
			break;	
			
		case 'p': /* paste text button */
			widget = fbtk_create_button(toolbar,
						    xpos,
						    padding + 30,
						    22,
						    22,
						    frame_col,
						    load_bitmap(add_theme_path("/paste.png")),
						    fb_paste_click,
						    gw->bw);
			paste = widget;
			fbtk_set_handler(widget, FBTK_CBT_POINTERENTER, 
				 set_paste_status, gw->bw);		
			/* toolbar is complete */
			xdir = 0;				 
			break;		
		
			/* met url going forwards, note position and
			 * reverse direction 
			 */
			itmtype = toolbar_layout + strlen(toolbar_layout);
			xdir = -1;
			xlhs = xpos;
			xpos = (1 * fbtk_get_width(toolbar)); /* ?*/
			widget = toolbar;
	
			break;

		default:
			widget = NULL;
			xdir = 0;
			LOG(("Unknown element %c in toolbar layout", *itmtype));
		        break;

		}

		if (widget != NULL) {
			xpos += (xdir * (fbtk_get_width(widget) + padding));
		}

		LOG(("xpos is %d",xpos));

		itmtype += xdir;
	}

	fbtk_set_mapping(toolbar, true);
	
	Bpp = nsoption_int(window_depth);

	return toolbar;
}

/** Resize a toolbar.
 *
 * @param gw Parent window
 * @param toolbar_height The height in pixels of the toolbar
 * @param padding The padding in pixels round each element of the toolbar
 * @param toolbar_layout A string defining which buttons and controls
 *                       should be added to the toolbar. May be empty
 *                       string to disable the bar.
 */
static void
resize_toolbar(struct gui_window *gw,
	       int toolbar_height,
	       int padding,
	       const char *toolbar_layout)
{
	fbtk_widget_t *widget;

	int xpos; /* The position of the next widget. */
	int xlhs = 0; /* extent of the left hand side widgets */
	int xdir = 1; /* the direction of movement + or - 1 */
	const char *itmtype; /* type of the next item */
	int x = 0, y = 0, w = 0, h = 0;

	if (gw->toolbar == NULL)
		return;

	if (toolbar_layout == NULL)
		toolbar_layout = NSFB_TOOLBAR_DEFAULT_LAYOUT;
	
	int calc_favs;
	calc_favs = (fbtk_get_width(gw->window) - 100) / 100;

	if (calc_favs > 12) calc_favs = 12;

	switch (calc_favs) {
	case 12:
		toolbar_layout = strdup("blfrhuvaqetk123456789xwzgdnmyop");
		break;
	case 11:
		toolbar_layout = strdup("blfrhuvaqetk123456789xwgdnmyop");
		if (fav12 != NULL) {
			fbtk_destroy_widget(fav12);
			fbtk_destroy_widget(label12);
			fav12 = NULL;
			label12 = NULL;	}		
		break;		
	case 10:
		toolbar_layout = strdup("blfrhuvaqetk123456789xgdnmyop");
		if (fav12 != NULL) {
			fbtk_destroy_widget(fav12);
			fbtk_destroy_widget(label12);
			fav12 = NULL;
			label12 = NULL;	}			
		if (fav11 != NULL) {
			fbtk_destroy_widget(fav11);
			fbtk_destroy_widget(label11);
			fav11 = NULL;
			label11 = NULL;	}		
		break;
	case 9:
		toolbar_layout = strdup("blfrhuvaqetk123456789gdnmyop");
		if (fav12 != NULL) {
			fbtk_destroy_widget(fav12);
			fbtk_destroy_widget(label12);
			fav12 = NULL;
			label12 = NULL;	}			
		if (fav11 != NULL) {
			fbtk_destroy_widget(fav11);
			fbtk_destroy_widget(label11);
			fav11 = NULL;
			label11 = NULL;	}			
		if (fav10 != NULL) {
			fbtk_destroy_widget(fav10);
			fbtk_destroy_widget(label10);
			fav10 = NULL;
			label10 = NULL;	}	
		break;
	case 8:
		toolbar_layout = strdup("blfrhuvaqetk12345678gdnmyop");
		if (fav12 != NULL) {
			fbtk_destroy_widget(fav12);
			fbtk_destroy_widget(label12);
			fav12 = NULL;
			label12 = NULL;	}			
		if (fav11 != NULL) {
			fbtk_destroy_widget(fav11);
			fbtk_destroy_widget(label11);
			fav11 = NULL;
			label11 = NULL;	}			
		if (fav10 != NULL) {
			fbtk_destroy_widget(fav10);
			fbtk_destroy_widget(label10);
			fav10 = NULL;
			label10 = NULL;	}
		if (fav9 != NULL) {
			fbtk_destroy_widget(fav9);
			fbtk_destroy_widget(label9);
			fav9 = NULL;
			label9 = NULL;	}
		break;
	case 7:
		toolbar_layout = strdup("blfrhuvaqetk1234567gdnmyop");
		if (fav12 != NULL) {
			fbtk_destroy_widget(fav12);
			fbtk_destroy_widget(label12);
			fav12 = NULL;
			label12 = NULL;	}			
		if (fav11 != NULL) {
			fbtk_destroy_widget(fav11);
			fbtk_destroy_widget(label11);
			fav11 = NULL;
			label11 = NULL;	}			
		if (fav10 != NULL) {
			fbtk_destroy_widget(fav10);
			fbtk_destroy_widget(label10);
			fav10 = NULL;
			label10 = NULL;	}
		if (fav9 != NULL) {
			fbtk_destroy_widget(fav9);
			fbtk_destroy_widget(label9);
			fav9 = NULL;
			label9 = NULL;	}
		if (fav8 != NULL) {
			fbtk_destroy_widget(fav8);
			fbtk_destroy_widget(label8);
			fav8 = NULL;
			label8 = NULL;	}	
		break;	
	case 6:
		toolbar_layout = strdup("blfrhuvaqetk123456gdnmyop");
		if (fav12 != NULL) {
			fbtk_destroy_widget(fav12);
			fbtk_destroy_widget(label12);
			fav12 = NULL;
			label12 = NULL;	}			
		if (fav11 != NULL) {
			fbtk_destroy_widget(fav11);
			fbtk_destroy_widget(label11);
			fav11 = NULL;
			label11 = NULL;	}			
		if (fav10 != NULL) {
			fbtk_destroy_widget(fav10);
			fbtk_destroy_widget(label10);
			fav10 = NULL;
			label10 = NULL;	}
		if (fav9 != NULL) {
			fbtk_destroy_widget(fav9);
			fbtk_destroy_widget(label9);
			fav9 = NULL;
			label9 = NULL;	}
		if (fav8 != NULL) {
			fbtk_destroy_widget(fav8);
			fbtk_destroy_widget(label8);
			fav8 = NULL;
			label8 = NULL;	}	
		if (fav7 != NULL) {
			fbtk_destroy_widget(fav7);
			fbtk_destroy_widget(label7);
			fav7 = NULL;
			label7 = NULL;	}	
		break;
	case 5:
		toolbar_layout = strdup("blfrhuvaqetk12345gdnmyop");
		if (fav12 != NULL) {
			fbtk_destroy_widget(fav12);
			fbtk_destroy_widget(label12);
			fav12 = NULL;
			label12 = NULL;	}			
		if (fav11 != NULL) {
			fbtk_destroy_widget(fav11);
			fbtk_destroy_widget(label11);
			fav11 = NULL;
			label11 = NULL;	}			
		if (fav10 != NULL) {
			fbtk_destroy_widget(fav10);
			fbtk_destroy_widget(label10);
			fav10 = NULL;
			label10 = NULL;	}
		if (fav9 != NULL) {
			fbtk_destroy_widget(fav9);
			fbtk_destroy_widget(label9);
			fav9 = NULL;
			label9 = NULL;	}
		if (fav8 != NULL) {
			fbtk_destroy_widget(fav8);
			fbtk_destroy_widget(label8);
			fav8 = NULL;
			label8 = NULL;	}	
		if (fav7 != NULL) {
			fbtk_destroy_widget(fav7);
			fbtk_destroy_widget(label7);
			fav7 = NULL;
			label7 = NULL;	}	
		if (fav6 != NULL) {
			fbtk_destroy_widget(fav6);
			fbtk_destroy_widget(label6);
			fav6 = NULL;
			label6 = NULL;	}	
		break;		
	case 4:
		toolbar_layout = strdup("blfrhuvaqetk1234gdnmyop");
		if (fav12 != NULL) {
			fbtk_destroy_widget(fav12);
			fbtk_destroy_widget(label12);
			fav12 = NULL;
			label12 = NULL;	}			
		if (fav11 != NULL) {
			fbtk_destroy_widget(fav11);
			fbtk_destroy_widget(label11);
			fav11 = NULL;
			label11 = NULL;	}			
		if (fav10 != NULL) {
			fbtk_destroy_widget(fav10);
			fbtk_destroy_widget(label10);
			fav10 = NULL;
			label10 = NULL;	}
		if (fav9 != NULL) {
			fbtk_destroy_widget(fav9);
			fbtk_destroy_widget(label9);
			fav9 = NULL;
			label9 = NULL;	}
		if (fav8 != NULL) {
			fbtk_destroy_widget(fav8);
			fbtk_destroy_widget(label8);
			fav8 = NULL;
			label8 = NULL;	}	
		if (fav7 != NULL) {
			fbtk_destroy_widget(fav7);
			fbtk_destroy_widget(label7);
			fav7 = NULL;
			label7 = NULL;	}	
		if (fav6 != NULL) {
			fbtk_destroy_widget(fav6);
			fbtk_destroy_widget(label6);
			fav6 = NULL;
			label6 = NULL;	}	
		if (fav5 != NULL) {
			fbtk_destroy_widget(fav5);
			fbtk_destroy_widget(label5);
			fav5 = NULL;
			label5 = NULL;
			}	
		break;
		}
		
	if (calc_favs < 5 )
		toolbar_layout = strdup("blfrhuvaqetk1234gdnmyop");
				
	itmtype = toolbar_layout;

	if (*itmtype == 0) {
		return;
	}

	fbtk_set_pos_and_size(gw->toolbar, 0, 0, 0, toolbar_height);

	xpos = padding;

	/* loop proceeds creating widget on the left hand side until
	 * it runs out of layout or encounters a url bar declaration
	 * wherupon it works backwards from the end of the layout
	 * untill the space left is for the url bar
	 */
	while (itmtype >= toolbar_layout && xdir != 0) {

		switch (*itmtype) {
		case 'b': /* back */
			widget = gw->back;
			x = (xdir == 1) ? xpos : xpos - 22;
			y = padding;
			w = 22;
			h = 22;
			break;

		case 'l': /* local history */
			widget = gw->history;
			x = (xdir == 1) ? xpos : xpos - 22;
			y = padding;
			w = 22;
			h = 22;
			break;

		case 'f': /* forward */
			widget = gw->forward;
			x = (xdir == 1) ? xpos : xpos - 22;
			y = padding;
			w = 22;
			h = 22;
			break;

		case 'c': /* close the current window */
			widget = gw->close;
			x = (xdir == 1) ? xpos : xpos - 22;
			y = padding;
			w = 22;
			h = 22;
			break;

		case 's': /* stop  */
			widget = gw->stop;
			x = (xdir == 1) ? xpos : xpos - 22;
			y = padding;
			w = 22;
			h = 22;
			break;

		case 'r': /* reload */
			widget = gw->reload;
			x = (xdir == 1) ? xpos : xpos - 22;
			y = padding;
			w = 22;
			h = 22;
			break;
			
		case 'h': /* home */
			widget = home;
			x = (xdir == 1) ? xpos : xpos - 22;
			y = padding;
			w = 22;
			h = 22;
			break;
			
		case 'v': /* add to favourites button */
			widget = addfav;
			x = fbtk_get_width(gw->window) - throbber0.width - 259;
			y = padding;
			w = 22;
			h = 22;	 						
			break;
			
		case 'a': /* add to bookmarks button */
			widget = addbook;			
			x = fbtk_get_width(gw->window) - throbber0.width - 237;
			y = padding;
			w = 22;
			h = 22;	 
			break;
			
		case 'q': /* quick search button */
			widget = quick;
			x = fbtk_get_width(gw->window) - throbber0.width - 212;
			y = padding+3;
			w = 16;
			h = 16;	 
			break;	
			
		case 't': /* throbber/activity indicator */
			widget = gw->throbber;
			x = fbtk_get_width(gw->window) - throbber0.width - 2;
			y = padding;
			w = throbber0.width;
			h = throbber0.height;
			break;

		case 'e':  /* searchbar */
			widget = searchbar;
			x = fbtk_get_width(gw->toolbar) - 215;
			y = padding;
			w = 185;
			h = 23;			
			break;
			
		case 'u': /* url bar*/
			if (xdir == -1) {
				/* met the u going backwards add url
				 * now we know available extent
				 */
				widget = gw->url;
				x = 122;//xlhs;
				y = padding;
				w = fbtk_get_width(gw->window) - 409;
				h = 23;

				/* toolbar is complete */
				xdir = 0;
				break;

		case '5':
			if (fav5 == NULL){	
				text_w5 = szerokosc(nsoption_charp(favourite_5_label));			
				x5 = x4 + text_w4 + 30;	
				fav5 = create_fav_widget(5, x5, text_w5, gw->toolbar, gw);
				widget = fav5;	
				x = x5;
				y = 32;
				w = 16;
				h = 16;									
			} 			
			break;
	
		case '6':
			if (fav6 == NULL){	
				text_w6 = szerokosc(nsoption_charp(favourite_6_label));			
				x6 = x5 + text_w5 + 30;	
				fav6 = create_fav_widget(6, x6, text_w6, gw->toolbar, gw);
				widget = fav6;	
				x = x6;
				y = 32;
				w = 16;
				h = 16;									
			} 		
			break;
			
		case '7':
			if (fav7 == NULL){	
				text_w7 = szerokosc(nsoption_charp(favourite_7_label))-20;			
				x7 = x6 + text_w6 + 30;	
				fav7 = create_fav_widget(7, x7, text_w7, gw->toolbar, gw);
				widget = fav7;	
				x = x7;
				y = 32;
				w = 16;
				h = 16;									
			} 
			break;
	
		case '8':
			if (fav8 == NULL){	
				text_w8 = szerokosc(nsoption_charp(favourite_8_label));			
				x8 = x7 + text_w7 + 30;	
				fav8 = create_fav_widget(8, x8, text_w8, gw->toolbar, gw);
				widget = fav8;	
				x = x8;
				y = 32;
				w = 16;
				h = 16;									
			} 
			break;
			
		case '9':
			if (fav9 == NULL){	
				text_w9 = szerokosc(nsoption_charp(favourite_9_label));			
				x9 = x8 + text_w8 + 30;	
				fav9 = create_fav_widget(9, x9, text_w9, gw->toolbar, gw);
				widget = fav9;	
				x = x9;
				y = 32;
				w = 16;
				h = 16;									
			} 	
			break;
			
		case 'x':
			if (fav10 == NULL){	
				text_w10 = szerokosc(nsoption_charp(favourite_10_label));			
				x10 = x9 + text_w9 + 30;	
				fav10 = create_fav_widget(10, x10, text_w10, gw->toolbar, gw);
				widget = fav10;	
				x = x10;
				y = 32;
				w = 16;
				h = 16;									
			} 
			break;
			
		case 'w':
			if (fav11 == NULL){	
				text_w11 = szerokosc(nsoption_charp(favourite_11_label));			
				x11 = x10 + text_w10 + 30;	
				fav11 = create_fav_widget(11, x11, text_w11, gw->toolbar, gw);
				widget = fav11;	
				x = x11;
				y = 32;
				w = 16;
				h = 16;									
			} 
			break;	

		case 'z':
			if (fav12 == NULL){
				text_w12 = szerokosc(nsoption_charp(favourite_12_label));			
				x12 = x11 + text_w11 + 30;	
				fav12 = create_fav_widget(12, x12, text_w12, gw->toolbar, gw);
				widget = fav12;
				x = x12;
				y = 32;
				w = 16;
				h = 16;									
			} 
			break;
			
		case 'g': /* edit preferences file button */		
			widget = prefs;
			
			x = fbtk_get_width(gw->window) - 135;
			y = padding + 35;
			w = 17;
			h = 17;										
			break;	
			
		case 'd': /* download button */
			widget = download;
			
			x = fbtk_get_width(gw->window) - 115;
			y = padding + 30;
			w = 22;
			h = 22;											
			break;	

		case 'm': /* set home button */
			widget = sethome;
			
			x = fbtk_get_width(gw->window) - 92;
			y = padding + 30;
			w = 22;
			h = 22;									
			break;		

		case 'y': /* getvideo button */
			widget = getvideo;	
			
			x = fbtk_get_width(gw->window) - 69;
			y = padding + 30;
			w = 22;
			h = 22;					
			break;				

		case 'o': /* copy text button */
			widget = copy;	
			
			x = fbtk_get_width(gw->window) - 46;
			y = padding + 30;
			w = 22;
			h = 22;			
			break;	
			
		case 'p': /* paste text button */
			widget = paste;	
			
			x = fbtk_get_width(gw->window) - 24;
			y = padding + 30;
			w = 22;
			h = 22;			
			break;	
			}


			/* met url going forwards, note position and
			 * reverse direction
			 */
			itmtype = toolbar_layout + strlen(toolbar_layout);
			xdir = -1;
			xlhs = xpos;
			w = fbtk_get_width(gw->toolbar);
			xpos = 2 * w;
			widget = gw->toolbar;
			break;

		default:
			widget = NULL;
		        break;

		}

		if (widget != NULL) {
			if (widget != gw->toolbar)
				fbtk_set_pos_and_size(widget, x, y, w, h);
			xpos += xdir * (w + padding);
		}
		itmtype += xdir;
	}
}

/** Routine called when "stripped of focus" event occours for browser widget.
 *
 * @param widget The widget reciving "stripped of focus" event.
 * @param cbi The callback parameters.
 * @return The callback result.
 */
static int
fb_browser_window_strip_focus(fbtk_widget_t *widget, fbtk_callback_info *cbi)
{
	fbtk_set_caret(widget, false, 0, 0, 0, NULL);

	return 0;
}

static void
create_browser_widget(struct gui_window *gw, int toolbar_height, int furniture_width)
{
	struct browser_widget_s *browser_widget;
	browser_widget = calloc(1, sizeof(struct browser_widget_s));

	gw->browser = fbtk_create_user(gw->window,
				       0,
				       toolbar_height,
				       -furniture_width,
				       -furniture_width,
				       browser_widget);

	fbtk_set_handler(gw->browser, FBTK_CBT_REDRAW, fb_browser_window_redraw, gw);
	fbtk_set_handler(gw->browser, FBTK_CBT_DESTROY, fb_browser_window_destroy, gw);
	fbtk_set_handler(gw->browser, FBTK_CBT_INPUT, fb_browser_window_input, gw);
	fbtk_set_handler(gw->browser, FBTK_CBT_CLICK, fb_browser_window_click, gw);

	fbtk_set_handler(gw->browser, FBTK_CBT_STRIP_FOCUS, fb_browser_window_strip_focus, gw);
	fbtk_set_handler(gw->browser, FBTK_CBT_POINTERMOVE, fb_browser_window_move, gw);
}

static void
resize_browser_widget(struct gui_window *gw, int x, int y,
		int width, int height)
{
	fbtk_set_pos_and_size(gw->browser, x, y, width, height);
	browser_window_reformat(gw->bw, false, width, height);
}


static void
create_normal_browser_window(struct gui_window *gw, int furniture_width)
{
	fbtk_widget_t *widget;
	fbtk_widget_t *toolbar;
	int statusbar_width = 0;
	int toolbar_height = nsoption_int(fb_toolbar_size);

	LOG(("Normal window"));

	gw->window = fbtk_create_window(fbtk, 0, 0, 0, 0, 0);

	statusbar_width = nsoption_int(toolbar_status_size) *
		fbtk_get_width(gw->window) / 10000;
		
	/* toolbar */
	toolbar = create_toolbar(gw, 
				 toolbar_height, 
				 2, 
				 FB_FRAME_COLOUR, 
#ifdef RTG
				 nsoption_charp(fb_toolbar_layout));
#else
				 NULL);
#endif
	gw->toolbar = toolbar;
	
	/* set the actually created toolbar height */
	if (toolbar != NULL) {
		toolbar_height = fbtk_get_height(toolbar);
	} else {
		toolbar_height = 0;
	}

	/* status bar */
	gw->status = fbtk_create_text(gw->window,
				      0,
				      fbtk_get_height(gw->window) - furniture_width,
				      statusbar_width, furniture_width,
				      FB_FRAME_COLOUR, FB_COLOUR_BLACK,
				      false);

	LOG(("status bar %p at %d,%d", gw->status, fbtk_get_absx(gw->status), fbtk_get_absy(gw->status)));

	/* create horizontal scrollbar */
	gw->hscroll = fbtk_create_hscroll(gw->window,
					  statusbar_width,
					  fbtk_get_height(gw->window) - furniture_width,
					  fbtk_get_width(gw->window) - statusbar_width - furniture_width,
					  furniture_width,
					  nsoption_colour(sys_colour_Scrollbar),
					  FB_FRAME_COLOUR,
					  fb_scroll_callback,
					  gw);

	/* fill bottom right area */

	widget = fbtk_create_fill(gw->window,
					  fbtk_get_width(gw->window) - furniture_width,
					  fbtk_get_height(gw->window) - furniture_width,
					  furniture_width,
					  furniture_width,
					  FB_FRAME_COLOUR);

	fbtk_set_handler(widget, FBTK_CBT_POINTERENTER, set_ptr_default_move, NULL);

	gw->bottom_right = widget;
	
	/* create vertical scrollbar */
	gw->vscroll = fbtk_create_vscroll(gw->window,
					  fbtk_get_width(gw->window) - furniture_width,
					  toolbar_height,
					  furniture_width,
					  fbtk_get_height(gw->window) - toolbar_height - furniture_width,
					  nsoption_colour(sys_colour_Scrollbar),
					  FB_FRAME_COLOUR,
					  fb_scroll_callback,
					  gw);

	/* browser widget */
	create_browser_widget(gw, toolbar_height, nsoption_int(fb_furniture_size));

	/* Give browser_window's user widget input focus */
	fbtk_set_focus(gw->browser);
}

static void
resize_normal_browser_window(struct gui_window *gw, int furniture_width)
{
	bool resized;
	int width, height;
	int statusbar_width;
	int toolbar_height = fbtk_get_height(gw->toolbar);

	/* Resize the main window widget */
	resized = fbtk_set_pos_and_size(gw->window, 0, 0, 0, 0);
	if (!resized)
		return;

	width = fbtk_get_width(gw->window);
	height = fbtk_get_height(gw->window);
	statusbar_width = nsoption_int(toolbar_status_size) * width / 10000;

	resize_toolbar(gw, toolbar_height, 2,
			nsoption_charp(fb_toolbar_layout));
	fbtk_set_pos_and_size(gw->status,
			0, height - furniture_width,
			statusbar_width, furniture_width);
	fbtk_reposition_hscroll(gw->hscroll,
			statusbar_width, height - furniture_width,
			width - statusbar_width - furniture_width,
			furniture_width);
	fbtk_set_pos_and_size(gw->bottom_right,
			width - furniture_width, height - furniture_width,
			furniture_width, furniture_width);
	fbtk_reposition_vscroll(gw->vscroll,
			width - furniture_width,
			toolbar_height, furniture_width,
			height - toolbar_height - furniture_width);
	resize_browser_widget(gw,
			0, toolbar_height,
			width - furniture_width,
			height - furniture_width - toolbar_height);
}

static void gui_window_add_to_window_list(struct gui_window *gw)
{
	gw->next = NULL;
	gw->prev = NULL;

	if (window_list == NULL) {
		window_list = gw;
	} else {
		window_list->prev = gw;
		gw->next = window_list;
		window_list = gw;
	}
}

static void gui_window_remove_from_window_list(struct gui_window *gw)
{
	struct gui_window *list;

	for (list = window_list; list != NULL; list = list->next) {
		if (list != gw)
			continue;

		if (list == window_list) {
			window_list = list->next;
			if (window_list != NULL)
				window_list->prev = NULL;
		} else {
			list->prev->next = list->next;
			if (list->next != NULL) {
				list->next->prev = list->prev;
			}
		}
		break;
	}
}

static struct gui_window *
gui_window_create(struct browser_window *bw,
		struct gui_window *existing,
		gui_window_create_flags flags)
{
	struct gui_window *gw;

	gw = calloc(1, sizeof(struct gui_window));

	if (gw == NULL)
		return NULL;

	/* associate the gui window with the underlying browser window
 	 */
	gw->bw = bw;

	create_normal_browser_window(gw, nsoption_int(fb_furniture_size));
	gw->localhistory = fb_create_localhistory(bw, fbtk, nsoption_int(fb_furniture_size));

	/* map and request redraw of gui window */
	fbtk_set_mapping(gw->window, true);

	/* Add it to the window list */
	gui_window_add_to_window_list(gw);
	
	return gw;
}

static void
gui_window_destroy(struct gui_window *gw)
{
	gui_window_remove_from_window_list(gw);
	fbtk_destroy_widget(gw->window);

	free(gw);
}

static void
gui_window_set_title(struct gui_window *g, const char *title)
{
	stitle = strdup(title);
	utf8_to_local_encoding(title,strlen(title),&stitle);
	strcat(stitle," - Greyhound");
	SDL_WM_SetCaption(stitle, "Greyhound");
	stitle = strdup(title);	
	utf8_from_local_encoding(title,strlen(title),&stitle);

	LOG(("%p, %s", g, title));
}

void
gui_window_redraw(struct gui_window *g, int x0, int y0, int x1, int y1)
{
	fb_queue_redraw(g->browser, x0, y0, x1, y1);
}

void
gui_window_redraw_window(struct gui_window *g)
{
	fb_queue_redraw(g->browser, 0, 0, fbtk_get_width(g->browser), fbtk_get_height(g->browser) );
	g_ui = g;
}

static void
gui_window_update_box(struct gui_window *g, const struct rect *rect)
{
	struct browser_widget_s *bwidget = fbtk_get_userpw(g->browser);
	fb_queue_redraw(g->browser,
			rect->x0 - bwidget->scrollx,
			rect->y0 - bwidget->scrolly,
			rect->x1 - bwidget->scrollx,
			rect->y1 - bwidget->scrolly);
}

static bool
gui_window_get_scroll(struct gui_window *g, int *sx, int *sy)
{
	struct browser_widget_s *bwidget = fbtk_get_userpw(g->browser);
	float scale = browser_window_get_scale(g->bw);

	*sx = bwidget->scrollx / scale;
	*sy = bwidget->scrolly / scale;

	return true;
}

static void
gui_window_set_scroll(struct gui_window *gw, int sx, int sy)
{
	struct browser_widget_s *bwidget = fbtk_get_userpw(gw->browser);
	float scale = browser_window_get_scale(gw->bw);

	assert(bwidget);

	widget_scroll_x(gw, sx * scale, true);
	widget_scroll_y(gw, sy * scale, true);
}

static void
gui_window_get_dimensions(struct gui_window *g,
			  int *width,
			  int *height,
			  bool scaled)
{
	float scale = browser_window_get_scale(g->bw);
	*width = fbtk_get_width(g->browser);
	*height = fbtk_get_height(g->browser);

	if (scaled) {
		*width /= scale;
		*height /= scale;
	}
}

static void
gui_window_update_extent(struct gui_window *gw)
{
	int w, h;
	browser_window_get_extents(gw->bw, true, &w, &h);

	fbtk_set_scroll_parameters(gw->hscroll, 0, w,

			fbtk_get_width(gw->browser), 100);

	fbtk_set_scroll_parameters(gw->vscroll, 0, h,

			fbtk_get_height(gw->browser), 100);
}

void
gui_window_set_status(struct gui_window *g, const char *text)
{
	if (text)
		{
		fbtk_set_text(g->status, text);
		status_txt = strdup(text);
		}
}

static void
gui_window_set_pointer(struct gui_window *g, gui_pointer_shape shape)
{
	switch (shape) {
	case GUI_POINTER_POINT:
		framebuffer_set_cursor(&hand_image);
		SDL_ShowCursor(SDL_DISABLE);
		break;

	case GUI_POINTER_CARET:
		framebuffer_set_cursor(&caret_image);
		SDL_ShowCursor(SDL_DISABLE);
		break;

	case GUI_POINTER_MENU:
		framebuffer_set_cursor(&menu_image);
		#ifdef  RTG
			SDL_ShowCursor(SDL_ENABLE);	
		#else
			SDL_ShowCursor(SDL_DISABLE);
		#endif     
		break;

	case GUI_POINTER_PROGRESS:
		framebuffer_set_cursor(&progress_image);
		SDL_ShowCursor(SDL_DISABLE);                
		break;

				
	case GUI_POINTER_MOVE:
		framebuffer_set_cursor(&hand_image);
		SDL_ShowCursor(SDL_DISABLE); 
		break;
		
    default:	            				
		#ifdef  RTG
			SDL_ShowCursor(SDL_ENABLE);
			framebuffer_set_cursor(&null_image);
		#else
			SDL_ShowCursor(SDL_DISABLE);
			framebuffer_set_cursor(&pointer_image);
		#endif
		break;			           
        }
}

void
gui_window_hide_pointer(struct gui_window *g)
{
	SDL_ShowCursor(SDL_ENABLE);
	framebuffer_set_cursor(&null_image);
}

static void
gui_window_set_url(struct gui_window *g, const char *url)
{
	fbtk_set_text(g->url, url);
}

static void
throbber_advance(void *pw)
{
	struct gui_window *g = pw;
	struct fbtk_bitmap *image;

	switch (g->throbber_index) {
	case 0:
		image = &throbber1;
		g->throbber_index = 1;
		break;

	case 1:
		image = &throbber2;
		g->throbber_index = 2;
		break;

	case 2:
		image = &throbber3;
		g->throbber_index = 3;
		break;

	case 3:
		image = &throbber4;
		g->throbber_index = 4;
		break;

	case 4:
		image = &throbber5;
		g->throbber_index = 5;
		break;

	case 5:
		image = &throbber6;
		g->throbber_index = 6;
		break;

	case 6:
		image = &throbber7;
		g->throbber_index = 7;
		break;

	case 7:
		image = &throbber8;

		g->throbber_index = 8;
		break;

	case 8:
		image = &throbber9;
		g->throbber_index = 9;
		break;

	case 9:
		image = &throbber10;
		g->throbber_index = 10;
		break;

	case 10:
		image = &throbber11;
		g->throbber_index = 11;
		break;
		
	case 11:
		image = &throbber12;
		g->throbber_index = 12;
		break;	
		
	case 12:
		image = &throbber0;
		g->throbber_index = 0;
		break;			
	default:
		return;
	}

	if (g->throbber_index >= 0) {
		fbtk_set_bitmap(g->throbber, image);
		framebuffer_schedule(100, throbber_advance, g);
	}
}

static void
gui_window_start_throbber(struct gui_window *g)
{
	g->throbber_index = 0;
	framebuffer_schedule(100, throbber_advance, g);
}

static void
gui_window_stop_throbber(struct gui_window *gw)
{
	gw->throbber_index = -1;
	fbtk_set_bitmap(gw->throbber, &throbber0);

	fb_update_back_forward(gw);

}

static void
gui_window_remove_caret_cb(fbtk_widget_t *widget)
{
	struct browser_widget_s *bwidget = fbtk_get_userpw(widget);
	int c_x, c_y, c_h;

	if (fbtk_get_caret(widget, &c_x, &c_y, &c_h)) {
		/* browser window already had caret:
		 * redraw its area to remove it first */
		fb_queue_redraw(widget,
				c_x - bwidget->scrollx,
				c_y - bwidget->scrolly,
				c_x + 1 - bwidget->scrollx,
				c_y + c_h - bwidget->scrolly);
	}
}

static void
gui_window_place_caret(struct gui_window *g, int x, int y, int height,
		const struct rect *clip)
{
	struct browser_widget_s *bwidget = fbtk_get_userpw(g->browser);

	/* set new pos */
	fbtk_set_caret(g->browser, true, x, y, height,
			gui_window_remove_caret_cb);

	/* redraw new caret pos */
	fb_queue_redraw(g->browser,
			x - bwidget->scrollx,
			y - bwidget->scrolly,
			x + 1 - bwidget->scrollx,
			y + height - bwidget->scrolly);
}

static void
gui_window_remove_caret(struct gui_window *g)
{
	int c_x, c_y, c_h;

	if (fbtk_get_caret(g->browser, &c_x, &c_y, &c_h)) {
		/* browser window owns the caret, so can remove it */
		fbtk_set_caret(g->browser, false, 0, 0, 0, NULL);
	}
}

static struct gui_download_window *
gui_download_window_create(download_context *ctx, struct gui_window *parent)
{		
	if (Cbi->event->type == NSFB_EVENT_KEY_UP)
		return 0;
			
	char *url = strdup(nsurl_access(download_context_get_url(ctx)));
	const char *mime_type = download_context_get_mime_type(ctx);
	
	char *run = AllocVec(1000, MEMF_CLEAR | MEMF_ANY);	
	bool wget = false;
	
	if (mouse_2_click == 0)	
		{
		if (strcmp(mime_type, "audio/x-mpegurl") == 0) {
			strlcpy(url, url, strlen(url)-3);
			mime_type = strdup("video");
			}
		}	
		
	if ((strcmp(mime_type, "audio/mpeg") == 0) && (mouse_2_click == 0) )
			{	
			strcpy(run, "Run ");	
			strcat(run, nsoption_charp(net_player));	
			strcat(run, "  "); 
			}
	else if ((strncmp(mime_type, "video", 5) == 0) && (mouse_2_click == 0) )
			{	
			strcpy(run, "Run ");	
			strcat(run, "ffplay");	
			strcat(run, "  "); 
			}	
	else 
		{
		if (strcmp(nsoption_charp(download_manager), "wallget")==0)
			strcpy(run, "run Sys:Rexxc/rx rexx/wallget.rexx ");	
		else if (strcmp(nsoption_charp(download_manager), "httpresume")==0)
			strcpy(run, "run c/httpresume GUI STARTDIR=");	
		else {
			strcpy(run, "run c/wget ");
			strcat(run, "--max-filename-length=18 -P");	
			wget = true;
			}
		strcat(run, nsoption_charp(download_path));		
		strcat(run, " ");				
		}
		
	strcat(run, url);
	
	if (wget)
		{
		BPTR fh;
		fh = Open("CON:", MODE_NEWFILE);
		Execute(run, fh, 0);	
		Close(fh);	
		}
	else	
		{
		SetTaskPri(FindTask(0), -1);
		Execute(run, 0, 0);	
		}

	mouse_2_click = 0;		
	
	free(url);
	FreeVec(run);
		
	return NULL;
}

void
gui_launch_url(const char *url)
{
	if ((!strncmp("mailto:", url, 7)) || (!strncmp("ftp:", url, 4)))
	{
		if (OpenURLBase == NULL)
			OpenURLBase = OpenLibrary("openurl.library", 6);

		if (OpenURLBase)		{
			URL_OpenA((STRPTR)url, NULL);
			CloseLibrary(OpenURLBase);	
			}
	}
}

static void framebuffer_window_reformat(struct gui_window *gw)
{
	/** @todo if we ever do zooming reformat should be implemented */
	LOG(("window:%p", gw));

	/*
	  browser_window_reformat(gw->bw, false, width, height);
	*/

}

static struct gui_window_table framebuffer_window_table = {
	.create = gui_window_create,
	.destroy = gui_window_destroy,
	.redraw = gui_window_redraw_window,
	.update = gui_window_update_box,
	.get_scroll = gui_window_get_scroll,
	.set_scroll = gui_window_set_scroll,
	.get_dimensions = gui_window_get_dimensions,
	.update_extent = gui_window_update_extent,
	.reformat = framebuffer_window_reformat,

	.set_title = gui_window_set_title,
	.set_url = gui_window_set_url,
	.set_status = gui_window_set_status,
	.set_pointer = gui_window_set_pointer,
	.place_caret = gui_window_place_caret,
	.remove_caret = gui_window_remove_caret,
	.start_throbber = gui_window_start_throbber,
	.stop_throbber = gui_window_stop_throbber,
};

static struct gui_browser_table framebuffer_browser_table = {
	.poll = framebuffer_poll,
	.schedule = framebuffer_schedule,
	
	.quit = gui_quit,
};

nserror
gui_download_window_data(struct gui_download_window *dw,
			 const char *data,
			 unsigned int size)
{
	return NSERROR_OK;
}

void
gui_download_window_error(struct gui_download_window *dw,
			  const char *error_msg)
{
}

void
gui_download_window_done(struct gui_download_window *dw)
{
}

static struct gui_download_table download_table = {
	.create = gui_download_window_create,
	.data = gui_download_window_data,
	.error = gui_download_window_error,
	.done = gui_download_window_done,
};

struct gui_download_table *amiga_download_table = &download_table;

/** Entry point from OS.
 *
 * /param argc The number of arguments in the string vector.
 * /param argv The argument string vector.
 * /return The return code to the OS
 */
nsurl *html_default_stylesheet_url;
int
main(int argc, char** argv)
{
	struct browser_window *bw;
	nsurl *url;
	nserror ret;
	nsfb_t *nsfb;
	STRPTR current_user_cache = NULL;

	struct greyhound_table framebuffer_table = {
		.browser = &framebuffer_browser_table,
		.window = &framebuffer_window_table,
		.clipboard = framebuffer_clipboard_table,
		.fetch = framebuffer_fetch_table,
		.download = amiga_download_table,
		.utf8 = amiga_utf8_table,
		.llcache = filesystem_llcache_table,
	};


	Execute("assign >nil: greyhound: /greyhound",0,0);
	Execute("execute greyhound:Assigns >ram:ns.log",0,0);
	
	respaths = calloc(128, sizeof(char *));	
	*respaths = strdup("/greyhound/resources");
	
	const char *current_user_dir = "greyhound:Resources";
	snprintf(current_user_cache,sizeof(current_user_cache),"%s/Cache",current_user_dir);

	/* initialise logging. Not fatal if it fails but not much we
	 * can do about it either.
	 */
	nslog_init(nslog_stream_configure, &argc, argv);
#ifdef DT	
	DataTypesBase = OpenLibrary("datatypes.library", 43);
	PNGAlphaBase = OpenLibrary("pngalpha.library", 2);	
	P96Base = OpenLibrary("Picasso96API.library", 1);
#endif
#ifndef RTG	
	KeymapBase = OpenLibrary("keymap.library", 37);
	CxBase = OpenLibrary("commodities.library", 37);
#endif

	/* user options setup */
	ret = nsoption_init(set_defaults, &nsoptions, &nsoptions_default);
	if (ret != NSERROR_OK) {
		die("Options failed to initialise");
	}
	nsoption_commandline(&argc, argv, nsoptions);
	options = strdup("PROGDIR:Resources/Options");
	nsoption_read(options, nsoptions);
	
    ret = greyhound_register(&framebuffer_table);
	if (ret != NSERROR_OK) {
		die("Greyhound operation table failed registration");
	}
	/* cache initialisation */	
	char path[40];
	char *cachepath = NULL;
	cachepath = strdup("PROGDIR:Resources/Cache");

	/* common initialisation */
	ret = greyhound_init(path, cachepath);
	free(cachepath);
	if (ret != NSERROR_OK) {
		die("Greyhound failed to initialise");
	}

	/* Override, since we have no support for non-core SELECT menu */
	nsoption_set_bool(core_select_menu, true);

	if (process_cmdline(argc,argv) != true)
		die("unable to process command line.\n");

	nsfb = framebuffer_initialise(fename, fewidth, feheight, febpp);
	if (nsfb == NULL)
		die("Unable to initialise framebuffer");

	framebuffer_set_cursor(&null_image);

	if (fb_font_init() == false)
		die("Unable to initialise the font system");
			
	fbtk = fbtk_init(nsfb);

	urldb_load_cookies(nsoption_charp(cookie_file));
	
	/* create an initial browser window */

	LOG(("calling browser_window_create"));

	ret = nsurl_create(feurl, &url);
	if (ret == NSERROR_OK) {
		ret = browser_window_create(BW_CREATE_HISTORY,
					      url,
					      NULL,
					      NULL,
					      &bw);
		nsurl_unref(url);
	}
	if (ret != NSERROR_OK) {
		warn_user(messages_get_errorcode(ret), 0);
	} else {
		greyhound_main_loop();
		browser_window_destroy(bw);
	}
	
	if(current_user_cache != NULL) free(current_user_cache);
	
	free(respaths);	
	greyhound_exit();

	if (fb_font_finalise() == false)
		LOG(("Font finalisation failed."));

	/* finalise options */
	nsoption_finalise(nsoptions, nsoptions_default);

	return 0;
}

void gui_resize(fbtk_widget_t *root, int width, int height)
{
	struct gui_window *gw;
	nsfb_t *nsfb = fbtk_get_nsfb(root);

	/* Enforce a minimum */
	if (width < 300)
		width = 300;
	if (height < 200)
		height = 200;

	if (framebuffer_resize(nsfb, width, height, febpp) == false) {
		return;
	}

	fbtk_set_pos_and_size(root, 0, 0, width, height);

	fewidth = width;
	feheight = height;

	for (gw = window_list; gw != NULL; gw = gw->next) {
		resize_normal_browser_window(gw,
				nsoption_int(fb_furniture_size));
	}

	fbtk_request_redraw(root);
}

/*
 * Local Variables:
 * c-basic-offset:8
 * End:
 */
