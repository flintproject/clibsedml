/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#include <assert.h>
#include <string.h>

#include "sedml/document.h"

int main(void)
{
	struct sedml_document *doc;
	struct sedml_sedml *sedml;

	doc = sedml_create_document();
	assert(doc);
	assert(doc->num_xml_namespaces == 0);

	sedml_document_add_namespace(doc, "http://example.com/namespace", "ex");
	assert(doc->num_xml_namespaces == 1);
	assert(doc->xml_namespaces[0]);
	assert(strcmp(doc->xml_namespaces[0]->uri, "http://example.com/namespace") == 0);
	assert(strcmp(doc->xml_namespaces[0]->prefix, "ex") == 0);

	sedml = sedml_create_sedml(1, 1);
	assert(sedml);
	assert(sedml->num_xml_attributes == 0);
	assert(sedml->version == 1);
	assert(sedml->level == 1);
	assert(strcmp(sedml->xmlns, SEDML_NAMESPACE) == 0);

	sedml_sedbase_add_xml_attribute((struct sedml_sedbase *)sedml,
					doc->xml_namespaces[0],
					"foo",
					"bar");
	assert(sedml->num_xml_attributes == 1);
	assert(sedml->xml_attributes[0]);
	assert(sedml->xml_attributes[0]->ns == doc->xml_namespaces[0]);
	assert(strcmp(sedml->xml_attributes[0]->local_name, "foo") == 0);
	assert(strcmp(sedml->xml_attributes[0]->value, "bar") == 0);

	doc->sedml = sedml;

	sedml_destroy_document(doc);
	return 0;
}
