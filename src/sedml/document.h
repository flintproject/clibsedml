/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#ifndef SEDML_DOCUMENT_H
#define SEDML_DOCUMENT_H

#include <sedml/common.h>
#include <sedml/mathml.h>
#include <sedml/xhtml.h>

SEDML_C_DECL_BEGIN

struct sedml_xml_namespace {
	char *uri;
	char *prefix;
};

struct sedml_xml_attribute {
	const struct sedml_xml_namespace *ns;
	char *local_name;
	char *value;
};

#define SEDML_LIST_OF(singular, plural)		\
	int num_ ## plural;			\
	struct sedml_ ## singular **plural

#define SEDML_SEDBASE				\
	char *metaid;				\
	struct sedml_xhtml *notes;		\
	char *annotations;			\
	SEDML_LIST_OF(xml_attribute, xml_attributes)

struct sedml_sedbase {
	SEDML_SEDBASE;
};

/*
 * @since SED-ML L1V3
 */
struct sedml_dimensiondescription {
	SEDML_SEDBASE;
};

/*
 * @since SED-ML L1V3
 */
struct sedml_slice {
	SEDML_SEDBASE;
	char *reference;
	char *value;
};

/*
 * @since SED-ML L1V3
 */
struct sedml_datasource {
	SEDML_SEDBASE;
	char *id;
	char *name; /* optional */
	char *indexSet; /* optional */
	SEDML_LIST_OF(slice, slices);
};

/*
 * @since SED-ML L1V3
 */
struct sedml_datadescription {
	SEDML_SEDBASE;
	char *id;
	char *name; /* optional */
	char *format; /* optional */
	char *source;
	struct sedml_dimensiondescription *dimensionDescription; /* optional */
	SEDML_LIST_OF(datasource, datasources); /* optional */
};

struct sedml_variable {
	SEDML_SEDBASE;
	char *id;
	char *name; /* optional */
	char *target;
	char *symbol;
	char *taskReference;
	char *modelReference;
};

struct sedml_parameter {
	SEDML_SEDBASE;
	char *id;
	char *name; /* optional */
	double value;
};

enum sedml_change_type {
	SEDML_COMPUTE_CHANGE,
	SEDML_CHANGE_ATTRIBUTE,
	SEDML_CHANGE_XML,
	SEDML_ADD_XML,
	SEDML_REMOVE_XML,
	SEDML_SET_VALUE /* @since SED-ML L1V2 */
};

#define SEDML_CHANGE				\
	SEDML_SEDBASE;				\
	char *target;				\
	enum sedml_change_type change_type

struct sedml_change {
	SEDML_CHANGE;
};

#define SEDML_COMPUTECHANGE					\
	SEDML_CHANGE;						\
	SEDML_LIST_OF(variable, variables); /* optional */	\
	SEDML_LIST_OF(parameter, parameters); /* optional */	\
	struct sedml_mathml_element *math

struct sedml_computechange {
	SEDML_COMPUTECHANGE;
};

struct sedml_changeattribute {
	SEDML_CHANGE;
	char *newValue;
};

struct sedml_changexml {
	SEDML_CHANGE;
	void *newxml;
};

struct sedml_addxml {
	SEDML_CHANGE;
	void *newxml;
};

struct sedml_removexml {
	SEDML_CHANGE;
};

/*
 * @since SED-ML L1V2
 */
struct sedml_setvalue {
	SEDML_COMPUTECHANGE;
	char *modelReference;
	char *range; /* optional */
	char *symbol; /* optional */
};

struct sedml_model {
	SEDML_SEDBASE;
	char *id;
	char *name; /* optional */
	char *language; /* optional */
	char *source;
	SEDML_LIST_OF(change, changes); /* optional */
};

/*
 * @since SED-ML L1V2
 */
struct sedml_algorithmparameter {
	SEDML_SEDBASE;
	char *kisaoID;
	char *value;
};

struct sedml_algorithm {
	SEDML_SEDBASE;
	char *kisaoID;
	SEDML_LIST_OF(algorithmparameter, algorithmparameters); /* optional, @since SED-ML L1V2 */
};

