/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include <stdio.h>
#include "sedml/document.h"
#include "sedml/mathml.h"

#define MATHML(name) MATHML_ ## name

enum mathml_operation {
#include "mathml.txt"
};

#undef MATHML

#define MATHML(name) {MATHML_ ## name, #name}

struct {
	enum mathml_operation op;
	const char *name;
} restricted_operations[] = {
#include "mathml.txt"
};

#undef MATHML

#define NUM_RESTRICTED_OPERATIONS \
	(sizeof(restricted_operations)/sizeof(restricted_operations[0]))

enum mathml_function {
	MATHML_MIN,
	MATHML_MAX,
	MATHML_SUM,
	MATHML_PRODUCT
};

#define SEDML_MATHML_FUNCTION_DEFINITION_URL(name) (SEDML_NAMESPACE "#" #name)

int main()
{
	unsigned int i;
	for (i = 0; i < NUM_RESTRICTED_OPERATIONS; i++) {
		printf("%d: %s\n",
		       restricted_operations[i].op,
		       restricted_operations[i].name);
	}
	return 0;
}
