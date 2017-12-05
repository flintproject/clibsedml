/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/xmlreader.h>
#include "sedml/reader.h"

static int add_simulation(struct sedml_sedml *sedml,
			  struct sedml_simulation *simulation)
{
	size_t s;
	int r, i;

	if (!sedml || !simulation) {
		r = -1;
		goto out;
	}
	i = sedml->num_simulations++;
	s = sizeof(simulation) * (unsigned int)sedml->num_simulations;
	sedml->simulations = realloc(sedml->simulations, s);
	if (!sedml->simulations) {
		r = -1;
		goto out;
	}
	sedml->simulations[i] = simulation;
	r = 0;
 out:
	return r;
}

#define MARK_CURRENT_SEDBASE(x) \
	reader->sedbase = (struct sedml_sedbase *)(x)

struct attribute {
	const char *local_name;
	void (*setter)(void *, const char *);
};

static int compare_attribute(const void *x, const void *y)
{
	const struct attribute *a = x;
	const struct attribute *b = y;
	return strcmp(a->local_name, b->local_name);
}

#define COPY_VALUE(src, type, key, dst) do {	\
		const char *s = src;		\
		char *tmp;			\
		size_t len;			\
						\
		len = strlen(s);		\
		tmp = malloc(len + 1);		\
		memcpy(tmp, s, len);		\
		tmp[len] = '\0';		\
		((type *)dst)->key = tmp;	\
	} while (0)

#define SET_INT(src, type, key, dst) do {				\
		((type *)dst)->key = atoi(src); /* TODO: handle errors */ \
	} while (0)

#define SET_DOUBLE(src, type, key, dst) do {		\
		char *tmp;				\
							\
		((type *)dst)->key = strtod(src, &tmp);	\
		if (src == tmp) {			\
			/* TODO: handle errors */	\
		}					\
	} while (0)

#define SET_BOOL(src, type, key, dst) do {		\
		if (strcmp(src, "true") == 0) {		\
			((type *)dst)->key = 1;		\
		} else if (strcmp(src, "false") == 0) {	\
			((type *)dst)->key = 0;		\
		} else {				\
			/* TODO: handle errors */	\
		}					\
	} while (0)

#define LOOP_ATTRIBUTES(x_attributes, x) do {				\
		struct attribute a, *found = NULL;			\
		size_t s;						\
		int i, k;						\
									\
		for (i = 0; i < num_attrs; i++) {			\
			a.local_name = attrs[i]->local_name;		\
			found = bsearch(&a, x_attributes,		\
					sizeof(x_attributes)/sizeof(x_attributes[0]), \
					sizeof(x_attributes[0]),	\
					compare_attribute);		\
			if (found) {					\
				found->setter(x, attrs[i]->value);	\
			} else {					\
				/* add xml attribute */			\
				k = x->num_xml_attributes++;		\
				s = (unsigned int)x->num_xml_attributes * sizeof(*x->xml_attributes); \
				x->xml_attributes = realloc(x->xml_attributes, s); \
				x->xml_attributes[k] = attrs[i];	\
				attrs[i] = NULL;			\
			}						\
		}							\
	} while (0)

static void sedbase_set_metaid(void *sedbase, const char *value)
{
	COPY_VALUE(value, struct sedml_sedbase, metaid, sedbase);
}

static void sedml_set_level(void *x, const char *value)
{
	SET_INT(value, struct sedml_sedml, level, x);
}

static void sedml_set_version(void *x, const char *value)
{
	SET_INT(value, struct sedml_sedml, version, x);
}

static struct attribute sedml_attributes[] = {
	/* in alphabetical order */
	{"level",   sedml_set_level},
	{"metaid",  sedbase_set_metaid},
	{"version", sedml_set_version},
};

struct sedml_reader {
	xmlTextReaderPtr text_reader;
	struct sedml_sedbase *sedbase;
	struct sedml_simulation *simulation;
	struct sedml_model *model;
	struct sedml_change *change;
	struct sedml_abstracttask *task;
	struct sedml_vectorrange *vectorrange;
	struct sedml_functionalrange *functionalrange;
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
	int num_xe;
	struct sedml_xhtml_element **xe;
	int *c_xe;
	const char *error_message;
};

static int read_sedml(struct sedml_reader *reader,
		      int num_attrs,
		      struct sedml_xml_attribute **attrs,
		      struct sedml_document *doc)
{
	struct sedml_sedml *sedml;
	int r;

	assert(reader);
	sedml = calloc(1, sizeof(*sedml));
	if (!sedml) {
		r = -1;
		goto fail;
	}
	LOOP_ATTRIBUTES(sedml_attributes, sedml);

	doc->sedml = sedml;
	MARK_CURRENT_SEDBASE(sedml);
	return 0;

 fail:
	sedml_destroy_sedml(sedml);
	return r;
}

static int read_xhtml_element(struct sedml_reader *reader)
{
	xmlTextReaderPtr text_reader;
	const xmlChar *uri, *local_name;
	struct sedml_xhtml_element *e;
	size_t s;
	int i, r = 0;

	text_reader = reader->text_reader;
	uri = xmlTextReaderConstNamespaceUri(text_reader);
	local_name = xmlTextReaderConstLocalName(text_reader);
	if (!xmlStrEqual(uri, (const xmlChar *)SEDML_XHTML_NAMESPACE)) {
		r = -1;
		goto out;
	}
	e = sedml_create_xhtml_element((const char *)local_name);
	if (!e) {
		r = -1;
		goto out;
	}
	i = reader->num_xe++;
	s = (unsigned int)reader->num_xe * sizeof(int);
	reader->c_xe = realloc(reader->c_xe, s);
	if (!reader->c_xe) {
		r = -1;
		goto out;
	}
	s = (unsigned int)reader->num_xe * sizeof(e);
	reader->xe = realloc(reader->xe, s);
	if (!reader->xe) {
		r = -1;
		goto out;
	}
	if (xmlTextReaderIsEmptyElement(text_reader)) {
		reader->c_xe[i] = 1;
	} else {
		reader->c_xe[i] = 0;
	}
	if (e->type == SEDML_XHTML_TEXT) {
		struct sedml_xhtml_text *text;
		xmlChar *str;

		text = (struct sedml_xhtml_text *)e;
		str = xmlTextReaderReadString(text_reader);
		if (!str) {
			r = -1;
			goto out;
		}
		s = (size_t)xmlStrlen(str);
		text->body = malloc(s + 1);
		if (!text->body) {
			xmlFree(str);
			r = -1;
			goto out;
		}
		r = xmlStrPrintf((xmlChar *)text->body, (int)s + 1,
#if LIBXML_VERSION < 20904
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-sign"
#endif
				 (const xmlChar *)
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#endif
				 "%s", str);
		xmlFree(str);
		if (r < 0) {
			free(text->body);
			r = -1;
			goto out;
		}
	}
	reader->xe[i] = e;
 out:
	return r;
}

/**
 * returning 1 means "continue read_notes"
 */
static int end_xhtml_element(struct sedml_reader *reader)
{
	xmlTextReaderPtr text_reader;
	const xmlChar *uri, *local_name;
	struct sedml_xhtml_node *node;
	int r, i, j;

	text_reader = reader->text_reader;
	uri = xmlTextReaderConstNamespaceUri(text_reader);
	local_name = xmlTextReaderConstLocalName(text_reader);
	if ( xmlStrEqual(uri, (const xmlChar *)SEDML_NAMESPACE) ||
	     xmlStrEqual(uri, (const xmlChar *)SEDML_NAMESPACE_L1V2) ||
	     xmlStrEqual(uri, (const xmlChar *)SEDML_NAMESPACE_L1V3) ) {
		struct sedml_xhtml *notes;

		if (!xmlStrEqual(local_name, (const xmlChar *)"notes")) {
			r = -1;
			goto out;
		}
		notes = malloc(sizeof(*notes));
		if (!notes) {
			r = -1;
			goto out;
		}
		notes->num_elements = reader->num_xe;
		notes->elements = reader->xe;
		free(reader->c_xe);
		reader->sedbase->notes = notes;
		r = 0;
		goto out;
	}
	for (i = reader->num_xe - 1; i >= 0; i--) {
		if (!reader->c_xe[i]) break;
	}
	if (i < 0) {
		r = i;
		goto out;
	}
	if (i == reader->num_xe - 1) {
		/* empty element */
		reader->c_xe[i] = 1;
		r = 1;
		goto out;
	}
	if (reader->xe[i]->type == SEDML_XHTML_TEXT) {
		r = -1;
		goto out;
	}
	node = (struct sedml_xhtml_node *)reader->xe[i];
	for (j = i + 1; j < reader->num_xe; j++) {
		r = sedml_xhtml_node_add_child(node, reader->xe[j]);
		if (r < 0) goto out;
	}
	reader->num_xe = i + 1;
	reader->c_xe[i] = 1;
	r = 1;
 out:
	return r;
}