enum sedml_simulation_type {
	SEDML_UNIFORM_TIME_COURSE,
	SEDML_ONE_STEP,    /* @since SED-ML L1V2 */
	SEDML_STEADY_STATE /* @since SED-ML L1V2 */
};

#define SEDML_SIMULATION				\
	SEDML_SEDBASE;					\
	char *id;					\
	char *name; /* optional */			\
	struct sedml_algorithm *algorithm;		\
	enum sedml_simulation_type simulation_type

struct sedml_simulation {
	SEDML_SIMULATION;
};

struct sedml_uniformtimecourse {
	SEDML_SIMULATION;
	double initialTime;
	double outputStartTime;
	double outputEndTime;
	int numberOfPoints;
};

/*
 * @since SED-ML L1V2
 */
struct sedml_onestep {
	SEDML_SIMULATION;
	double step;
};

/*
 * @since SED-ML L1V2
 */
struct sedml_steadystate {
	SEDML_SIMULATION;
};

enum sedml_abstracttask_type {
	SEDML_TASK,
	SEDML_REPEATED_TASK /* @since SED-ML L1V2 */
};

#define SEDML_ABSTRACTTASK				\
	SEDML_SEDBASE;					\
	char *id;					\
	char *name;					\
	enum sedml_abstracttask_type abstracttask_type

/*
 * @since SED-ML L1V2
 */
struct sedml_abstracttask {
	SEDML_ABSTRACTTASK;
};

struct sedml_task {
	SEDML_ABSTRACTTASK;
	char *modelReference;
	char *simulationReference;
};

/*
 * @since SED-ML L1V2
 */
enum sedml_range_type {
	SEDML_UNIFORM_RANGE,
	SEDML_VECTOR_RANGE,
	SEDML_FUNCTIONAL_RANGE
};

#define SEDML_RANGE				\
	SEDML_SEDBASE;				\
	char *id;				\
	enum sedml_range_type range_type

/*
 * @since SED-ML L1V2
 */
struct sedml_range {
	SEDML_RANGE;
};

/*
 * @since SED-ML L1V2
 */
struct sedml_uniformrange {
	SEDML_RANGE;
	double start;
	double end;
	int numberOfPoints;
	char *type;
};

/*
 * @since SED-ML L1V2
 */
struct sedml_vectorrange {
	SEDML_RANGE;
	int num_values;
	double *values;
};

/*
 * @since SED-ML L1V2
 */
struct sedml_functionalrange {
	SEDML_RANGE;
	char *range; /* optional */
	SEDML_LIST_OF(variable, variables);
	SEDML_LIST_OF(parameter, parameters);
	struct sedml_mathml_element *math;
};

/*
 * @since SED-ML L1V2
 */
struct sedml_subtask {
	SEDML_SEDBASE;
	char *task;
	int order;
};

/*
 * @since SED-ML L1V2
 */
struct sedml_repeatedtask {
	SEDML_ABSTRACTTASK;
	char *range;
	int resetModel; /* boolean */
	SEDML_LIST_OF(change, changes);
	SEDML_LIST_OF(range, ranges);
	SEDML_LIST_OF(subtask, subtasks);
};

struct sedml_datagenerator {
	SEDML_SEDBASE;
	char *id;
	char *name; /* optional */
	SEDML_LIST_OF(variable, variables);
	SEDML_LIST_OF(parameter, parameters);
	struct sedml_mathml_element *math;
};

enum sedml_output_type {
	SEDML_PLOT2D,
	SEDML_PLOT3D,
	SEDML_REPORT
};

#define SEDML_OUTPUT				\
	SEDML_SEDBASE;				\
	char *id;				\
	char *name;				\
	enum sedml_output_type output_type

struct sedml_output {
	SEDML_OUTPUT;
};

#define SEDML_CURVE				\
	SEDML_SEDBASE;				\
	char *id;				\
	char *name;				\
	int logX; /* boolean */			\
	char *xDataReference;			\
	int logY; /* boolean */			\
	char *yDataReference

