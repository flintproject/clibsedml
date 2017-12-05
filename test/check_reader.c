/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include <assert.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sedml/reader.h"

#define EXTRA 64
#define SEDML_EXAMPLE "/data/sedMLBIOM21.xml"
#define SEDML_XSD "/data/sed-ml-L1-V1.xsd"

int main(void)
{
	size_t len;
	char *buf, *dir;
	struct sedml_document *doc;
	struct sedml_uniformtimecourse *utc;
	struct sedml_model *model;
	struct sedml_change *change;
	struct sedml_changeattribute *ca;
	struct sedml_abstracttask *task;
	struct sedml_datagenerator *datagenerator;
	struct sedml_variable *variable;
	struct sedml_mathml_token *token;
	struct sedml_mathml_node *node, *child_node;
	struct sedml_plot2d *plot2d;
	struct sedml_curve *curve;
	int r;

	len = strlen(__FILE__);
	buf = malloc(len + EXTRA); /* sufficiently large */
	strcpy(buf, __FILE__);
	dir = dirname(buf);
	len = strlen(dir);
	sprintf(buf, "%s/%s", dir, SEDML_EXAMPLE);

	doc = sedml_create_document();
	assert(doc);

	r = sedml_read_file(buf, NULL, doc);
	assert(r == 0);

	assert(doc);
	assert(doc->num_xml_namespaces == 0);
	assert(doc->xml_namespaces == NULL);
	assert(doc->sedml);
	assert(doc->sedml->num_xml_attributes == 0);
	assert(doc->sedml->xml_attributes == NULL);
	assert(doc->sedml->level == 1);
	assert(doc->sedml->version == 1);
	assert(doc->sedml->num_simulations == 1);
	assert(doc->sedml->simulations[0]);
	assert(doc->sedml->simulations[0]->simulation_type == SEDML_UNIFORM_TIME_COURSE);

	utc = (struct sedml_uniformtimecourse *)doc->sedml->simulations[0];
	assert(utc);
	assert(utc->num_xml_attributes == 0);
	assert(utc->xml_attributes == NULL);
	assert(strcmp(utc->id, "simulation1") == 0);
	assert(utc->initialTime == 0.0);
	assert(utc->outputStartTime == 50.0);
	assert(utc->outputEndTime == 1000.0);
	assert(utc->numberOfPoints == 1000.0);
	assert(utc->algorithm);
	assert(strcmp(utc->algorithm->kisaoID, "KISAO:0000027") == 0);

	assert(doc->sedml->num_models == 2);
	model = doc->sedml->models[0];
	assert(model);
	assert(model->num_xml_attributes == 0);
	assert(model->xml_attributes == NULL);
	assert(strcmp(model->id, "model1") == 0);
	assert(strcmp(model->name, "Circadian Oscillations") == 0);
	assert(strcmp(model->language, "SBML") == 0);
	assert(strcmp(model->source, "urn:miriam:biomodels.db:BIOMD0000000021") == 0);
	assert(model->num_changes == 0);
	model = doc->sedml->models[1];
	assert(model);
	assert(model->num_xml_attributes == 0);
	assert(model->xml_attributes == NULL);
	assert(strcmp(model->id, "model2") == 0);
	assert(strcmp(model->name, "Circadian Chaos") == 0);
	assert(strcmp(model->language, "SBML") == 0);
	assert(strcmp(model->source, "model1") == 0);
	assert(model->num_changes == 2);
	change = model->changes[0];
	assert(change);
	assert(change->num_xml_attributes == 0);
	assert(change->xml_attributes == NULL);
	assert(change->change_type == SEDML_CHANGE_ATTRIBUTE);
	ca = (struct sedml_changeattribute *)change;
	assert(strcmp(ca->target, "/sbml:sbml/sbml:model/sbml:listOfParameters/sbml:parameter[@id='V_mT']/@value") == 0);
	assert(strcmp(ca->newValue, "0.28") == 0);
	change = model->changes[1];
	assert(change);
	assert(change->num_xml_attributes == 0);
	assert(change->xml_attributes == NULL);
	assert(change->change_type == SEDML_CHANGE_ATTRIBUTE);
	ca = (struct sedml_changeattribute *)change;
	assert(strcmp(ca->target, "/sbml:sbml/sbml:model/sbml:listOfParameters/sbml:parameter[@id='V_dT']/@value") == 0);
	assert(strcmp(ca->newValue, "4.8") == 0);

	assert(doc->sedml->num_tasks == 2);
	task = doc->sedml->tasks[0];
	assert(task);
	assert(task->num_xml_attributes == 0);
	assert(task->xml_attributes == NULL);
	assert(strcmp(task->id, "task1") == 0);
	assert(strcmp(task->name, "Baseline") == 0);
	assert(task->abstracttask_type == SEDML_TASK);
	assert(strcmp(((const struct sedml_task *)task)->modelReference, "model1") == 0);
	assert(strcmp(((const struct sedml_task *)task)->simulationReference, "simulation1") == 0);
	task = doc->sedml->tasks[1];
	assert(task);
	assert(task->num_xml_attributes == 0);
	assert(task->xml_attributes == NULL);
	assert(strcmp(task->id, "task2") == 0);
	assert(strcmp(task->name, "Modified parameters") == 0);
	assert(task->abstracttask_type == SEDML_TASK);
	assert(strcmp(((const struct sedml_task *)task)->modelReference, "model2") == 0);
	assert(strcmp(((const struct sedml_task *)task)->simulationReference, "simulation1") == 0);

	assert(doc->sedml->num_datagenerators == 3);
	assert(doc->sedml->datagenerators);
	/* 1st datagenerator */
	datagenerator = doc->sedml->datagenerators[0];
	assert(datagenerator);
	assert(datagenerator->num_xml_attributes == 0);
	assert(datagenerator->xml_attributes == NULL);
	assert(strcmp(datagenerator->id, "timeDG") == 0);
	assert(strcmp(datagenerator->name, "Time") == 0);
	assert(datagenerator->num_variables == 1);
	assert(datagenerator->variables);
	variable = datagenerator->variables[0];
	assert(variable);
	assert(variable->num_xml_attributes == 0);
	assert(variable->xml_attributes == NULL);
	assert(strcmp(variable->id, "time") == 0);
	assert(strcmp(variable->name, "time") == 0);
	assert(strcmp(variable->taskReference, "task1") == 0);
	assert(strcmp(variable->symbol, "urn:sedml:symbol:time") == 0);
	assert(variable->target == NULL);
	assert(datagenerator->math);
	assert(!SEDML_MATHML_IS_TOKEN(datagenerator->math));
	node = (struct sedml_mathml_node *)datagenerator->math;
	assert(node->type == SEDML_MATHML_APPLY);
	assert(node->num_children == 2);
	assert(node->children);
	child_node = (struct sedml_mathml_node *)node->children[0];
	assert(child_node);
	assert(!SEDML_MATHML_IS_TOKEN(child_node));
	assert(child_node->type == SEDML_MATHML_PLUS);
	assert(child_node->num_children == 0);
	token = (struct sedml_mathml_token *)node->children[1];
	assert(token);
	assert(SEDML_MATHML_IS_TOKEN(token));
	assert(strcmp(token->body, "time") == 0);
	/* 2nd datagenerator */
	datagenerator = doc->sedml->datagenerators[1];
	assert(datagenerator);
	assert(datagenerator->num_xml_attributes == 0);
	assert(datagenerator->xml_attributes == NULL);
	assert(strcmp(datagenerator->id, "tim1") == 0);
	assert(strcmp(datagenerator->name, "tim mRNA (total)") == 0);
	assert(datagenerator->num_variables == 1);
	assert(datagenerator->variables);
	variable = datagenerator->variables[0];
	assert(variable);
	assert(variable->num_xml_attributes == 0);
	assert(variable->xml_attributes == NULL);
	assert(strcmp(variable->id, "v1") == 0);
	assert(strcmp(variable->taskReference, "task1") == 0);
	assert(strcmp(variable->target, "/sbml:sbml/sbml:model/sbml:listOfSpecies/sbml:species[@id='Mt']") == 0);
	assert(variable->name == NULL);
	assert(variable->symbol == NULL);
	assert(datagenerator->math);
	assert(!SEDML_MATHML_IS_TOKEN(datagenerator->math));
	node = (struct sedml_mathml_node *)datagenerator->math;
	assert(node->type == SEDML_MATHML_APPLY);
	assert(node->num_children == 2);
	assert(node->children);
	child_node = (struct sedml_mathml_node *)node->children[0];
	assert(child_node);
	assert(!SEDML_MATHML_IS_TOKEN(child_node));
	assert(child_node->type == SEDML_MATHML_PLUS);
	assert(child_node->num_children == 0);
	token = (struct sedml_mathml_token *)node->children[1];
	assert(token);
	assert(SEDML_MATHML_IS_TOKEN(token));
	assert(strcmp(token->body, "v1") == 0);
	/* 3rd datagenerator */
	datagenerator = doc->sedml->datagenerators[2];
	assert(datagenerator);
	assert(datagenerator->num_xml_attributes == 0);
	assert(datagenerator->xml_attributes == NULL);
	assert(strcmp(datagenerator->id, "tim2") == 0);
	assert(strcmp(datagenerator->name, "tim mRNA (changed parameters)") == 0);
	assert(datagenerator->num_variables == 1);
	assert(datagenerator->variables);
	variable = datagenerator->variables[0];
	assert(variable);
	assert(variable->num_xml_attributes == 0);
	assert(variable->xml_attributes == NULL);
	assert(strcmp(variable->id, "v2") == 0);
	assert(strcmp(variable->taskReference, "task2") == 0);
	assert(strcmp(variable->target, "/sbml:sbml/sbml:model/sbml:listOfSpecies/sbml:species[@id='Mt']") == 0);
	assert(variable->name == NULL);
	assert(variable->symbol == NULL);
	assert(datagenerator->math);
	assert(!SEDML_MATHML_IS_TOKEN(datagenerator->math));
	node = (struct sedml_mathml_node *)datagenerator->math;
	assert(node->type == SEDML_MATHML_APPLY);
	assert(node->num_children == 2);
	assert(node->children);
	child_node = (struct sedml_mathml_node *)node->children[0];
	assert(child_node);
	assert(!SEDML_MATHML_IS_TOKEN(child_node));
	assert(child_node->type == SEDML_MATHML_PLUS);
	assert(child_node->num_children == 0);
	token = (struct sedml_mathml_token *)node->children[1];
	assert(token);
	assert(SEDML_MATHML_IS_TOKEN(token));
	assert(strcmp(token->body, "v2") == 0);

	assert(doc->sedml->num_outputs == 1);
	assert(doc->sedml->outputs);
	plot2d = (struct sedml_plot2d *)doc->sedml->outputs[0];
	assert(plot2d);
	assert(plot2d->num_xml_attributes == 0);
	assert(plot2d->xml_attributes == NULL);
	assert(strcmp(plot2d->id, "plot1") == 0);
	assert(strcmp(plot2d->name, "tim mRNA with Oscillation and Chaos") == 0);
	assert(plot2d->num_curves == 2);
	assert(plot2d->curves);
	/* 1st curve */
	curve = plot2d->curves[0];
	assert(curve);
	assert(curve->num_xml_attributes == 0);
	assert(curve->xml_attributes == NULL);
	assert(strcmp(curve->id, "c1") == 0);
	assert(curve->logX == 0);
	assert(curve->logY == 0);
	assert(strcmp(curve->xDataReference, "timeDG") == 0);
	assert(strcmp(curve->yDataReference, "tim1") == 0);
	/* 2nd curve */
	curve = plot2d->curves[1];
	assert(curve);
	assert(curve->num_xml_attributes == 0);
	assert(curve->xml_attributes == NULL);
	assert(strcmp(curve->id, "c2") == 0);
	assert(curve->logX == 0);
	assert(curve->logY == 0);
	assert(strcmp(curve->xDataReference, "timeDG") == 0);
	assert(strcmp(curve->yDataReference, "tim2") == 0);

	sedml_destroy_document(doc);
	free(buf);
	return 0;
}
