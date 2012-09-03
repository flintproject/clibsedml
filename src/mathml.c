/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "sedml/common.h"
#include "sedml/mathml.h"

static struct mathml_element {
	const char *name;
	enum sedml_mathml_type type;
} mathml_elements[] = {
	/* token */
	{"cn", SEDML_MATHML_CN},
	{"ci", SEDML_MATHML_CI},
	{"csymbol", SEDML_MATHML_CSYMBOL},
	{"sep", SEDML_MATHML_SEP},
	/* general */
	{"apply", SEDML_MATHML_APPLY},
	{"piecewise", SEDML_MATHML_PIECEWISE},
	{"piece", SEDML_MATHML_PIECE},
	{"otherwise", SEDML_MATHML_OTHERWISE},
	{"lambda", SEDML_MATHML_LAMBDA},
	/* relational operators */
	{"eq", SEDML_MATHML_EQ},
	{"neq", SEDML_MATHML_NEQ},
	{"gt", SEDML_MATHML_GT},
	{"lt", SEDML_MATHML_LT},
	{"geq", SEDML_MATHML_GEQ},
	{"leq", SEDML_MATHML_LEQ},
	/* arithmetic operators */
	{"plus", SEDML_MATHML_PLUS},
	{"minus", SEDML_MATHML_MINUS},
	{"times", SEDML_MATHML_TIMES},
	{"divide", SEDML_MATHML_DIVIDE},
	{"power", SEDML_MATHML_POWER},
	{"root", SEDML_MATHML_ROOT},
	{"abs", SEDML_MATHML_ABS},
	{"exp", SEDML_MATHML_EXP},
	{"ln", SEDML_MATHML_LN},
	{"log", SEDML_MATHML_LOG},
	{"floor", SEDML_MATHML_FLOOR},
	{"ceiling", SEDML_MATHML_CEILING},
	{"factorial", SEDML_MATHML_FACTORIAL},
	/* logical operators */
	{"and", SEDML_MATHML_AND},
	{"or", SEDML_MATHML_OR},
	{"xor", SEDML_MATHML_XOR},
	{"not", SEDML_MATHML_NOT},
	/* quantifiers */
	{"degree", SEDML_MATHML_DEGREE},
	{"bvar", SEDML_MATHML_BVAR},
	{"logbase", SEDML_MATHML_LOGBASE},
	/* trigonometric operators */
	{"sin", SEDML_MATHML_SIN},
	{"cos", SEDML_MATHML_COS},
	{"tan", SEDML_MATHML_TAN},
	{"sec", SEDML_MATHML_SEC},
	{"csc", SEDML_MATHML_CSC},
	{"cot", SEDML_MATHML_COT},
	{"sinh", SEDML_MATHML_SINH},
	{"cosh", SEDML_MATHML_COSH},
	{"tanh", SEDML_MATHML_TANH},
	{"sech", SEDML_MATHML_SECH},
	{"csch", SEDML_MATHML_CSCH},
	{"coth", SEDML_MATHML_COTH},
	{"arcsin", SEDML_MATHML_ARCSIN},
	{"arccos", SEDML_MATHML_ARCCOS},
	{"arctan", SEDML_MATHML_ARCTAN},
	{"arcsec", SEDML_MATHML_ARCSEC},
	{"arccsc", SEDML_MATHML_ARCCSC},
	{"arccot", SEDML_MATHML_ARCCOT},
	{"arcsinh", SEDML_MATHML_ARCSINH},
	{"arccosh", SEDML_MATHML_ARCCOSH},
	{"arctanh", SEDML_MATHML_ARCTANH},
	{"arcsech", SEDML_MATHML_ARCSECH},
	{"arccsch", SEDML_MATHML_ARCCSCH},
	{"arccoth", SEDML_MATHML_ARCCOTH},
	/* constants */
	{"true", SEDML_MATHML_TRUE},
	{"false", SEDML_MATHML_FALSE},
	{"notanumber", SEDML_MATHML_NOTANUMBER},
	{"pi", SEDML_MATHML_PI},
	{"infinity", SEDML_MATHML_INFINITY},
	{"exponentiale", SEDML_MATHML_EXPONENTIALE},
	/* MathML annotations */
	{"semantics", SEDML_MATHML_SEMANTICS},
	{"annotation", SEDML_MATHML_ANNOTATION},
	{"annotation_xml", SEDML_MATHML_ANNOTATION_XML},
	/* function */
	{"min", SEDML_MATHML_MIN},
	{"max", SEDML_MATHML_MAX},
	{"sum", SEDML_MATHML_SUM},
	{"product", SEDML_MATHML_PRODUCT}
};

