/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "sedml/xhtml.h"

static struct xhtml_attribute {
	const char *name;
} xhtml_attributes[] = {
	/* Ordered in alphabetical order, otherwise bsearch() will not work */
	{"id"}
	/* TODO */
};

#define num_xhtml_attributes \
	(sizeof(xhtml_attributes)/sizeof(xhtml_attributes[0]))

static int cmp_attribute_name(const void *x, const void *y)
{
	const struct xhtml_attribute *a1, *a2;
	a1 = (const struct xhtml_attribute *)x;
	a2 = (const struct xhtml_attribute *)y;
	return strcmp(a1->name, a2->name);
}

static struct xhtml_element {
	const char *name;
	enum sedml_xhtml_type type;
} xhtml_elements[] = {
	{"p", SEDML_XHTML_TEXT}
	/* TODO */
};

#define num_xhtml_elements (sizeof(xhtml_elements)/sizeof(xhtml_elements[0]))

static int cmp_element_name(const void *x, const void *y)
{
	const struct xhtml_element *e1, *e2;
	e1 = (const struct xhtml_element *)x;
	e2 = (const struct xhtml_element *)y;
	return strcmp(e1->name, e2->name);
}

static void destroy_attribute(struct sedml_xhtml_attribute *a)
{
	if (!a) return;
	free(a->value);
	free(a);
}

static void destroy_element(struct sedml_xhtml_element *e)
{
	if (!e) return;
	if (e->type == SEDML_XHTML_TEXT) {
		struct sedml_xhtml_text *text;

		text = (struct sedml_xhtml_text *)e;
		free(text->body);
	} else {
		struct sedml_xhtml_node *node;
		int i;

		node = (struct sedml_xhtml_node *)e;
		for (i = 0; i < node->num_attributes; i++) {
			destroy_attribute(node->attributes[i]);
		}
		free(node->attributes);
		for (i = 0; i < node->num_children; i++) {
			destroy_element(node->children[i]);
		}
		free(node->children);
	}
	free(e);
}

/* API */

struct sedml_xhtml_element *sedml_create_xhtml_element(const char *name)
{
	struct sedml_xhtml_element *e = NULL;
	struct xhtml_element xe, *found;

	if (!name) goto out;
	qsort(xhtml_elements, num_xhtml_elements, sizeof(xhtml_elements[0]),
		  cmp_element_name);
	xe.name = name;
	found = bsearch(&xe, xhtml_elements, num_xhtml_elements,
					sizeof(xhtml_elements[0]), cmp_element_name);
	if (!found) goto out;
	if (found->type == SEDML_XHTML_TEXT) {
		struct sedml_xhtml_text *text;

		text = calloc(1, sizeof(*text));
		e = (struct sedml_xhtml_element *)text;
	} else {
		struct sedml_xhtml_node *node;

		node = calloc(1, sizeof(*node));
		e = (struct sedml_xhtml_element *)node;
	}
	e->type = found->type;

 out:
	return e;
}

int sedml_xhtml_node_add_attribute(struct sedml_xhtml_node *node,
								   const char *name, const char *value)
{
	struct sedml_xhtml_attribute *a;
	struct xhtml_attribute xa, *found;
	int r = -1, i;
	size_t s;

	if (!node || !name || !value) goto out;
	xa.name = name;
	found = bsearch(&xa, xhtml_attributes, num_xhtml_attributes,
					sizeof(xhtml_attributes[0]), cmp_attribute_name);
	if (!found) goto out;
	i = node->num_attributes++;
	s = node->num_attributes * sizeof(a);
	node->attributes = realloc(node->attributes, s);
	if (!node->attributes) goto out;
	a = malloc(sizeof(*a));
	a->name = found->name; /* interned */
	s = strlen(value);
	a->value = malloc(s + 1);
	if (!a->value) {
		free(a);
		goto out;
	}
	strcpy(a->value, value);
	node->attributes[i] = a;
	r = 0;
 out:
	return r;
}

struct sedml_xhtml *sedml_create_xhtml(void)
{
	struct sedml_xhtml *x;

	x = calloc(1, sizeof(*x));
	return x;
}

int sedml_xhtml_add_element(struct sedml_xhtml *xhtml,
							struct sedml_xhtml_element *e)
{
	int r = -1, i;
	size_t s;

	if (!xhtml || !e) goto out;
	i = xhtml->num_elements++;
	s = xhtml->num_elements * sizeof(e);
	xhtml->elements = realloc(xhtml->elements, s);
	if (!xhtml->elements) goto out;
	xhtml->elements[i] = e;
	r = 0;
 out:
	return r;
}

void sedml_destroy_xhtml(struct sedml_xhtml *xhtml)
{
	int i;

	if (!xhtml) return;
	for (i = 0; i < xhtml->num_elements; i++) {
		destroy_element(xhtml->elements[i]);
	}
	free(xhtml);
}
