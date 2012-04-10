/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include <assert.h>
#include <libgen.h>
#include <string.h>

#include "sedml/document.h"
#include "sedml/reader.h"
#include "sedml/writer.h"

#define EXTRA 64
#define SEDML_EXAMPLE "/data/sedMLBIOM12.xml"

int main(int argc, char *argv[])
{
	size_t len;
	char *buf, *dir;
	struct sedml_document *doc;
	struct sedml_reader *reader;
	struct sedml_writer *writer;
	int r;

	doc = sedml_create_document();
	assert(doc);

	len = strlen(__FILE__);
	buf = malloc(len + EXTRA); /* sufficiently large */
	strcpy(buf, __FILE__);
	dir = dirname(buf);
	len = strlen(dir);

	sprintf(buf, "%s/%s", dir, SEDML_EXAMPLE);
	reader = sedml_create_reader(buf);
	assert(reader);

	r = sedml_reader_read(reader, doc);
	assert(r == 0);

	sedml_destroy_reader(reader);

	len = strlen(argv[0]);
	buf = realloc(buf, len + EXTRA); /* sufficiently large */
	strcpy(buf, argv[0]);
	dir = dirname(buf);
	len = strlen(dir);

	sprintf(buf, "%s/check_rw.xml", dir);
	writer = sedml_create_writer(buf);
	assert(writer);

	r = sedml_writer_write(writer, doc);
	assert(r == 0);

	sedml_destroy_writer(writer);
	sedml_destroy_document(doc);
	free(buf);
	return 0;
}
