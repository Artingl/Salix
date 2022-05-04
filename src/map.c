#include <map.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


MapField_t*createIntIndexField(uint64_t v, void *va)
{
    MapField_t* field = (MapField_t*)malloc(sizeof(MapField_t));

    field->compare_type = INT;
    field->v1 = v;
    field->value = va;

    return field;
}

MapField_t*createStrIndexField(const char* v, void *va)
{
    MapField_t* field = (MapField_t*)malloc(sizeof(MapField_t));

    field->compare_type = STRING;
    field->v0 = (char*)v;
    field->value = va;

    return field;
}

void map_push(Map *map, MapField_t *field)
{
    if (map->fields == NULL) {
        map->fields = calloc(1, sizeof(MapField_t));
        map->fields[0] = field;
        map->size = 1;
        return;
    }

    MapField_t **previous = map->fields;
    
    map->size++;
    map->fields = calloc(map->size, sizeof(MapField_t));

    for (int i = 0; i < map->size - 1; ++i) {
        map->fields[i] = previous[i];
    }

    map->fields[map->size - 1] = field;
    free((void*)previous);
}

MapField_t *map_get(Map* map, MapField_t *field)
{
    if (map->fields == NULL) {
        return NULL;
    }

    for (int i = 0; i < map->size; i++) {
        if (map->fields[i] == 0 || map->fields[i] == NULL) {
            continue;
        }

        MapField_t *compare_field = map->fields[i];

        if (compare_field->value != NULL)

        if (field->compare_type == INT) {
            if (field->v1 == compare_field->v1) {
                return compare_field;
            }
        }
        else if (field->compare_type == STRING) {
            if (strcmp((const char*)field->v0, (const char*)compare_field->v0) == 0) {
                return compare_field;
            }
        }
    }

    return NULL;
}
