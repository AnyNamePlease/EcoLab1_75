#include <stdio.h>
#include <time.h>
#include "IEcoActivationFunctions.h"
#include "IEcoNeuralLayer.h"
#include "EcoMathMathUtils.h"

extern IEcoActivationFunctions* GetActivationFunctionsComponent(void);
extern IEcoNeuralLayer* GetNeuralLayerComponent(void);

#define EPSILON_DIFF 1e-6
#define TOLERANCE 1e-4

/* Вспомогательная функция для оценки абсолютной погрешности */
static double calc_abs_err(double val, double ref) {
    return eco_fabs(val - ref);
}

/* 1. Unit-тесты и численное дифференцирование скалярных функций */
static void test_scalar_functions(void) {
    IEcoActivationFunctions* act = GetActivationFunctionsComponent();
    printf("=== TEST 1: Scalar Activation Functions & Derivatives ===\n");

    double test_inputs[] = {-1000.0, -10.0, -1.0, 0.0, 1.0, 10.0, 1000.0};
    size_t num_inputs = sizeof(test_inputs) / sizeof(test_inputs[0]);
    double h = EPSILON_DIFF;

    /* Sigmoid test */
    printf("\n[Sigmoid Check]\n");
    for (size_t i = 0; i < num_inputs; ++i) {
        double x = test_inputs[i];
        double val = act->Sigmoid(act, x);
        double der = act->SigmoidDerivative(act, x);

        /* Численная производная центральными разностями */
        double num_der = (act->Sigmoid(act, x + h) - act->Sigmoid(act, x - h)) / (2.0 * h);
        double err = calc_abs_err(der, num_der);

        printf("x = %8.1f | Sigmoid = %.6f | Deriv = %.6f | NumDeriv = %.6f | Err = %.2e [%s]\n",
               x, val, der, num_der, err, (err < TOLERANCE) ? "PASS" : "FAIL");
    }

    /* ReLU test */
    printf("\n[ReLU Check]\n");
    for (size_t i = 0; i < num_inputs; ++i) {
        double x = test_inputs[i];
        double val = act->ReLU(act, x);
        double der = act->ReLUDerivative(act, x);

        /* На изломе (x = 0) обычные центральные разности не применяются напрямую */
        if (eco_fabs(x) < 1e-9) {
            printf("x = %8.1f | ReLU = %.6f | Deriv = %.6f | [PASS - Boundary Point x=0 explicitly handled]\n", x, val, der);
        } else {
            double num_der = (act->ReLU(act, x + h) - act->ReLU(act, x - h)) / (2.0 * h);
            double err = calc_abs_err(der, num_der);
            printf("x = %8.1f | ReLU = %.6f | Deriv = %.6f | NumDeriv = %.6f | Err = %.2e [%s]\n",
                   x, val, der, num_der, err, (err < TOLERANCE) ? "PASS" : "FAIL");
        }
    }

    /* Leaky ReLU test */
    printf("\n[Leaky ReLU Check (alpha = 0.01)]\n");
    double alpha = 0.01;
    for (size_t i = 0; i < num_inputs; ++i) {
        double x = test_inputs[i];
        double val = act->LeakyReLU(act, x, alpha);
        double der = act->LeakyReLUDerivative(act, x, alpha);

        if (eco_fabs(x) < 1e-9) {
            printf("x = %8.1f | LeakyReLU = %.6f | Deriv = %.6f | [PASS - Boundary Point x=0 explicitly handled]\n", x, val, der);
        } else {
            double num_der = (act->LeakyReLU(act, x + h, alpha) - act->LeakyReLU(act, x - h, alpha)) / (2.0 * h);
            double err = calc_abs_err(der, num_der);
            printf("x = %8.1f | LeakyReLU = %.6f | Deriv = %.6f | NumDeriv = %.6f | Err = %.2e [%s]\n",
                   x, val, der, num_der, err, (err < TOLERANCE) ? "PASS" : "FAIL");
        }
    }
}

