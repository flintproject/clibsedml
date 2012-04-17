/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include <assert.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sedml/reader.h"
#include "sedml/writer.h"

#define EXTRA 64
#define SEDML_EXAMPLE "/data/sedMLBIOM12.xml"

int main(int argc, char *argv[])
{
	size_t len;
	char *buf, *dir;
	struct sedml_document *doc;
	int r;

	doc = sedml_create_document();
	assert(doc);

	len = strlen(__FILE__);
	buf = malloc(len + EXTRA); /* sufficiently large */
	strcpy(buf, __FILE__);
	dir = dirname(buf);
	len = strlen(dir);
	sprintf(buf, "%s/%s", dir, SEDML_EXAMPLE);

	r = sedml_read_file(buf, NULL, doc);
	assert(r == 0);

	assert(argc > 0);
	len = strlen(argv[0]);
	buf = realloc(buf, len + EXTRA); /* sufficiently large */
	strcpy(buf, argv[0]);
	dir = dirname(buf);
	len = strlen(dir);
	sprintf(buf, "%s/check_rw.xml", dir);

	r = sedml_write_file(buf, doc);
	assert(r == 0);

	sedml_destroy_document(doc);
	free(buf);
	return 0;
}