static int read_notes(struct sedml_reader *reader,
		      int num_attrs,
		      struct sedml_xml_attribute **attrs,
		      struct sedml_document *doc)
{
	xmlTextReaderPtr text_reader;
	int r = 0, i, type;

	assert(doc);

	(void)num_attrs;
	(void)attrs;

	reader->num_xe = 0;
	reader->xe = NULL;
	reader->c_xe = NULL;
	text_reader = reader->text_reader;
	for (;;) {
		i = xmlTextReaderRead(text_reader);
		if (i <= 0) {
			r = i-2;
			goto out;
		}
		type = xmlTextReaderNodeType(text_reader);
		switch (type) { /* enum xmlReaderTypes */
		case XML_READER_TYPE_ELEMENT:
			r = read_xhtml_element(reader);
			if (r < 0) goto out;
			break;
		case XML_READER_TYPE_ATTRIBUTE:
		case XML_READER_TYPE_TEXT:
		case XML_READER_TYPE_CDATA:
		case XML_READER_TYPE_ENTITY_REFERENCE:
		case XML_READER_TYPE_ENTITY:
		case XML_READER_TYPE_PROCESSING_INSTRUCTION:
		case XML_READER_TYPE_COMMENT:
		case XML_READER_TYPE_DOCUMENT:
		case XML_READER_TYPE_DOCUMENT_TYPE:
		case XML_READER_TYPE_DOCUMENT_FRAGMENT:
		case XML_READER_TYPE_NOTATION:
		case XML_READER_TYPE_WHITESPACE:
		case XML_READER_TYPE_SIGNIFICANT_WHITESPACE:
			break;
		case XML_READER_TYPE_END_ELEMENT:
			r = end_xhtml_element(reader);
			if (r <= 0) goto out;
			break;
		case XML_READER_TYPE_END_ENTITY:
		case XML_READER_TYPE_XML_DECLARATION:
			break;
		default:
			r = -4;
			goto out;
			break;
		}
	}
 out:
	return r;
}

static void uniformtimecourse_set_id(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_uniformtimecourse, id, x);
}

static void uniformtimecourse_set_initialTime(void *x, const char *value)
{
	SET_DOUBLE(value, struct sedml_uniformtimecourse, initialTime, x);
}

static void uniformtimecourse_set_name(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_uniformtimecourse, name, x);
}

static void uniformtimecourse_set_numberOfPoints(void *x, const char *value)
{
	SET_INT(value, struct sedml_uniformtimecourse, numberOfPoints, x);
}

static void uniformtimecourse_set_outputEndTime(void *x, const char *value)
{
	SET_DOUBLE(value, struct sedml_uniformtimecourse, outputEndTime, x);
}

static void uniformtimecourse_set_outputStartTime(void *x, const char *value)
{
	SET_DOUBLE(value, struct sedml_uniformtimecourse, outputStartTime, x);
}

static struct attribute uniformtimecourse_attributes[] = {
	/* in alphabetical order */
	{"id",              uniformtimecourse_set_id},
	{"initialTime",     uniformtimecourse_set_initialTime},
	{"metaid",          sedbase_set_metaid},
	{"name",            uniformtimecourse_set_name},
	{"numberOfPoints",  uniformtimecourse_set_numberOfPoints},
	{"outputEndTime",   uniformtimecourse_set_outputEndTime},
	{"outputStartTime", uniformtimecourse_set_outputStartTime},
};

static int read_uniformtimecourse(struct sedml_reader *reader,
				  int num_attrs,
				  struct sedml_xml_attribute **attrs,
				  struct sedml_document *doc)
{
	struct sedml_uniformtimecourse *utc;
	int r;

	utc = calloc(1, sizeof(*utc));
	if (!utc) {
		r = -1;
		goto fail;
	}
	utc->simulation_type = SEDML_UNIFORM_TIME_COURSE;
	LOOP_ATTRIBUTES(uniformtimecourse_attributes, utc);

	r = add_simulation(doc->sedml, (struct sedml_simulation *)utc);
	if (r < 0) goto fail;
	reader->simulation = (struct sedml_simulation *)utc;
	MARK_CURRENT_SEDBASE(utc);
	return r;

 fail:
	sedml_destroy_simulation((struct sedml_simulation *)utc);
	return r;
}

static void onestep_set_step(void *x, const char *value)
{
	SET_DOUBLE(value, struct sedml_onestep, step, x);
}

static struct attribute onestep_attributes[] = {
	/* in alphabetical order */
	{"metaid", sedbase_set_metaid},
	{"step", onestep_set_step},
};

static int read_onestep(struct sedml_reader *reader,
			int num_attrs,
			struct sedml_xml_attribute **attrs,
			struct sedml_document *doc)
{
	struct sedml_onestep *os;
	int r;

	os = calloc(1, sizeof(*os));
	if (!os) {
		r = -1;
		goto fail;
	}
	os->simulation_type = SEDML_ONE_STEP;
	LOOP_ATTRIBUTES(onestep_attributes, os);

	r = add_simulation(doc->sedml, (struct sedml_simulation *)os);
	if (r < 0)
		goto fail;
	reader->simulation = (struct sedml_simulation *)os;
	MARK_CURRENT_SEDBASE(os);
	return r;

 fail:
	sedml_destroy_simulation((struct sedml_simulation *)os);
	return r;
}

static int read_steadystate(struct sedml_reader *reader,
			    int num_attrs,
			    struct sedml_xml_attribute **attrs,
			    struct sedml_document *doc)
{
	struct sedml_steadystate *ss;
	int r;

	(void)num_attrs;
	(void)attrs;

	ss = calloc(1, sizeof(*ss));
	if (!ss) {
		r = -1;
		goto fail;
	}
	ss->simulation_type = SEDML_STEADY_STATE;

	r = add_simulation(doc->sedml, (struct sedml_simulation *)ss);
	if (r < 0)
		goto fail;
	reader->simulation = (struct sedml_simulation *)ss;
	MARK_CURRENT_SEDBASE(ss);
	return r;

 fail:
	sedml_destroy_simulation((struct sedml_simulation *)ss);
	return r;
}

static int end_simulation(struct sedml_reader *reader)
{
	reader->simulation = NULL;
	return 0;
}

static void algorithm_set_kisaoID(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_algorithm, kisaoID, x);
}

static struct attribute algorithm_attributes[] = {
	/* in alphabetical order */
	{"kisaoID", algorithm_set_kisaoID},
	{"metaid",  sedbase_set_metaid},
};

static int read_algorithm(struct sedml_reader *reader,
			  int num_attrs,
			  struct sedml_xml_attribute **attrs,
			  struct sedml_document *doc)
{
	struct sedml_algorithm *algorithm;
	int r = 0;

	assert(reader);
	assert(doc);
	algorithm = calloc(1, sizeof(*algorithm));
	if (!algorithm) {
		r = -1;
		goto fail;
	}
	LOOP_ATTRIBUTES(algorithm_attributes, algorithm);

	if (!reader->simulation) {
		r = -1;
		goto fail;
	}
	if (reader->simulation->algorithm) {
		r = -1;
		goto fail;
	}
	reader->simulation->algorithm = algorithm;
	MARK_CURRENT_SEDBASE(algorithm);
	return r;

 fail:
	sedml_destroy_algorithm(algorithm);
	return r;
}

static int add_algorithmparameter(struct sedml_algorithm *algorithm,
				  struct sedml_algorithmparameter *ap)
{
	size_t s;
	int r, i;
	void *x;

	if (!algorithm || !ap) {
		r = -1;
		goto out;
	}
	i = algorithm->num_algorithmparameters++;
	s = sizeof(ap) * (unsigned int)algorithm->num_algorithmparameters;
	x = realloc(algorithm->algorithmparameters, s);
	if (!x) {
		r = -1;
		goto out;
	}
	algorithm->algorithmparameters = x;
	algorithm->algorithmparameters[i] = ap;
	r = 0;
 out:
	return r;
}

static void algorithmparameter_set_kisaoID(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_algorithmparameter, kisaoID, x);
}

static void algorithmparameter_set_value(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_algorithmparameter, value, x);
}

static struct attribute algorithmparameter_attributes[] = {
	/* in alphabetical order */
	{"kisaoID", algorithmparameter_set_kisaoID},
	{"metaid", sedbase_set_metaid},
	{"value", algorithmparameter_set_value},
};

static int read_algorithmparameter(struct sedml_reader *reader,
				   int num_attrs,
				   struct sedml_xml_attribute **attrs,
				   struct sedml_document *doc)
{
	struct sedml_algorithmparameter *ap;
	int r = 0;

	assert(reader);
	assert(doc);
	ap = calloc(1, sizeof(*ap));
	if (!ap) {
		r = -1;
		goto fail;
	}
	LOOP_ATTRIBUTES(algorithmparameter_attributes, ap);

