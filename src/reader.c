/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "sedml/reader.h"

static int fetch_attribute(struct sedml_reader *reader,
			   const xmlChar *name, char **value)
{
	xmlTextReaderPtr text_reader;
	const xmlChar *v;
	char *val;
	size_t len;
	int r;

	text_reader = reader->text_reader;
	r = xmlTextReaderMoveToAttribute(text_reader, name);
	if (r <= 0) goto out;
	v = xmlTextReaderConstValue(text_reader);
	if (!v) {
		r = -1;
		goto out;
	}
	len = strlen((const char *)v);
	val = malloc(len+1);
	if (!val) {
		r = -1;
		goto out;
	}
	memcpy(val, v, len);
	val[len] = '\0';
	*value = val;
 out:
	return r;
}

#define FETCH_ATTR(reader, x, attr) fetch_attribute(reader, #attr, &(x)->attr)

#define FETCH_ID(reader, x) FETCH_ATTR(reader, x, id)

#define FETCH_NAME(reader, x) FETCH_ATTR(reader, x, name)

static int fetch_attribute_as_double(struct sedml_reader *reader,
				     const xmlChar *name, double *value)
{
	xmlTextReaderPtr text_reader;
	const xmlChar *v;
	char *e;
	double d;
	int r;

	text_reader = reader->text_reader;
	r = xmlTextReaderMoveToAttribute(text_reader, name);
	if (r <= 0) goto out;
	v = xmlTextReaderConstValue(text_reader);
	if (!v) {
		r = -1;
		goto out;
	}
	d = strtod((const char *)v, &e);
	if ((const char *)v == e) {
		r = -1;
		goto out;
	}
	*value = d;
 out:
	return r;
}

