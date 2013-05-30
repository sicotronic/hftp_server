#include "list_s.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mem_fun.h"

struct s_node {
	string_t elem;
	node  next;
};

struct s_list_s {
	node  fst;
	node  lst;
	node  mark;
	int size;
};

static void list_mark_reset (list_s l);
static void list_next (list_s l);

list_s list_empty (void){

	list_s l = NULL;
	l = (list_s) req_mem (1, sizeof(struct s_list_s), MAXTRIES);
	if (l != NULL) {
	        l->fst = NULL;
		l->lst = NULL;
		list_mark_reset (l);
		l->size = 0;
	}
	 /* If there is not enough memory returns NULL */
	return l;
}

void list_destroy (list_s l) {
	string_t elem = NULL;
	assert (l != NULL);
	
	while (!list_isempty (l)) {
		elem = list_disjoin_first (l);
		free (elem);
		elem = NULL;
	}
	free (l);
	l = NULL;
}

void list_adjoin_front (list_s l, string_t incoming) {
	node new = NULL, aux = NULL;
	assert (incoming != NULL && l != NULL);
	
	new = (node) req_mem (1, sizeof(struct s_node), MAXTRIES);
	if (new != NULL){
		new->elem = incoming;
		if (list_isempty (l)) {
			l->fst = new;
			l->lst = new;
			l->fst->next = NULL;
		}
		else {
			aux = l->fst;
			l->fst = new;
			l->fst->next = aux;
		}
	}
	l->size = l->size+1;
	/* If there is not enough memory the function does nothing */
	assert (l != NULL && l->size > 0);
}

void list_adjoin_back (list_s l, string_t incoming) {
	node new = NULL;
	assert (incoming != NULL && l != NULL);

	new = (node) req_mem (1, sizeof(struct s_node), MAXTRIES);
	if (new != NULL) {
		new->elem = incoming;
		if (list_isempty (l)) {
			l->fst = new;
			l->lst = l->fst;
		}
		else {
			l->lst->next = new;
			l->lst = l->lst->next;
		}
		new->next = NULL;
		l->size = l->size+1;
	}
	/* If there is not enough memory the function does nothing */
	assert (l != NULL && l->size > 0);
}

string_t list_disjoin_first (list_s l) {
	string_t ret = NULL;
	node aux = NULL;
	assert (l != NULL && !list_isempty (l));
	
	aux = l->fst;
	if (list_cardinal(l) == 1) {
		l->fst = NULL;
		l->lst = NULL;
	}
	else
		l->fst = l->fst->next;
	l->size = l->size-1;
	ret = aux->elem;	
	free (aux);
	aux = NULL;
	assert (list_cardinal (l) >= 0);

	return ret;
}

string_t list_disjoin_last (list_s l) {
	string_t ret = NULL;
	node aux = NULL;
	assert (l != NULL && !list_isempty (l));

	aux = l->lst;
	if (list_cardinal (l) == 1) {
		l->fst = NULL;
		l->lst = NULL;
	}
	else {
		int i;
		list_mark_reset (l);
		for (i = 1; i < list_cardinal (l) - 1; i++) {
			list_next (l);
		}
		l->lst = l->mark;
		list_mark_reset (l);
	}
	l->size = l->size - 1;
	ret = aux->elem;
	free (aux);
	aux = NULL;
	assert (list_cardinal (l) >= 0);

	return ret;
}


int list_cardinal (list_s l) {
	assert (l != NULL);
	return (l->size);
}

bool list_isempty (list_s l) {
	assert (l != NULL);
	return (list_cardinal (l) == 0);
}

static void list_next (list_s l) {
	/* Advances the mark in the list (without falling out from the list) */
	assert (l != NULL && !list_isempty (l));
	if (l->mark->next != NULL) {
		l->mark = l->mark->next;
	}
}

static void list_mark_reset (list_s l) {
	/* Goes backwards with the mark to the first element */
	assert (l != NULL);
	l->mark = l->fst;
}

string_t list_elem (list_s l) {
	string_t ret = NULL;
	int size = 0;
	assert (l != NULL && !list_isempty (l));

	size = strlen (l->mark->elem) + 1;
	ret = (char*) req_mem (size, sizeof(char), MAXTRIES);
	if (ret != NULL)
		ret = strncpy (ret, l->mark->elem, strlen (l->mark->elem));
	/* If there is not enough memory the function does nothing and returns NULL */
	return ret;
}

string_t list_head (list_s l) {
	assert (l != NULL && !list_isempty (l));
	return list_index (l, 0);
}

list_s list_tail (list_s l) {
	list_s tail;
	string_t basura = NULL;
	assert (l != NULL);

	tail = list_empty ();
	list_clone (l, tail);
	basura = list_disjoin_first (tail);
	free (basura);
	basura = NULL;

	return tail;
}

void list_clone (list_s l, list_s clon) {
	assert (l != NULL);

	if (!list_isempty (l)) {
		int i;
		string_t s = NULL;
		for (i = list_cardinal(l) - 1; i >= 0; i--) {
			s = list_index (l, i);
			list_adjoin_front (clon, s);
		}
	}
}

void list_concat (list_s l, const list_s p) {
	assert (l != NULL && p != NULL);
	
	if (!list_isempty (p)) {
		string_t elem;
		list_s lista;

		lista = list_empty ();
		list_clone (p, lista);
		while (!list_isempty (lista)) {
			elem = list_disjoin_first (lista);
			list_adjoin_back (l, elem);
		}
		free (lista);
		lista = NULL;
	}
}

string_t list_index (list_s l, int num) {
	string_t ret = NULL;
	int i = 1, size = 0;
	assert (l != NULL && !list_isempty (l));
	
	list_mark_reset (l);
	if (num >= 0 && num < list_cardinal (l)) {
		for (i = 0; i < num; i++) {
			list_next (l);
		}
		if (l->mark != NULL)
			size = strlen (l->mark->elem) + 1;
		ret = (string_t) req_mem (size, sizeof(char), MAXTRIES);
		if (ret != NULL)
			ret = strncpy (ret, l->mark->elem, size);
	}
	list_mark_reset (l);
	/* If there is not enough memory the function does nothing and returns NULL */
	return ret;
}

int list_amount (list_s l, string_t elem) {
	int amount = 0;
	int i = 1;
	int res = 20;
	string_t aux;
	assert (!list_isempty (l) && elem != NULL);

	list_mark_reset (l);
	for (i = 1; i <= list_cardinal (l); i++){
		aux = list_elem (l);
		res = strcmp (elem, aux);
		list_next (l);
		if (res == 0)
			amount++;
		free (aux);
		aux = NULL;
	}
	list_mark_reset (l);
	return amount;
}
