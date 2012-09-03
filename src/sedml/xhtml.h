/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#ifndef SEDML_XHTML_H
#define SEDML_XHTML_H

#include <sedml/common.h>

SEDML_C_DECL_BEGIN

#define SEDML_XHTML_NAMESPACE "http://www.w3.org/1999/xhtml"

enum sedml_xhtml_type {
	SEDML_XHTML_TEXT,
	SEDML_XHTML_NODE
};

#define SEDML_XHTML_ELEMENT			\
	enum sedml_xhtml_type type;		\
	const char *name

struct sedml_xhtml_element {
	SEDML_XHTML_ELEMENT;
};

struct sedml_xhtml_text {
	SEDML_XHTML_ELEMENT;
	char *body;
};

struct sedml_xhtml_attribute {
	const char *name;
	char *value;
};

struct sedml_xhtml_node {
	SEDML_XHTML_ELEMENT;
	int num_attributes;
	struct sedml_xhtml_attribute **attributes;
	int num_children;
	struct sedml_xhtml_element **children;
};

struct sedml_xhtml {
	int num_elements;
	struct sedml_xhtml_element **elements;
};

SEDML_FUNCTION struct sedml_xhtml_element *
               sedml_create_xhtml_element(const char *);

SEDML_FUNCTION int sedml_xhtml_node_add_attribute(struct sedml_xhtml_node *,
						  const char *, const char *);

SEDML_FUNCTION int sedml_xhtml_node_add_child(struct sedml_xhtml_node *,
					      struct sedml_xhtml_element *);

SEDML_FUNCTION struct sedml_xhtml *sedml_create_xhtml(void);

SEDML_FUNCTION int sedml_xhtml_add_element(struct sedml_xhtml *,
					   struct sedml_xhtml_element *);

SEDML_FUNCTION int sedml_xhtml_compare(const struct sedml_xhtml *,
				       const struct sedml_xhtml *);

SEDML_FUNCTION void sedml_destroy_xhtml(struct sedml_xhtml *);

SEDML_C_DECL_END

#endif
