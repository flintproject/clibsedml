/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "sedml/writer.h"

#define WRITE_ATTR(x, attr) do {					\
		assert(x);						\
		if (!(x)->attr || strlen((x)->attr) == 0) break;	\
		r = xmlTextWriterWriteAttribute(text_writer,		\
						BAD_CAST #attr,		\
						BAD_CAST (x)->attr);	\
		if (r < 0) goto out;					\
	} while (0)

#define WRITE_ID(x) WRITE_ATTR(x, id)

#define WRITE_NAME(x) WRITE_ATTR(x, name)

static int write_attribute_as_double(xmlTextWriterPtr text_writer,
				     const char *name,
				     double value,
				     char *buf)
{
	int r;

	r = sprintf(buf, "%f", value);
	if (r < 0) goto out;
	r = xmlTextWriterWriteAttribute(text_writer, BAD_CAST name, BAD_CAST buf);
 out:
	return r;
}

#define WRITE_ATTR_DOUBLE(x, attr) do {					\
		r = write_attribute_as_double(text_writer, #attr,	\
					      (x)->attr, buf);		\
		if (r < 0) goto out;					\
	} while (0)

static int write_attribute_as_int(xmlTextWriterPtr text_writer,
				  const char *name,
				  int value,
				  char *buf)
{
	int r;

	r = sprintf(buf, "%d", value);
	if (r < 0) goto out;
	r = xmlTextWriterWriteAttribute(text_writer, BAD_CAST name,
					BAD_CAST buf);
 out:
	return r;
}

#define WRITE_ATTR_INT(x, attr) do {				\
		r = write_attribute_as_int(text_writer, #attr,	\
					   (x)->attr, buf);	\
		if (r < 0) goto out;				\
	} while (0)

#define WRITE_ATTR_BOOL(x, attr) do {				\
		const char *b = ((x)->attr) ? "true" : "false";	\
		r = xmlTextWriterWriteAttribute(text_writer,	\
						BAD_CAST #attr,	\
						BAD_CAST b);	\
		if (r < 0) goto out;				\
	} while (0)

static int write_sedbase_attributes(xmlTextWriterPtr text_writer,
				    const struct sedml_sedbase *sedbase)
{
	int r = 0;

	if (!sedbase->metaid || strlen(sedbase->metaid) == 0) {
		goto out;
	}
	r = xmlTextWriterWriteAttribute(text_writer,
					BAD_CAST "metaid",
					BAD_CAST sedbase->metaid);
 out:
	return r;
}

#define WRITE_SEDBASE_ATTRIBUTES(x) do {				\
		const struct sedml_sedbase *sedbase;			\
		sedbase = (const struct sedml_sedbase *)(x);		\
		r = write_sedbase_attributes(text_writer, sedbase);	\
		if (r < 0) goto out;					\
	} while (0)

static int write_xhtml_element(xmlTextWriterPtr text_writer,
			       const struct sedml_xhtml_element *e,
			       int top)
{
	int r;

	if (!e) {
		r = -1;
		goto out;
	}
	if (top) {
		r = xmlTextWriterStartElementNS(text_writer, NULL,
						BAD_CAST e->name,
						BAD_CAST SEDML_XHTML_NAMESPACE);
	} else {
		r = xmlTextWriterStartElement(text_writer, BAD_CAST e->name);
	}
	if (r < 0) goto out;
	if (e->type == SEDML_XHTML_TEXT) {
		const struct sedml_xhtml_text *text;

		text = (const struct sedml_xhtml_text *)e;
		r = xmlTextWriterWriteString(text_writer, BAD_CAST text->body);
		if (r < 0) goto out;
	} else {
		const struct sedml_xhtml_node *node;
		int i;

		node = (const struct sedml_xhtml_node *)e;
		for (i = 0; i < node->num_children; i++) {
			r = write_xhtml_element(text_writer,
						node->children[i], 0);
			if (r < 0) goto out;
		}
	}
	r = xmlTextWriterEndElement(text_writer);
 out:
	return r;
}

static int write_notes(xmlTextWriterPtr text_writer,
		       const struct sedml_xhtml *notes)
{
	int i, r = 0;

	if (!notes) goto out;
	r = xmlTextWriterStartElement(text_writer, BAD_CAST "notes");
	if (r < 0) goto out;
	for (i = 0; i < notes->num_elements; i++) {
		r = write_xhtml_element(text_writer, notes->elements[i], 1);
		if (r < 0) goto out;
	}
	r = xmlTextWriterEndElement(text_writer);
 out:
	return r;
}

