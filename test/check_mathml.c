/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "sedml/mathml.h"

#define FAILURE(n) longjmp(jb, n)

static jmp_buf jb;
static struct sedml_mathml_element *e;

static void test_create_mathml_element(void)
{
	e = sedml_create_mathml_element("plus");
	if (!e || e->type != SEDML_MATHML_PLUS) {
		FAILURE(1);
	}
}

static void test_element_name(void)
{
	const char *name;

	name = sedml_mathml_element_name(e);
	if (!name || strcmp(name, "plus") != 0) {
		FAILURE(2);
	}
}

static char *get_body(const char *x)
{
	char *body;

	body = malloc(strlen(x) + 1);
	if (!body) FAILURE(3);
	strcpy(body, x);
	return body;
}

static void test_node_add_child(void)
{
	struct sedml_mathml_element *e0, *e1, *e2, *e3;
	struct sedml_mathml_node *node;

	e0 = sedml_create_mathml_element("times");
	if (!e0) FAILURE(3);

	e1 = sedml_create_mathml_element("ci");
	if (!e1) FAILURE(3);
	((struct sedml_mathml_token *)e1)->body = get_body("x");

	e2 = sedml_create_mathml_element("cn");
	if (!e2) FAILURE(3);
	((struct sedml_mathml_token *)e2)->body = get_body("1");

	node = (struct sedml_mathml_node *)e0;
	sedml_mathml_node_add_child(node, e1);
	sedml_mathml_node_add_child(node, e2);
	if (node->num_children != 2) {
		FAILURE(3);
	}

	e3 = sedml_create_mathml_element("ci");
	if (!e3) FAILURE(3);
	((struct sedml_mathml_token *)e3)->body = get_body("y");

	node = (struct sedml_mathml_node *)e;
	sedml_mathml_node_add_child(node, e0);
	sedml_mathml_node_add_child(node, e3);
	if (node->num_children != 2) {
		FAILURE(3);
	}
}

static void test_destroy_mathml_element(void)
{
	sedml_destroy_mathml_element(e);
}

int main(void)
{
	int r;

	if ( (r = setjmp(jb)) != 0) return r;
	test_create_mathml_element();
	test_element_name();
	test_node_add_child();
	test_destroy_mathml_element();
	return 0;
}
