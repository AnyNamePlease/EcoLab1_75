#ifndef __ECO_MATH_UTILS_H__
#define __ECO_MATH_UTILS_H__

#include <stddef.h>

/* Собственные аналоги функций math.h для работы без libc */
double eco_fabs(double x);
double eco_max(double a, double b);
double eco_exp(double x);

#endif /* __ECO_MATH_UTILS_H__ */
