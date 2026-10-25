#include "IEcoBase1.h"
#include "IEcoActivationFunctions.h"
#include "EcoMathMathUtils.h"
#include <stdlib.h>

#ifndef IsEqualUGUID
#define IsEqualUGUID(rguid1, rguid2) \
    ((((const byte_t *)(rguid1))[0] == ((const byte_t *)(rguid2))[0]) && \
     (((const byte_t *)(rguid1))[1] == ((const byte_t *)(rguid2))[1]) && \
     (((const byte_t *)(rguid1))[2] == ((const byte_t *)(rguid2))[2]) && \
     (((const byte_t *)(rguid1))[3] == ((const byte_t *)(rguid2))[3]) && \
     (((const byte_t *)(rguid1))[4] == ((const byte_t *)(rguid2))[4]) && \
     (((const byte_t *)(rguid1))[5] == ((const byte_t *)(rguid2))[5]) && \
     (((const byte_t *)(rguid1))[6] == ((const byte_t *)(rguid2))[6]) && \
     (((const byte_t *)(rguid1))[7] == ((const byte_t *)(rguid2))[7]) && \
     (((const byte_t *)(rguid1))[8] == ((const byte_t *)(rguid2))[8]) && \
     (((const byte_t *)(rguid1))[9] == ((const byte_t *)(rguid2))[9]) && \
     (((const byte_t *)(rguid1))[10] == ((const byte_t *)(rguid2))[10]) && \
     (((const byte_t *)(rguid1))[11] == ((const byte_t *)(rguid2))[11]) && \
     (((const byte_t *)(rguid1))[12] == ((const byte_t *)(rguid2))[12]) && \
     (((const byte_t *)(rguid1))[13] == ((const byte_t *)(rguid2))[13]) && \
     (((const byte_t *)(rguid1))[14] == ((const byte_t *)(rguid2))[14]) && \
     (((const byte_t *)(rguid1))[15] == ((const byte_t *)(rguid2))[15]) && \
     (((const byte_t *)(rguid1))[16] == ((const byte_t *)(rguid2))[16]) && \
     (((const byte_t *)(rguid1))[17] == ((const byte_t *)(rguid2))[17]))
#endif

// [ИЗМЕНЕНО]: Структура компонента содержит указатель на VTbl и счетчик ссылок
typedef struct CEcoActivationFunctions {
    struct IEcoActivationFunctionsVTbl* pVTbl;
    uint32_t m_cRef;
} CEcoActivationFunctions;

static int16_t ECOCALLMETHOD CEcoActivationFunctions_QueryInterface(IEcoActivationFunctionsPtr_t me, const UGUID* riid, voidptr_t* ppv) {
    CEcoActivationFunctions* pCMe = (CEcoActivationFunctions*)me;

    if (me == NULL || ppv == NULL) {
        return -2; // ACOM_E_POINTER
    }

    if (IsEqualUGUID(riid, &IID_IEcoActivationFunctions) || IsEqualUGUID(riid, &IID_IEcoUnknown)) {
        *ppv = pCMe;
        pCMe->pVTbl->AddRef(me);
        return 0; // ACOM_OK
    }

    *ppv = NULL;
    return -1; // ACOM_E_FAIL
}

static uint32_t ECOCALLMETHOD CEcoActivationFunctions_AddRef(IEcoActivationFunctionsPtr_t me) {
    CEcoActivationFunctions* pCMe = (CEcoActivationFunctions*)me;
    if (me == NULL) return (uint32_t)-1;
    return ++pCMe->m_cRef;
}

static uint32_t ECOCALLMETHOD CEcoActivationFunctions_Release(IEcoActivationFunctionsPtr_t me) {
    CEcoActivationFunctions* pCMe = (CEcoActivationFunctions*)me;
    if (me == NULL) return (uint32_t)-1;

    --pCMe->m_cRef;
    if (pCMe->m_cRef == 0) {
        return 0;
    }
    return pCMe->m_cRef;
}

