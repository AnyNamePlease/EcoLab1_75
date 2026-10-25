#ifndef __I_ECO_ACTIVATION_FUNCTIONS_H__
#define __I_ECO_ACTIVATION_FUNCTIONS_H__

#include "IEcoBase1.h"
#include "IEcoVector.h"

// [ИЗМЕНЕНО]: Регистрация IID интерфейса функций активации в стиле Eco
#ifndef __IID_IEcoActivationFunctions
static const UGUID IID_IEcoActivationFunctions = { 0x01, 0x10, {0x93, 0x22, 0x22, 0x48, 0x47, 0x42, 0xAE, 0x06, 0x82, 0x81, 0x94, 0x47, 0x84, 0x3D} };
#endif

typedef struct IEcoActivationFunctions* IEcoActivationFunctionsPtr_t;

// [ИЗМЕНЕНО]: Виртуальная таблица (VTbl) построена по стандартам Eco с макросом ECOCALLMETHOD
typedef struct IEcoActivationFunctionsVTbl {
    int16_t (ECOCALLMETHOD *QueryInterface)(/* in */ IEcoActivationFunctionsPtr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(/* in */ IEcoActivationFunctionsPtr_t me);
    uint32_t (ECOCALLMETHOD *Release)(/* in */ IEcoActivationFunctionsPtr_t me);
    double (ECOCALLMETHOD *Sigmoid)(/* in */ IEcoActivationFunctionsPtr_t me, /* in */ double x);
    double (ECOCALLMETHOD *SigmoidDerivative)(/* in */ IEcoActivationFunctionsPtr_t me, /* in */ double x);
    double (ECOCALLMETHOD *ReLU)(/* in */ IEcoActivationFunctionsPtr_t me, /* in */ double x);
    double (ECOCALLMETHOD *ReLUDerivative)(/* in */ IEcoActivationFunctionsPtr_t me, /* in */ double x);
    double (ECOCALLMETHOD *LeakyReLU)(/* in */ IEcoActivationFunctionsPtr_t me, /* in */ double x, /* in */ double alpha);
    double (ECOCALLMETHOD *LeakyReLUDerivative)(/* in */ IEcoActivationFunctionsPtr_t me, /* in */ double x, /* in */ double alpha);
    int16_t (ECOCALLMETHOD *Softmax)(/* in */ IEcoActivationFunctionsPtr_t me, /* in */ const EcoVector* input, /* out */ EcoVector* output);
    int16_t (ECOCALLMETHOD *SoftmaxJacobian)(/* in */ IEcoActivationFunctionsPtr_t me, /* in */ const EcoVector* input, /* out */ EcoMatrix* jacobian);
    int16_t (ECOCALLMETHOD *SoftmaxVectorGrad)(/* in */ IEcoActivationFunctionsPtr_t me, /* in */ const EcoVector* input, /* in */ const EcoVector* grad_output, /* out */ EcoVector* grad_input);
} IEcoActivationFunctionsVTbl;

interface IEcoActivationFunctions {
    struct IEcoActivationFunctionsVTbl *pVTbl;
} IEcoActivationFunctions;

#endif /* __I_ECO_ACTIVATION_FUNCTIONS_H__ */
