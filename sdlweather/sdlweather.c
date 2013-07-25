#include "picframe.h"
#include <time.h>
#include  "window.h"
#include <assert.h>

#define START_WINDOW 1
#define NUM_WINDOWS  5

/* General elements for all windows */
Element_t * leftArrow;
Element_t * rightArrow;

Window *window[NUM_WINDOWS];

char *weatherFileTemplate = "/tmp/weather-%d.png";

/*
 * Global vars
 */
int curr_window_idx = START_WINDOW;
int prev_window_idx = START_WINDOW;

SDL_Color fg = { 0, 0, 0, 0 };
SDL_Color bg = { 255, 255, 255, 0 };

/*
 * Generic Input handling
 */
int handle_input() {
	int i = 0;
	SDL_Event event;

	while (picframe_get_event(&event)) {

		switch (event.type) {
		case SDL_QUIT:
			printf("Quitting\n");
			return 1;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_LEFT) {
				leftArrow->selected = 1;
				curr_window_idx--;
				if (curr_window_idx < 0)
					curr_window_idx = 0;
				printf("Current window: %d\n", curr_window_idx);

			}

			if (event.key.keysym.sym == SDLK_RIGHT) {
				rightArrow->selected = 1;
				curr_window_idx++;
				if (curr_window_idx > NUM_WINDOWS - 1)
					curr_window_idx = NUM_WINDOWS - 1;

				printf("Current window: %d\n", curr_window_idx);
			}

			if (event.key.keysym.sym == SDLK_RETURN) {
				i = picframe_get_lightsensor();
				printf("Light sensor: %d\n", i);
			}

			break;

		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_LEFT) {
				leftArrow->selected = 0;
			}

			if (event.key.keysym.sym == SDLK_RIGHT) {
				rightArrow->selected = 0;
			}

			break;
		default:
			break;
		}
		if (prev_window_idx != curr_window_idx) {
			prev_window_idx = curr_window_idx;
			return 2;
		}
		prev_window_idx = curr_window_idx;
	}
	return 0;
}

/*
 *
 */
Element_t * app_create_arrow(char *path, char *pathSelected, int x, int y) {

	Element_t *arrow = (Element_t *) calloc(1, sizeof(Element_t));

	arrow->surface = IMG_Load(path);
	assert(arrow->surface);

	arrow->surface_selected = IMG_Load(pathSelected);
	assert(arrow->surface_selected);

	arrow->rect.x = x;
	arrow->rect.y = y;
	arrow->selected = 0;
	arrow->dynamic = 0;

	return arrow;

}

/*
 *
 */
void app_free() {

	picframe_cleanup();

	for (int i = 0; i < NUM_WINDOWS; i++) {
		window_free(window[i]);
	}

	SDL_FreeSurface(leftArrow->surface);
	SDL_FreeSurface(leftArrow->surface_selected);

	SDL_FreeSurface(rightArrow->surface);
	SDL_FreeSurface(rightArrow->surface_selected);

}

/*
 *  Execution of the clock window
 */
int clock_loop() {
	time_t now, prev = 0;
	struct tm* tm;
	char buff[100];
	int ret;
	int clockDeli = 1;

	Window *w = window[curr_window_idx];

	while (1) {

		if ((ret = handle_input()))
			return ret;

		now = time(0);

		if (now > prev) {

			tm = localtime(&now);
			prev = now;

			clockDeli = clockDeli == 1 ? 0 : 1;

			if (clockDeli) {
				sprintf(buff, "%02d:%02d", tm->tm_hour, tm->tm_min);
			} else {
				sprintf(buff, "%02d %02d", tm->tm_hour, tm->tm_min);
			}

			picframe_load_font("/usr/share/fonts/UbuntuMono-R.ttf", 115);
			Element_t *time_disp = window_get_element_byName(w, "time");
			time_disp->surface = TTF_RenderText_Shaded(_font, buff, fg, bg);

			strftime(buff, sizeof(buff), "%a, %d %b %Y %Z", tm);

			picframe_load_font("/usr/share/fonts/Ubuntu-L.ttf", 29);
			Element_t *info_disp = window_get_element_byName(w, "info");
			info_disp->surface = TTF_RenderText_Shaded(_font, buff, fg, bg);

			window_update(window[curr_window_idx]);

		}

		SDL_Delay(1);
	}
	return 0;
}

/*
 * Setup routine for the clock window
 */
void clock_setup() {

	int num = 0;

	window[num] = windowFactory(_screen, clock_loop);

	window_add_element(window[num], leftArrow);
	window_add_element(window[num], rightArrow);

	/* Dynamic info label */
	Element_t *info_disp = (Element_t *) calloc(1, sizeof(Element_t));
	info_disp->name = "info";
	info_disp->surface = NULL;
	info_disp->rect.x = 20;
	info_disp->rect.y = 50;
	info_disp->selected = 0;
	info_disp->dynamic = 1;
	window_add_element(window[num], info_disp);

	/* Dynamic watch label */
	Element_t *time_disp = (Element_t *) calloc(1, sizeof(Element_t));
	time_disp->name = "time";
	time_disp->surface = NULL;
	time_disp->rect.x = 15;
	time_disp->rect.y = 100;
	time_disp->selected = 0;
	time_disp->dynamic = 1; /* Free up the surface after use, otherwise memory leak! */
	window_add_element(window[num], time_disp);

}

//
//
//
int app_loop() {
	int ret;

	while (1) {

		if ((ret = handle_input()) != 0) {
			break;
		}

		SDL_Delay(10);

	}

	return ret;
}

//
// weather_today_loop
//
int weather_loop() {

	char weather[200];

	Window *w = window[curr_window_idx];

	Element_t *e = window_get_element_byName(w, "weather");

	/* format weather file name */
	sprintf(weather, weatherFileTemplate, curr_window_idx - 1);

	e->surface = IMG_Load(weather);

	window_update(window[curr_window_idx]);

	SDL_FreeSurface(e->surface);

	// calling `app_loop`
	return app_loop();

}

//
// weather_today_setup
//
void weather_setup(int num) {

	window[num] = windowFactory(_screen, weather_loop);

	Element_t *pic = (Element_t *) calloc(1, sizeof(Element_t));
	pic->name = "weather";

	// window manager
	window_add_element(window[num], pic);
	window_add_element(window[num], leftArrow);
	window_add_element(window[num], rightArrow);

}

//
//
//
void app_create() {

	char *leftPath = "/usr/share/data/glyphicons_210_left_arrow.png";
	char *leftSelected =
			"/usr/share/data/glyphicons_210_left_arrow_selected.png";
	char *rightPath = "/usr/share/data/glyphicons_211_right_arrow.png";
	char *rightSelected =
			"/usr/share/data/glyphicons_211_right_arrow_selected.png";

	picframe_init();

	leftArrow = app_create_arrow(leftPath, leftSelected, 10, 5);
	rightArrow = app_create_arrow(rightPath, rightSelected, 320 - 30, 5);

	clock_setup();

	for (int i = 1; i < 5; i++) {
		weather_setup(i);
	}

}

///////////////////////////////////////////////////////////////////////////////

/*
 * Generic main loop to keep things running...
 */

int main() {

	int ret = 0;

	app_create();

	picframe_set_backlight(500);

	while (1) {

		picframe_clear_screen();

		if ((ret = window[curr_window_idx]->wndProc()) == 1)
			break;

	}

	app_free();

	return 0;
}
