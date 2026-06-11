//// @file tab_generic.h
/// @brief Déclaration des macros pour les tableaux dynamiques génériques

#ifndef TAB_GENERIC_H
#define TAB_GENERIC_H


/// Macro pour définir un tableau dynamique générique
/// @param type Type des éléments du tableau
#define Array(type)     \
struct Array_##type {   \
    type* values;       \
    size_t size;        \
    size_t capacity;    \
}

/// Macro pour ajouter un élément à un tableau dynamique générique
/// @param array Tableau dynamique
/// @param element Élément à ajouter
#define array_push(array, element) do {\
    if (array.capacity == array.size) {\
        if (array.capacity == 0) {\
            array.capacity = 2;\
        }\
        else {\
            array.capacity *= 2;\
        }\
        array.values = realloc(array.values, array.capacity * sizeof(*array.values)); \
        if (array.values == NULL) {\
            exit(1);\
        }\
    }\
    array.values[array.size] = element;\
    array.size++;\
} while(0)\

/// Macro pour libérer la mémoire d'un tableau dynamique générique
/// @param array Tableau dynamique à libérer
#define array_free(array) do {\
    if (array.values == NULL) {\
        exit(1);\
    }\
    free(array.values);\
    array.values = NULL;\
    array.capacity = 0;\
    array.size = 0;\
} while(0)\

#endif