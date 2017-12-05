/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include <assert.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sedml/reader.h"

#define EXTRA 64
#define SEDML_EXAMPLE "/data/example1.xml"

int main(void)
{
	size_t len;
	char *buf, *dir;
	struct sedml_document *doc;
	struct sedml_xml_namespace *ns;
	struct sedml_uniformtimecourse *utc;
	struct sedml_xml_attribute *attr;
	struct sedml_model *model;
	struct sedml_abstracttask *task;
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
	assert(doc->sedml);
	assert(doc->num_xml_namespaces == 1);
	assert(doc->xml_namespaces);
	ns = doc->xml_namespaces[0];
	assert(ns);
	assert(strcmp(ns->uri, "http://example.com/namespace") == 0);
	assert(strcmp(ns->prefix, "ex") == 0);
	assert(doc->sedml->num_xml_attributes == 0);
	assert(doc->sedml->xml_attributes == NULL);
	assert(doc->sedml->level == 1);
	assert(doc->sedml->version == 1);
	assert(doc->sedml->num_simulations == 1);
	assert(doc->sedml->simulations[0]);
	assert(doc->sedml->simulations[0]->simulation_type == SEDML_UNIFORM_TIME_COURSE);

	utc = (struct sedml_uniformtimecourse *)doc->sedml->simulations[0];
	assert(utc);
	assert(utc->num_xml_attributes == 1);
	assert(utc->xml_attributes);
	attr = utc->xml_attributes[0];
	assert(attr);
	assert(attr->ns == ns);
	assert(strcmp(attr->local_name, "xyz") == 0);
	assert(strcmp(attr->value, "1") == 0);
	assert(strcmp(utc->id, "sim0") == 0);
	assert(strcmp(utc->name, "Simulation 0") == 0);
	assert(utc->initialTime == 0.0);
	assert(utc->outputStartTime == 0.0);
	assert(utc->outputEndTime == 100.0);
	assert(utc->numberOfPoints == 100);
	assert(utc->algorithm);
	assert(strcmp(utc->algorithm->kisaoID, "KISAO:0000032") == 0);

	assert(doc->sedml->num_models == 1);
	model = doc->sedml->models[0];
	assert(model);
	assert(model->num_xml_attributes == 0);
	assert(model->xml_attributes == NULL);
	assert(strcmp(model->id, "model0") == 0);
	assert(strcmp(model->name, "Model 0") == 0);
	assert(strcmp(model->language, "urn:sedml:language:phml") == 0);
	assert(strcmp(model->source, "m/g/mg3ya0w7") == 0);
	assert(model->num_changes == 0);

	assert(doc->sedml->num_tasks == 1);
	task = doc->sedml->tasks[0];
	assert(task);
	assert(task->num_xml_attributes == 0);
	assert(task->xml_attributes == NULL);
	assert(strcmp(task->id, "task0") == 0);
	assert(strcmp(task->name, "Task 0") == 0);
	assert(task->abstracttask_type == SEDML_TASK);
	assert(strcmp(((const struct sedml_task *)task)->modelReference, "model0") == 0);
	assert(strcmp(((const struct sedml_task *)task)->simulationReference, "sim0") == 0);

	assert(doc->sedml->num_datagenerators == 0);
	assert(doc->sedml->datagenerators == NULL);

	assert(doc->sedml->num_outputs == 0);
	assert(doc->sedml->outputs == NULL);

	sedml_destroy_document(doc);
	free(buf);
	return 0;
}
