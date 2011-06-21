/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "sedml/writer.h"

#define WRITE_ATTR(text_writer, x, attr, r) do {			\
		assert(x);						\
		if (!(x)->attr || strlen((x)->attr) == 0) break;	\
		r = xmlTextWriterWriteAttribute(text_writer,		\
						#attr, (x)->attr);	\
		if (r < 0) return r;					\
	} while (0)

#define WRITE_ID(text_writer, x, r) WRITE_ATTR(text_writer, x, id, r)

#define WRITE_NAME(text_writer, x, r) WRITE_ATTR(text_writer, x, name, r)

static int write_attribute_as_double(xmlTextWriterPtr text_writer,
				     const xmlChar *name,
				     double value,
				     char *buf)
{
	int r;

	r = sprintf(buf, "%ld", value);
	if (r < 0) goto out;
	r = xmlTextWriterWriteAttribute(text_writer, name, buf);
 out:
	return r;
}

#define WRITE_ATTR_DOUBLE(text_writer, x, attr, buf)			\
	write_attribute_as_double(text_writer, #attr, (x)->attr, buf)

static int write_attribute_as_int(xmlTextWriterPtr text_writer,
				  const xmlChar *name,
				  int value,
				  char *buf)
{
	int r;

	r = sprintf(buf, "%d", value);
	if (r < 0) goto out;
	r = xmlTextWriterWriteAttribute(text_writer, name, buf);
 out:
	return r;
}

#define WRITE_ATTR_INT(text_writer, x, attr, buf) \
	write_attribute_as_int(text_writer, #attr, (x)->attr, buf)

static int write_sedbase(xmlTextWriterPtr text_writer,
			 const struct sedml_sedbase *sedbase)
{
	int r = 0;

	if (!sedbase->metaid || strlen(sedbase->metaid) == 0) {
		goto out;
	}
	r = xmlTextWriterWriteAttribute(text_writer,
					"metaid",
					sedbase->metaid);
 out:
	return r;
}

#define WRITE_SEDBASE(text_writer, x, r) do {				\
		r = write_sedbase(text_writer,				\
				  (const struct sedml_sedbase *)x);	\
		if (r < 0) return r;					\
	} while (0)

static int write_change(xmlTextWriterPtr text_writer,
			const struct sedml_change *change,
			char *buf)
{
	int r;

	WRITE_SEDBASE(text_writer, change, r);
	return r;
}

static int write_model(xmlTextWriterPtr text_writer,
		       struct sedml_model *model,
		       char *buf)
{
	int i, r;

	r = xmlTextWriterStartElement(text_writer, "model");
	if (r < 0) goto out;
	WRITE_SEDBASE(text_writer, model, r);
	WRITE_ID(text_writer, model, r);
	WRITE_NAME(text_writer, model, r);
	WRITE_ATTR(text_writer, model, language, r);
	WRITE_ATTR(text_writer, model, source, r);
	if (model->num_changes > 0) {
		r = xmlTextWriterStartElement(text_writer, "listOfChanges");
		if (r < 0) goto out;
		for (i = 0; i < model->num_changes; i++) {
			assert(model->changes[i]);
			r = write_change(text_writer, model->changes[i], buf);
			if (r < 0) goto out;
		}
		r = xmlTextWriterEndElement(text_writer);
		if (r < 0) goto out;
	}
	r = xmlTextWriterEndElement(text_writer);
 out:
	return r;
}

static int write_algorithm(xmlTextWriterPtr text_writer,
			   const struct sedml_algorithm *algorithm,
			   char *buf)
{
	int r;

	r = xmlTextWriterStartElement(text_writer, "algorithm");
	if (r < 0) goto out;
	WRITE_SEDBASE(text_writer, algorithm, r);
	WRITE_ATTR(text_writer, algorithm, kisaoID, r);
	r = xmlTextWriterEndElement(text_writer);
 out:
	return r;
}

static int write_simulation(xmlTextWriterPtr text_writer,
			    const struct sedml_simulation *simulation,
			    char *buf)
{
	int r;

	r = xmlTextWriterStartElement(text_writer, "simulation");
	if (r < 0) goto out;
	WRITE_SEDBASE(text_writer, simulation, r);
	WRITE_ID(text_writer, simulation, r);
	WRITE_NAME(text_writer, simulation, r);
	switch (simulation->simulation_type) {
	case SEDML_UNIFORM_TIME_COURSE:
		{
			struct sedml_uniformtimecourse *utc;
			utc = (struct sedml_uniformtimecourse *)simulation;
			r = WRITE_ATTR_DOUBLE(text_writer, utc,
					      initialTime, buf);
			if (r < 0) goto out;
			r = WRITE_ATTR_DOUBLE(text_writer, utc,
					      outputStartTime, buf);
			if (r < 0) goto out;
			r = WRITE_ATTR_DOUBLE(text_writer, utc,
					      outputEndTime, buf);
			if (r < 0) goto out;
			r = WRITE_ATTR_INT(text_writer, utc,
					   numberOfPoints, buf);
			if (r < 0) goto out;
		}
		break;
	default:
		assert(0); /* N/A */
		break;
	}
	r = write_algorithm(text_writer, simulation->algorithm, buf);
	if (r < 0) goto out;
	r = xmlTextWriterEndElement(text_writer);
 out:
	return r;
}

static int write_task(xmlTextWriterPtr text_writer,
		      const struct sedml_task *task,
		      char *buf)
{
	int r;

	r = xmlTextWriterStartElement(text_writer, "task");
	if (r < 0) goto out;
	WRITE_SEDBASE(text_writer, task, r);
	WRITE_ID(text_writer, task, r);
	WRITE_NAME(text_writer, task, r);
	WRITE_ATTR(text_writer, task, modelReference, r);
	WRITE_ATTR(text_writer, task, simulationReference, r);
	r = xmlTextWriterEndElement(text_writer);
 out:
	return r;
}

static int write_variable(xmlTextWriterPtr text_writer,
			  const struct sedml_variable *variable,
			  char *buf)
{
	int r;

	r = xmlTextWriterStartElement(text_writer, "variable");
	if (r < 0) goto out;
	WRITE_SEDBASE(text_writer, variable, r);
	r = xmlTextWriterEndElement(text_writer);
 out:
	return r;
}

static int write_parameter(xmlTextWriterPtr text_writer,
			   const struct sedml_parameter *parameter,
			   char *buf)
{
	int r;

	r = xmlTextWriterStartElement(text_writer, "variable");
	if (r < 0) goto out;
	WRITE_SEDBASE(text_writer, parameter, r);
	r = xmlTextWriterEndElement(text_writer);
 out:
	return r;
}

static int write_datagenerator(xmlTextWriterPtr text_writer,
			       const struct sedml_datagenerator *datagenerator,
			       char *buf)
{
	int r, i;

	r = xmlTextWriterStartElement(text_writer, "dataGenerator");
	if (r < 0) goto out;
	WRITE_SEDBASE(text_writer, datagenerator, r);
	WRITE_ID(text_writer, datagenerator, r);
	WRITE_NAME(text_writer, datagenerator, r);
	if (datagenerator->num_variables > 0) {
		r = xmlTextWriterStartElement(text_writer, "listOfVariables");
		if (r < 0) goto out;
		for (i = 0; i < datagenerator->num_variables; i++) {
			assert(datagenerator->variables[i]);
			write_variable(text_writer, datagenerator->variables[i], buf);
		}
		r = xmlTextWriterEndElement(text_writer);
		if (r < 0) goto out;
	}
	if (datagenerator->num_parameters > 0) {
		r = xmlTextWriterStartElement(text_writer, "listOfParameters");
		if (r < 0) goto out;
		for (i = 0; i < datagenerator->num_parameters; i++) {
			assert(datagenerator->parameters[i]);
			write_parameter(text_writer, datagenerator->parameters[i], buf);
		}
		r = xmlTextWriterEndElement(text_writer);
		if (r < 0) goto out;
	}
	/* TODO: math */
	r = xmlTextWriterEndElement(text_writer);
 out:
	return r;
}

static int write_curve(xmlTextWriterPtr text_writer,
		       const struct sedml_curve *curve,
		       char *buf)
{
	int r;

	r = xmlTextWriterStartElement(text_writer, "curve");
	if (r < 0) goto out;
	WRITE_SEDBASE(text_writer, curve, r);
	r = xmlTextWriterEndElement(text_writer);
 out:
	return r;
}

static int write_surface(xmlTextWriterPtr text_writer,
			 const struct sedml_surface *surface,
			 char *buf)
{
	int r;

	r = xmlTextWriterStartElement(text_writer, "surface");
	if (r < 0) goto out;
	WRITE_SEDBASE(text_writer, surface, r);
	r = xmlTextWriterEndElement(text_writer);
 out:
	return r;
}

static int write_dataset(xmlTextWriterPtr text_writer,
			 const struct sedml_dataset *dataset,
			 char *buf)
{
	int r;

	r = xmlTextWriterStartElement(text_writer, "dataset");
	if (r < 0) goto out;
	WRITE_SEDBASE(text_writer, dataset, r);
	r = xmlTextWriterEndElement(text_writer);
 out:
	return r;
}

static int write_output(xmlTextWriterPtr text_writer,
			const struct sedml_output *output,
			char *buf)
{
	int r, i;

	switch (output->output_type) {
	case SEDML_PLOT2D:
		r = xmlTextWriterStartElement(text_writer, "plot2d");
		break;
	case SEDML_PLOT3D:
		r = xmlTextWriterStartElement(text_writer, "plot3d");
		break;
	case SEDML_REPORT:
		r = xmlTextWriterStartElement(text_writer, "report");
		break;
	default:
		assert(0); /* N/A */
		break;
	}
	if (r < 0) goto out;
	WRITE_SEDBASE(text_writer, output, r);
	WRITE_ID(text_writer, output, r);
	WRITE_NAME(text_writer, output, r);
	switch (output->output_type) {
	case SEDML_PLOT2D:
		{
			const struct sedml_plot2d *plot2d;
			plot2d = (const struct sedml_plot2d *)output;
			if (plot2d->num_curves == 0) goto out;
			r = xmlTextWriterStartElement(text_writer,
						      "listOfCurves");
			if (r < 0) goto out;
			for (i = 0; i < plot2d->num_curves; i++) {
				assert(plot2d->curves[i]);
				r = write_curve(text_writer,
						plot2d->curves[i], buf);
				if (r < 0) goto out;
			}
			r = xmlTextWriterEndElement(text_writer);
			if (r < 0) goto out;
		}
		break;
	case SEDML_PLOT3D:
		{
			const struct sedml_plot3d *plot3d;
			plot3d = (const struct sedml_plot3d *)output;
			if (plot3d->num_surfaces == 0) goto out;
			r = xmlTextWriterStartElement(text_writer,
						      "listOfSurfaces");
			if (r < 0) goto out;
			for (i = 0; i < plot3d->num_surfaces; i++) {
				assert(plot3d->surfaces[i]);
				r = write_surface(text_writer,
						  plot3d->surfaces[i], buf);
				if (r < 0) goto out;
			}
			r = xmlTextWriterEndElement(text_writer);
			if (r < 0) goto out;
		}
		break;
	case SEDML_REPORT:
		{
			const struct sedml_report *report;
			report = (const struct sedml_report *)output;
			if (report->num_datasets == 0) goto out;
			r = xmlTextWriterStartElement(text_writer,
						      "listOfDatasets");
			if (r < 0) goto out;
			for (i = 0; i < report->num_datasets; i++) {
				assert(report->datasets[i]);
				r = write_dataset(text_writer,
						  report->datasets[i], buf);
				if (r < 0) goto out;
			}
			r = xmlTextWriterEndElement(text_writer);
			if (r < 0) goto out;
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
	r = xmlTextWriterStartDocument(text_writer, NULL, NULL, NULL);
	if (r < 0) goto tidy;

	sedml = doc->sedml;
	if (!sedml) goto tidy;
	r = sprintf(buf, "%s", sedml->xmlns);
	if (r < 0) goto tidy;
	r = xmlTextWriterStartElementNS(text_writer, NULL, "sedML", buf);
	if (r < 0) goto tidy;
	r = sprintf(buf, "%d", sedml->level);
	if (r < 0) goto tidy;
	r = xmlTextWriterWriteAttribute(text_writer, "level", buf);
	if (r < 0) goto tidy;
	r = sprintf(buf, "%d", sedml->version);
	if (r < 0) goto tidy;
	r = xmlTextWriterWriteAttribute(text_writer, "version", buf);

	r = xmlTextWriterStartElement(text_writer, "listOfModels");
	if (r < 0) goto tidy;
	for (i = 0; i < sedml->num_models; i++) {
		assert(sedml->models[i]);
		r = write_model(text_writer, sedml->models[i], buf);
		if (r < 0) goto tidy;
	}
	r = xmlTextWriterEndElement(text_writer);
	if (r < 0) goto tidy;

	r = xmlTextWriterStartElement(text_writer, "listOfSimulations");
	if (r < 0) goto tidy;
	for (i = 0; i < sedml->num_simulations; i++) {
		assert(sedml->simulations[i]);
		r = write_simulation(text_writer, sedml->simulations[i], buf);
		if (r < 0) goto tidy;
	}
	r = xmlTextWriterEndElement(text_writer);
	if (r < 0) goto tidy;

	r = xmlTextWriterStartElement(text_writer, "listOfTasks");
	if (r < 0) goto tidy;
	for (i = 0; i < sedml->num_tasks; i++) {
		assert(sedml->tasks[i]);
		r = write_task(text_writer, sedml->tasks[i], buf);
		if (r < 0) goto tidy;
	}
	r = xmlTextWriterEndElement(text_writer);
	if (r < 0) goto tidy;

	r = xmlTextWriterStartElement(text_writer, "listOfDataGenerators");
	if (r < 0) goto tidy;
	for (i = 0; i < sedml->num_datagenerators; i++) {
		assert(sedml->datagenerators[i]);
		r = write_datagenerator(text_writer, sedml->datagenerators[i], buf);
		if (r < 0) goto tidy;
	}
	r = xmlTextWriterEndElement(text_writer);
	if (r < 0) goto tidy;

	r = xmlTextWriterStartElement(text_writer, "listOfOutputs");
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
