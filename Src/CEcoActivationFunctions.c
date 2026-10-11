#include "IEcoActivationFunctions.h"
#include "EcoMathMathUtils.h"
#include <stdlib.h>

static int IsEqualUGUID(const UGUID* g1, const UGUID* g2) {
    if (g1 == NULL || g2 == NULL) return 0;
    if (g1->Data1 != g2->Data1) return 0;
    if (g1->Data2 != g2->Data2) return 0;
    if (g1->Data3 != g2->Data3) return 0;
    for (int i = 0; i < 8; ++i) {
        if (g1->Data4[i] != g2->Data4[i]) return 0;
    }
    return 1;
}

/* Структура компонента, полностью повторяющая порядок полей IEcoActivationFunctions */
typedef struct CEcoActivationFunctions {
    int16_t (*QueryInterface)(struct IEcoActivationFunctions* me, const UGUID* riid, void** ppv);
    uint32_t (*AddRef)(struct IEcoActivationFunctions* me);
    uint32_t (*Release)(struct IEcoActivationFunctions* me);
    double (*Sigmoid)(struct IEcoActivationFunctions* me, double x);
    double (*SigmoidDerivative)(struct IEcoActivationFunctions* me, double x);
    double (*ReLU)(struct IEcoActivationFunctions* me, double x);
    double (*ReLUDerivative)(struct IEcoActivationFunctions* me, double x);
    double (*LeakyReLU)(struct IEcoActivationFunctions* me, double x, double alpha);
    double (*LeakyReLUDerivative)(struct IEcoActivationFunctions* me, double x, double alpha);
    int16_t (*Softmax)(struct IEcoActivationFunctions* me, const EcoVector* input, EcoVector* output);
    int16_t (*SoftmaxJacobian)(struct IEcoActivationFunctions* me, const EcoVector* input, EcoMatrix* jacobian);
    int16_t (*SoftmaxVectorGrad)(struct IEcoActivationFunctions* me, const EcoVector* input, const EcoVector* grad_output, EcoVector* grad_input);

    /* Закрытые данные экземпляра */
    uint32_t m_cRef;
} CEcoActivationFunctions;

static int16_t CEcoActivationFunctions_QueryInterface(IEcoActivationFunctionsPtr_t me, const UGUID* riid, void** ppv) {
    CEcoActivationFunctions* pCMe = (CEcoActivationFunctions*)me;

    if (me == NULL || ppv == NULL) {
        return -1;
    }

    if (IsEqualUGUID(riid, &IID_IEcoActivationFunctions) || IsEqualUGUID(riid, &IID_IEcoUnknown)) {
        *ppv = pCMe;
        pCMe->AddRef(me);
        return 0;
    }

    *ppv = NULL;
    return -1;
}

static uint32_t CEcoActivationFunctions_AddRef(IEcoActivationFunctionsPtr_t me) {
    CEcoActivationFunctions* pCMe = (CEcoActivationFunctions*)me;
    if (me == NULL) return (uint32_t)-1;
    return ++pCMe->m_cRef;
}

static uint32_t CEcoActivationFunctions_Release(IEcoActivationFunctionsPtr_t me) {
    CEcoActivationFunctions* pCMe = (CEcoActivationFunctions*)me;
    if (me == NULL) return (uint32_t)-1;

    --pCMe->m_cRef;
    if (pCMe->m_cRef == 0) {
        return 0;
    }
    return pCMe->m_cRef;
}

static double CEcoActivationFunctions_Sigmoid(IEcoActivationFunctionsPtr_t me, double x) {
    (void)me;
    if (x >= 0.0) {
        double z = eco_exp(-x);
        return 1.0 / (1.0 + z);
    } else {
        double z = eco_exp(x);
        return z / (1.0 + z);
    }
}

static double CEcoActivationFunctions_SigmoidDerivative(IEcoActivationFunctionsPtr_t me, double x) {
    double s = CEcoActivationFunctions_Sigmoid(me, x);
    return s * (1.0 - s);
}

static double CEcoActivationFunctions_ReLU(IEcoActivationFunctionsPtr_t me, double x) {
    (void)me;
    return (x > 0.0) ? x : 0.0;
}

static double CEcoActivationFunctions_ReLUDerivative(IEcoActivationFunctionsPtr_t me, double x) {
    (void)me;
    return (x > 0.0) ? 1.0 : 0.0;
}

static double CEcoActivationFunctions_LeakyReLU(IEcoActivationFunctionsPtr_t me, double x, double alpha) {
    (void)me;
    return (x >= 0.0) ? x : alpha * x;
}

