/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include <assert.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sedml/writer.h"

#define EXTRA 64

int main(void)
{
	size_t len;
	char *buf, *dir, *path;
	struct sedml_document *doc;
	int r;

	len = strlen(__FILE__);
	buf = malloc(len + EXTRA); /* sufficiently large */
	path = malloc(len + EXTRA); /* sufficiently large */
	strcpy(buf, __FILE__);
	dir = dirname(buf);
	sprintf(path, "%s/tmp/check_writer.xml", dir);

	doc = sedml_create_document();
	assert(doc);

	r = sedml_write_file(path, doc);
	assert(r == 0);

	sedml_destroy_document(doc);
	free(buf);
	free(path);
	return 0;
}