	if (!reader->simulation) {
		r = -1;
		goto fail;
	}
	if (!reader->simulation->algorithm) {
		r = -1;
		goto fail;
	}
	r = add_algorithmparameter(reader->simulation->algorithm, ap);
	if (r < 0)
		goto fail;
	MARK_CURRENT_SEDBASE(ap);
	return r;

 fail:
	sedml_destroy_algorithmparameter(ap);
	return r;
}

static int add_model(struct sedml_sedml *sedml, struct sedml_model *model)
{
	size_t s;
	int r, i;

	if (!sedml || !model) {
		r = -1;
		goto out;
	}
	i = sedml->num_models++;
	s = sizeof(model) * (unsigned int)sedml->num_models;
	sedml->models = realloc(sedml->models, s);
	if (!sedml->models) {
		r = -1;
		goto out;
	}
	sedml->models[i] = model;
	r = 0;
 out:
	return r;
}

static void model_set_id(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_model, id, x);
}

static void model_set_language(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_model, language, x);
}

static void model_set_name(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_model, name, x);
}

static void model_set_source(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_model, source, x);
}

static struct attribute model_attributes[] = {
	/* in alphabetical order */
	{"id",       model_set_id},
	{"language", model_set_language},
	{"metaid",   sedbase_set_metaid},
	{"name",     model_set_name},
	{"source",   model_set_source},
};

static int read_model(struct sedml_reader *reader,
		      int num_attrs,
		      struct sedml_xml_attribute **attrs,
		      struct sedml_document *doc)
{
	struct sedml_model *model;
	int r = 0;

	model = calloc(1, sizeof(*model));
	if (!model) {
		r = -1;
		goto fail;
	}
	LOOP_ATTRIBUTES(model_attributes, model);

	r = add_model(doc->sedml, model);
	if (r < 0) goto fail;
	reader->model = model;
	MARK_CURRENT_SEDBASE(model);
	return r;

 fail:
	sedml_destroy_model(model);
	return r;
}

static int end_model(struct sedml_reader *reader)
{
	reader->model = NULL;
	return 0;
}

static int add_change(struct sedml_model *model, struct sedml_change *change)
{
	size_t s;
	int r, i;

	if (!model || !change) {
		r = -1;
		goto out;
	}
	i = model->num_changes++;
	s = sizeof(change) * (unsigned int)model->num_changes;
	model->changes = realloc(model->changes, s);
	if (!model->changes) {
		r = -1;
		goto out;
	}
	model->changes[i] = change;
	r = 0;
 out:
	return r;
}

static void changeattribute_set_newValue(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_changeattribute, newValue, x);
}

static void changeattribute_set_target(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_changeattribute, target, x);
}

static struct attribute changeattribute_attributes[] = {
	/* in alphabetical order */
	{"metaid",   sedbase_set_metaid},
	{"newValue", changeattribute_set_newValue},
	{"target",   changeattribute_set_target},
};

static int read_changeattribute(struct sedml_reader *reader,
				int num_attrs,
				struct sedml_xml_attribute **attrs,
				struct sedml_document *doc)
{
	struct sedml_changeattribute *ca;
	int r = 0;

	assert(reader);
	assert(doc);
	ca = calloc(1, sizeof(*ca));
	if (!ca) {
		r = -1;
		goto fail;
	}
	ca->change_type = SEDML_CHANGE_ATTRIBUTE;
	LOOP_ATTRIBUTES(changeattribute_attributes, ca);

	r = add_change(reader->model, (struct sedml_change *)ca);
	if (r < 0) goto fail;
	reader->change = (struct sedml_change *)ca;
	MARK_CURRENT_SEDBASE(ca);
	return r;

 fail:
	sedml_destroy_change((struct sedml_change *)ca);
	return r;
}

static void computechange_set_target(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_computechange, target, x);
}

static struct attribute computechange_attributes[] = {
	/* in alphabetical order */
	{"metaid", sedbase_set_metaid},
	{"target", computechange_set_target},
};

static int read_computechange(struct sedml_reader *reader,
			      int num_attrs,
			      struct sedml_xml_attribute **attrs,
			      struct sedml_document *doc)
{
	struct sedml_computechange *cc;
	int r = 0;

	assert(reader);
	assert(doc);
	cc = calloc(1, sizeof(*cc));
	if (!cc) {
		r = -1;
		goto fail;
	}
	cc->change_type = SEDML_COMPUTE_CHANGE;
	LOOP_ATTRIBUTES(computechange_attributes, cc);

	r = add_change(reader->model, (struct sedml_change *)cc);
	if (r < 0) goto fail;
	reader->change = (struct sedml_change *)cc;
	MARK_CURRENT_SEDBASE(cc);
	return r;

 fail:
	sedml_destroy_change((struct sedml_change *)cc);
	return r;
}

static int end_computechange(struct sedml_reader *reader)
{
	struct sedml_computechange *cc;
	int r = 0;

	if ( !reader->change
	     || reader->num_math != 1
	     || !reader->math
	     || !reader->math[0]
	     || !reader->c_math
	     || !reader->c_math[0] ) {
		r = -1;
		goto out;
	}
	cc = (struct sedml_computechange *)reader->change;
	cc->math = reader->math[0];
	free(reader->c_math);
	free(reader->math);
	reader->change = NULL;
 out:
	return r;
}

static int end_change(struct sedml_reader *reader)
{
	reader->change = NULL;
	return 0;
}

static int add_abstracttask(struct sedml_sedml *sedml, struct sedml_abstracttask *task)
{
	int r, i;

	if (!sedml || !task) {
		r = -1;
		goto out;
	}
	i = sedml->num_tasks++;
	sedml->tasks = realloc(sedml->tasks, sizeof(task) * (unsigned int)sedml->num_tasks);
	if (!sedml->tasks) {
		r = -1;
		goto out;
	}
	sedml->tasks[i] = task;
	r = 0;
 out:
	return r;
}

static void abstracttask_set_id(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_task, id, x);
}

static void abstracttask_set_name(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_task, name, x);
}

static void task_set_modelReference(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_task, modelReference, x);
}

static void task_set_simulationReference(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_task, simulationReference, x);
}

static struct attribute task_attributes[] = {
	/* in alphabetical order */
	{"id",                  abstracttask_set_id},
	{"metaid",              sedbase_set_metaid},
	{"modelReference",      task_set_modelReference},
	{"name",                abstracttask_set_name},
	{"simulationReference", task_set_simulationReference},
};

static int read_task(struct sedml_reader *reader,
		     int num_attrs,
		     struct sedml_xml_attribute **attrs,
		     struct sedml_document *doc)
{
	struct sedml_task *task;
	int r = 0;

	task = calloc(1, sizeof(*task));
	if (!task) {
		r = -1;
		goto fail;
	}
	task->abstracttask_type = SEDML_TASK;
	LOOP_ATTRIBUTES(task_attributes, task);

	r = add_abstracttask(doc->sedml, (struct sedml_abstracttask *)task);
	if (r < 0)
		goto fail;
	reader->task = (struct sedml_abstracttask *)task;
	MARK_CURRENT_SEDBASE(task);
	return r;

 fail:
	sedml_destroy_abstracttask((struct sedml_abstracttask *)task);
	return r;
}

static void repeatedtask_set_range(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_repeatedtask, range, x);
}

static void repeatedtask_set_resetModel(void *x, const char *value)
{
	SET_BOOL(value, struct sedml_repeatedtask, resetModel, x);
}

static struct attribute repeatedtask_attributes[] = {
	/* in alphabetical order */
	{"id",         abstracttask_set_id},
	{"metaid",     sedbase_set_metaid},
	{"name",       abstracttask_set_name},
	{"range",      repeatedtask_set_range},
	{"resetModel", repeatedtask_set_resetModel}
};

static int read_repeatedtask(struct sedml_reader *reader,
			     int num_attrs,
			     struct sedml_xml_attribute **attrs,
			     struct sedml_document *doc)
{
	struct sedml_repeatedtask *rt;
	int r = 0;

	rt = calloc(1, sizeof(*rt));
	if (!rt) {
		r = -1;
		goto fail;
	}
	rt->abstracttask_type = SEDML_REPEATED_TASK;
	LOOP_ATTRIBUTES(repeatedtask_attributes, rt);

	r = add_abstracttask(doc->sedml, (struct sedml_abstracttask *)rt);
	if (r < 0)
		goto fail;
	reader->task = (struct sedml_abstracttask *)rt;
	MARK_CURRENT_SEDBASE(rt);
	return r;

 fail:
	sedml_destroy_abstracttask((struct sedml_abstracttask *)rt);
	return r;
}

static int end_abstracttask(struct sedml_reader *reader)
{
	reader->task = NULL;
	return 0;
}

