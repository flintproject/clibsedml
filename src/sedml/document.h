/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#ifndef SEDML_DOCUMENT_H
#define SEDML_DOCUMENT_H

#include <sedml/common.h>

SEDML_C_DECL_BEGIN

#define SEDML_NAMESPACE "http://sed-ml.org/"
#define SEDML_LEVEL 1
#define SEDML_VERSION 1

#define SEDML_LIST_OF(singular, plural)		\
	int num_ ## plural;			\
	struct sedml_ ## singular **plural

#define SEDML_SEDBASE				\
	char *metaid;				\
	char *notes;				\
	char *annotations

struct sedml_sedbase {
	SEDML_SEDBASE;
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
	SEDML_REMOVE_XML
};

#define SEDML_CHANGE				\
	SEDML_SEDBASE;				\
	char *target;				\
	enum sedml_change_type change_type

struct sedml_change {
	SEDML_CHANGE;
};

struct sedml_computechange {
	SEDML_CHANGE;
	SEDML_LIST_OF(variable, variables);
	SEDML_LIST_OF(parameter, parameters);
	void *math;
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

struct sedml_model {
	SEDML_SEDBASE;
	char *id;
	char *name; /* optional */
	char *language; /* optional */
	char *source;
	SEDML_LIST_OF(change, changes);
};

struct sedml_algorithm {
	SEDML_SEDBASE;
	char *kisaoID;
};

enum sedml_simulation_type {
	SEDML_UNIFORM_TIME_COURSE
};

#define SEDML_SIMULATION				\
	SEDML_SEDBASE;					\
	char *id;					\
	char *name;					\
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

struct sedml_task {
	SEDML_SEDBASE;
	char *id;
	char *name; /* optional */
	char *modelReference;
	char *simulationReference;
};

struct sedml_datagenerator {
	SEDML_SEDBASE;
	char *id;
	char *name; /* optional */
	SEDML_LIST_OF(variable, variables);
	SEDML_LIST_OF(parameter, parameters);
	void *math;
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
	SEDML_LIST_OF(model, models);
	SEDML_LIST_OF(simulation, simulations);
	SEDML_LIST_OF(task, tasks);
	SEDML_LIST_OF(datagenerator, datagenerators);
	SEDML_LIST_OF(output, outputs);
};

struct sedml_document {
	struct sedml_sedml *sedml;
};

SEDML_FUNCTION void sedml_destroy_sedbase(struct sedml_sedbase *);

SEDML_FUNCTION void sedml_destroy_variable(struct sedml_variable *);

SEDML_FUNCTION void sedml_destroy_parameter(struct sedml_parameter *);

SEDML_FUNCTION void sedml_destroy_change(struct sedml_change *);

SEDML_FUNCTION void sedml_destroy_model(struct sedml_model *);

SEDML_FUNCTION void sedml_destroy_simulation(struct sedml_simulation *);

SEDML_FUNCTION void sedml_destroy_algorithm(struct sedml_algorithm *);

SEDML_FUNCTION void sedml_destroy_task(struct sedml_task *);

SEDML_FUNCTION void sedml_destroy_datagenerator(struct sedml_datagenerator *);

SEDML_FUNCTION void sedml_destroy_output(struct sedml_output *);

SEDML_FUNCTION void sedml_destroy_curve(struct sedml_curve *);

SEDML_FUNCTION void sedml_destroy_surface(struct sedml_surface *);

SEDML_FUNCTION void sedml_destroy_dataset(struct sedml_dataset *);

SEDML_FUNCTION void sedml_destroy_sedml(struct sedml_sedml *);

SEDML_FUNCTION struct sedml_document *sedml_create_document(void);

SEDML_FUNCTION void sedml_destroy_document(struct sedml_document *doc);

SEDML_C_DECL_END

#endif
