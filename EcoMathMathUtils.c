#include "EcoMathMathUtils.h"

#define LN2 0.693147180559945309417
#define EXP_MAX_ARG 709.0
#define EXP_MIN_ARG -709.0

double eco_fabs(double x) {
    return (x < 0.0) ? -x : x;
}

double eco_max(double a, double b) {
    return (a > b) ? a : b;
}

/* Реализация exp(x) через сведе́ние аргумента e^x = 2^k * e^r и ряд Тейлора */
double eco_exp(double x) {
    if (x > EXP_MAX_ARG) {
        /* Защита от переполнения double (насыщение) */
        x = EXP_MAX_ARG;
    }
    if (x < EXP_MIN_ARG) {
        /* Защита от потери точности снизу */
        return 0.0;
    }

    /* Приведение аргумента: x = k * ln(2) + r, где r in [-ln(2)/2, ln(2)/2] */
    int k = (int)(x / LN2 + (x >= 0.0 ? 0.5 : -0.5));
    double r = x - k * LN2;

    /* Вычисление e^r через ряд Тейлора: sum_{n=0}^{12} r^n / n! */
    double term = 1.0;
    double sum = 1.0;
    for (int i = 1; i <= 12; ++i) {
        term *= r / i;
        sum += term;
    }

    /* Вычисление 2^k * sum */
    if (k > 0) {
        for (int i = 0; i < k; ++i) sum *= 2.0;
    } else if (k < 0) {
        for (int i = 0; i < -k; ++i) sum /= 2.0;
    }

    return sum;
}