static int add_range(struct sedml_repeatedtask *rt,
		     struct sedml_range *range)
{
	size_t s;
	int r, i;
	void *x;

	if (!rt || !range) {
		r = -1;
		goto out;
	}
	i = rt->num_ranges++;
	s = sizeof(range) * (unsigned int)rt->num_ranges;
	x = realloc(rt->ranges, s);
	if (!x) {
		r = -1;
		goto out;
	}
	rt->ranges = x;
	rt->ranges[i] = range;
	r = 0;
 out:
	return r;
}

static void uniformrange_set_start(void *x, const char *value)
{
	SET_DOUBLE(value, struct sedml_uniformrange, start, x);
}

static void uniformrange_set_end(void *x, const char *value)
{
	SET_DOUBLE(value, struct sedml_uniformrange, end, x);
}

static void uniformrange_set_numberOfPoints(void *x, const char *value)
{
	SET_INT(value, struct sedml_uniformrange, numberOfPoints, x);
}

static void uniformrange_set_type(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_uniformrange, type, x);
}

static struct attribute uniformrange_attributes[] = {
	/* in alphabetical order */
	{"end", uniformrange_set_end},
	{"metaid", sedbase_set_metaid},
	{"numberOfPoints", uniformrange_set_numberOfPoints},
	{"start", uniformrange_set_start},
	{"type", uniformrange_set_type}
};

static int read_uniformrange(struct sedml_reader *reader,
			     int num_attrs,
			     struct sedml_xml_attribute **attrs,
			     struct sedml_document *doc)
{
	struct sedml_uniformrange *ur;
	int r = 0;

	(void)doc;

	ur = calloc(1, sizeof(*ur));
	if (!ur) {
		r = -1;
		goto fail;
	}
	ur->range_type = SEDML_UNIFORM_RANGE;
	LOOP_ATTRIBUTES(uniformrange_attributes, ur);

	if (reader->task->abstracttask_type != SEDML_REPEATED_TASK) {
		r = -1;
		goto fail;
	}
	r = add_range((struct sedml_repeatedtask *)reader->task,
		      (struct sedml_range *)ur);
	if (r < 0)
		goto fail;
	MARK_CURRENT_SEDBASE(ur);
	return r;

 fail:
	sedml_destroy_range((struct sedml_range *)ur);
	return r;
}

static int read_value(struct sedml_reader *reader,
		      int num_attrs,
		      struct sedml_xml_attribute **attrs,
		      struct sedml_document *doc)
{
	size_t s;
	int r = 0, i;
	xmlChar *str = NULL;
	struct sedml_vectorrange *vr;
	void *x;

	(void)num_attrs;
	(void)attrs;
	(void)doc;

	if ( !reader->task ||
	     reader->task->abstracttask_type != SEDML_REPEATED_TASK ) {
		r = -1;
		goto fail;
	}
	str = xmlTextReaderReadString(reader->text_reader);
	if (!str) {
		r = -1;
		goto fail;
	}
	vr = reader->vectorrange;
	i = vr->num_values++;
	s = sizeof(double) * (unsigned int)vr->num_values;
	x = realloc(vr->values, s);
	if (!x) {
		r = -1;
		goto fail;
	}
	vr->values = x;
	vr->values[i] = strtod((const char *)str, NULL); // TODO: handle errors
 fail:
	xmlFree(str);
	return r;
}

static int read_vectorrange(struct sedml_reader *reader,
			    int num_attrs,
			    struct sedml_xml_attribute **attrs,
			    struct sedml_document *doc)
{
	struct sedml_vectorrange *vr;
	int r = 0;

	(void)num_attrs;
	(void)attrs;
	(void)doc;

	vr = calloc(1, sizeof(*vr));
	if (!vr) {
		r = -1;
		goto fail;
	}
	vr->range_type = SEDML_VECTOR_RANGE;

	if (reader->task->abstracttask_type != SEDML_REPEATED_TASK) {
		r = -1;
		goto fail;
	}
	r = add_range((struct sedml_repeatedtask *)reader->task,
		      (struct sedml_range *)vr);
	if (r < 0)
		goto fail;
	reader->vectorrange = vr;
	MARK_CURRENT_SEDBASE(vr);
	return r;

 fail:
	sedml_destroy_range((struct sedml_range *)vr);
	return r;
}

static int end_vectorrange(struct sedml_reader *reader)
{
	reader->vectorrange = NULL;
	return 0;
}

static void functionalrange_set_range(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_functionalrange, range, x);
}

static struct attribute functionalrange_attributes[] = {
	/* in alphabetical order */
	{"metaid", sedbase_set_metaid},
	{"range", functionalrange_set_range}
};

static int read_functionalrange(struct sedml_reader *reader,
				int num_attrs,
				struct sedml_xml_attribute **attrs,
				struct sedml_document *doc)
{
	struct sedml_functionalrange *fr;
	int r = 0;

	(void)doc;

	fr = calloc(1, sizeof(*fr));
	if (!fr) {
		r = -1;
		goto fail;
	}
	fr->range_type = SEDML_FUNCTIONAL_RANGE;
	LOOP_ATTRIBUTES(functionalrange_attributes, fr);

	if (reader->task->abstracttask_type != SEDML_REPEATED_TASK) {
		r = -1;
		goto fail;
	}
	r = add_range((struct sedml_repeatedtask *)reader->task,
		      (struct sedml_range *)fr);
	if (r < 0)
		goto fail;
	reader->functionalrange = fr;
	MARK_CURRENT_SEDBASE(fr);
	return r;

 fail:
	sedml_destroy_range((struct sedml_range *)fr);
	return r;
}

static int end_functionalrange(struct sedml_reader *reader)
{
	int r = 0;

	if ( !reader->functionalrange
	     || reader->num_math != 1
	     || !reader->math
	     || !reader->math[0]
	     || !reader->c_math
	     || !reader->c_math[0] ) {
		r = -1;
		goto out;
	}
	reader->functionalrange->math = reader->math[0];
	free(reader->c_math);
	free(reader->math);
	reader->functionalrange = NULL;
 out:
	return r;
}

static int add_subtask(struct sedml_repeatedtask *rt,
		       struct sedml_subtask *st)
{
	size_t s;
	int r, i;
	void *x;

	if (!rt || !st) {
		r = -1;
		goto out;
	}
	i = rt->num_subtasks++;
	s = sizeof(st) * (unsigned int)rt->num_subtasks;
	x = realloc(rt->subtasks, s);
	if (!x) {
		r = -1;
		goto out;
	}
	rt->subtasks = x;
	rt->subtasks[i] = st;
	r = 0;
 out:
	return r;
}

static void subtask_set_task(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_subtask, task, x);
}

static void subtask_set_order(void *x, const char *value)
{
	SET_INT(value, struct sedml_subtask, order, x);
}

static struct attribute subtask_attributes[] = {
	/* in alphabetical order */
	{"metaid", sedbase_set_metaid},
	{"order", subtask_set_order},
	{"task", subtask_set_task}
};

static int read_subtask(struct sedml_reader *reader,
			int num_attrs,
			struct sedml_xml_attribute **attrs,
			struct sedml_document *doc)
{
	struct sedml_subtask *st;
	int r = 0;

	(void)doc;

	st = calloc(1, sizeof(*st));
	if (!st) {
		r = -1;
		goto fail;
	}
	LOOP_ATTRIBUTES(subtask_attributes, st);

	if (reader->task->abstracttask_type != SEDML_REPEATED_TASK) {
		r = -1;
		goto fail;
	}
	r = add_subtask((struct sedml_repeatedtask *)reader->task, st);
	if (r < 0)
		goto fail;
	reader->task = (struct sedml_abstracttask *)st;
	MARK_CURRENT_SEDBASE(st);
	return r;

 fail:
	sedml_destroy_subtask(st);
	return r;
}

static int add_datagenerator(struct sedml_sedml *sedml,
			     struct sedml_datagenerator *datagenerator)
{
	size_t s;
	int r, i;

	if (!sedml || !datagenerator) {
		r = -1;
		goto out;
	}
	i = sedml->num_datagenerators++;
	s = sizeof(datagenerator) * (unsigned int)sedml->num_datagenerators;
	sedml->datagenerators = realloc(sedml->datagenerators, s);
	if (!sedml->datagenerators) {
		r = -1;
		goto out;
	}
	sedml->datagenerators[i] = datagenerator;
	r = 0;
 out:
	return r;
}

static void datagenerator_set_id(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_datagenerator, id, x);
}

static void datagenerator_set_name(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_datagenerator, name, x);
}

static struct attribute datagenerator_attributes[] = {
	/* in alphabetical order */
	{"id",     datagenerator_set_id},
	{"metaid", sedbase_set_metaid},
	{"name",   datagenerator_set_name},
};

