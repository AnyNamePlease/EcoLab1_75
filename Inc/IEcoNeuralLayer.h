#ifndef __I_ECO_NEURAL_LAYER_H__
#define __I_ECO_NEURAL_LAYER_H__

#include "IEcoBase1.h"
#include "IEcoVector.h"

typedef enum {
    ACTIVATION_SIGMOID,
    ACTIVATION_RELU,
    ACTIVATION_LEAKY_RELU,
    ACTIVATION_SOFTMAX
} ActivationType;

// [ИЗМЕНЕНО]: Регистрация IID для нейронного слоя
#ifndef __IID_IEcoNeuralLayer
static const UGUID IID_IEcoNeuralLayer = { 0x02, 0x10, {0x93, 0x22, 0x22, 0x48, 0x47, 0x42, 0xAE, 0x06, 0x82, 0x81, 0x94, 0x47, 0x84, 0x3D} };
#endif

typedef struct IEcoNeuralLayer* IEcoNeuralLayerPtr_t;

// [ИЗМЕНЕНО]: Виртуальная таблица методов нейронного слоя
typedef struct IEcoNeuralLayerVTbl {
    int16_t (ECOCALLMETHOD *QueryInterface)(/* in */ IEcoNeuralLayerPtr_t me, /* in */ const UGUID* riid, /* out */ voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(/* in */ IEcoNeuralLayerPtr_t me);
    uint32_t (ECOCALLMETHOD *Release)(/* in */ IEcoNeuralLayerPtr_t me);
    int16_t (ECOCALLMETHOD *Init)(/* in */ IEcoNeuralLayerPtr_t me, /* in */ ActivationType type, /* in */ double alpha, /* in */ size_t size);
    int16_t (ECOCALLMETHOD *Forward)(/* in */ IEcoNeuralLayerPtr_t me, /* in */ const EcoVector* input, /* out */ EcoVector* output);
    int16_t (ECOCALLMETHOD *Backward)(/* in */ IEcoNeuralLayerPtr_t me, /* in */ const EcoVector* grad_output, /* out */ EcoVector* grad_input);
} IEcoNeuralLayerVTbl;

interface IEcoNeuralLayer {
    struct IEcoNeuralLayerVTbl *pVTbl;
} IEcoNeuralLayer;

#endif /* __I_ECO_NEURAL_LAYER_H__ */
