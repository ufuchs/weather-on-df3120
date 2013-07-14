#include "picframe.h"
#include <time.h>
#include  "window.h"

#define START_WINDOW 0
#define NUM_WINDOWS  4

/* General elements for all windows */
Element_t leftArrow;
Element_t rightArrow;
Element_t window_label;

/* Clock window elements */
Element_t time_disp, info_disp;

//char *window_labels[NUM_WINDOWS] = { "Date and Time", "Media Center", "Wther", "Lights", "Security Cam" };

Window *window[NUM_WINDOWS];

char *weatherFileTemplate = "/exports/df3120-rootfs/tmp/weather-%d.png";

/*
 * Global vars
 */
int curr_window_idx = START_WINDOW, prev_window_idx = START_WINDOW;

SDL_Color fg = { 0, 0, 0, 0 };
SDL_Color bg = { 255, 255, 255, 0 };
int progress = 0;

/*
 * Generic Input handling
 */
int handle_input() {
	int i = 0;
	SDL_Event event;

//	while (picframe_get_event(&event)) {
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			printf("Quitting\n");
			return 1;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_LEFT) {
				leftArrow.selected = 1;
				curr_window_idx--;
				if (curr_window_idx < 0)
					curr_window_idx = 0;
				printf("Current window: %d\n", curr_window_idx);

			}

			if (event.key.keysym.sym == SDLK_RIGHT) {
				rightArrow.selected = 1;
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
				leftArrow.selected = 0;
			}

			if (event.key.keysym.sym == SDLK_RIGHT) {
				rightArrow.selected = 0;
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
void arrow_setup() {

	char *leftPath = "/usr/share/data/glyphicons_210_left_arrow.png";
	char *leftSelected =
			"/usr/share/data/glyphicons_210_left_arrow_selected.png";
	char *rightPath = "/usr/share/data/glyphicons_211_right_arrow.png";
	char *rightSelected =
			"/usr/share/data/glyphicons_211_right_arrow_selected.png";

	/* Left arrow */
	leftArrow.surface = IMG_Load(leftPath);
	leftArrow.surface_selected = IMG_Load(leftSelected);
	leftArrow.rect.x = 10;
	leftArrow.rect.y = 5;
	leftArrow.selected = 0;
	leftArrow.dynamic = 0;

	/* Right arrow */
	rightArrow.surface = IMG_Load(rightPath);
	rightArrow.surface_selected = IMG_Load(rightSelected);
	rightArrow.rect.x = 320 - 30;
	rightArrow.rect.y = 5;
	rightArrow.selected = 0;
	rightArrow.dynamic = 0;

}

/*
 *
 */
void app_free() {

	window_free(window[0]);
	window_free(window[1]);
	window_free(window[2]);
	window_free(window[3]);

	SDL_FreeSurface(leftArrow.surface);
	SDL_FreeSurface(leftArrow.surface_selected);

	SDL_FreeSurface(rightArrow.surface);
	SDL_FreeSurface(rightArrow.surface_selected);
}

/*
 * Setup routine for the clock window
 */
void clock_setup() {
	struct LList_t *first_window = NULL;

	picframe_add_element_to_window(first_window, &leftArrow);

	picframe_add_element_to_window(first_window, &rightArrow);

	/////// Arrows are HERE //////

	window_label.surface = NULL;
	window_label.rect.x = 110;
	window_label.rect.y = 5;
	window_label.selected = 0;
	window_label.dynamic = 0;
	picframe_add_element_to_window(first_window, &window_label);

	picframe_load_font("/usr/share/fonts/Ubuntu-L.ttf", 80);

	/* Dynamic watch label */
	time_disp.surface = NULL; //sometext;
	time_disp.rect.x = 5;
	time_disp.rect.y = 50;
	time_disp.selected = 0;
	time_disp.dynamic = 1; /* Free up the surface after use, otherwise memory leak! */
	picframe_add_element_to_window(first_window, &time_disp);

	/* Dynamic info label */
	info_disp.surface = NULL;
	info_disp.rect.x = 10;
	info_disp.rect.y = 200;
	info_disp.selected = 0;
	info_disp.dynamic = 1;
	picframe_add_element_to_window(first_window, &info_disp);
}

/*
 *  Execution of the clock window
 */
int clock_loop() {
	time_t now, prev = 0;
	struct tm* tm;
	char buff[50];
	int ret, lightlevel, brightness;

	SDL_FreeSurface(window_label.surface);
	picframe_load_font("/usr/share/fonts/Ubuntu-L.ttf", 30);

//	picframe_gen_text(&window_label.surface, fg, bg, window_labels[0]);
	window_label.rect.x = (WIDTH / 2) - (window_label.surface->clip_rect.w / 2);

	while (1) {
		ret = handle_input();
		if (ret)
			return ret;

		now = time(0);
		if (now > prev) {
			tm = localtime(&now);
			sprintf(buff, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min,
					tm->tm_sec);
			picframe_load_font("/usr/share/fonts/Ubuntu-L.ttf", 80);

			picframe_gen_text(&time_disp.surface, fg, bg, buff);

			// FREE_FONT
			/*
			 debug_printf("Setting time_disp surface to: %p\n",
			 time_disp.surface);
			 */
			prev = now;

			lightlevel = picframe_get_lightsensor();
			brightness = picframe_get_backlight();

			sprintf(buff, "Light level: %d\tBrightness: %d", lightlevel,
					brightness);
			picframe_load_font("/usr/share/fonts/Ubuntu-L.ttf", 20);
			picframe_gen_text(&info_disp.surface, fg, bg, buff);
			/*
			 debug_printf("Setting info_disp surface to: %p\n",
			 info_disp.surface);
			 */
		}


		SDL_Delay(1);
	}
	return 0;
}

//
//
//
int wndProc() {
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

	Element_t *e = window_get_element_byName(window[curr_window_idx], "weather");

	sprintf(weather, weatherFileTemplate, curr_window_idx);

	e->surface = IMG_Load(weather);

	window_update(window[curr_window_idx]);

	SDL_FreeSurface(e->surface);

	return wndProc();

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
	window_add_element(window[num], &leftArrow);
	window_add_element(window[num], &rightArrow);

}

///////////////////////////////////////////////////////////////////////////////

/*
 * Generic main loop to keep things running...
 */

int main() {
	int ret = 0;

	picframe_init();

	arrow_setup();
//	clock_setup();

	weather_setup(0);
	weather_setup(1);
	weather_setup(2);
	weather_setup(3);

	picframe_set_backlight(500);

	while (1) {

		picframe_clear_screen();

		if ((ret = window[curr_window_idx]->wndProc()) == 1)
			break;

	}

	// Try a friendly shotdown to prevent memory leaks...
	picframe_cleanup();
	app_free();

	return 0;
}