static int read_datagenerator(struct sedml_reader *reader,
			      int num_attrs,
			      struct sedml_xml_attribute **attrs,
			      struct sedml_document *doc)
{
	struct sedml_datagenerator *datagenerator;
	int r = 0;

	datagenerator = calloc(1, sizeof(*datagenerator));
	if (!datagenerator) {
		r = -1;
		goto fail;
	}
	LOOP_ATTRIBUTES(datagenerator_attributes, datagenerator);

	r = add_datagenerator(doc->sedml, datagenerator);
	if (r < 0) goto fail;
	reader->datagenerator = datagenerator;
	MARK_CURRENT_SEDBASE(datagenerator);
	return r;

 fail:
	sedml_destroy_datagenerator(datagenerator);
	return r;
}

static int end_datagenerator(struct sedml_reader *reader)
{
	int r = 0;

	if ( !reader->datagenerator
	     || reader->num_math != 1
	     || !reader->math
	     || !reader->math[0]
	     || !reader->c_math
	     || !reader->c_math[0] ) {
		r = -1;
		goto out;
	}
	reader->datagenerator->math = reader->math[0];
	free(reader->c_math);
	free(reader->math);
	reader->datagenerator = NULL;
 out:
	return r;
}

static int add_variable_to_datagenerator(struct sedml_datagenerator *dg,
					 struct sedml_variable *variable)
{
	size_t s;
	int r, i;

	if (!dg || !variable) {
		r = -1;
		goto out;
	}
	i = dg->num_variables++;
	s = sizeof(variable) * (unsigned int)dg->num_variables;
	dg->variables = realloc(dg->variables, s);
	if (!dg->variables) {
		r = -1;
		goto out;
	}
	dg->variables[i] = variable;
	r = 0;
 out:
	return r;
}

static int add_variable_to_computechange(struct sedml_computechange *cc,
					 struct sedml_variable *variable)
{
	size_t s;
	int r, i;

	if (!cc || !variable) {
		r = -1;
		goto out;
	}
	i = cc->num_variables++;
	s = sizeof(variable) * (unsigned int)cc->num_variables;
	cc->variables = realloc(cc->variables, s);
	if (!cc->variables) {
		r = -1;
		goto out;
	}
	cc->variables[i] = variable;
	r = 0;
 out:
	return r;
}

static int add_variable_to_functionalrange(struct sedml_functionalrange *fr,
					   struct sedml_variable *variable)
{
	size_t s;
	int r, i;
	void *x;

	if (!fr || !variable) {
		r = -1;
		goto out;
	}
	i = fr->num_variables++;
	s = sizeof(variable) * (unsigned int)fr->num_variables;
	x = realloc(fr->variables, s);
	if (!x) {
		r = -1;
		goto out;
	}
	fr->variables = x;
	fr->variables[i] = variable;
	r = 0;
 out:
	return r;
}

static void variable_set_id(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_variable, id, x);
}

static void variable_set_name(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_variable, name, x);
}

static void variable_set_target(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_variable, target, x);
}

static void variable_set_symbol(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_variable, symbol, x);
}

static void variable_set_modelReference(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_variable, modelReference, x);
}

static void variable_set_taskReference(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_variable, taskReference, x);
}

static struct attribute variable_attributes[] = {
	/* in alphabetical order */
	{"id",             variable_set_id},
	{"metaid", sedbase_set_metaid},
	{"modelReference", variable_set_modelReference},
	{"name",           variable_set_name},
	{"symbol",         variable_set_symbol},
	{"target",         variable_set_target},
	{"taskReference",  variable_set_taskReference},
};

static int read_variable(struct sedml_reader *reader,
			 int num_attrs,
			 struct sedml_xml_attribute **attrs,
			 struct sedml_document *doc)
{
	struct sedml_variable *variable;
	int r = 0;

	assert(reader);
	assert(doc);
	variable = calloc(1, sizeof(*variable));
	if (!variable) {
		r = -1;
		goto fail;
	}
	LOOP_ATTRIBUTES(variable_attributes, variable);

	if (reader->datagenerator) {
		r = add_variable_to_datagenerator(reader->datagenerator,
						  variable);
	} else if (reader->change) {
		struct sedml_computechange *cc;
		cc = (struct sedml_computechange *)reader->change;
		r = add_variable_to_computechange(cc, variable);
	} else if (reader->functionalrange) {
		r = add_variable_to_functionalrange(reader->functionalrange,
						    variable);
	} else {
		r = -1;
	}
	if (r < 0) goto fail;
	reader->variable = variable;
	MARK_CURRENT_SEDBASE(variable);
	return r;

 fail:
	sedml_destroy_variable(variable);
	return r;
}

static int end_variable(struct sedml_reader *reader)
{
	reader->variable = NULL;
	return 0;
}

static int add_parameter_to_datagenerator(struct sedml_datagenerator *dg,
					  struct sedml_parameter *parameter)
{
	size_t s;
	int r, i;

	if (!dg || !parameter) {
		r = -1;
		goto out;
	}
	i = dg->num_parameters++;
	s = sizeof(parameter) * (unsigned int)dg->num_parameters;
	dg->parameters = realloc(dg->parameters, s);
	if (!dg->parameters) {
		r = -1;
		goto out;
	}
	dg->parameters[i] = parameter;
	r = 0;
 out:
	return r;
}

static int add_parameter_to_computechange(struct sedml_computechange *cc,
					  struct sedml_parameter *parameter)
{
	size_t s;
	int r, i;

	if (!cc || !parameter) {
		r = -1;
		goto out;
	}
	i = cc->num_parameters++;
	s = sizeof(parameter) * (unsigned int)cc->num_parameters;
	cc->parameters = realloc(cc->parameters, s);
	if (!cc->parameters) {
		r = -1;
		goto out;
	}
	cc->parameters[i] = parameter;
	r = 0;
 out:
	return r;
}

static int add_parameter_to_functionalrange(struct sedml_functionalrange *fr,
					    struct sedml_parameter *parameter)
{
	size_t s;
	int r, i;
	void *x;

	if (!fr || !parameter) {
		r = -1;
		goto out;
	}
	i = fr->num_parameters++;
	s = sizeof(parameter) * (unsigned int)fr->num_parameters;
	x = realloc(fr->parameters, s);
	if (!x) {
		r = -1;
		goto out;
	}
	fr->parameters = x;
	fr->parameters[i] = parameter;
	r = 0;
 out:
	return r;
}

static void parameter_set_id(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_parameter, id, x);
}

static void parameter_set_name(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_parameter, name, x);
}

static void parameter_set_value(void *x, const char *value)
{
	SET_DOUBLE(value, struct sedml_parameter, value, x);
}

static struct attribute parameter_attributes[] = {
	/* in alphabetical order */
	{"id",     parameter_set_id},
	{"metaid", sedbase_set_metaid},
	{"name",   parameter_set_name},
	{"value",  parameter_set_value},
};

static int read_parameter(struct sedml_reader *reader,
			  int num_attrs,
			  struct sedml_xml_attribute **attrs,
			  struct sedml_document *doc)
{
	struct sedml_parameter *parameter;
	int r = 0;

	assert(reader);
	assert(doc);
	parameter = calloc(1, sizeof(*parameter));
	if (!parameter) {
		r = -1;
		goto fail;
	}
	LOOP_ATTRIBUTES(parameter_attributes, parameter);

	if (reader->datagenerator) {
		r = add_parameter_to_datagenerator(reader->datagenerator,
						   parameter);
	} else if (reader->change) {
		struct sedml_computechange *cc;
		cc = (struct sedml_computechange *)reader->change;
		r = add_parameter_to_computechange(cc, parameter);
	} else if (reader->functionalrange) {
		r = add_parameter_to_functionalrange(reader->functionalrange,
						     parameter);
	} else {
		r = -1;
	}
	if (r < 0) goto fail;
	reader->parameter = parameter;
	MARK_CURRENT_SEDBASE(parameter);
	return r;

 fail:
	sedml_destroy_parameter(parameter);
	return r;
}

static int end_parameter(struct sedml_reader *reader)
{
	reader->parameter = NULL;
	return 0;
}

static int add_output(struct sedml_sedml *sedml, struct sedml_output *output)
{
	size_t s;
	int r, i;

	if (!sedml || !output) {
		r = -1;
		goto out;
	}
	i = sedml->num_outputs++;
	s = sizeof(output) * (unsigned int)sedml->num_outputs;
	sedml->outputs = realloc(sedml->outputs, s);
	if (!sedml->outputs) {
		r = -1;
		goto out;
	}
	sedml->outputs[i] = output;
	r = 0;
 out:
	return r;
}

static void plot2d_set_id(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_plot2d, id, x);
}

static void plot2d_set_name(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_plot2d, name, x);
}

static struct attribute plot2d_attributes[] = {
	/* in alphabetical order */
	{"id",     plot2d_set_id},
	{"metaid", sedbase_set_metaid},
	{"name",   plot2d_set_name},
};

