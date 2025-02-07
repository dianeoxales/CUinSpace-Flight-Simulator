#include "defs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Resource functions */

/**
 * Creates a new `Resource` object.
 *
 * Allocates memory for a new `Resource` and initializes its fields.
 * The `name` is dynamically allocated.
 *
 * @param[out] resource      Pointer to the `Resource*` to be allocated and initialized.
 * @param[in]  name          Name of the resource (the string is copied).
 * @param[in]  amount        Initial amount of the resource.
 * @param[in]  max_capacity  Maximum capacity of the resource.
 */
void resource_create(Resource **resource, const char *name, int amount, int max_capacity) {
    // allocate memory for the new resource 
    (*resource) = (Resource*)malloc(sizeof(Resource));
    if((*resource) == NULL) {
        perror("Error: Memory allocation error. Could not initialize for Resource.\n");
        exit(EXIT_FAILURE);
    }
    (*resource)->name = (char*) malloc(strlen(name) + 1); //store the name in the resource 
    
    if ((*resource)->name == NULL) {
        perror("Error: Memory allocation fail for Resource name.\n");
        free(*resource);
        exit(EXIT_FAILURE);
    }
    strcpy((*resource)->name, name);

    (*resource)->amount = amount;
    (*resource)->max_capacity = max_capacity;

    if (sem_init(&(*resource)->mutex, 0, 1) != 0) {
        perror("Error: on sem init.\n");
        exit(EXIT_FAILURE);
    }

}

/**
 * Destroys a `Resource` object.
 *
 * Frees all memory associated with the `Resource`.
 *
 * @param[in,out] resource  Pointer to the `Resource` to be destroyed.
 */
void resource_destroy(Resource *resource) {
    sem_destroy(&resource->mutex);
    free(resource->name);
    free(resource);
    resource = NULL; 
}

/* ResourceAmount functions */

/**
 * Initializes a `ResourceAmount` structure.
 *
 * Associates a `Resource` with a specific `amount`.
 *
 * @param[out] resource_amount  Pointer to the `ResourceAmount` to initialize.
 * @param[in]  resource         Pointer to the `Resource`.
 * @param[in]  amount           The amount associated with the `Resource`.
 */
void resource_amount_init(ResourceAmount *resource_amount, Resource *resource, int amount) {
    resource_amount->resource = resource;
    resource_amount->amount = amount;
}

/**
 * Initializes the `ResourceArray`.
 *
 * Allocates memory for the array of `Resource*` pointers and sets initial values.
 *
 * @param[out] array  Pointer to the `ResourceArray` to initialize.
 */
void resource_array_init(ResourceArray *array) {
    // allocate memory 
    if (array != NULL) {
        array->resources = (Resource**) malloc(sizeof(Resource**) * 1); // dynamically allocate memory for hte resources with an initial capacity of 1
        array->size = 0;
        array->capacity = 1;
    }
}

/**
 * Cleans up the `ResourceArray` by destroying all resources and freeing memory.
 *
 * Iterates through the array, calls `resource_destroy` on each `Resource`,
 * and frees the array memory.
 *
 * @param[in,out] array  Pointer to the `ResourceArray` to clean.
 */
void resource_array_clean(ResourceArray *array) {
    if (array != NULL) {
        for (int i = 0; i < array->size; i++) {
            if(array->resources[i] != NULL) {
                resource_destroy(array->resources[i]);
            }
        }

        free(array->resources);

        //reset values of array
        array->resources = NULL;
        array->size = 0;
        array->capacity = 0;
    }
}

/**
 * Adds a `Resource` to the `ResourceArray`, resizing if necessary (doubling the size).
 *
 * Resizes the array when the capacity is reached and adds the new `Resource`.
 * Use of realloc is NOT permitted.
 * 
 * @param[in,out] array     Pointer to the `ResourceArray`.
 * @param[in]     resource  Pointer to the `Resource` to add.
 */
void resource_array_add(ResourceArray *array, Resource *resource) {
    // check if there is insufficient capacity for a new element and double the size before adding the resource to the array
    if (array->size == array->capacity) {
        // double the capacity
        int new_capacity = (array->capacity) * 2;

        // allocate new resource with the new capacity
        Resource **new_resources = (Resource **) malloc(sizeof(Resource*) * new_capacity);
        if (new_resources == NULL) {
            perror("Failed to allocate memory for resource array");
            exit(EXIT_FAILURE);
        }

        // copy existing elements to the new array
        for (int i = 0; i < array->size; i++) {
            new_resources[i] = array->resources[i];
        }

        free(array->resources);

        //update to point to the new array
        array->resources = new_resources;
        array->capacity = new_capacity;
    }

    // add the new resource to the end of the array
    array->resources[array->size] = resource;
    array->size++;
}