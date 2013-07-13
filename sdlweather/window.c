/*
 * window.c
 *
 *  Created on: Jul 11, 2013
 *      Author: ufuchs
 */

#include "window.h"
#include <assert.h>

//
//
//
void window_free(Window* w) {
	w->free(w->params);
	free(w->params);
	free(w);
}

//
//
//
int window_events(Window* w, SDL_Event *event) {
	return w->event(event);
}

//
//
//
void window_update(Window* w) {

	struct LList_t *curr = w->params->elements;
	SDL_Surface *screen = w->params->screen;
	Element_t *element;
	SDL_Surface *ts;

	while (curr) {

		element = (Element_t *)curr->data;

		ts = element->selected ? element->surface_selected : element->surface;

		assert(ts);

		SDL_BlitSurface(ts, NULL, screen, &element->rect);

		if (element->dynamic) {
			SDL_FreeSurface(element->surface);
			SDL_FreeSurface(element->surface_selected);
		}

		curr = curr->next;
	}

	SDL_Flip(screen);
}

//
//
//
struct LList_t * window_add_element(Window *w, Element_t *e) {

	struct LList_t *newNode = (struct LList_t *) calloc(1, sizeof(struct LList_t));
	struct LList_t **x = &w->params->elements;
	struct LList_t *curr = *x;

	// an empty element
	assert(e);

	newNode->data = e;

	if (curr == NULL) {

		newNode->id = 1;

		*x = newNode;

	} else {

		int i = 1;

		while (curr->next != NULL) {
			curr = curr->next;
			i += 1;
		}

		newNode->id = i + 1;

		curr->next = newNode;

	}

	return newNode;

}

//
//
//
Window* windowFactory(SDL_Surface *screen, eventHandler event) {

	Window *w =
			(Window *) calloc(1, sizeof(Window));

	printf("%lu", sizeof(Window));

	w->params = (Params *) calloc(1, sizeof(Params));
	w->setup = setup;
	w->event = event;
	w->free = free_local;
	w->setup(screen, w->params);

	return w;

};
