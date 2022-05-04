#pragma once

#include <stdint.h>

enum ComparingType {
    STRING, INT

};

typedef struct MapField_t {
    uint8_t compare_type;

    void*v0;
    uint64_t v1;

    void*value;

} MapField_t;

typedef struct Map {
    struct MapField_t **fields;
    uint64_t size;

} Map;

MapField_t*createIntIndexField(uint64_t v, void *va);
MapField_t*createStrIndexField(const char* v, void *va);

void map_push(Map *map, MapField_t *field);
MapField_t *map_get(Map *map, MapField_t *field);