#define WRITE_NOTES(x) do {					\
		r = write_notes(text_writer, (x)->notes);	\
		if (r < 0) goto out;				\
	} while (0)

#define WRITE_LIST_OF(name, x, list, f) do {				\
		if (x->num_ ## list > 0) {				\
			int i;						\
			r = xmlTextWriterStartElement(text_writer,	\
						      BAD_CAST name);	\
			if (r < 0) goto out;				\
			for (i = 0; i < x->num_ ## list; i++) {	\
				assert(x->list[i]);			\
				r = f(text_writer, x->list[i], buf);	\
				if (r < 0) goto out;			\
			}						\
			r = xmlTextWriterEndElement(text_writer);	\
			if (r < 0) goto out;				\
		}							\
	} while (0)

static int write_variable(xmlTextWriterPtr text_writer,
			  const struct sedml_variable *variable,
			  char *buf)
{
	int r;

	assert(buf);
	r = xmlTextWriterStartElement(text_writer, BAD_CAST "variable");
	if (r < 0) goto out;
	WRITE_SEDBASE_ATTRIBUTES(variable);
	WRITE_ID(variable);
	WRITE_NAME(variable);
	WRITE_ATTR(variable, target);
	WRITE_ATTR(variable, symbol);
	WRITE_ATTR(variable, taskReference);
	WRITE_ATTR(variable, modelReference);
	WRITE_NOTES(variable);
	r = xmlTextWriterEndElement(text_writer);
 out:
	return r;
}

static int write_parameter(xmlTextWriterPtr text_writer,
			   const struct sedml_parameter *parameter,
			   char *buf)
{
	int r;

	r = xmlTextWriterStartElement(text_writer, BAD_CAST "parameter");
	if (r < 0) goto out;
	WRITE_SEDBASE_ATTRIBUTES(parameter);
	WRITE_ID(parameter);
	WRITE_NAME(parameter);
	WRITE_ATTR_DOUBLE(parameter, value);
	WRITE_NOTES(parameter);
	r = xmlTextWriterEndElement(text_writer);
 out:
	return r;
}

static int write_math_element(xmlTextWriterPtr text_writer,
			      const struct sedml_mathml_element *e)
{
	const char *name;
	int r;

	name = sedml_mathml_element_name(e);
	if (!name) {
		r = -1;
		goto out;
	}
	r = xmlTextWriterStartElement(text_writer, BAD_CAST name);
	if (r < 0) goto out;
	if (SEDML_MATHML_IS_TOKEN(e)) {
		const struct sedml_mathml_token *token;

		token = (const struct sedml_mathml_token *)e;
		if (token->type == SEDML_MATHML_CSYMBOL) {
			size_t s, i;
			char *url;

			s = strlen(SEDML_NAMESPACE) + 1 + strlen(token->body);
			url = malloc(s + 1);
			if (!url) {
				r = -1;
				goto out;
			}
			snprintf(url, s + 1, "%s#%s", SEDML_NAMESPACE, token->body);
			/* trim tailing spaces */
			for (i = strlen(SEDML_NAMESPACE) + 1; i < strlen(url); i++) {
				if (!isalpha(url[i])) {
					url[i] = '\0';
					break;
				}
			}
			r = xmlTextWriterWriteAttribute(text_writer,
							BAD_CAST "definitionURL",
							BAD_CAST url);
			free(url);
			if (r < 0) goto out;
			r = xmlTextWriterWriteAttribute(text_writer,
							BAD_CAST "encoding",
							BAD_CAST "text");
			if (r < 0) goto out;
		}
		r = xmlTextWriterWriteString(text_writer, BAD_CAST token->body);
		if (r < 0) goto out;
	} else {
		const struct sedml_mathml_node *node;
		int i;

		node = (const struct sedml_mathml_node *)e;
		for (i = 0; i < node->num_children; i++) {
			assert(node->children[i]);
			r = write_math_element(text_writer, node->children[i]);
			if (r < 0) goto out;
		}
	}
	r = xmlTextWriterEndElement(text_writer);
 out:
	return r;
}

static int write_math(xmlTextWriterPtr text_writer,
		      const struct sedml_mathml_element *e)
{
	int r;

	assert(e);
	r = xmlTextWriterStartElementNS(text_writer, NULL, BAD_CAST "math",
					BAD_CAST SEDML_MATHML_NAMESPACE);
	if (r < 0) goto out;
	r = write_math_element(text_writer, e);
	if (r < 0) goto out;
	r = xmlTextWriterEndElement(text_writer);

 out:
	return r;
}

#define WRITE_MATH(x) do {				\
		r = write_math(text_writer, (x)->math); \
		if (r < 0) goto out;			\
	} while (0)

