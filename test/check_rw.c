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
		char *buf, *dir;					\
		struct sedml_document *doc0, *doc1;			\
		int r;							\
									\
		doc0 = sedml_create_document();				\
		assert(doc0);						\
									\
		len = strlen(__FILE__);					\
		buf = malloc(len + EXTRA); /* sufficiently large */	\
		strcpy(buf, __FILE__);					\
		dir = dirname(buf);					\
		len = strlen(dir);					\
		sprintf(buf, "%s/data/%s", dir, stem);			\
									\
		r = sedml_read_file(buf, NULL, doc0);			\
		assert(r == 0);						\
									\
		assert(argc > 0);					\
		len = strlen(argv[0]);					\
		buf = realloc(buf, len + EXTRA); /* sufficiently large */ \
		strcpy(buf, argv[0]);					\
		dir = dirname(buf);					\
		len = strlen(dir);					\
		sprintf(buf, "%s/%s", dir, stem);			\
									\
		r = sedml_write_file(buf, doc0);			\
		assert(r == 0);						\
									\
		/* read written document */				\
		doc1 = sedml_create_document();				\
		assert(doc1);						\
									\
		r = sedml_read_file(buf, NULL, doc1);			\
		assert(r == 0);						\
									\
		r = sedml_document_compare(doc0, doc1);			\
		if (r != 0) result++;					\
									\
		sedml_destroy_document(doc1);				\
		sedml_destroy_document(doc0);				\
		free(buf);						\
	} while (0)

int main(int argc, char *argv[])
{
	int result = 0;
	ROUNDTRIP("example1.xml");
	ROUNDTRIP("example2.xml");
	ROUNDTRIP("sedMLBIOM12.xml");
	return result;
}