static double ECOCALLMETHOD CEcoActivationFunctions_Sigmoid(IEcoActivationFunctionsPtr_t me, double x) {
    (void)me;
    if (x >= 0.0) {
        double z = eco_exp(-x);
        return 1.0 / (1.0 + z);
    } else {
        double z = eco_exp(x);
        return z / (1.0 + z);
    }
}

static double ECOCALLMETHOD CEcoActivationFunctions_SigmoidDerivative(IEcoActivationFunctionsPtr_t me, double x) {
    // [ИЗМЕНЕНО]: Вызов метода через pVTbl
    double s = me->pVTbl->Sigmoid(me, x);
    return s * (1.0 - s);
}

static double ECOCALLMETHOD CEcoActivationFunctions_ReLU(IEcoActivationFunctionsPtr_t me, double x) {
    (void)me;
    return (x > 0.0) ? x : 0.0;
}

static double ECOCALLMETHOD CEcoActivationFunctions_ReLUDerivative(IEcoActivationFunctionsPtr_t me, double x) {
    (void)me;
    return (x > 0.0) ? 1.0 : 0.0;
}

static double ECOCALLMETHOD CEcoActivationFunctions_LeakyReLU(IEcoActivationFunctionsPtr_t me, double x, double alpha) {
    (void)me;
    return (x >= 0.0) ? x : alpha * x;
}

static double ECOCALLMETHOD CEcoActivationFunctions_LeakyReLUDerivative(IEcoActivationFunctionsPtr_t me, double x, double alpha) {
    (void)me;
    return (x > 0.0) ? 1.0 : alpha;
}

static int16_t ECOCALLMETHOD CEcoActivationFunctions_Softmax(IEcoActivationFunctionsPtr_t me, const EcoVector* input, EcoVector* output) {
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

static int16_t ECOCALLMETHOD CEcoActivationFunctions_SoftmaxJacobian(IEcoActivationFunctionsPtr_t me, const EcoVector* input, EcoMatrix* jacobian) {
    if (me == NULL || input == NULL || jacobian == NULL) return -1;
    size_t n = input->size;

    double* stack_s = (double*)malloc(n * sizeof(double));
    if (!stack_s) return -1;

    EcoVector s_vec = { stack_s, n, 0 };
    if (me->pVTbl->Softmax(me, input, &s_vec) != 0) {
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

static int16_t ECOCALLMETHOD CEcoActivationFunctions_SoftmaxVectorGrad(IEcoActivationFunctionsPtr_t me, const EcoVector* input, const EcoVector* grad_output, EcoVector* grad_input) {
    if (me == NULL || input == NULL || grad_output == NULL || grad_input == NULL) return -1;
    size_t n = input->size;

    double* stack_s = (double*)malloc(n * sizeof(double));
    if (!stack_s) return -1;

    EcoVector s_vec = { stack_s, n, 0 };
    if (me->pVTbl->Softmax(me, input, &s_vec) != 0) {
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

// [ИЗМЕНЕНО]: Инициализация таблицы виртуальных функций
static IEcoActivationFunctionsVTbl g_x3209322VTbl = {
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
    CEcoActivationFunctions_SoftmaxVectorGrad
};

static CEcoActivationFunctions g_ActivationFunctionsInstance = {
    &g_x3209322VTbl,
    1
};

int16_t ECOCALLMETHOD initCEcoActivationFunctions(IEcoUnknown* pIUnkOuter, const UGUID* riid, voidptr_t* ppv) {
    (void)pIUnkOuter;
    if (ppv == NULL) return -2;
    return g_ActivationFunctionsInstance.pVTbl->QueryInterface((IEcoActivationFunctionsPtr_t)&g_ActivationFunctionsInstance, riid, ppv);
}

IEcoActivationFunctions* GetActivationFunctionsComponent(void) {
    static IEcoActivationFunctions* s_pActivationFunctions = NULL;
    if (s_pActivationFunctions == NULL) {
        initCEcoActivationFunctions(NULL, &IID_IEcoActivationFunctions, (void**)&s_pActivationFunctions);
    }
    return s_pActivationFunctions;
}
