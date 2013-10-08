/*
 * gpio.h
 *
 *  Created on: Oct 1, 2013
 *      Author: ufuchs
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <poll.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#define NUM_GPIO_KEYS 3

unsigned int gpio_fd[NUM_GPIO_KEYS];

int gpio_keyValue[NUM_GPIO_KEYS];

struct pollfd gpio_pollfd[NUM_GPIO_KEYS];

void gpio_init();
void gpio_cleanup();
int gpio_handle_events(SDL_Event *event);

#endif /* GPIO_H_ */
