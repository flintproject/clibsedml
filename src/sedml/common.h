/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#ifndef SEDML_COMMON_H
#define SEDML_COMMON_H

#ifdef __cplusplus
#define SEDML_C_DECL_BEGIN extern "C" {
#define SEDML_C_DECL_END }
#else
#define SEDML_C_DECL_BEGIN
#define SEDML_C_DECL_END
#endif

#if defined(_WIN32) && !defined(__MINGW32__) && !defined(SEDML_STATIC)
#ifdef SEDML_DLLEXPORT
#define SEDML_FUNCTION __declspec(dllexport)
#else
#define SEDML_FUNCTION __declspec(dllimport)
#endif
#else
#define SEDML_FUNCTION
#endif

#define SEDML_NAMESPACE "http://sed-ml.org/"
#define SEDML_NAMESPACE_L1 SEDML_NAMESPACE "sed-ml/level1"
#define SEDML_NAMESPACE_L1V2 SEDML_NAMESPACE_L1 "/version2"
#define SEDML_NAMESPACE_L1V3 SEDML_NAMESPACE_L1 "/version3"

#endif
