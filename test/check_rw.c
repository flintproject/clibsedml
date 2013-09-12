/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include <assert.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sedml/reader.h"
#include "sedml/writer.h"

#define EXTRA 64

#define ROUNDTRIP(stem) do {						\
		size_t len;						\
		char *buf, *dir, *path;					\
		struct sedml_document *doc0, *doc1;			\
		int r;							\
									\
		doc0 = sedml_create_document();				\
		assert(doc0);						\
									\
		len = strlen(__FILE__);					\
		buf = malloc(len + EXTRA); /* sufficiently large */	\
		path = malloc(len + EXTRA); /* sufficiently large */	\
		strcpy(buf, __FILE__);					\
		dir = dirname(buf);					\
									\
		sprintf(path, "%s/data/%s", dir, #stem);		\
		r = sedml_read_file(path, NULL, doc0);			\
		assert(r == 0);						\
									\
		sprintf(path, "%s/tmp/%s", dir, #stem);			\
		r = sedml_write_file(path, doc0);			\
		assert(r == 0);						\
									\
		/* read written document */				\
		doc1 = sedml_create_document();				\
		assert(doc1);						\
									\
		r = sedml_read_file(path, NULL, doc1);			\
		assert(r == 0);						\
									\
		r = sedml_document_compare(doc0, doc1);			\
		if (r != 0) result++;					\
									\
		sedml_destroy_document(doc1);				\
		sedml_destroy_document(doc0);				\
		free(buf);						\
		free(path);						\
	} while (0)

int main(void)
{
	int result = 0;
	ROUNDTRIP(example1.xml);
	ROUNDTRIP(example2.xml);
	ROUNDTRIP(sedMLBIOM12.xml);
	return result;
}
