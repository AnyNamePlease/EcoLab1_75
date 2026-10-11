#ifndef __I_ECO_VECTOR_H__
#define __I_ECO_VECTOR_H__

#include "IEcoUnknown.h"

// Структура вектора на выделенной памяти
typedef struct EcoVector {
    double* data;
    size_t capacity;
    size_t size;
} EcoVector;

// Простая плоская матрица (Row-major) для матрицы Якоби N x N
typedef struct EcoMatrix {
    double* data; // Размер N * N
    size_t rows;
    size_t cols;
} EcoMatrix;

#endif /* __I_ECO_VECTOR_H__ */