static int read_plot2d(struct sedml_reader *reader,
		       int num_attrs,
		       struct sedml_xml_attribute **attrs,
		       struct sedml_document *doc)
{
	struct sedml_plot2d *plot2d;
	int r = 0;

	plot2d = calloc(1, sizeof(*plot2d));
	if (!plot2d) {
		r = -1;
		goto fail;
	}
	plot2d->output_type = SEDML_PLOT2D;
	LOOP_ATTRIBUTES(plot2d_attributes, plot2d);

	r = add_output(doc->sedml, (struct sedml_output *)plot2d);
	if (r < 0) goto fail;
	reader->output = (struct sedml_output *)plot2d;
	MARK_CURRENT_SEDBASE(plot2d);
	return r;

 fail:
	sedml_destroy_output((struct sedml_output *)plot2d);
	return r;
}

static int end_output(struct sedml_reader *reader)
{
	reader->output = NULL;
	return 0;
}

static int add_curve(struct sedml_plot2d *plot2d,
		     struct sedml_curve *curve)
{
	size_t s;
	int r, i;

	if (!plot2d) {
		r = -1;
		goto out;
	}
	i = plot2d->num_curves++;
	s = sizeof(curve) * (unsigned int)plot2d->num_curves;
	plot2d->curves = realloc(plot2d->curves, s);
	if (!plot2d->curves) {
		r = -1;
		goto out;
	}
	plot2d->curves[i] = curve;
	r = 0;
 out:
	return r;
}

static void curve_set_id(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_curve, id, x);
}

static void curve_set_name(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_curve, name, x);
}

static void curve_set_logX(void *x, const char *value)
{
	SET_BOOL(value, struct sedml_curve, logX, x);
}

static void curve_set_logY(void *x, const char *value)
{
	SET_BOOL(value, struct sedml_curve, logY, x);
}

static void curve_set_xDataReference(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_curve, xDataReference, x);
}

static void curve_set_yDataReference(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_curve, yDataReference, x);
}

static struct attribute curve_attributes[] = {
	/* in alphabetical order */
	{"id",             curve_set_id},
	{"logX",           curve_set_logX},
	{"logY",           curve_set_logY},
	{"metaid", sedbase_set_metaid},
	{"name",           curve_set_name},
	{"xDataReference", curve_set_xDataReference},
	{"yDataReference", curve_set_yDataReference},
};

static int read_curve(struct sedml_reader *reader,
		      int num_attrs,
		      struct sedml_xml_attribute **attrs,
		      struct sedml_document *doc)
{
	struct sedml_curve *curve;
	int r = 0;

	assert(doc);
	curve = calloc(1, sizeof(*curve));
	if (!curve) {
		r = -1;
		goto fail;
	}
	LOOP_ATTRIBUTES(curve_attributes, curve);

	r = add_curve((struct sedml_plot2d *)reader->output, curve);
	if (r < 0) goto fail;
	reader->curve = curve;
	MARK_CURRENT_SEDBASE(curve);
	return r;

 fail:
	sedml_destroy_curve(curve);
	return r;
}

static int end_curve(struct sedml_reader *reader)
{
	reader->curve = NULL;
	return 0;
}

static int add_surface(struct sedml_plot3d *plot3d,
		       struct sedml_surface *surface)
{
	size_t s;
	int r, i;

	if (!plot3d) {
		r = -1;
		goto out;
	}
	i = plot3d->num_surfaces++;
	s = sizeof(surface) * (unsigned int)plot3d->num_surfaces;
	plot3d->surfaces = realloc(plot3d->surfaces, s);
	if (!plot3d->surfaces) {
		r = -1;
		goto out;
	}
	plot3d->surfaces[i] = surface;
	r = 0;
 out:
	return r;
}

static void surface_set_id(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_surface, id, x);
}

static void surface_set_name(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_surface, name, x);
}

static void surface_set_logX(void *x, const char *value)
{
	SET_BOOL(value, struct sedml_surface, logX, x);
}

static void surface_set_logY(void *x, const char *value)
{
	SET_BOOL(value, struct sedml_surface, logY, x);
}

static void surface_set_logZ(void *x, const char *value)
{
	SET_BOOL(value, struct sedml_surface, logZ, x);
}

static void surface_set_xDataReference(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_surface, xDataReference, x);
}

static void surface_set_yDataReference(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_surface, yDataReference, x);
}

static void surface_set_zDataReference(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_surface, zDataReference, x);
}

static struct attribute surface_attributes[] = {
	/* in alphabetical order */
	{"id",             surface_set_id},
	{"logX",           surface_set_logX},
	{"logY",           surface_set_logY},
	{"logZ",           surface_set_logZ},
	{"metaid", sedbase_set_metaid},
	{"name",           surface_set_name},
	{"xDataReference", surface_set_xDataReference},
	{"yDataReference", surface_set_yDataReference},
	{"zDataReference", surface_set_zDataReference},
};

static int read_surface(struct sedml_reader *reader,
			int num_attrs,
			struct sedml_xml_attribute **attrs,
			struct sedml_document *doc)
{
	struct sedml_surface *surface;
	int r = 0;

	assert(doc);
	surface = calloc(1, sizeof(*surface));
	if (!surface) {
		r = -1;
		goto fail;
	}
	LOOP_ATTRIBUTES(surface_attributes, surface);

	r = add_surface((struct sedml_plot3d *)reader->output, surface);
	if (r < 0) goto fail;
	reader->surface = surface;
	MARK_CURRENT_SEDBASE(surface);
	return r;

 fail:
	sedml_destroy_surface(surface);
	return r;
}

static int end_surface(struct sedml_reader *reader)
{
	reader->surface = NULL;
	return 0;
}

static int add_dataset(struct sedml_report *report,
		       struct sedml_dataset *dataset)
{
	size_t s;
	int r, i;

	if (!report || !dataset) {
		r = -1;
		goto out;
	}
	i = report->num_datasets++;
	s = sizeof(dataset) * (unsigned int)report->num_datasets;
	report->datasets = realloc(report->datasets, s);
	if (!report->datasets) {
		r = -1;
		goto out;
	}
	report->datasets[i] = dataset;
	r = 0;
 out:
	return r;
}

static void dataset_set_id(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_dataset, id, x);
}

static void dataset_set_name(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_dataset, name, x);
}

static void dataset_set_label(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_dataset, label, x);
}

static void dataset_set_dataReference(void *x, const char *value)
{
	COPY_VALUE(value, struct sedml_dataset, dataReference, x);
}

static struct attribute dataset_attributes[] = {
	/* in alphabetical order */
	{"dataReference", dataset_set_dataReference},
	{"id",            dataset_set_id},
	{"label",         dataset_set_label},
	{"metaid", sedbase_set_metaid},
	{"name",          dataset_set_name},
};

static int read_dataset(struct sedml_reader *reader,
			int num_attrs,
			struct sedml_xml_attribute **attrs,
			struct sedml_document *doc)
{
	struct sedml_dataset *dataset;
	int r = 0;

	assert(doc);
	dataset = calloc(1, sizeof(*dataset));
	if (!dataset) {
		r = -1;
		goto fail;
	}
	LOOP_ATTRIBUTES(dataset_attributes, dataset);

	r = add_dataset((struct sedml_report *)reader->output, dataset);
	if (r < 0) goto fail;
	reader->dataset = dataset;
	MARK_CURRENT_SEDBASE(dataset);
	return r;

 fail:
	sedml_destroy_dataset(dataset);
	return r;
}

static int end_dataset(struct sedml_reader *reader)
{
	reader->dataset = NULL;
	return 0;
}

typedef int (*read_function)(struct sedml_reader *,
			     int num_attrs,
			     struct sedml_xml_attribute **attrs,
			     struct sedml_document *);
typedef int (*end_function)(struct sedml_reader *);

#define READ_NOP ((read_function)NULL)
#define END_NOP ((end_function)NULL)

