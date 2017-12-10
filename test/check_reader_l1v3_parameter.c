/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include <assert.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sedml/reader.h"

#define EXTRA 96
#define SEDML_EXAMPLE "../sed-ml/specification/level-1-version-3/examples/parameter-scan-2d/parameter-scan-2d.xml"

int main(void)
{
	size_t len;
	char *buf, *dir;
	struct sedml_document *doc;
	struct sedml_steadystate *steadystate;
	struct sedml_model *model;
	struct sedml_abstracttask *at;
	struct sedml_repeatedtask *rt;
	struct sedml_uniformrange *ur;
	struct sedml_vectorrange *vr;
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
	assert(doc->sedml->simulations[0]->simulation_type == SEDML_STEADY_STATE);

	steadystate = (struct sedml_steadystate *)doc->sedml->simulations[0];
	assert(steadystate);
	assert(steadystate->num_xml_attributes == 0);
	assert(steadystate->xml_attributes == NULL);
	assert(strcmp(steadystate->id, "steady1") == 0);
	assert(steadystate->name == NULL);
	assert(steadystate->algorithm);
	assert(strcmp(steadystate->algorithm->kisaoID, "KISAO:0000282") == 0);

	assert(doc->sedml->num_models == 1);
	model = doc->sedml->models[0];
	assert(model);
	assert(model->num_xml_attributes == 0);
	assert(model->xml_attributes == NULL);
	assert(strcmp(model->id, "model1") == 0);
	assert(model->name == NULL);
	assert(strcmp(model->language, "urn:sedml:language:sbml") == 0);
	assert(strcmp(model->source, "BorisEJB.xml") == 0);
	assert(model->num_changes == 0);

	assert(doc->sedml->num_tasks == 3);
	at = doc->sedml->tasks[0];
	assert(at);
	assert(at->num_xml_attributes == 0);
	assert(at->xml_attributes == NULL);
	assert(strcmp(at->id, "task0") == 0);
	assert(at->name == NULL);
	assert(at->abstracttask_type == SEDML_TASK);
	assert(strcmp(((const struct sedml_task *)at)->modelReference, "model1") == 0);
	assert(strcmp(((const struct sedml_task *)at)->simulationReference, "steady1") == 0);

	at = doc->sedml->tasks[1];
	assert(at);
	assert(at->num_xml_attributes == 0);
	assert(at->xml_attributes == NULL);
	assert(strcmp(at->id, "task1") == 0);
	assert(at->name == NULL);
	assert(at->abstracttask_type == SEDML_REPEATED_TASK);
	assert(strcmp(((const struct sedml_repeatedtask *)at)->range, "current") == 0);
	assert(((const struct sedml_repeatedtask *)at)->resetModel == 0);

	rt = (struct sedml_repeatedtask *)at;
	assert(rt->num_ranges == 1);
	assert(rt->ranges);
	assert(rt->ranges[0]->range_type == SEDML_VECTOR_RANGE);
	vr = (struct sedml_vectorrange *)rt->ranges[0];
	assert(strcmp(vr->id, "current") == 0);
	assert(vr->num_values == 9);
	assert(vr->values);
	assert(vr->values[0] == 1.0);
	assert(vr->values[8] == 100.0);

	assert(rt->num_changes == 1);
	assert(rt->changes);
	assert(rt->changes[0]->change_type == SEDML_SET_VALUE);
	sv = (struct sedml_setvalue *)rt->changes[0];
	assert(strcmp(sv->target, "/sbml:sbml/sbml:model/sbml:listOfParameters/sbml:parameter[@id='J1_KK2']") == 0);
	assert(strcmp(sv->range, "current") == 0);
	assert(strcmp(sv->modelReference, "model1") == 0);
	assert(sv->math);

	assert(rt->num_subtasks == 1);
	assert(rt->subtasks);
	assert(rt->subtasks[0]->order == 1);
	assert(strcmp(rt->subtasks[0]->task, "task2") == 0);

	at = doc->sedml->tasks[2];
	assert(at);
	assert(at->num_xml_attributes == 0);
	assert(at->xml_attributes == NULL);
	assert(strcmp(at->id, "task2") == 0);
	assert(at->name == NULL);
	assert(at->abstracttask_type == SEDML_REPEATED_TASK);
	assert(strcmp(((const struct sedml_repeatedtask *)at)->range, "current1") == 0);
	assert(((const struct sedml_repeatedtask *)at)->resetModel == 0);

	rt = (struct sedml_repeatedtask *)at;
	assert(rt->num_ranges == 1);
	assert(rt->ranges);
	assert(rt->ranges[0]->range_type == SEDML_UNIFORM_RANGE);
	ur = (struct sedml_uniformrange *)rt->ranges[0];
	assert(strcmp(ur->id, "current1") == 0);
	assert(ur->start == 1.0);
	assert(ur->end == 40.0);
	assert(ur->numberOfPoints == 100);
	assert(strcmp(ur->type, "linear") == 0);

	assert(rt->num_changes == 1);
	assert(rt->changes);
	assert(rt->changes[0]->change_type == SEDML_SET_VALUE);
	sv = (struct sedml_setvalue *)rt->changes[0];
	assert(strcmp(sv->target, "/sbml:sbml/sbml:model/sbml:listOfParameters/sbml:parameter[@id='J4_KK5']") == 0);
	assert(strcmp(sv->range, "current1") == 0);
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
