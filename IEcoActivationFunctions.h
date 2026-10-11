#ifndef __I_ECO_ACTIVATION_FUNCTIONS_H__
#define __I_ECO_ACTIVATION_FUNCTIONS_H__

#include "IEcoUnknown.h"
#include "IEcoVector.h"

#ifndef IID_IEcoActivationFunctions_DEFINED
#define IID_IEcoActivationFunctions_DEFINED
static const UGUID IID_IEcoActivationFunctions = { 0x01109322, 0x2248, 0x4742, {0xAE, 0x06, 0x82, 0x81, 0x94, 0x47, 0x84, 0x3D} };
#endif

typedef struct IEcoActivationFunctions {
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
} IEcoActivationFunctions, *IEcoActivationFunctionsPtr_t;

typedef struct IEcoActivationFunctionsVTbl {
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
} IEcoActivationFunctionsVTbl;

#endif /* __I_ECO_ACTIVATION_FUNCTIONS_H__ */