struct sedml_element {
	const char *name;
	read_function read;
	end_function end;
} sedml_elements[] = {
	{"sedML", read_sedml, END_NOP,},
	{"notes", read_notes, END_NOP,},
	{"annotation", READ_NOP, END_NOP,},
	{"listOfSimulations", READ_NOP, END_NOP,},
	{"uniformTimeCourse", read_uniformtimecourse, end_simulation,},
	{"oneStep", read_onestep, end_simulation,},
	{"steadyState", read_steadystate, end_simulation,},
	{"algorithm", read_algorithm, END_NOP,},
	{"listOfAlgorithmParameters", READ_NOP, END_NOP,},
	{"algorithmParameter", read_algorithmparameter, END_NOP,},
	{"listOfModels", READ_NOP, END_NOP,},
	{"model", read_model, end_model,},
	{"listOfChanges", READ_NOP, END_NOP,},
	{"changeAttribute", read_changeattribute, end_change,},
	{"computeChange", read_computechange, end_computechange,},
	{"listOfTasks", READ_NOP, END_NOP,},
	{"task", read_task, end_abstracttask,},
	{"repeatedTask", read_repeatedtask, end_abstracttask,},
	{"listOfRanges", READ_NOP, END_NOP,},
	{"uniformRange", read_uniformrange, END_NOP,},
	{"vectorRange", read_vectorrange, end_vectorrange,},
	{"value", read_value, END_NOP,},
	{"functionalRange", read_functionalrange, end_functionalrange,},
	{"listOfSubTasks", READ_NOP, END_NOP,},
	{"subTask", read_subtask, END_NOP,},
	{"listOfDataSets", READ_NOP, END_NOP,},
	{"listOfDataGenerators", READ_NOP, END_NOP,},
	{"dataGenerator", read_datagenerator, end_datagenerator,},
	{"listOfVariables", READ_NOP, END_NOP,},
	{"listOfParameters", READ_NOP, END_NOP,},
	{"variable", read_variable, end_variable,},
	{"parameter", read_parameter, end_parameter,},
	{"listOfOutputs", READ_NOP, END_NOP,},
	{"plot2D", read_plot2d, end_output,},
	{"listOfCurves", READ_NOP, END_NOP,},
	{"curve", read_curve, end_curve,},
	{"listOfSurfaces", READ_NOP, END_NOP,},
	{"surface", read_surface, end_surface,},
	{"listOfDataSets", READ_NOP, END_NOP,},
	{"dataSet", read_dataset, end_dataset,}
};

#define num_sedml_elements (sizeof(sedml_elements)/sizeof(sedml_elements[0]))

static int cmpse(const void *x, const void *y)
{
	const struct sedml_element *e1 = x;
	const struct sedml_element *e2 = y;
	return strcmp(e1->name, e2->name);
}

static int read_mathml_element(struct sedml_reader *reader)
{
	xmlTextReaderPtr text_reader;
	const xmlChar *uri, *local_name;
	struct sedml_mathml_element *e;
	size_t s;
	int r = 0, i;

	text_reader = reader->text_reader;
	uri = xmlTextReaderConstNamespaceUri(text_reader);
	local_name = xmlTextReaderConstLocalName(text_reader);
	if (!xmlStrEqual(uri, (const xmlChar *)SEDML_MATHML_NAMESPACE)) {
		r = -1;
		goto out;
	}
	e = sedml_create_mathml_element((const char *)local_name);
	if (!e) {
		r = -1;
		goto out;
	}
	i = reader->num_math++;
	s = (unsigned int)reader->num_math * sizeof(int);
	reader->c_math = realloc(reader->c_math, s);
	if (!reader->c_math) {
		r = -1;
		goto out;
	}
	s = (unsigned int)reader->num_math * sizeof(e);
	reader->math = realloc(reader->math, s);
	if (!reader->math) {
		r = -1;
		goto out;
	}
	if (xmlTextReaderIsEmptyElement(text_reader)) {
		reader->c_math[i] = 1;
	} else {
		reader->c_math[i] = 0;
	}
	if (SEDML_MATHML_IS_TOKEN(e)) {
		struct sedml_mathml_token *token;
		xmlChar *str;

		token = (struct sedml_mathml_token *)e;
		str = xmlTextReaderReadString(text_reader);
		if (!str) {
			r = -1;
			goto out;
		}
		s = (size_t)xmlStrlen(str);
		token->body = malloc(s + 1);
		if (!token->body) {
			xmlFree(str);
			r = -1;
			goto out;
		}
		r = xmlStrPrintf((xmlChar *)token->body, (int)s + 1,
#if LIBXML_VERSION < 20904
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-sign"
#endif
				 (const xmlChar *)
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#endif
				 "%s", str);
		xmlFree(str);
		if (r < 0) {
			free(token->body);
			r = -1;
			goto out;
		}
	}
	reader->math[i] = e;
 out:
	return r;
}

/**
 * returning 1 means "continue read_math"
 */
static int end_mathml_element(struct sedml_reader *reader)
{
	xmlTextReaderPtr text_reader;
	const xmlChar *uri, *local_name;
	struct sedml_mathml_node *node;
	int r, i, j;

	text_reader = reader->text_reader;
	uri = xmlTextReaderConstNamespaceUri(text_reader);
	local_name = xmlTextReaderConstLocalName(text_reader);
	if (!xmlStrEqual(uri, (const xmlChar *)SEDML_MATHML_NAMESPACE)) {
		r = -1;
		goto out;
	}
	if (xmlStrEqual(local_name, (const xmlChar *)"math")) {
		if (reader->num_math == 1) {
			r = 0;
		} else {
			r = -1;
		}
		goto out;
	}
	for (i = reader->num_math - 1; i >= 0; i--) {
		if (!reader->c_math[i]) break;
	}
	if (i < 0) {
		r = i;
		goto out;
	}
	if (i == reader->num_math - 1) {
		/* empty element */
		reader->c_math[i] = 1;
		r = 1;
		goto out;
	}
	if (SEDML_MATHML_IS_TOKEN(reader->math[i])) {
		r = -1;
		goto out;
	}
	node = (struct sedml_mathml_node *)reader->math[i];
	for (j = i + 1; j < reader->num_math; j++) {
		r = sedml_mathml_node_add_child(node, reader->math[j]);
		if (r < 0) goto out;
	}
	reader->num_math = i + 1;
	reader->c_math[i] = 1;
	r = 1;
 out:
	return r;
}

static int read_math(struct sedml_reader *reader)
{
	xmlTextReaderPtr text_reader;
	int r = 0, i, type;

	reader->num_math = 0;
	reader->math = NULL;
	reader->c_math = NULL;
	text_reader = reader->text_reader;
	for (;;) {
		i = xmlTextReaderRead(text_reader);
		if (i <= 0) {
			r = i-2;
			goto out;
		}
		type = xmlTextReaderNodeType(text_reader);
		switch (type) { /* enum xmlReaderTypes */
		case XML_READER_TYPE_ELEMENT:
			r = read_mathml_element(reader);
			if (r < 0) goto out;
			break;
		case XML_READER_TYPE_ATTRIBUTE:
		case XML_READER_TYPE_TEXT:
		case XML_READER_TYPE_CDATA:
		case XML_READER_TYPE_ENTITY_REFERENCE:
		case XML_READER_TYPE_ENTITY:
		case XML_READER_TYPE_PROCESSING_INSTRUCTION:
		case XML_READER_TYPE_COMMENT:
		case XML_READER_TYPE_DOCUMENT:
		case XML_READER_TYPE_DOCUMENT_TYPE:
		case XML_READER_TYPE_DOCUMENT_FRAGMENT:
		case XML_READER_TYPE_NOTATION:
		case XML_READER_TYPE_WHITESPACE:
		case XML_READER_TYPE_SIGNIFICANT_WHITESPACE:
			break;
		case XML_READER_TYPE_END_ELEMENT:
			r = end_mathml_element(reader);
			if (r <= 0) goto out;
			break;
		case XML_READER_TYPE_END_ENTITY:
		case XML_READER_TYPE_XML_DECLARATION:
			break;
		default:
			r = -4;
			goto out;
			break;
		}
	}
 out:
	return r;
}

static struct sedml_xml_attribute *create_xml_attribute(const xmlChar *uri,
							const xmlChar *prefix,
							const xmlChar *local_name,
							const xmlChar *value,
							struct sedml_document *doc)
{
	struct sedml_xml_attribute *attr;
	struct sedml_xml_namespace *ns;
	size_t s, len;
	int i;

	if (!local_name) return NULL;

	attr = calloc(1, sizeof(*attr));
	if (!attr) return NULL;

	if ( !uri ||
	     ( xmlStrEqual(uri, (const xmlChar *)SEDML_NAMESPACE) ||
	       xmlStrEqual(uri, (const xmlChar *)SEDML_NAMESPACE_L1V2) ||
	       xmlStrEqual(uri, (const xmlChar *)SEDML_NAMESPACE_L1V3) ) ) {
		attr->ns = NULL;
	} else {
		for (i = 0; i < doc->num_xml_namespaces; i++) {
			if (xmlStrEqual(uri, (const xmlChar *)doc->xml_namespaces[i]->uri))
				goto next;
		}
		ns = malloc(sizeof(*ns));
		if (!ns) return NULL;

		len = (size_t)xmlStrlen(uri);
		ns->uri = malloc(len + 1);
		memcpy(ns->uri, uri, len);
		ns->uri[len] = '\0';

		if (prefix) {
			len = (size_t)xmlStrlen(prefix);
			ns->prefix = malloc(len + 1);
			memcpy(ns->prefix, prefix, len);
			ns->prefix[len] = '\0';
		} else {
			ns->prefix = NULL;
		}

		doc->num_xml_namespaces++;
		s = (unsigned int)doc->num_xml_namespaces * sizeof(ns);
		doc->xml_namespaces = realloc(doc->xml_namespaces, s);
		doc->xml_namespaces[i] = ns;
		attr->ns = ns;
	}
 next:
	len = (size_t)xmlStrlen(local_name);
	attr->local_name = malloc(len + 1);
	memcpy(attr->local_name, local_name, len);
	attr->local_name[len] = '\0';