const char *change_elements[] = {
	"computeChange",
	"changeAttribute",
	"changeXML",
	"addXML",
	"removeXML",
};

static int write_change(xmlTextWriterPtr text_writer,
			const struct sedml_change *change,
			char *buf)
{
	const char *name;
	int r;

	name = change_elements[change->change_type];
	r = xmlTextWriterStartElement(text_writer, BAD_CAST name);
	WRITE_SEDBASE_ATTRIBUTES(change);
	WRITE_ATTR(change, target);
	switch (change->change_type) {
	case SEDML_COMPUTE_CHANGE:
		{
			const struct sedml_computechange *cc;
			cc = (const struct sedml_computechange *)change;
			WRITE_LIST_OF("listOfVariables", cc, variables,
				      write_variable);
			WRITE_LIST_OF("listOfParameters", cc, parameters,
				      write_parameter);
			WRITE_MATH(cc);
		}
		break;
	case SEDML_CHANGE_ATTRIBUTE:
		{
			const struct sedml_changeattribute *ca;
			ca = (const struct sedml_changeattribute *)change;
			WRITE_ATTR(ca, newValue);
		}
		break;
	case SEDML_CHANGE_XML:
		/* TODO: newxml */
		break;
	case SEDML_ADD_XML:
		/* TODO: newxml */
		break;
	case SEDML_REMOVE_XML:
		/* nothing to do */
		break;
	default:
		assert(0); /* N/A */
		break;
	}
	WRITE_NOTES(change);
	r = xmlTextWriterEndElement(text_writer);
 out:
	return r;
}

static int write_model(xmlTextWriterPtr text_writer,
		       struct sedml_model *model,
		       char *buf)
{
	int r;

	r = xmlTextWriterStartElement(text_writer, BAD_CAST "model");
	if (r < 0) goto out;
	WRITE_SEDBASE_ATTRIBUTES(model);
	WRITE_ID(model);
	WRITE_NAME(model);
	WRITE_ATTR(model, language);
	WRITE_ATTR(model, source);
	WRITE_NOTES(model);
	WRITE_LIST_OF("listOfChanges", model, changes, write_change);
	r = xmlTextWriterEndElement(text_writer);
 out:
	return r;
}

static int write_algorithm(xmlTextWriterPtr text_writer,
			   const struct sedml_algorithm *algorithm,
			   char *buf)
{
	int r;

	assert(buf);
	r = xmlTextWriterStartElement(text_writer, BAD_CAST "algorithm");
	if (r < 0) goto out;
	WRITE_SEDBASE_ATTRIBUTES(algorithm);
	WRITE_ATTR(algorithm, kisaoID);
	WRITE_NOTES(algorithm);
	r = xmlTextWriterEndElement(text_writer);
 out:
	return r;
}

const char *simulation_elements[] = {
	"uniformTimeCourse",
};

static int write_simulation(xmlTextWriterPtr text_writer,
			    const struct sedml_simulation *simulation,
			    char *buf)
{
	const char *name;
	int r;

	name = simulation_elements[simulation->simulation_type];
	r = xmlTextWriterStartElement(text_writer, BAD_CAST name);
	if (r < 0) goto out;
	WRITE_SEDBASE_ATTRIBUTES(simulation);
	WRITE_ID(simulation);
	WRITE_NAME(simulation);
	switch (simulation->simulation_type) {
	case SEDML_UNIFORM_TIME_COURSE:
		{
			struct sedml_uniformtimecourse *utc;
			utc = (struct sedml_uniformtimecourse *)simulation;
			WRITE_ATTR_DOUBLE(utc, initialTime);
			WRITE_ATTR_DOUBLE(utc, outputStartTime);
			WRITE_ATTR_DOUBLE(utc, outputEndTime);
			WRITE_ATTR_INT(utc, numberOfPoints);
		}
		break;
	default:
		assert(0); /* N/A */
		break;
	}
	r = write_algorithm(text_writer, simulation->algorithm, buf);
	if (r < 0) goto out;
	WRITE_NOTES(simulation);
	r = xmlTextWriterEndElement(text_writer);
 out:
	return r;
}

static int write_task(xmlTextWriterPtr text_writer,
		      const struct sedml_task *task,
		      char *buf)
{
	int r;

	assert(buf);
	r = xmlTextWriterStartElement(text_writer, BAD_CAST "task");
	if (r < 0) goto out;
	WRITE_SEDBASE_ATTRIBUTES(task);
	WRITE_ID(task);
	WRITE_NAME(task);
	WRITE_ATTR(task, modelReference);
	WRITE_ATTR(task, simulationReference);
	WRITE_NOTES(task);
	r = xmlTextWriterEndElement(text_writer);
 out:
	return r;
}