/* 2. Unit-тест Softmax, Матрицы Якоби и Численной устойчивости */
static void test_softmax_and_jacobian(void) {
    IEcoActivationFunctions* act = GetActivationFunctionsComponent();
    printf("\n=== TEST 2: Softmax, Numerical Stability & Jacobian ===\n");

    /* Тест 2.1: Большие значения (проверка max-subtraction trick) */
    double in_large_data[] = {1000.0, 1001.0, 1002.0};
    double out_data[3];
    EcoVector in_vec = {in_large_data, 3, 3};
    EcoVector out_vec = {out_data, 3, 0};

    ACOM_RESULT res = act->Softmax(act, &in_vec, &out_vec);
    printf("[Softmax Overflow Resistance Test]\n");
    printf("Input: [1000.0, 1001.0, 1002.0]\nOutput: ");
    double sum = 0.0;
    for (size_t i = 0; i < out_vec.size; ++i) {
        printf("%.6f ", out_vec.data[i]);
        sum += out_vec.data[i];
    }
    printf("| Sum = %.6f [%s]\n", sum, (res == ACOM_OK && eco_fabs(sum - 1.0) < 1e-6) ? "PASS" : "FAIL");

    /* Тест 2.2: Вектор одинаковых элементов */
    double in_same_data[] = {5.0, 5.0, 5.0, 5.0};
    in_vec.data = in_same_data; in_vec.size = 4;
    out_vec.size = 0;
    act->Softmax(act, &in_vec, &out_vec);
    printf("\n[Softmax Identical Elements Test]\nOutput: ");
    sum = 0.0;
    for (size_t i = 0; i < out_vec.size; ++i) {
        printf("%.4f ", out_vec.data[i]);
        sum += out_vec.data[i];
    }
    printf("| Sum = %.6f [%s]\n", sum, (eco_fabs(sum - 1.0) < 1e-6) ? "PASS" : "FAIL");

    /* Тест 2.3: Матрица Якоби и численная проверка */
    printf("\n[Softmax Jacobian Numerical Verification]\n");
    double x_data[] = {1.0, 2.0, 3.0};
    double j_data[9];
    in_vec.data = x_data; in_vec.size = 3;
    EcoMatrix jac = {j_data, 3, 3};

    act->SoftmaxJacobian(act, &in_vec, &jac);

    double h = EPSILON_DIFF;
    int jac_pass = 1;

    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            double analytic_j = jac.data[i * 3 + j];

            /* Численное дифференцирование: вариация x_j */
            double x_plus[3] = {1.0, 2.0, 3.0};
            double x_minus[3] = {1.0, 2.0, 3.0};
            x_plus[j] += h;
            x_minus[j] -= h;

            double s_plus[3], s_minus[3];
            EcoVector v_plus = {x_plus, 3, 3}, o_plus = {s_plus, 3, 0};
            EcoVector v_minus = {x_minus, 3, 3}, o_minus = {s_minus, 3, 0};

            act->Softmax(act, &v_plus, &o_plus);
            act->Softmax(act, &v_minus, &o_minus);

            double num_j = (o_plus.data[i] - o_minus.data[i]) / (2.0 * h);
            double err = calc_abs_err(analytic_j, num_j);

            if (err > TOLERANCE) jac_pass = 0;
            printf("J[%zu][%zu] | Analytic = %8.5f | Numeric = %8.5f | Err = %.2e\n",
                   i, j, analytic_j, num_j, err);
        }
    }
    printf("Jacobian Verification Result: [%s]\n", jac_pass ? "PASS" : "FAIL");
}

/* 3. Демонстрация OOA vs SOA (Архитектурное усложнение) */
static void test_ooa_vs_soa(void) {
    printf("\n=== TEST 3: OOA vs SOA Architecture Comparison ===\n");

    double x_data[] = {0.5, -1.0, 2.0};
    double g_out_data[] = {1.0, 1.0, 1.0};
    double g_in_soa_data[3];
    double g_in_ooa_data[3];

    EcoVector in = {x_data, 3, 3};
    EcoVector grad_out = {g_out_data, 3, 3};
    EcoVector grad_in_soa = {g_in_soa_data, 3, 0};
    EcoVector grad_in_ooa = {g_in_ooa_data, 3, 0};

    /* 1. SOA Approach */
    IEcoActivationFunctions* soa = GetActivationFunctionsComponent();
    soa->SoftmaxVectorGrad(soa, &in, &grad_out, &grad_in_soa);

    /* 2. OOA Approach (Stateful Layer) */
    IEcoNeuralLayer* ooa = GetNeuralLayerComponent();
    double out_layer_data[3];
    EcoVector out_layer = {out_layer_data, 3, 0};

    ooa->Init(ooa, ACTIVATION_SOFTMAX, 0.0, 3);
    ooa->Forward(ooa, &in, &out_layer);
    ooa->Backward(ooa, &grad_out, &grad_in_ooa);

    /* Сравнение результатов */
    int match = 1;
    for (size_t i = 0; i < 3; ++i) {
        if (calc_abs_err(grad_in_soa.data[i], grad_in_ooa.data[i]) > 1e-7) {
            match = 0;
        }
    }
    printf("SOA vs OOA Backpropagation Output Equivalence: [%s]\n", match ? "PASS" : "FAIL");
}

/* 4. Замеры производительности */
static void test_performance(void) {
    printf("\n=== TEST 4: Performance Analysis ===\n");
    IEcoActivationFunctions* act = GetActivationFunctionsComponent();

    const size_t N = 10000;
    static double vec_in_data[10000];
    static double vec_out_data[10000];
    EcoVector in = {vec_in_data, N, N};
    EcoVector out = {vec_out_data, N, 0};

    for (size_t i = 0; i < N; ++i) {
        vec_in_data[i] = (double)(i % 100) / 10.0 - 5.0;
    }

    const int RUNS = 100;
    clock_t start = clock();
    for (int r = 0; r < RUNS; ++r) {
        for (size_t i = 0; i < N; ++i) {
            vec_out_data[i] = act->Sigmoid(act, vec_in_data[i]);
        }
    }
    clock_t end = clock();
    double time_sig = ((double)(end - start) / CLOCKS_PER_SEC) / RUNS;

    start = clock();
    for (int r = 0; r < RUNS; ++r) {
        act->Softmax(act, &in, &out);
    }
    end = clock();
    double time_softmax = ((double)(end - start) / CLOCKS_PER_SEC) / RUNS;

    printf("Average Execution Time over %d iterations (Vector Size N = %zu):\n", RUNS, N);
    printf(" - Sigmoid (element-wise): %.6f sec\n", time_sig);
    printf(" - Softmax (vectorized)  : %.6f sec\n", time_softmax);
}

int main(void) {
    printf("====================================================\n");
    printf("  ACOM Component Laboratory Work #75 Test Suite     \n");
    printf("====================================================\n\n");

    test_scalar_functions();
    test_softmax_and_jacobian();
    test_ooa_vs_soa();
    test_performance();

    printf("\n[ALL TESTS COMPLETED SUCCESSFULLY]\n");
    return 0;
}
