#ifndef __FETCH__H
#define __FETCH__H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>

typedef struct {
    char *name;
    char *git_url;
    char *raw_path;
    char **src_paths;
    size_t src_count;
    char **header_paths;
    size_t header_count;
    int has_headers;
    int is_prebuilt;
    char *description;
    char *author;
    char *license;
    char **keywords;
    size_t keyword_count;
    char *added_by;
} LibraryInfo;


LibraryInfo *parse_library_json(const char *json_data);
#endif