struct sedml_curve {
	SEDML_CURVE;
};

struct sedml_plot2d {
	SEDML_OUTPUT;
	SEDML_LIST_OF(curve, curves);
};

struct sedml_surface {
	SEDML_CURVE;
	int logZ;
	char *zDataReference;
};

struct sedml_plot3d {
	SEDML_OUTPUT;
	SEDML_LIST_OF(surface, surfaces);
};

struct sedml_dataset {
	SEDML_SEDBASE;
	char *id;
	char *name; /* optional */
	char *label;
	char *dataReference;
};

struct sedml_report {
	SEDML_OUTPUT;
	SEDML_LIST_OF(dataset, datasets);
};

struct sedml_sedml {
	SEDML_SEDBASE;
	int level;
	int version;
	const char *xmlns;
	SEDML_LIST_OF(datadescription, datadescriptions); /* @since SED-ML L1V3 */
	SEDML_LIST_OF(model, models);
	SEDML_LIST_OF(simulation, simulations);
	SEDML_LIST_OF(abstracttask, tasks);
	SEDML_LIST_OF(datagenerator, datagenerators);
	SEDML_LIST_OF(output, outputs);
};

struct sedml_document {
	SEDML_LIST_OF(xml_namespace, xml_namespaces);
	struct sedml_sedml *sedml;
};

SEDML_FUNCTION void sedml_sedbase_add_xml_attribute(struct sedml_sedbase *,
						    const struct sedml_xml_namespace *,
						    const char *,
						    const char *);

SEDML_FUNCTION void sedml_destroy_xml_attribute(struct sedml_xml_attribute *);

SEDML_FUNCTION void sedml_destroy_sedbase(struct sedml_sedbase *);

SEDML_FUNCTION void sedml_destroy_dimensiondescription(struct sedml_dimensiondescription *);

SEDML_FUNCTION void sedml_destroy_slice(struct sedml_slice *);

SEDML_FUNCTION void sedml_destroy_datasource(struct sedml_datasource *);

SEDML_FUNCTION void sedml_destroy_datadescription(struct sedml_datadescription *);

SEDML_FUNCTION void sedml_destroy_variable(struct sedml_variable *);

SEDML_FUNCTION void sedml_destroy_parameter(struct sedml_parameter *);

SEDML_FUNCTION void sedml_destroy_change(struct sedml_change *);

SEDML_FUNCTION void sedml_destroy_model(struct sedml_model *);

SEDML_FUNCTION void sedml_destroy_simulation(struct sedml_simulation *);

SEDML_FUNCTION void sedml_destroy_algorithm(struct sedml_algorithm *);

SEDML_FUNCTION void sedml_destroy_algorithmparameter(struct sedml_algorithmparameter *);

SEDML_FUNCTION void sedml_destroy_abstracttask(struct sedml_abstracttask *);

SEDML_FUNCTION void sedml_destroy_range(struct sedml_range *);

SEDML_FUNCTION void sedml_destroy_subtask(struct sedml_subtask *);

SEDML_FUNCTION void sedml_destroy_datagenerator(struct sedml_datagenerator *);

SEDML_FUNCTION void sedml_destroy_output(struct sedml_output *);

SEDML_FUNCTION void sedml_destroy_curve(struct sedml_curve *);

SEDML_FUNCTION void sedml_destroy_surface(struct sedml_surface *);

SEDML_FUNCTION void sedml_destroy_dataset(struct sedml_dataset *);

SEDML_FUNCTION struct sedml_sedml *sedml_create_sedml(int, int);

SEDML_FUNCTION void sedml_destroy_sedml(struct sedml_sedml *);

SEDML_FUNCTION struct sedml_document *sedml_create_document(void);

SEDML_FUNCTION void sedml_document_add_namespace(struct sedml_document *,
						 const char *,
						 const char *);

SEDML_FUNCTION int sedml_document_compare(const struct sedml_document *,
					  const struct sedml_document *);

SEDML_FUNCTION void sedml_destroy_document(struct sedml_document *doc);

SEDML_C_DECL_END

#endif
