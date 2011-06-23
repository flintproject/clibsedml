/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include <assert.h>
#include <libgen.h>
#include <string.h>

#include "sedml/document.h"
#include "sedml/writer.h"

#define EXTRA 64

int main(int argc, char *argv[])
{
	size_t len;
	char *buf, *dir;
	struct sedml_document *doc;
	struct sedml_writer *writer;
	int r;

	len = strlen(argv[0]);
	buf = malloc(len + EXTRA); /* sufficiently large */
	strcpy(buf, argv[0]);
	dir = dirname(buf);
	len = strlen(dir);

	doc = sedml_create_document();
	assert(doc);

	sprintf(buf, "%s/check_writer.xml", dir);
	writer = sedml_create_writer(buf);
	assert(writer);

	r = sedml_writer_write(writer, doc);
	assert(r == 0);

	sedml_destroy_writer(writer);
	sedml_destroy_document(doc);
	free(buf);
	return 0;
}
