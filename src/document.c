/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "sedml/document.h"

#define DESTROY_SEDBASE(x) do {				\
		sedml_destroy_xhtml((x)->notes);	\
		free((x)->metaid);			\
		free(x);				\
	} while (0)

/* API */

void sedml_destroy_sedbase(struct sedml_sedbase *sedbase)
{
	if (!sedbase) return;
	DESTROY_SEDBASE(sedbase);
}

void sedml_destroy_variable(struct sedml_variable *variable)
{
	if (!variable) return;
	free(variable->modelReference);
	free(variable->taskReference);
	free(variable->symbol);
	free(variable->target);
	free(variable->name);
	free(variable->id);
	DESTROY_SEDBASE(variable);
}

void sedml_destroy_parameter(struct sedml_parameter *parameter)
{
	if (!parameter) return;
	free(parameter->name);
	free(parameter->id);
	DESTROY_SEDBASE(parameter);
}

void sedml_destroy_change(struct sedml_change *change)
{
	int i;

	if (!change) return;
	switch (change->change_type) {
	case SEDML_COMPUTE_CHANGE:
		{
			struct sedml_computechange *cc;
			cc = (struct sedml_computechange *)change;
			sedml_destroy_mathml_element(cc->math);
			for (i = 0; i < cc->num_parameters; i++) {
				sedml_destroy_parameter(cc->parameters[i]);
			}
			free(cc->parameters);
			for (i = 0; i < cc->num_variables; i++) {
				sedml_destroy_variable(cc->variables[i]);
			}
			free(cc->variables);
		}
		break;
	case SEDML_CHANGE_ATTRIBUTE:
		{
			struct sedml_changeattribute *ca;
			ca = (struct sedml_changeattribute *)change;
			free(ca->newValue);
		}
		break;
	case SEDML_CHANGE_XML:
		{
			struct sedml_changexml *cx;
			cx = (struct sedml_changexml *)change;
			free(cx->newxml);
		}
		break;
	case SEDML_ADD_XML:
		{
			struct sedml_addxml *ax;
			ax = (struct sedml_addxml *)change;
			free(ax->newxml);
		}
		break;
	case SEDML_REMOVE_XML:
		/* nothing to do */
		break;
	default:
		assert(0); /* N/A */
		break;
	}
	free(change->target);
	DESTROY_SEDBASE(change);
}

void sedml_destroy_model(struct sedml_model *model)
{
	int i;

	if (!model) return;
	for (i = 0; i < model->num_changes; i++) {
		sedml_destroy_change(model->changes[i]);
	}
	free(model->changes);
	free(model->source);
	free(model->language);
	free(model->name);
	free(model->id);
	DESTROY_SEDBASE(model);
}

void sedml_destroy_algorithm(struct sedml_algorithm *algorithm)
{
	if (!algorithm) return;
	free(algorithm->kisaoID);
	DESTROY_SEDBASE(algorithm);
}

void sedml_destroy_simulation(struct sedml_simulation *simulation)
{
	if (!simulation) return;
	switch (simulation->simulation_type) {
	case SEDML_UNIFORM_TIME_COURSE:
		/* nothing to do */
		break;
	default:
		assert(0); /* N/A */
		break;
	}
	sedml_destroy_algorithm(simulation->algorithm);
	free(simulation->name);
	free(simulation->id);
	DESTROY_SEDBASE(simulation);
}

void sedml_destroy_task(struct sedml_task *task)
{
	if (!task) return;
	free(task->simulationReference);
	free(task->modelReference);
	free(task->name);
	free(task->id);
	DESTROY_SEDBASE(task);
}

void sedml_destroy_datagenerator(struct sedml_datagenerator *dg)
{
	int i;

	if (!dg) return;
	sedml_destroy_mathml_element(dg->math);
	for (i = 0; i < dg->num_parameters; i++) {
		sedml_destroy_parameter(dg->parameters[i]);
	}
	free(dg->parameters);
	for (i = 0; i < dg->num_variables; i++) {
		sedml_destroy_variable(dg->variables[i]);
	}
	free(dg->variables);
	free(dg->name);
	free(dg->id);
	DESTROY_SEDBASE(dg);
}


void sedml_destroy_output(struct sedml_output *output)
{
	int i;

	if (!output) return;
	switch (output->output_type) {
	case SEDML_PLOT2D:
		{
			struct sedml_plot2d *plot2d;
			plot2d = (struct sedml_plot2d *)output;
			for (i = 0; i < plot2d->num_curves; i++) {
				sedml_destroy_curve(plot2d->curves[i]);
			}
			free(plot2d->curves);
		}
		break;
	case SEDML_PLOT3D:
		{
			struct sedml_plot3d *plot3d;
			plot3d = (struct sedml_plot3d *)output;
			for (i = 0; i < plot3d->num_surfaces; i++) {
				sedml_destroy_surface(plot3d->surfaces[i]);
			}
			free(plot3d->surfaces);
		}
		break;
	case SEDML_REPORT:
		{
			struct sedml_report *report;
			report = (struct sedml_report *)output;
			for (i = 0; i < report->num_datasets; i++) {
				sedml_destroy_dataset(report->datasets[i]);
			}
			free(report->datasets);
		}
		break;
	default:
		assert(0); /* N/A */
		break;
	}
	free(output->name);
	free(output->id);
	DESTROY_SEDBASE(output);
}

void sedml_destroy_curve(struct sedml_curve *curve)
{
	if (!curve) return;
	free(curve->yDataReference);
	free(curve->xDataReference);
	free(curve->name);
	free(curve->id);
	DESTROY_SEDBASE(curve);
}

void sedml_destroy_surface(struct sedml_surface *surface)
{
	if (!surface) return;
	free(surface->zDataReference);
	free(surface->yDataReference);
	free(surface->xDataReference);
	free(surface->name);
	free(surface->id);
	DESTROY_SEDBASE(surface);
}

void sedml_destroy_dataset(struct sedml_dataset *dataset)
{
	if (!dataset) return;
	free(dataset->dataReference);
	free(dataset->label);
	free(dataset->name);
	free(dataset->id);
	DESTROY_SEDBASE(dataset);
}

void sedml_destroy_sedml(struct sedml_sedml *sedml)
{
	int i;

	if (!sedml) return;
	for (i = 0; i < sedml->num_outputs; i++) {
		sedml_destroy_output(sedml->outputs[i]);
	}
	free(sedml->outputs);
	for (i = 0; i < sedml->num_datagenerators; i++) {
		sedml_destroy_datagenerator(sedml->datagenerators[i]);
	}
	free(sedml->datagenerators);
	for (i = 0; i < sedml->num_tasks; i++) {
		sedml_destroy_task(sedml->tasks[i]);
	}
	free(sedml->tasks);
	for (i = 0; i < sedml->num_simulations; i++) {
		sedml_destroy_simulation(sedml->simulations[i]);
	}
	free(sedml->simulations);
	for (i = 0; i < sedml->num_models; i++) {
		sedml_destroy_model(sedml->models[i]);
	}
	free(sedml->models);
	DESTROY_SEDBASE(sedml);
}

struct sedml_document *sedml_create_document(void)
{
	struct sedml_document *doc;

	doc = calloc(1, sizeof(*doc));
	if (!doc) return NULL;
	return doc;
}

void sedml_destroy_document(struct sedml_document *doc)
{
	if (!doc) return;
	sedml_destroy_sedml(doc->sedml);
	free(doc);
}
