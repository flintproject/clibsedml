/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include <assert.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sedml/reader.h"

#define EXTRA 64
#define SEDML_EXAMPLE "/data/example2.xml"

int main(void)
{
	size_t len;
	char *buf, *dir;
	struct sedml_document *doc;
	struct sedml_xml_namespace *ns;
	struct sedml_uniformtimecourse *utc;
	struct sedml_xml_attribute *attr;
	struct sedml_model *model;
	struct sedml_task *task;
	struct sedml_datagenerator *datagenerator;
	struct sedml_variable *variable;
	struct sedml_mathml_token *token;
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
	assert(doc->num_xml_namespaces == 1);
	assert(doc->xml_namespaces);
	ns = doc->xml_namespaces[0];
	assert(ns);
	assert(strcmp(ns->uri, "http://example.com/namespace") == 0);
	assert(strcmp(ns->prefix, "ex") == 0);
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
	assert(utc->num_xml_attributes == 1);
	assert(utc->xml_attributes);
	attr = utc->xml_attributes[0];
	assert(attr);
	assert(attr->ns == ns);
	assert(strcmp(attr->local_name, "xyz") == 0);
	assert(strcmp(attr->value, "2") == 0);
	assert(strcmp(utc->id, "sim0") == 0);
	assert(strcmp(utc->name, "Simulation 0") == 0);
	assert(utc->initialTime == 0.0);
	assert(utc->outputStartTime == 0.0);
	assert(utc->outputEndTime == 100.0);
	assert(utc->numberOfPoints == 100.0);
	assert(utc->algorithm);
	assert(strcmp(utc->algorithm->kisaoID, "KISAO:0000030") == 0);

	assert(doc->sedml->num_models == 1);
	model = doc->sedml->models[0];
	assert(model);
	assert(model->num_xml_attributes == 0);
	assert(model->xml_attributes == NULL);
	assert(strcmp(model->id, "model0") == 0);
	assert(strcmp(model->name, "Model 0") == 0);
	assert(strcmp(model->language, "urn:sedml:language:phml") == 0);
	assert(strcmp(model->source, "8/v/8va3fphg") == 0);
	assert(model->num_changes == 0);

	assert(doc->sedml->num_tasks == 1);
	task = doc->sedml->tasks[0];
	assert(task);
	assert(task->num_xml_attributes == 0);
	assert(task->xml_attributes == NULL);
	assert(strcmp(task->id, "task0") == 0);
	assert(strcmp(task->name, "Task 0") == 0);
	assert(strcmp(task->modelReference, "model0") == 0);
	assert(strcmp(task->simulationReference, "sim0") == 0);

	assert(doc->sedml->num_datagenerators == 2);
	assert(doc->sedml->datagenerators);
	/* 1st datagenerator */
	datagenerator = doc->sedml->datagenerators[0];
	assert(datagenerator);
	assert(datagenerator->num_xml_attributes == 0);
	assert(datagenerator->xml_attributes == NULL);
	assert(strcmp(datagenerator->id, "dg0") == 0);
	assert(strcmp(datagenerator->name, "11e7bed7-097d-4351-9ae6-d68e95c4cbd2:iks") == 0);
	assert(datagenerator->num_variables == 1);
	assert(datagenerator->variables);
	variable = datagenerator->variables[0];
	assert(variable);
	assert(variable->num_xml_attributes == 0);
	assert(variable->xml_attributes == NULL);
	assert(strcmp(variable->id, "v0") == 0);
	assert(strcmp(variable->taskReference, "task0") == 0);
	assert(strcmp(variable->target, "11e7bed7-097d-4351-9ae6-d68e95c4cbd2:iks") == 0);
	assert(variable->name == NULL);
	assert(variable->symbol == NULL);
	assert(datagenerator->math);
	assert(SEDML_MATHML_IS_TOKEN(datagenerator->math));
	token = (struct sedml_mathml_token *)datagenerator->math;
	assert(SEDML_MATHML_IS_TOKEN(token));
	assert(strcmp(token->body, "v0") == 0);
	/* 2nd datagenerator */
	datagenerator = doc->sedml->datagenerators[1];
	assert(datagenerator);
	assert(datagenerator->num_xml_attributes == 0);
	assert(datagenerator->xml_attributes == NULL);
	assert(strcmp(datagenerator->id, "dg1") == 0);
	assert(strcmp(datagenerator->name, "11e7bed7-097d-4351-9ae6-d68e95c4cbd2:xs2") == 0);
	assert(datagenerator->num_variables == 1);
	assert(datagenerator->variables);
	variable = datagenerator->variables[0];
	assert(variable);
	assert(variable->num_xml_attributes == 0);
	assert(variable->xml_attributes == NULL);
	assert(strcmp(variable->id, "v1") == 0);
	assert(strcmp(variable->taskReference, "task0") == 0);
	assert(strcmp(variable->target, "11e7bed7-097d-4351-9ae6-d68e95c4cbd2:xs2") == 0);
	assert(variable->name == NULL);
	assert(variable->symbol == NULL);
	assert(datagenerator->math);
	assert(SEDML_MATHML_IS_TOKEN(datagenerator->math));
	token = (struct sedml_mathml_token *)datagenerator->math;
	assert(SEDML_MATHML_IS_TOKEN(token));
	assert(strcmp(token->body, "v1") == 0);

	assert(doc->sedml->num_outputs == 0);
	assert(doc->sedml->outputs == NULL);

	sedml_destroy_document(doc);
	return 0;
}
