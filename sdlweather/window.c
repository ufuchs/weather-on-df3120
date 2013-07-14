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
Window* windowFactory(SDL_Surface *screen, eventHandler wndProc) {

	Window *w = (Window *) calloc(1, sizeof(Window));

	initialize(w, wndProc);

	/* invoke the `setup` function */
	w->setup(screen, w->params);

	return w;

};

//
//
//
void window_free(Window* w) {
	w->release(w->params);
	free(w);
}

//
//
//
void window_update(Window* w) {

	struct LList_t *curr = w->params->elements;
	Element_t *element;
	SDL_Surface *ts;

	while (curr) {

		element = (Element_t *)curr->data;

		ts = element->selected ? element->surface_selected : element->surface;
		assert(ts);

		SDL_BlitSurface(ts, NULL, w->params->screen, &element->rect);

		if (element->dynamic) {
			if (element->surface) {
				SDL_FreeSurface(element->surface);
			}
			if (element->surface_selected) {
				SDL_FreeSurface(element->surface_selected);
			}
		}

		curr = curr->next;

	}

	SDL_Flip(w->params->screen);

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

	if (!curr) {
		newNode->id = 1;
		*x = newNode;
	} else {
		int i = 1;
		while (curr->next) {
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
Element_t * window_get_element_byName(Window *w, char *name) {

	struct LList_t *curr = w->params->elements;
	Element_t *e;

	while (curr) {

		e = (Element_t *) curr->data;

		if (strcmp(name, e->name) == 0) {
			break;
		}

		curr = curr->next;

	}

	return e;

}


