#ifndef __PICFRAME_FRAMEWORK__
#define __PICFRAME_FRAMEWORK__

#define WIDTH 320
#define HEIGHT 240

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>
#include "datatypes.h"

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#define ELEMENT_TYPE_NONE 1
#define ELEMENT_TYPE_BUTTON 2

/* SDL specific stuff */
SDL_Surface *_screen;
TTF_Font *_font;

/* Picture frame GUI stuff */
struct LList_t *_windows;
int _num_windows;

/* GPIO event handling */
typedef struct {
	int val;
	int pval;
} button_t;

#define NUM_GPIO_KEYS 3
unsigned int _gpio_fds[NUM_GPIO_KEYS];
button_t _buttons[NUM_GPIO_KEYS];
struct pollfd _fd_set[NUM_GPIO_KEYS];
int _max_fds;

typedef struct {
	SDL_Surface *surface;
	SDL_Surface *surface_selected;
	SDL_Rect rect;
	int selected;
	int dynamic;
} Element_t;


int picframe_init();
int picframe_gpio_init();
int picframe_gpio_cleanup();
int picframe_gpio_handle_events();
struct LList_t *picframe_add_window();
struct LList_t *picframe_get_window();
struct LList_t *picframe_add_element_to_window(struct LList_t *window, Element_t *data);
void picframe_clear_screen();
int picframe_get_event(SDL_Event *event);
int picframe_get_lightsensor();
int picframe_get_backlight();
int picframe_set_backlight(int i);
void picframe_draw_line(SDL_Rect r);
int picframe_load_font(char *path, int size);
int picframe_gen_text(SDL_Surface **target, SDL_Color fgcolor, SDL_Color bgcolor, char *text);
int picframe_load_image(SDL_Surface **target, char *path);
int picframe_disp_surface(SDL_Surface *src, SDL_Rect *rect);
int picframe_add_button(Element_t *b, SDL_Rect *rect, char *path, char *selected);
int picframe_add_button_text(Element_t *b, SDL_Rect *rect, int textsize, char *text);
int picframe_add_progress_bar(Element_t *b, SDL_Rect *rect, int val);
int picframe_update_progress_bar(Element_t *b, int val);
int picframe_update();
void picframe_cleanup();

#endif
