#ifndef __I_ECO_NEURAL_LAYER_H__
#define __I_ECO_NEURAL_LAYER_H__

#include "IEcoUnknown.h"
#include "IEcoVector.h"

typedef enum {
    ACTIVATION_SIGMOID,
    ACTIVATION_RELU,
    ACTIVATION_LEAKY_RELU,
    ACTIVATION_SOFTMAX
} ActivationType;

typedef struct IEcoNeuralLayer {
    /* IEcoUnknown */
    ACOM_RESULT (*QueryInterface)(struct IEcoNeuralLayer* me, const UGUID* riid, void** ppv);
    uint32_t (*AddRef)(struct IEcoNeuralLayer* me);
    uint32_t (*Release)(struct IEcoNeuralLayer* me);

    /* Метод инициализации слоя */
    ACOM_RESULT (*Init)(struct IEcoNeuralLayer* me, ActivationType type, double alpha, size_t size);

    /* Прямой проход: сохраняет вход/выход во внутреннем состоянии */
    ACOM_RESULT (*Forward)(struct IEcoNeuralLayer* me, const EcoVector* input, EcoVector* output);

    /* Обратный проход: использует сохранённое состояние для расчёта градиентов */
    ACOM_RESULT (*Backward)(struct IEcoNeuralLayer* me, const EcoVector* grad_output, EcoVector* grad_input);

} IEcoNeuralLayer, *IEcoNeuralLayerPtr;

#endif /* __I_ECO_NEURAL_LAYER_H__ */