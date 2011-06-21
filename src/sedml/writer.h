/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#ifndef SEDML_WRITER_H
#define SEDML_WRITER_H

#include <libxml/xmlwriter.h>
#include <sedml/common.h>
#include <sedml/document.h>

SEDML_C_DECL_BEGIN

struct sedml_writer {
	xmlTextWriterPtr text_writer;
};

SEDML_FUNCTION struct sedml_writer *sedml_create_writer(const char *path);

SEDML_FUNCTION int sedml_writer_write(struct sedml_writer *writer,
				      const struct sedml_document *doc);

SEDML_FUNCTION void sedml_destroy_writer(struct sedml_writer *writer);

SEDML_C_DECL_END

#endif
