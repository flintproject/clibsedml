/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include <string.h>

#include "sedml/mathml.h"

int main()
{
	struct sedml_mathml_element *e;
	const char *name;
	int r = 0;

	e = sedml_create_mathml_element("plus");
	if (!e || e->type != SEDML_MATHML_PLUS) {
		r = 1;
		goto out;
	}
	name = sedml_mathml_element_name(e);
	if (!name || strcmp(name, "plus") != 0) {
		r = 2;
		goto out;
	}
 out:
	return r;
}