#define num_mathml_elements \
	(sizeof(mathml_elements)/sizeof(mathml_elements[0]))

static int cmp_name(const void *x, const void *y)
{
	const struct mathml_element *e1, *e2;
	e1 = (const struct mathml_element *)x;
	e2 = (const struct mathml_element *)y;
	return strcmp(e1->name, e2->name);
}

static int cmp_type(const void *x, const void *y)
{
	const struct mathml_element *e1, *e2;
	e1 = (const struct mathml_element *)x;
	e2 = (const struct mathml_element *)y;
	return e1->type - e2->type;
}

/* API */

struct sedml_mathml_element *sedml_create_mathml_element(const char *name)
{
	struct sedml_mathml_element *e = NULL;
	struct mathml_element me, *found;

	if (!name) goto out;
	qsort(mathml_elements, num_mathml_elements,
	      sizeof(mathml_elements[0]), cmp_name);
	me.name = name;
	found = bsearch(&me, mathml_elements, num_mathml_elements,
			sizeof(mathml_elements[0]), cmp_name);
	if (!found) goto out;
	if (SEDML_MATHML_IS_TYPE_TOKEN(found->type)) {
		struct sedml_mathml_token *token;

		token = calloc(1, sizeof(*token));
		if (!token) goto out;
		e = (struct sedml_mathml_element *)token;
	} else {
		struct sedml_mathml_node *node;

		node = calloc(1, sizeof(*node));
		if (!node) goto out;
		e = (struct sedml_mathml_element *)node;
	}
	e->type = found->type;
 out:
	return e;
}

int sedml_mathml_node_add_child(struct sedml_mathml_node *node,
				struct sedml_mathml_element *e)
{
	size_t s;
	int r = 0, i;

	i = node->num_children++;
	s = node->num_children * sizeof(e);
	node->children = realloc(node->children, s);
	if (!node->children) {
		r = -1;
		goto out;
	}
	node->children[i] = e;
 out:
	return r;
}

void sedml_destroy_mathml_element(struct sedml_mathml_element *e)
{
	if (!e) return;
	if (SEDML_MATHML_IS_TOKEN(e)) {
		struct sedml_mathml_token *token;

		token = (struct sedml_mathml_token *)e;
		free(token->body);
	} else {
		struct sedml_mathml_node *node;
		int i;

		node = (struct sedml_mathml_node *)e;
		for (i = 0; i < node->num_children; i++) {
			sedml_destroy_mathml_element(node->children[i]);
		}
		free(node->children);
	}
	free(e);
}

int sedml_mathml_element_compare(const struct sedml_mathml_element *e0,
				 const struct sedml_mathml_element *e1)
{
	int r;

	if (e0 == e1) return 0;
	r = e0->type - e1->type;
	if (r != 0) return r;
	if (SEDML_MATHML_IS_TOKEN(e0)) {
		struct sedml_mathml_token *t0, *t1;

		t0 = (struct sedml_mathml_token *)e0;
		t1 = (struct sedml_mathml_token *)e1;
		return strcmp(t0->body, t1->body);
	} else {
		struct sedml_mathml_node *n0, *n1;
		int i;

		n0 = (struct sedml_mathml_node *)e0;
		n1 = (struct sedml_mathml_node *)e1;
		r = n0->num_children - n1->num_children;
		if (r != 0) return r;
		for (i = 0; i < n0->num_children; i++) {
			r = sedml_mathml_element_compare(n0->children[i], n1->children[i]);
			if (r != 0) return r;
		}
		return 0;
	}
}

const char *sedml_mathml_element_name(const struct sedml_mathml_element *e)
{
	struct mathml_element me, *found;

	qsort(mathml_elements, num_mathml_elements,
	      sizeof(mathml_elements[0]), cmp_type);
	me.type = e->type;
	found = bsearch(&me, mathml_elements, num_mathml_elements,
			sizeof(mathml_elements[0]), cmp_type);
	return found ? found->name : NULL;
}