	if (value) {
		len = (size_t)xmlStrlen(value);
		attr->value = malloc(len + 1);
		memcpy(attr->value, value, len);
		attr->value[len] = '\0';
	} else {
		attr->value = NULL;
	}

	return attr;
}

static int traverse_xml_attributes(struct sedml_reader *reader,
				   struct sedml_xml_attribute ***pattrs,
				   struct sedml_document *doc)
{
	xmlTextReaderPtr text_reader;
	const xmlChar *uri, *prefix, *local_name, *value;
	struct sedml_xml_attribute **attrs = NULL, *attr;
	int n = 0, r;

	text_reader = reader->text_reader;
	r = xmlTextReaderMoveToFirstAttribute(text_reader);
	if (r < 0) return r;
	do {
		if (xmlTextReaderIsNamespaceDecl(text_reader)) continue;

		uri = xmlTextReaderConstNamespaceUri(text_reader);
		prefix = xmlTextReaderConstPrefix(text_reader);
		local_name = xmlTextReaderConstLocalName(text_reader);
		value = xmlTextReaderConstValue(text_reader);

		attr = create_xml_attribute(uri, prefix, local_name, value, doc);
		if (attr) {
			struct sedml_xml_attribute **ra;

			ra = realloc(attrs, ((unsigned int)n + 1) * sizeof(attr));
			if (!ra) {
				int i;

				sedml_destroy_xml_attribute(attr);
				for (i = 0; i < n; i++) {
					sedml_destroy_xml_attribute(attrs[i]);
				}
				free(attrs);
				return -1;
			}
			attrs = ra;
			attrs[n++] = attr;
		}
	} while (xmlTextReaderMoveToNextAttribute(text_reader) == 1);
	*pattrs = attrs;
	return n;
}

static int read_element(struct sedml_reader *reader, struct sedml_document *doc)
{
	xmlTextReaderPtr text_reader;
	const xmlChar *uri, *local_name;
	struct sedml_element se, *found;
	struct sedml_xml_attribute **attrs;
	int i, n, r = 0;
	int l1v1 = 0, l1v2 = 0, l1v3 = 0;

	text_reader = reader->text_reader;
	uri = xmlTextReaderConstNamespaceUri(text_reader);
	local_name = xmlTextReaderConstLocalName(text_reader);
	if ( (l1v1 = xmlStrEqual(uri, (const xmlChar *)SEDML_NAMESPACE) ) ||
	     (l1v2 = xmlStrEqual(uri, (const xmlChar *)SEDML_NAMESPACE_L1V2) ) ||
	     (l1v3 = xmlStrEqual(uri, (const xmlChar *)SEDML_NAMESPACE_L1V3) ) ) {
		se.name = (const char *)local_name;
		found = bsearch(&se, sedml_elements, num_sedml_elements,
				sizeof(sedml_elements[0]), cmpse);
		if (found) {
			if (found->read) {
				n = traverse_xml_attributes(reader, &attrs, doc);
				if (n < 0) {
					r = -1;
					goto out;
				}
				r = found->read(reader, n, attrs, doc);
				if (n > 0) {
					for (i = 0; i < n; i++) {
						sedml_destroy_xml_attribute(attrs[i]);
					}
					free(attrs);
				}
				if (r < 0)
					goto out;
				/* fill sedML's xmlns */
				if (xmlStrEqual(local_name, (const xmlChar *)"sedML")) {
					if (l1v1) {
						doc->sedml->xmlns = SEDML_NAMESPACE;
					} else if (l1v2) {
						doc->sedml->xmlns = SEDML_NAMESPACE_L1V2;
					} else if (l1v3) {
						doc->sedml->xmlns = SEDML_NAMESPACE_L1V3;
					} else {
						assert(0);
					}
				}
			}
		} else {
			r = -1;
			goto out;
		}
	} else if (xmlStrEqual(uri, (const xmlChar *)SEDML_MATHML_NAMESPACE)) {
		if (xmlStrEqual(local_name, (const xmlChar *)"math")) {
			r = read_math(reader);
			if (r < 0) goto out;
		}
	} else {
		/* ignored */
	}
 out:
	return r;
}

static int end_element(struct sedml_reader *reader)
{
	xmlTextReaderPtr text_reader;
	const xmlChar *uri, *local_name;
	int r = 0;

	text_reader = reader->text_reader;
	uri = xmlTextReaderConstNamespaceUri(text_reader);
	local_name = xmlTextReaderConstLocalName(text_reader);
	if ( xmlStrEqual(uri, (const xmlChar *)SEDML_NAMESPACE) ||
	     xmlStrEqual(uri, (const xmlChar *)SEDML_NAMESPACE_L1V2) ||
	     xmlStrEqual(uri, (const xmlChar *)SEDML_NAMESPACE_L1V3) ) {
		struct sedml_element se, *found;
		se.name = (const char *)local_name;
		found = bsearch(&se, sedml_elements, num_sedml_elements,
				sizeof(sedml_elements[0]), cmpse);
		if (found) {
			if (found->end) {
				r = found->end(reader);
				if (r < 0) goto out;
			}
		}
	} else if (xmlStrEqual(uri, (const xmlChar *)SEDML_MATHML_NAMESPACE)) {
		r = -1;
		goto out;
	} else {
		/* ignored */
	}
 out:
	return r;
}

static struct sedml_reader *sedml_create_reader(const char *path)
{
	struct sedml_reader *reader;
	xmlTextReaderPtr text_reader;

	reader = calloc(1, sizeof(*reader));
	if (!reader) return NULL;
	text_reader = xmlReaderForFile(path, NULL, 0);
	if (!text_reader) {
		free(reader);
		return NULL;
	}
	reader->text_reader = text_reader;
	return reader;
}

static int sedml_reader_set_xsd(struct sedml_reader *reader, const char *xsd)
{
	if (!reader) return -1;
	return xmlTextReaderSchemaValidate(reader->text_reader, xsd);
}

static int sedml_reader_read(struct sedml_reader *reader, struct sedml_document *doc)
{
	xmlTextReaderPtr text_reader;
	int i, r = 0, type;

	if (!reader) return -1;
	if (!doc) return -1;
	qsort(sedml_elements, num_sedml_elements, sizeof(sedml_elements[0]), cmpse);
	text_reader = reader->text_reader;
	for (;;) {
		i = xmlTextReaderRead(text_reader);
		if (i < 0) {
			r = i;
			goto out;
		}
		if (i == 0) goto out;
		type = xmlTextReaderNodeType(text_reader);
		switch (type) { /* enum xmlReaderTypes */
		case XML_READER_TYPE_ELEMENT:
			r = read_element(reader, doc);
			if (r < 0) goto out;
			break;
		case XML_READER_TYPE_ATTRIBUTE:
		case XML_READER_TYPE_TEXT:
		case XML_READER_TYPE_CDATA:
		case XML_READER_TYPE_ENTITY_REFERENCE:
		case XML_READER_TYPE_ENTITY:
		case XML_READER_TYPE_PROCESSING_INSTRUCTION:
		case XML_READER_TYPE_COMMENT:
		case XML_READER_TYPE_DOCUMENT:
		case XML_READER_TYPE_DOCUMENT_TYPE:
		case XML_READER_TYPE_DOCUMENT_FRAGMENT:
		case XML_READER_TYPE_NOTATION:
		case XML_READER_TYPE_WHITESPACE:
		case XML_READER_TYPE_SIGNIFICANT_WHITESPACE:
			break;
		case XML_READER_TYPE_END_ELEMENT:
			r = end_element(reader);
			if (r < 0) goto out;
			break;
		case XML_READER_TYPE_END_ENTITY:
		case XML_READER_TYPE_XML_DECLARATION:
			break;
		default:
			r = -2;
			goto out;
			break;
		}
	}
 out:
	return r;
}

static void sedml_destroy_reader(struct sedml_reader *reader)
{
	if (!reader) return;
	xmlFreeTextReader(reader->text_reader);
	free(reader);
}

/* API */

int sedml_read_file(const char *path, const char *xsd,
		    struct sedml_document *doc)
{
	struct sedml_reader *reader;
	int r;

	reader = sedml_create_reader(path);
	if (!reader) return -1;
	if (xsd) {
		r = sedml_reader_set_xsd(reader, xsd);
		if (r < 0) return r;
	}
	r = sedml_reader_read(reader, doc);
	sedml_destroy_reader(reader);
	return r;
}