static int write_datagenerator(xmlTextWriterPtr text_writer,
			       const struct sedml_datagenerator *datagenerator,
			       char *buf)
{
	int r;

	r = xmlTextWriterStartElement(text_writer, BAD_CAST "dataGenerator");
	if (r < 0) goto out;
	WRITE_SEDBASE_ATTRIBUTES(datagenerator);
	WRITE_ID(datagenerator);
	WRITE_NAME(datagenerator);
	WRITE_NOTES(datagenerator);
	WRITE_LIST_OF("listOfVariables", datagenerator, variables,
		      write_variable);
	WRITE_LIST_OF("listOfParameters", datagenerator, parameters,
		      write_parameter);
	WRITE_MATH(datagenerator);
	r = xmlTextWriterEndElement(text_writer);
 out:
	return r;
}

static int write_curve(xmlTextWriterPtr text_writer,
		       const struct sedml_curve *curve,
		       char *buf)
{
	int r;

	assert(buf);
	r = xmlTextWriterStartElement(text_writer, BAD_CAST "curve");
	if (r < 0) goto out;
	WRITE_SEDBASE_ATTRIBUTES(curve);
	WRITE_ID(curve);
	WRITE_NAME(curve);
	WRITE_ATTR_BOOL(curve, logX);
	WRITE_ATTR(curve, xDataReference);
	WRITE_ATTR_BOOL(curve, logY);
	WRITE_ATTR(curve, yDataReference);
	WRITE_NOTES(curve);
	r = xmlTextWriterEndElement(text_writer);
 out:
	return r;
}

static int write_surface(xmlTextWriterPtr text_writer,
			 const struct sedml_surface *surface,
			 char *buf)
{
	int r;

	assert(buf);
	r = xmlTextWriterStartElement(text_writer, BAD_CAST "surface");
	if (r < 0) goto out;
	WRITE_SEDBASE_ATTRIBUTES(surface);
	WRITE_ID(surface);
	WRITE_NAME(surface);
	WRITE_ATTR_BOOL(surface, logX);
	WRITE_ATTR(surface, xDataReference);
	WRITE_ATTR_BOOL(surface, logY);
	WRITE_ATTR(surface, yDataReference);
	WRITE_ATTR_BOOL(surface, logZ);
	WRITE_ATTR(surface, zDataReference);
	WRITE_NOTES(surface);
	r = xmlTextWriterEndElement(text_writer);
 out:
	return r;
}

static int write_dataset(xmlTextWriterPtr text_writer,
			 const struct sedml_dataset *dataset,
			 char *buf)
{
	int r;

	assert(buf);
	r = xmlTextWriterStartElement(text_writer, BAD_CAST "dataSet");
	if (r < 0) goto out;
	WRITE_SEDBASE_ATTRIBUTES(dataset);
	WRITE_ID(dataset);
	WRITE_NAME(dataset);
	WRITE_ATTR(dataset, label);
	WRITE_ATTR(dataset, dataReference);
	WRITE_NOTES(dataset);
	r = xmlTextWriterEndElement(text_writer);
 out:
	return r;
}

const char *output_elements[] = {
	"plot2D",
	"plot3D",
	"report",
};

static int write_output(xmlTextWriterPtr text_writer,
			const struct sedml_output *output,
			char *buf)
{
	const char *name;
	int r;

	name = output_elements[output->output_type];
	r = xmlTextWriterStartElement(text_writer, BAD_CAST name);
	if (r < 0) goto out;
	WRITE_SEDBASE_ATTRIBUTES(output);
	WRITE_ID(output);
	WRITE_NAME(output);
	WRITE_NOTES(output);
	switch (output->output_type) {
	case SEDML_PLOT2D:
		{
			const struct sedml_plot2d *plot2d;
			plot2d = (const struct sedml_plot2d *)output;
			WRITE_LIST_OF("listOfCurves", plot2d, curves,
				      write_curve);
		}
		break;
	case SEDML_PLOT3D:
		{
			const struct sedml_plot3d *plot3d;
			plot3d = (const struct sedml_plot3d *)output;
			WRITE_LIST_OF("listOfSurfaces", plot3d, surfaces,
				      write_surface);
		}
		break;
	case SEDML_REPORT:
		{
			const struct sedml_report *report;
			report = (const struct sedml_report *)output;
			WRITE_LIST_OF("listOfDataSets", report, datasets,
				      write_dataset);
		}
		break;
	default:
		assert(0); /* N/A */
		break;
	}
	r = xmlTextWriterEndElement(text_writer);
 out:
	return r;
}