#define FETCH_ATTR_DOUBLE(reader, x, attr) \
	fetch_attribute_as_double(reader, #attr, &(x)->attr)

static int fetch_attribute_as_int(struct sedml_reader *reader,
				  const xmlChar *name, int *value)
{
	xmlTextReaderPtr text_reader;
	const xmlChar *v;
	char *e;
	int i, r;

	text_reader = reader->text_reader;
	r = xmlTextReaderMoveToAttribute(text_reader, name);
	if (r <= 0) goto out;
	v = xmlTextReaderConstValue(text_reader);
	if (!v) {
		r = -1;
		goto out;
	}
	*value = atoi((const char *)v); /* TODO: handle errors */
 out:
	return r;
}

#define FETCH_ATTR_INT(reader, x, attr) \
	fetch_attribute_as_int(reader, #attr, &(x)->attr)

static int fetch_attribute_as_bool(struct sedml_reader *reader,
				   const xmlChar *name, int *value)
{
	xmlTextReaderPtr text_reader;
	const xmlChar *v;
	char *e;
	int i, r;

	text_reader = reader->text_reader;
	r = xmlTextReaderMoveToAttribute(text_reader, name);
	if (r <= 0) goto out;
	v = xmlTextReaderConstValue(text_reader);
	if (!v) {
		r = -1;
		goto out;
	}
	if (xmlStrEqual(v, "true")) {
		*value = 1;
	} else if (xmlStrEqual(v, "false")) {
		*value = 0;
	} else {
		r = -1;
	}
 out:
	return r;
}

#define FETCH_ATTR_BOOL(reader, x, attr) \
	fetch_attribute_as_bool(reader, #attr, &(x)->attr)

static int check_attribute_value(struct sedml_reader *reader,
				 const xmlChar *name, const xmlChar *value)
{
	xmlTextReaderPtr text_reader;
	const xmlChar *v;
	int r;

	text_reader = reader->text_reader;
	r = xmlTextReaderMoveToAttribute(text_reader, name);
	if (r < 0) {
		goto out;
	}
	if (r == 0) {
		r = -1;
		goto out;
	}
	v = xmlTextReaderConstValue(text_reader);
	if (!v) {
		r = -1;
		goto out;
	}
	if (!xmlStrEqual(v, value)) {
		r = -1;
		goto out;
	}
 out:
	return r;
}

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
	s = sizeof(simulation) * sedml->num_simulations;
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

static int fetch_sedbase_attributes(struct sedml_reader *reader, struct sedml_sedbase *sedbase)
{
	assert(reader);
	return FETCH_ATTR(reader, sedbase, metaid);
}

#define FETCH_SEDBASE_ATTRIBUTES(reader, x) \
	fetch_sedbase_attributes(reader, (struct sedml_sedbase *)x)

static int read_sedml(struct sedml_reader *reader, struct sedml_document *doc)
{
	struct sedml_sedml *sedml;
	int r, level, version;

	assert(reader);
	sedml = calloc(1, sizeof(*sedml));
	if (!sedml) {
		r = -1;
		goto fail;
	}
	r = FETCH_SEDBASE_ATTRIBUTES(reader, sedml);
	if (r < 0) goto fail;
	r = check_attribute_value(reader, "xmlns", SEDML_NAMESPACE);
	if (r < 0) goto fail;
	r = check_attribute_value(reader, "level", "1");
	if (r < 0) goto fail;
	r = check_attribute_value(reader, "version", "1");
	if (r < 0) goto fail;

	sedml->xmlns = SEDML_NAMESPACE;
	sedml->level = 1;
	sedml->version = 1;
	doc->sedml = sedml;
	return 0;

 fail:
	sedml_destroy_sedml(sedml);
	return r;
}

static int read_notes(struct sedml_reader *reader, struct sedml_document *doc)
{
	int r = 0;
	/* TODO */
	return r;
}

static int read_uniformtimecourse(struct sedml_reader *reader, struct sedml_document *doc)
{
	struct sedml_uniformtimecourse *utc;
	int r;

	utc = calloc(1, sizeof(*utc));
	if (!utc) {
		r = -1;
		goto fail;
	}
	r = FETCH_SEDBASE_ATTRIBUTES(reader, utc);
	if (r < 0) goto fail;
	r = FETCH_ID(reader, utc);
	if (r < 0) goto fail;
	r = FETCH_NAME(reader, utc);
	if (r < 0) goto fail;
	utc->simulation_type = SEDML_UNIFORM_TIME_COURSE;
	r = FETCH_ATTR_DOUBLE(reader, utc, initialTime);
	if (r < 0) goto fail;
	r = FETCH_ATTR_DOUBLE(reader, utc, outputStartTime);
	if (r < 0) goto fail;
	r = FETCH_ATTR_DOUBLE(reader, utc, outputEndTime);
	if (r < 0) goto fail;
	r = FETCH_ATTR_INT(reader, utc, numberOfPoints);
	if (r < 0) goto fail;
	r = add_simulation(doc->sedml, (struct sedml_simulation *)utc);
	if (r < 0) goto fail;
	reader->simulation = (struct sedml_simulation *)utc;
	return r;

 fail:
	sedml_destroy_simulation((struct sedml_simulation *)utc);
	return r;
}

static int end_simulation(struct sedml_reader *reader, struct sedml_document *doc)
{
	reader->simulation = NULL;
	return 0;
}

static int read_algorithm(struct sedml_reader *reader, struct sedml_document *doc)
{
	struct sedml_algorithm *algorithm;
	int r = 0;

	algorithm = calloc(1, sizeof(*algorithm));
	if (!algorithm) {
		r = -1;
		goto fail;
	}
	r = FETCH_SEDBASE_ATTRIBUTES(reader, algorithm);
	if (r < 0) goto fail;
	r = FETCH_ATTR(reader, algorithm, kisaoID);
	if (r < 0) goto fail;
	if (!reader->simulation) {
		r = -1;
		goto fail;
	}
	if (reader->simulation->algorithm) {
		r = -1;
		goto fail;
	}
	reader->simulation->algorithm = algorithm;
	return r;

 fail:
	sedml_destroy_algorithm(algorithm);
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
	s = sizeof(model) * sedml->num_models;
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

static int read_model(struct sedml_reader *reader, struct sedml_document *doc)
{
	struct sedml_model *model;
	int r = 0;

	model = calloc(1, sizeof(*model));
	if (!model) {
		r = -1;
		goto fail;
	}
	r = FETCH_SEDBASE_ATTRIBUTES(reader, model);
	if (r < 0) goto fail;
	r = FETCH_ID(reader, model);
	if (r < 0) goto fail;
	r = FETCH_NAME(reader, model);
	if (r < 0) goto fail;
	r = FETCH_ATTR(reader, model, language);
	if (r < 0) goto fail;
	r = FETCH_ATTR(reader, model, source);
	if (r < 0) goto fail;
	r = add_model(doc->sedml, model);
	if (r < 0) goto fail;
	reader->model = model;
	return r;

 fail:
	sedml_destroy_model(model);
	return r;
}

static int end_model(struct sedml_reader *reader, struct sedml_document *doc)
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
	s = sizeof(change) * model->num_changes;
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

static int read_changeattribute(struct sedml_reader *reader,
				struct sedml_document *doc)
{
	struct sedml_changeattribute *ca;
	int r = 0;

	ca = calloc(1, sizeof(*ca));
	if (!ca) {
		r = -1;
		goto fail;
	}
	r = FETCH_SEDBASE_ATTRIBUTES(reader, ca);
	if (r < 0) goto fail;
	ca->change_type = SEDML_CHANGE_ATTRIBUTE;
	r = fetch_attribute(reader, "target", &ca->target);
	if (r < 0) goto fail;
	r = fetch_attribute(reader, "newvalue", &ca->newvalue);
	if (r < 0) goto fail;
	r = add_change(reader->model, (struct sedml_change *)ca);
	if (r < 0) goto fail;
	reader->change = (struct sedml_change *)ca;
	return r;

 fail:
	sedml_destroy_change((struct sedml_change *)ca);
	return r;
}

static int read_computechange(struct sedml_reader *reader,
			      struct sedml_document *doc)
{
	struct sedml_computechange *cc;
	int r = 0;

	cc = calloc(1, sizeof(*cc));
	if (!cc) {
		r = -1;
		goto fail;
	}
	r = FETCH_SEDBASE_ATTRIBUTES(reader, cc);
	if (r < 0) goto fail;
	cc->change_type = SEDML_COMPUTE_CHANGE;
	r = fetch_attribute(reader, "target", &cc->target);
	if (r < 0) goto fail;
	r = add_change(reader->model, (struct sedml_change *)cc);
	if (r < 0) goto fail;
	reader->change = (struct sedml_change *)cc;
	return r;

 fail:
	sedml_destroy_change((struct sedml_change *)cc);
	return r;
}


static int end_change(struct sedml_reader *reader, struct sedml_document *doc)
{
	reader->change = NULL;
	return 0;
}

static int add_task(struct sedml_sedml *sedml, struct sedml_task *task)
{
	int r, i;

	if (!sedml || !task) {
		r = -1;
		goto out;
	}
	i = sedml->num_tasks++;
	sedml->tasks = realloc(sedml->tasks, sizeof(task) * sedml->num_tasks);
	if (!sedml->tasks) {
		r = -1;
		goto out;
	}
	sedml->tasks[i] = task;
	r = 0;
 out:
	return r;
}

static int read_task(struct sedml_reader *reader, struct sedml_document *doc)
{
	struct sedml_task *task;
	int r = 0;

	task = calloc(1, sizeof(*task));
	if (!task) {
		r = -1;
		goto fail;
	}
	r = FETCH_SEDBASE_ATTRIBUTES(reader, task);
	if (r < 0) goto fail;
	r = FETCH_ID(reader, task);
	if (r < 0) goto fail;
	r = FETCH_NAME(reader, task);
	if (r < 0) goto fail;
	r = FETCH_ATTR(reader, task, modelReference);
	if (r < 0) goto fail;
	r = FETCH_ATTR(reader, task, simulationReference);
	if (r < 0) goto fail;
	r = add_task(doc->sedml, task);
	if (r < 0) goto fail;
	reader->task = task;
	return r;

 fail:
	sedml_destroy_task(task);
	return r;
}

static int end_task(struct sedml_reader *reader, struct sedml_document *doc)
{
	reader->task = NULL;
	return 0;
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
	s = sizeof(datagenerator) * sedml->num_datagenerators;
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

static int read_datagenerator(struct sedml_reader *reader, struct sedml_document *doc)
{
	struct sedml_datagenerator *datagenerator;
	int r = 0;

	datagenerator = calloc(1, sizeof(*datagenerator));
	if (!datagenerator) {
		r = -1;
		goto fail;
	}
	r = FETCH_SEDBASE_ATTRIBUTES(reader, datagenerator);
	if (r < 0) goto fail;
	r = FETCH_ID(reader, datagenerator);
	if (r < 0) goto fail;
	r = FETCH_NAME(reader, datagenerator);
	if (r < 0) goto fail;
	r = add_datagenerator(doc->sedml, datagenerator);
	if (r < 0) goto fail;
	reader->datagenerator = datagenerator;
	return r;

 fail:
	sedml_destroy_datagenerator(datagenerator);
	return r;
}

static int end_datagenerator(struct sedml_reader *reader,
			     struct sedml_document *doc)
{
	reader->datagenerator = NULL;
	return 0;
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
	s = sizeof(variable) * dg->num_variables;
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
	s = sizeof(variable) * cc->num_variables;
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

static int read_variable(struct sedml_reader *reader, struct sedml_document *doc)
{
	struct sedml_variable *variable;
	int r = 0;

	variable = calloc(1, sizeof(*variable));
	if (!variable) {
		r = -1;
		goto fail;
	}
	r = FETCH_SEDBASE_ATTRIBUTES(reader, variable);
	if (r < 0) goto fail;
	r = FETCH_ID(reader, variable);
	if (r < 0) goto fail;
	r = FETCH_NAME(reader, variable);
	if (r < 0) goto fail;
	r = FETCH_ATTR(reader, variable, target);
	if (r < 0) goto fail;
	r = FETCH_ATTR(reader, variable, symbol);
	if (r < 0) goto fail;
	r = FETCH_ATTR(reader, variable, taskReference);
	if (r < 0) goto fail;
	r = FETCH_ATTR(reader, variable, modelReference);
	if (r < 0) goto fail;
	if (reader->datagenerator) {
		r = add_variable_to_datagenerator(reader->datagenerator,
						  variable);
	} else if (reader->change) {
		struct sedml_computechange *cc;
		cc = (struct sedml_computechange *)reader->change;
		r = add_variable_to_computechange(cc, variable);
	} else {
		r = -1;
	}
	if (r < 0) goto fail;
	reader->variable = variable;
	return r;

 fail:
	sedml_destroy_variable(variable);
	return r;
}

static int end_variable(struct sedml_reader *reader, struct sedml_document *doc)
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
	s = sizeof(parameter) * dg->num_parameters;
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
	s = sizeof(parameter) * cc->num_parameters;
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

static int read_parameter(struct sedml_reader *reader, struct sedml_document *doc)
{
	struct sedml_parameter *parameter;
	int r = 0;

	parameter = calloc(1, sizeof(*parameter));
	if (!parameter) {
		r = -1;
		goto fail;
	}
	r = FETCH_SEDBASE_ATTRIBUTES(reader, parameter);
	if (r < 0) goto fail;
	r = FETCH_ID(reader, parameter);
	if (r < 0) goto fail;
	r = FETCH_NAME(reader, parameter);
	if (r < 0) goto fail;
	r = FETCH_ATTR_DOUBLE(reader, parameter, value);
	if (r < 0) goto fail;
	if (reader->datagenerator) {
		r = add_parameter_to_datagenerator(reader->datagenerator,
						   parameter);
	} else if (reader->change) {
		struct sedml_computechange *cc;
		cc = (struct sedml_computechange *)reader->change;
		r = add_parameter_to_computechange(cc, parameter);
	} else {
		r = -1;
	}
	if (r < 0) goto fail;
	reader->parameter = parameter;
	return r;

 fail:
	sedml_destroy_parameter(parameter);
	return r;
}

static int end_parameter(struct sedml_reader *reader, struct sedml_document *doc)
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
	s = sizeof(output) * sedml->num_outputs;
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

static int read_plot2d(struct sedml_reader *reader, struct sedml_document *doc)
{
	struct sedml_plot2d *plot2d;
	int r = 0;

	plot2d = calloc(1, sizeof(*plot2d));
	if (!plot2d) {
		r = -1;
		goto fail;
	}
	r = FETCH_SEDBASE_ATTRIBUTES(reader, plot2d);
	if (r < 0) goto fail;
	r = FETCH_ID(reader, plot2d);
	if (r < 0) goto fail;
	r = FETCH_NAME(reader, plot2d);
	if (r < 0) goto fail;
	plot2d->output_type = SEDML_PLOT2D;
	r = add_output(doc->sedml, (struct sedml_output *)plot2d);
	if (r < 0) goto fail;
	reader->output = (struct sedml_output *)plot2d;
	return r;

 fail:
	sedml_destroy_output((struct sedml_output *)plot2d);
	return r;
}

static int end_output(struct sedml_reader *reader, struct sedml_document *doc)
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
	s = sizeof(curve) * plot2d->num_curves;
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

static int read_curve(struct sedml_reader *reader, struct sedml_document *doc)
{
	struct sedml_curve *curve;
	int r = 0;

	curve = calloc(1, sizeof(*curve));
	if (!curve) {
		r = -1;
		goto fail;
	}
	r = FETCH_SEDBASE_ATTRIBUTES(reader, curve);
	if (r < 0) goto fail;
	r = FETCH_ATTR_BOOL(reader, curve, logX);
	if (r < 0) goto fail;
	r = FETCH_ATTR(reader, curve, xDataReference);
	if (r < 0) goto fail;
	r = FETCH_ATTR_BOOL(reader, curve, logY);
	if (r < 0) goto fail;
	r = FETCH_ATTR(reader, curve, yDataReference);
	if (r < 0) goto fail;
	r = add_curve((struct sedml_plot2d *)reader->output, curve);
	if (r < 0) goto fail;
	reader->curve = curve;
	return r;

 fail:
	sedml_destroy_curve(curve);
	return r;
}

static int end_curve(struct sedml_reader *reader, struct sedml_document *doc)
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
	s = sizeof(surface) * plot3d->num_surfaces;
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

static int read_surface(struct sedml_reader *reader, struct sedml_document *doc)
{
	struct sedml_surface *surface;
	int r = 0;

	surface = calloc(1, sizeof(*surface));
	if (!surface) {
		r = -1;
		goto fail;
	}
	r = FETCH_SEDBASE_ATTRIBUTES(reader, surface);
	if (r < 0) goto fail;
	r = FETCH_ATTR_BOOL(reader, surface, logX);
	if (r < 0) goto fail;
	r = FETCH_ATTR(reader, surface, xDataReference);
	if (r < 0) goto fail;
	r = FETCH_ATTR_BOOL(reader, surface, logY);
	if (r < 0) goto fail;
	r = FETCH_ATTR(reader, surface, yDataReference);
	if (r < 0) goto fail;
	r = FETCH_ATTR_BOOL(reader, surface, logZ);
	if (r < 0) goto fail;
	r = FETCH_ATTR(reader, surface, zDataReference);
	if (r < 0) goto fail;
	r = add_surface((struct sedml_plot3d *)reader->output, surface);
	if (r < 0) goto fail;
	reader->surface = surface;
	return r;

 fail:
	sedml_destroy_surface(surface);
	return r;
}

static int end_surface(struct sedml_reader *reader, struct sedml_document *doc)
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
	s = sizeof(dataset) * report->num_datasets;
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

static int read_dataset(struct sedml_reader *reader, struct sedml_document *doc)
{
	struct sedml_dataset *dataset;
	int r = 0;

	dataset = calloc(1, sizeof(*dataset));
	if (!dataset) {
		r = -1;
		goto fail;
	}
	r = FETCH_SEDBASE_ATTRIBUTES(reader, dataset);
	if (r < 0) goto fail;
	r = FETCH_ATTR(reader, dataset, label);
	if (r < 0) goto fail;
	r = FETCH_NAME(reader, dataset);
	if (r < 0) goto fail;
	r = FETCH_ATTR(reader, dataset, dataReference);
	if (r < 0) goto fail;
	r = add_dataset((struct sedml_report *)reader->output, dataset);
	if (r < 0) goto fail;
	reader->dataset = dataset;
	return r;

 fail:
	sedml_destroy_dataset(dataset);
	return r;
}

static int end_dataset(struct sedml_reader *reader, struct sedml_document *doc)
{
	reader->dataset = NULL;
	return 0;
}

typedef int (*read_function)(struct sedml_reader *, struct sedml_document *);
typedef int (*end_function)(struct sedml_reader *, struct sedml_document *);

#define READ_NOP ((read_function)NULL)
#define END_NOP ((end_function)NULL)

struct sedml_element {
	const xmlChar *name;
	read_function read;
	end_function end;
} sedml_elements[] = {
	{"sedML", read_sedml, END_NOP,},
	{"notes", read_notes, END_NOP,},
	{"listOfSimulations", READ_NOP, END_NOP,},
	{"uniformTimeCourse", read_uniformtimecourse, end_simulation,},
	{"algorithm", read_algorithm, END_NOP,},
	{"listOfModels", READ_NOP, END_NOP,},
	{"model", read_model, end_model,},
	{"listOfChanges", READ_NOP, END_NOP,},
	{"changeAttribute", read_changeattribute, end_change,},
	{"computeChange", read_computechange, end_change,},
	{"listOfTasks", READ_NOP, END_NOP,},
	{"task", read_task, end_task,},
	{"listOfDatasets", READ_NOP, END_NOP,},
	{"listOfDataGenerators", READ_NOP, END_NOP,},
	{"dataGenerator", read_datagenerator, end_datagenerator,},
	{"listOfVariables", READ_NOP, END_NOP,},
	{"listOfParameters", READ_NOP, END_NOP,},
	{"variable", read_variable, end_variable,},
	{"listOfOutputs", READ_NOP, END_NOP,},
	{"plot2D", read_plot2d, end_output,},
	{"listOfCurves", READ_NOP, END_NOP,},
	{"curve", read_curve, end_curve,},
	{"listOfSurfaces", READ_NOP, END_NOP,},
	{"surface", read_surface, end_surface,},
	{"listOfDatasets", READ_NOP, END_NOP,},
	{"dataset", read_dataset, end_dataset,}
};

#define num_sedml_elements (sizeof(sedml_elements)/sizeof(sedml_elements[0]))

static int cmpse(const void *x, const void *y)
{
	const struct sedml_element *e1 = x;
	const struct sedml_element *e2 = y;
	return xmlStrcmp(e1->name, e2->name);
}

/* API */

struct sedml_reader *sedml_create_reader(const char *path)
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

int sedml_reader_set_xsd(struct sedml_reader *reader, const char *xsd)
{
	if (!reader) return -1;
	return xmlTextReaderSchemaValidate(reader->text_reader, xsd);
}

int sedml_reader_read(struct sedml_reader *reader, struct sedml_document *doc)
{
	xmlTextReaderPtr text_reader;
	int i, r = 0, type, mode;

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
		mode = xmlTextReaderReadState(text_reader);
		switch (mode) {
		case XML_TEXTREADER_MODE_EOF:
			goto out;
			break;
		case XML_TEXTREADER_MODE_INTERACTIVE:
			break;
		case XML_TEXTREADER_MODE_READING:
			break;
		default:
			r = -3-mode;
			goto out;
			break;
		}
		type = xmlTextReaderNodeType(text_reader);
		switch (type) { /* enum xmlReaderTypes */
		case XML_READER_TYPE_ELEMENT:
			{
				const xmlChar *name, *local_name;
				name = xmlTextReaderConstName(text_reader);
				local_name = xmlTextReaderConstLocalName(text_reader);
				if (xmlStrEqual(name, local_name)) {
					struct sedml_element se, *found;
					se.name = name;
					found = bsearch(&se, sedml_elements,
							num_sedml_elements,
							sizeof(sedml_elements[0]),
							cmpse);
					if (found) {
						if (found->read) {
							r = found->read(reader, doc);
							if (r < 0) goto out;
						}
					}
				}
			}
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
			{
				const xmlChar *name, *local_name;
				name = xmlTextReaderConstName(text_reader);
				local_name = xmlTextReaderConstLocalName(text_reader);
				if (xmlStrEqual(name, local_name)) {
					struct sedml_element se, *found;
					se.name = name;
					found = bsearch(&se, sedml_elements,
							num_sedml_elements,
							sizeof(sedml_elements[0]),
							cmpse);
					if (found) {
						if (found->end) {
							r = found->end(reader, doc);
							if (r < 0) goto out;
						}
					}
				}
			}
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

void sedml_destroy_reader(struct sedml_reader *reader)
{
	if (!reader) return;
	xmlFreeTextReader(reader->text_reader);
	free(reader);
}