static double CEcoActivationFunctions_LeakyReLUDerivative(IEcoActivationFunctionsPtr_t me, double x, double alpha) {
    (void)me;
    return (x > 0.0) ? 1.0 : alpha;
}

static int16_t CEcoActivationFunctions_Softmax(IEcoActivationFunctionsPtr_t me, const EcoVector* input, EcoVector* output) {
    (void)me;
    if (me == NULL || input == NULL || output == NULL) return -1;
    if (input->data == NULL || output->data == NULL || input->size == 0) return -1;

    size_t n = input->size;
    double max_val = input->data[0];
    for (size_t i = 1; i < n; ++i) {
        if (input->data[i] > max_val) max_val = input->data[i];
    }

    double sum = 0.0;
    for (size_t i = 0; i < n; ++i) {
        output->data[i] = eco_exp(input->data[i] - max_val);
        sum += output->data[i];
    }

    if (sum == 0.0) return -1;
    for (size_t i = 0; i < n; ++i) {
        output->data[i] /= sum;
    }
    output->size = n;
    return 0;
}

static int16_t CEcoActivationFunctions_SoftmaxJacobian(IEcoActivationFunctionsPtr_t me, const EcoVector* input, EcoMatrix* jacobian) {
    if (me == NULL || input == NULL || jacobian == NULL) return -1;
    size_t n = input->size;

    double* stack_s = (double*)malloc(n * sizeof(double));
    if (!stack_s) return -1;

    EcoVector s_vec = { stack_s, n, 0 };
    if (CEcoActivationFunctions_Softmax(me, input, &s_vec) != 0) {
        free(stack_s);
        return -1;
    }

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            double kronecker = (i == j) ? 1.0 : 0.0;
            jacobian->data[i * n + j] = s_vec.data[i] * (kronecker - s_vec.data[j]);
        }
    }
    jacobian->rows = n;
    jacobian->cols = n;

    free(stack_s);
    return 0;
}

static int16_t CEcoActivationFunctions_SoftmaxVectorGrad(IEcoActivationFunctionsPtr_t me, const EcoVector* input, const EcoVector* grad_output, EcoVector* grad_input) {
    if (me == NULL || input == NULL || grad_output == NULL || grad_input == NULL) return -1;
    size_t n = input->size;

    double* stack_s = (double*)malloc(n * sizeof(double));
    if (!stack_s) return -1;

    EcoVector s_vec = { stack_s, n, 0 };
    if (CEcoActivationFunctions_Softmax(me, input, &s_vec) != 0) {
        free(stack_s);
        return -1;
    }

    double dot = 0.0;
    for (size_t j = 0; j < n; ++j) {
        dot += s_vec.data[j] * grad_output->data[j];
    }

    for (size_t i = 0; i < n; ++i) {
        grad_input->data[i] = s_vec.data[i] * (grad_output->data[i] - dot);
    }
    grad_input->size = n;

    free(stack_s);
    return 0;
}

static CEcoActivationFunctions g_ActivationFunctionsInstance = {
    CEcoActivationFunctions_QueryInterface,
    CEcoActivationFunctions_AddRef,
    CEcoActivationFunctions_Release,
    CEcoActivationFunctions_Sigmoid,
    CEcoActivationFunctions_SigmoidDerivative,
    CEcoActivationFunctions_ReLU,
    CEcoActivationFunctions_ReLUDerivative,
    CEcoActivationFunctions_LeakyReLU,
    CEcoActivationFunctions_LeakyReLUDerivative,
    CEcoActivationFunctions_Softmax,
    CEcoActivationFunctions_SoftmaxJacobian,
    CEcoActivationFunctions_SoftmaxVectorGrad,
    1
};

int16_t initCEcoActivationFunctions(IEcoUnknown* pIUnkOuter, const UGUID* riid, void** ppv) {
    (void)pIUnkOuter;
    if (ppv == NULL) return -1;
    return g_ActivationFunctionsInstance.QueryInterface((IEcoActivationFunctionsPtr_t)&g_ActivationFunctionsInstance, riid, ppv);
}

IEcoActivationFunctions* GetActivationFunctionsComponent(void) {
    static IEcoActivationFunctions* s_pActivationFunctions = NULL;
    if (s_pActivationFunctions == NULL) {
        initCEcoActivationFunctions(NULL, &IID_IEcoActivationFunctions, (void**)&s_pActivationFunctions);
    }
    return s_pActivationFunctions;
}
