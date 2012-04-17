/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#ifndef SEDML_READER_H
#define SEDML_READER_H

#include <sedml/common.h>
#include <sedml/document.h>

SEDML_C_DECL_BEGIN

SEDML_FUNCTION int sedml_read_file(const char *path, const char *xsd,
				   struct sedml_document *doc);

SEDML_C_DECL_END

#endif
