/*
 * gpio.c
 *
 *  Created on: Oct 1, 2013
 *      Author: ufuchs
 */

#include "gpio.h"
//#include <SDL_events.h>
#include <fcntl.h>
#include "common.h"

static const int BTN_LEFT = 162;
static const int BTN_MIDDLE = 163;
static const int BTN_RIGHT = 164;

enum gpio_dir { in, out } gpio_dirs;


static const char GPIO_EDGE_NONE     = "none";
static const char GPIO_EDGE_FALLING  = "falling";
static const char GPIO_EDGE_RISING   = "rising";
static const char GPIO_EDGE_BOTH     = "both";

static const char GPIO_DIRECTION_IN  = "in";
static const char GPIO_DIRECTION_OUT = "out";

/**
 *
 */
static int gpio_export(int ex, int pin) {

	char buff[50];
	FILE *fp;

	if (ex) {
		fp = fopen("/sys/class/gpio/export", "ab");
	} else {
		fp = fopen("/sys/class/gpio/unexport", "ab");
	}

	if (!fp) {
		return -1;
	}

	sprintf(buff, "%d", pin);

	rewind(fp);

	fwrite(&buff, sizeof(char), strlen(buff), fp);

	fclose(fp);

	return 0;

}

// enum edge { none=0, falling, rising, both } edges;

int gpio_setcfg(int gpio, int dir, int edge) {

	char buff[50];
	int fp;

	//

	sprintf(buff, "/sys/class/gpio/gpio%d/edge", gpio);
	fp = open(buff, O_WRONLY);

	if (!fp) {
		debug_printf("failed to open %s\n", buff);
		return -1;
	}

	switch (edge) {
	case 0:
		write(fp, GPIO_EDGE_NONE, strlen(GPIO_EDGE_NONE));
		break;
	case 1:
		write(fp, GPIO_EDGE_FALLING, strlen(GPIO_EDGE_FALLING));
		break;
	case 2:
		write(fp, GPIO_EDGE_RISING, strlen(GPIO_EDGE_RISING));
		break;
	case 3:
		write(fp, GPIO_EDGE_BOTH, strlen(GPIO_EDGE_BOTH));
		break;
	}

	/*
	if (edge == 0)
		write(fp, "none", 4);
	else if (edge == 1)
		write(fp, "falling", 7);
	else if (edge == 2)
		write(fp, "rising", 6);
	else if (edge == 3)
		write(fp, "both", 4);
	*/
	close(fp);

	//

	sprintf(buff, "/sys/class/gpio/gpio%d/direction", gpio);
	fp = open(buff, O_WRONLY);

	if (!fp) {
		debug_printf("failed to open %s\n", buff);
		return -1;
	}

	switch (dir) {
	case 0 : write(fp, GPIO_DIRECTION_OUT, strlen(GPIO_DIRECTION_OUT)); break;
	case 1 : write(fp, GPIO_DIRECTION_IN, strlen(GPIO_DIRECTION_IN)); break;
	}

	//

	/*
	if (dir)
		write(fp, "out", 3);
	else
		write(fp, "in", 2);
	 */
	//

	close(fp);

	//

	return 0;
}

/* These are pretty DF3120 specific! */
void gpio_init() {

	gpio_export(1, 162);
	gpio_export(1, 163);
	gpio_export(1, 164);

	/* Settings: direction=in, edge=falling */
	gpio_setcfg(162, 0, 1);
	gpio_setcfg(163, 0, 1);
	gpio_setcfg(164, 0, 1);

	/* BUG: somehow the edge detect doesnt work first */
	system("echo falling > /sys/class/gpio/gpio162/edge");
	system("echo both > /sys/class/gpio/gpio162/edge");		// wieso beides?

	system("echo falling > /sys/class/gpio/gpio163/edge");
	system("echo both > /sys/class/gpio/gpio163/edge");

	system("echo falling > /sys/class/gpio/gpio164/edge");
	system("echo both > /sys/class/gpio/gpio164/edge");

	// TODO: Failure handling here
	gpio_fd[0] = open("/sys/class/gpio/gpio162/value", O_RDONLY | O_NONBLOCK);
	gpio_fd[1] = open("/sys/class/gpio/gpio163/value", O_RDONLY | O_NONBLOCK);
	gpio_fd[2] = open("/sys/class/gpio/gpio164/value", O_RDONLY | O_NONBLOCK);

	// TODO: currently blind handling of buttons, default is active
    gpio_keyValue[0] = 1;
    gpio_keyValue[1] = 1;
    gpio_keyValue[2] = 1;

}

void gpio_cleanup() {

	close(gpio_fd[0]);
	close(gpio_fd[1]);
	close(gpio_fd[2]);

	gpio_export(0, 162);
	gpio_export(0, 163);
	gpio_export(0, 164);
}

int gpio_handle_events(SDL_Event *event) {
	char buff[10];
	int rs, i;

	// clear
	memset((void*)gpio_pollfd, 0, sizeof(gpio_pollfd));

	gpio_pollfd[0].fd = gpio_fd[0];
	gpio_pollfd[0].events = POLLPRI;

	gpio_pollfd[1].fd = gpio_fd[1];
	gpio_pollfd[1].events = POLLPRI;

	gpio_pollfd[2].fd = gpio_fd[2];
	gpio_pollfd[2].events = POLLPRI;

	// !clear

	rs = poll(gpio_pollfd, NUM_GPIO_KEYS, 50);

	if (rs < 0) {
//		debug_printf("Error in poll(): %d,%d\n", rs, errno);
		return -1;
	} else if (rs == 0) {
	} else {
		rs = 1;
		for(i=0;i<NUM_GPIO_KEYS;i++) {

			if (gpio_pollfd[i].revents & POLLPRI) {

				(void)read(gpio_fd[i], buff, 1);

				debug_printf("Button %d value change: %d\n", i, buff[0]);

				if (gpio_keyValue[i]) {
					event->type = SDL_KEYUP;
					gpio_keyValue[i] = 0;
				} else {
					event->type = SDL_KEYDOWN;
					gpio_keyValue[i] = 1;
				}

				if (i==0)
					event->key.keysym.sym = SDLK_RIGHT;
				else if (i==1)
					event->key.keysym.sym = SDLK_LEFT;
				else if (i==2)
					event->key.keysym.sym = SDLK_RETURN;
				return rs;
			}
		}
	}
	return rs;
}

