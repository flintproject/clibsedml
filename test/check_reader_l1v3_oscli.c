/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include <assert.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sedml/reader.h"

#define EXTRA 96
#define SEDML_EXAMPLE "../sed-ml/specification/level-1-version-3/examples/oscli-nested-pulse/oscli-nested-pulse.xml"

int main(void)
{
	size_t len;
	char *buf, *dir;
	struct sedml_document *doc;
	struct sedml_onestep *onestep;
	struct sedml_model *model;
	struct sedml_abstracttask *at;
	struct sedml_repeatedtask *rt;
	struct sedml_uniformrange *ur;
	struct sedml_functionalrange *fr;
	struct sedml_setvalue *sv;
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
	assert(doc->sedml);
	assert(doc->sedml->num_xml_attributes == 0);
	assert(doc->sedml->xml_attributes == NULL);
	assert(doc->sedml->level == 1);
	assert(doc->sedml->version == 3);
	assert(doc->sedml->xmlns);
	assert(strcmp(doc->sedml->xmlns, SEDML_NAMESPACE_L1V3) == 0);
	assert(doc->sedml->num_simulations == 1);
	assert(doc->sedml->simulations[0]);
	assert(doc->sedml->simulations[0]->simulation_type == SEDML_ONE_STEP);

	onestep = (struct sedml_onestep *)doc->sedml->simulations[0];
	assert(onestep);
	assert(onestep->num_xml_attributes == 0);
	assert(onestep->xml_attributes == NULL);
	assert(strcmp(onestep->id, "stepper") == 0);
	assert(onestep->name == NULL);
	assert(onestep->algorithm);
	assert(onestep->step == 0.1);
	assert(strcmp(onestep->algorithm->kisaoID, "KISAO:0000019") == 0);

	assert(doc->sedml->num_models == 1);
	model = doc->sedml->models[0];
	assert(model);
	assert(model->num_xml_attributes == 0);
	assert(model->xml_attributes == NULL);
	assert(strcmp(model->id, "model1") == 0);
	assert(model->name == NULL);
	assert(strcmp(model->language, "urn:sedml:language:sbml") == 0);
	assert(strcmp(model->source, "./oscli.xml") == 0);
	assert(model->num_changes == 0);

	assert(doc->sedml->num_tasks == 2);
	at = doc->sedml->tasks[0];
	assert(at);
	assert(at->num_xml_attributes == 0);
	assert(at->xml_attributes == NULL);
	assert(strcmp(at->id, "task0") == 0);
	assert(at->name == NULL);
	assert(at->abstracttask_type == SEDML_TASK);
	assert(strcmp(((const struct sedml_task *)at)->modelReference, "model1") == 0);
	assert(strcmp(((const struct sedml_task *)at)->simulationReference, "stepper") == 0);
	at = doc->sedml->tasks[1];
	assert(at);
	assert(at->num_xml_attributes == 0);
	assert(at->xml_attributes == NULL);
	assert(strcmp(at->id, "task1") == 0);
	assert(at->name == NULL);
	assert(at->abstracttask_type == SEDML_REPEATED_TASK);
	assert(strcmp(((const struct sedml_repeatedtask *)at)->range, "index") == 0);
	assert(((const struct sedml_repeatedtask *)at)->resetModel == 0);
	rt = (struct sedml_repeatedtask *)at;

	assert(rt->num_ranges == 2);
	assert(rt->ranges);
	assert(rt->ranges[0]->range_type == SEDML_UNIFORM_RANGE);
	ur = (struct sedml_uniformrange *)rt->ranges[0];
	assert(strcmp(ur->id, "index") == 0);
	assert(ur->start == 0.0);
	assert(ur->end == 10.0);
	assert(ur->numberOfPoints == 100);
	assert(strcmp(ur->type, "linear") == 0);
	assert(rt->ranges[1]->range_type == SEDML_FUNCTIONAL_RANGE);
	fr = (struct sedml_functionalrange *)rt->ranges[1];
	assert(strcmp(fr->id, "current") == 0);
	assert(strcmp(fr->range, "index") == 0);
	assert(fr->math);

	assert(rt->num_changes == 1);
	assert(rt->changes);
	assert(rt->changes[0]->change_type == SEDML_SET_VALUE);
	sv = (struct sedml_setvalue *)rt->changes[0];
	assert(strcmp(sv->target, "/sbml:sbml/sbml:model/sbml:listOfParameters/sbml:parameter[@id='J0_v0']") == 0);
	assert(strcmp(sv->range, "current") == 0);
	assert(strcmp(sv->modelReference, "model1") == 0);
	assert(sv->math);

	assert(rt->num_subtasks == 1);
	assert(rt->subtasks);
	assert(rt->subtasks[0]->order == 1);
	assert(strcmp(rt->subtasks[0]->task, "task0") == 0);

	sedml_destroy_document(doc);
	free(buf);
	return 0;
}
