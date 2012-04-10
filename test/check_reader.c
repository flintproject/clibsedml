/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include <assert.h>
#include <libgen.h>
#include <string.h>

#include "sedml/reader.h"

#define EXTRA 64
#define SEDML_EXAMPLE "/data/sedMLBIOM21.xml"
#define SEDML_XSD "/data/sed-ml-L1-V1.xsd"

int main(void)
{
	size_t len;
	char *buf, *dir;
	struct sedml_reader *reader;
	struct sedml_document *doc;
	int r;

	len = strlen(__FILE__);
	buf = malloc(len + EXTRA); /* sufficiently large */
	strcpy(buf, __FILE__);
	dir = dirname(buf);
	len = strlen(dir);

	sprintf(buf, "%s/%s", dir, SEDML_EXAMPLE);
	reader = sedml_create_reader(buf);
	assert(reader);

	sprintf(buf+len, "%s", SEDML_XSD);
	r = sedml_reader_set_xsd(reader, buf);
	assert(r == 0);

	doc = sedml_create_document();
	assert(doc);

	r = sedml_reader_read(reader, doc);
	assert(r == 0);

	sedml_destroy_document(doc);
	sedml_destroy_reader(reader);
	free(buf);
	return 0;
}
