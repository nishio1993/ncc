#include "ncc.h"

Vector *newVector(void) {
    Vector *v = malloc(sizeof(Vector));
    v->data = malloc(sizeof(void*) * 8);
    v->capacity = 8;
    v->length = 0;
    return v;
}

void vectorPush(Vector *vector, void *element) {
    if (vector->length == vector->capacity) {
        vector->capacity *= 2;
        vector->data = realloc(vector->data, sizeof(void*) * vector->capacity);
    }
    vector->data[vector->length++] = element;
    return;
}

Map *newMap(void) {
    Map *map = malloc(sizeof(Map));
    map->key = newVector();
    map->value = newVector();
    return map;
}

void mapSet(Map *map, char *key, void *value) {
    vectorPush(map->key, key);
    vectorPush(map->value, value);
    return;
}

void *mapGet(Map *map, const char *key) {
    int i;
    for (i = map->key->length - 1; i >= 0; i--) {
        if (strcmp(map->key->data[i], key) == 0) {
            return map->value->data[i];
        }
    }
    return NULL;
}

Variable *getVariable(char *name, int length) {
    int i;
    for (i = 0; variableVector->data[i]; i++) {
        Variable *variable = variableVector->data[i];
        if (strncmp(variable->name, name, length) == 0) {
            return variable;
        }
    }
    return NULL;
}