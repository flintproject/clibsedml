/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include <assert.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sedml/writer.h"

#define EXTRA 64

int main(int argc, char *argv[])
{
	size_t len;
	char *buf, *dir;
	struct sedml_document *doc;
	int r;

	assert(argc > 0);
	len = strlen(argv[0]);
	buf = malloc(len + EXTRA); /* sufficiently large */
	strcpy(buf, argv[0]);
	dir = dirname(buf);
	len = strlen(dir);
	sprintf(buf, "%s/check_writer.xml", dir);

	doc = sedml_create_document();
	assert(doc);

	r = sedml_write_file(buf, doc);
	assert(r == 0);

	sedml_destroy_document(doc);
	free(buf);
	return 0;
}
