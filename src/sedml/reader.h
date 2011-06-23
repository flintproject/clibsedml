/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#ifndef SEDML_READER_H
#define SEDML_READER_H

#include <libxml/xmlreader.h>
#include <sedml/common.h>
#include <sedml/document.h>

SEDML_C_DECL_BEGIN

struct sedml_reader {
	xmlTextReaderPtr text_reader;
	struct sedml_simulation *simulation;
	struct sedml_model *model;
	struct sedml_change *change;
	struct sedml_task *task;
	struct sedml_datagenerator *datagenerator;
	struct sedml_variable *variable;
	struct sedml_parameter *parameter;
	struct sedml_output *output;
	struct sedml_curve *curve;
	struct sedml_surface *surface;
	struct sedml_dataset *dataset;
	int num_math;
	struct sedml_mathml_element **math;
	int *c_math;
	const char *error_message;
};

SEDML_FUNCTION struct sedml_reader *sedml_create_reader(const char *path);

SEDML_FUNCTION int sedml_reader_set_xsd(struct sedml_reader *reader,
					const char *xsd);

SEDML_FUNCTION int sedml_reader_read(struct sedml_reader *reader,
				     struct sedml_document *doc);

SEDML_FUNCTION void sedml_destroy_reader(struct sedml_reader *reader);

SEDML_C_DECL_END

#endif
