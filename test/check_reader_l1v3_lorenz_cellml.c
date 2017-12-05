/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include <assert.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sedml/reader.h"

#define EXTRA 96
#define SEDML_EXAMPLE "../sed-ml/specification/level-1-version-3/examples/lorenz-cellml/lorenz.xml"

int main(void)
{
	size_t len;
	char *buf, *dir;
	struct sedml_document *doc;
	struct sedml_uniformtimecourse *utc;
	struct sedml_model *model;
	struct sedml_abstracttask *task;
	struct sedml_algorithmparameter *ap;
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
	assert(doc->sedml->version == 2);
	assert(doc->sedml->xmlns);
	assert(strcmp(doc->sedml->xmlns, SEDML_NAMESPACE_L1V2) == 0);
	assert(doc->sedml->num_simulations == 1);
	assert(doc->sedml->simulations[0]);
	assert(doc->sedml->simulations[0]->simulation_type == SEDML_UNIFORM_TIME_COURSE);

	utc = (struct sedml_uniformtimecourse *)doc->sedml->simulations[0];
	assert(utc);
	assert(utc->num_xml_attributes == 0);
	assert(utc->xml_attributes == NULL);
	assert(strcmp(utc->id, "simulation1") == 0);
	assert(utc->name == NULL);
	assert(utc->initialTime == 0.0);
	assert(utc->outputStartTime == 0.0);
	assert(utc->outputEndTime == 50.0);
	assert(utc->numberOfPoints == 10000.0);
	assert(utc->algorithm);
	assert(strcmp(utc->algorithm->kisaoID, "KISAO:0000019") == 0);
	assert(utc->algorithm->num_algorithmparameters == 11);
	ap = utc->algorithm->algorithmparameters[0];
	assert(strcmp(ap->kisaoID, "KISAO:0000211") == 0);
	assert(strcmp(ap->value, "1e-07") == 0);
	ap = utc->algorithm->algorithmparameters[10];
	assert(strcmp(ap->kisaoID, "KISAO:0000479") == 0);
	assert(strcmp(ap->value, "0") == 0);

	assert(doc->sedml->num_models == 1);
	model = doc->sedml->models[0];
	assert(model);
	assert(model->num_xml_attributes == 0);
	assert(model->xml_attributes == NULL);
	assert(strcmp(model->id, "model") == 0);
	assert(model->name == NULL);
	assert(strcmp(model->language, "urn:sedml:language:cellml.1_0") == 0);
	assert(strcmp(model->source, "lorenz-model.cellml") == 0);
	assert(model->num_changes == 0);

	assert(doc->sedml->num_tasks == 2);
	task = doc->sedml->tasks[0];
	assert(task);
	assert(task->num_xml_attributes == 0);
	assert(task->xml_attributes == NULL);
	assert(strcmp(task->id, "repeatedTask") == 0);
	assert(task->name == NULL);
	assert(task->abstracttask_type == SEDML_REPEATED_TASK);
	assert(strcmp(((struct sedml_repeatedtask *)task)->range, "once") == 0);
	assert(((struct sedml_repeatedtask *)task)->resetModel == 1);
	assert(task->name == NULL);
	task = doc->sedml->tasks[1];
	assert(task);
	assert(task->num_xml_attributes == 0);
	assert(task->xml_attributes == NULL);
	assert(strcmp(task->id, "task1") == 0);
	assert(task->name == NULL);
	assert(task->abstracttask_type == SEDML_TASK);
	assert(strcmp(((const struct sedml_task *)task)->modelReference, "model") == 0);
	assert(strcmp(((const struct sedml_task *)task)->simulationReference, "simulation1") == 0);

	sedml_destroy_document(doc);
	free(buf);
	return 0;
}