/* API */

struct sedml_writer *sedml_create_writer(const char *path)
{
	struct sedml_writer *writer;
	xmlTextWriterPtr text_writer;

	writer = malloc(sizeof(*writer));
	if (!writer) return writer;
	text_writer = xmlNewTextWriterFilename(path, 0);
	if (!text_writer) {
		free(writer);
		return NULL;
	}
	writer->text_writer = text_writer;
	return writer;
}

int sedml_writer_write(struct sedml_writer *writer,
		       const struct sedml_document *doc)
{
	xmlTextWriterPtr text_writer;
	struct sedml_sedml *sedml;
	char *buf;
	int r = 0, i;

	buf = malloc(1024); /* TODO */
	if (!buf) return -1;

	text_writer = writer->text_writer;
	r = xmlTextWriterStartDocument(text_writer, NULL, "utf-8", NULL);
	if (r < 0) goto tidy;

	sedml = doc->sedml;
	if (!sedml) goto tidy;
	r = sprintf(buf, "%s", sedml->xmlns);
	if (r < 0) goto tidy;
	r = xmlTextWriterStartElementNS(text_writer, NULL, BAD_CAST "sedML",
					BAD_CAST buf);
	if (r < 0) goto tidy;
	r = sprintf(buf, "%d", sedml->level);
	if (r < 0) goto tidy;
	r = xmlTextWriterWriteAttribute(text_writer, BAD_CAST "level",
					BAD_CAST buf);
	if (r < 0) goto tidy;
	r = sprintf(buf, "%d", sedml->version);
	if (r < 0) goto tidy;
	r = xmlTextWriterWriteAttribute(text_writer, BAD_CAST "version",
					BAD_CAST buf);

	r = xmlTextWriterStartElement(text_writer, BAD_CAST "listOfModels");
	if (r < 0) goto tidy;
	for (i = 0; i < sedml->num_models; i++) {
		assert(sedml->models[i]);
		r = write_model(text_writer, sedml->models[i], buf);
		if (r < 0) goto tidy;
	}
	r = xmlTextWriterEndElement(text_writer);
	if (r < 0) goto tidy;

	r = xmlTextWriterStartElement(text_writer,
				      BAD_CAST "listOfSimulations");
	if (r < 0) goto tidy;
	for (i = 0; i < sedml->num_simulations; i++) {
		assert(sedml->simulations[i]);
		r = write_simulation(text_writer, sedml->simulations[i], buf);
		if (r < 0) goto tidy;
	}
	r = xmlTextWriterEndElement(text_writer);
	if (r < 0) goto tidy;

	r = xmlTextWriterStartElement(text_writer, BAD_CAST "listOfTasks");
	if (r < 0) goto tidy;
	for (i = 0; i < sedml->num_tasks; i++) {
		assert(sedml->tasks[i]);
		r = write_task(text_writer, sedml->tasks[i], buf);
		if (r < 0) goto tidy;
	}
	r = xmlTextWriterEndElement(text_writer);
	if (r < 0) goto tidy;

	r = xmlTextWriterStartElement(text_writer,
				      BAD_CAST "listOfDataGenerators");
	if (r < 0) goto tidy;
	for (i = 0; i < sedml->num_datagenerators; i++) {
		assert(sedml->datagenerators[i]);
		r = write_datagenerator(text_writer, sedml->datagenerators[i], buf);
		if (r < 0) goto tidy;
	}
	r = xmlTextWriterEndElement(text_writer);
	if (r < 0) goto tidy;

	r = xmlTextWriterStartElement(text_writer, BAD_CAST "listOfOutputs");
	if (r < 0) goto tidy;
	for (i = 0; i < sedml->num_outputs; i++) {
		assert(sedml->outputs[i]);
		r = write_output(text_writer, sedml->outputs[i], buf);
		if (r < 0) goto tidy;
	}
	r = xmlTextWriterEndElement(text_writer);
	if (r < 0) goto tidy;

	r = xmlTextWriterEndElement(text_writer);
	if (r < 0) goto tidy;
	r = xmlTextWriterEndDocument(text_writer);
	if (r < 0) goto tidy;
	r = xmlTextWriterFlush(text_writer);
 tidy:
	free(buf);
	return r;
}

void sedml_destroy_writer(struct sedml_writer *writer)
{
	if (!writer) return;
	if (writer->text_writer) xmlFreeTextWriter(writer->text_writer);
	free(writer);
}
