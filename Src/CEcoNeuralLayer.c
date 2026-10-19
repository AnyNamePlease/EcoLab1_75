#include "IEcoNeuralLayer.h"
#include "IEcoActivationFunctions.h"
#include <stdlib.h>

extern IEcoActivationFunctions* GetActivationFunctionsComponent(void);

// [ИЗМЕНЕНО]: Полная структура компонента нейронного слоя со скрытыми данными и указателем на VTbl
typedef struct CEcoNeuralLayer {
    struct IEcoNeuralLayerVTbl* pVTbl;
    uint32_t ref_count;
    ActivationType type;
    double alpha;
    size_t size;
    EcoVector cached_input;
    EcoVector cached_output;
    int is_forward_done;
} CEcoNeuralLayer;

static int16_t ECOCALLMETHOD CEcoNeuralLayer_QueryInterface(IEcoNeuralLayerPtr_t me, const UGUID* riid, voidptr_t* ppv) {
    (void)riid;
    if (me == NULL || ppv == NULL) {
        return -2; /* ACOM_E_POINTER */
    }
    *ppv = me;
    me->pVTbl->AddRef(me);
    return 0; /* ACOM_OK */
}

static uint32_t ECOCALLMETHOD CEcoNeuralLayer_AddRef(IEcoNeuralLayerPtr_t me) {
    CEcoNeuralLayer* this_ptr = (CEcoNeuralLayer*)me;
    if (me == NULL) return (uint32_t)-1;
    return ++this_ptr->ref_count;
}

static uint32_t ECOCALLMETHOD CEcoNeuralLayer_Release(IEcoNeuralLayerPtr_t me) {
    CEcoNeuralLayer* this_ptr = (CEcoNeuralLayer*)me;
    if (me == NULL) return (uint32_t)-1;

    --this_ptr->ref_count;
    if (this_ptr->ref_count == 0) {
        if (this_ptr->cached_input.data) free(this_ptr->cached_input.data);
        if (this_ptr->cached_output.data) free(this_ptr->cached_output.data);
        free(this_ptr);
        return 0;
    }
    return this_ptr->ref_count;
}

static int16_t ECOCALLMETHOD CEcoNeuralLayer_Init(IEcoNeuralLayerPtr_t me, ActivationType type, double alpha, size_t size) {
    if (me == NULL) return -2;
    CEcoNeuralLayer* this_ptr = (CEcoNeuralLayer*)me;

    this_ptr->type = type;
    this_ptr->alpha = alpha;
    this_ptr->size = size;

    if (this_ptr->cached_input.data) free(this_ptr->cached_input.data);
    if (this_ptr->cached_output.data) free(this_ptr->cached_output.data);

    this_ptr->cached_input.data = (double*)malloc(size * sizeof(double));
    this_ptr->cached_output.data = (double*)malloc(size * sizeof(double));

    if (!this_ptr->cached_input.data || !this_ptr->cached_output.data) {
        return -1;
    }

    this_ptr->cached_input.capacity = size;
    this_ptr->cached_input.size = 0;
    this_ptr->cached_output.capacity = size;
    this_ptr->cached_output.size = 0;
    this_ptr->is_forward_done = 0;

    return 0;
}

static int16_t ECOCALLMETHOD CEcoNeuralLayer_Forward(IEcoNeuralLayerPtr_t me, const EcoVector* input, EcoVector* output) {
    if (me == NULL || input == NULL || output == NULL) return -2;
    CEcoNeuralLayer* this_ptr = (CEcoNeuralLayer*)me;
    if (input->size != this_ptr->size) return -3;

    IEcoActivationFunctions* act = GetActivationFunctionsComponent();

    for (size_t i = 0; i < input->size; ++i) {
        this_ptr->cached_input.data[i] = input->data[i];
    }
    this_ptr->cached_input.size = input->size;

    if (this_ptr->type == ACTIVATION_SOFTMAX) {
        act->pVTbl->Softmax(act, input, output);
    } else {
        for (size_t i = 0; i < input->size; ++i) {
            double x = input->data[i];
            if (this_ptr->type == ACTIVATION_SIGMOID) {
                output->data[i] = act->pVTbl->Sigmoid(act, x);
            } else if (this_ptr->type == ACTIVATION_RELU) {
                output->data[i] = act->pVTbl->ReLU(act, x);
            } else if (this_ptr->type == ACTIVATION_LEAKY_RELU) {
                output->data[i] = act->pVTbl->LeakyReLU(act, x, this_ptr->alpha);
            }
        }
        output->size = input->size;
    }

    for (size_t i = 0; i < output->size; ++i) {
        this_ptr->cached_output.data[i] = output->data[i];
    }
    this_ptr->cached_output.size = output->size;
    this_ptr->is_forward_done = 1;

    return 0;
}

static int16_t ECOCALLMETHOD CEcoNeuralLayer_Backward(IEcoNeuralLayerPtr_t me, const EcoVector* grad_output, EcoVector* grad_input) {
    if (me == NULL || grad_output == NULL || grad_input == NULL) return -2;
    CEcoNeuralLayer* this_ptr = (CEcoNeuralLayer*)me;
    if (!this_ptr->is_forward_done) return -1;

    IEcoActivationFunctions* act = GetActivationFunctionsComponent();

    if (this_ptr->type == ACTIVATION_SOFTMAX) {
        return act->pVTbl->SoftmaxVectorGrad(act, &this_ptr->cached_input, grad_output, grad_input);
    } else {
        for (size_t i = 0; i < this_ptr->size; ++i) {
            double x = this_ptr->cached_input.data[i];
            double d = 0.0;
            if (this_ptr->type == ACTIVATION_SIGMOID) {
                d = act->pVTbl->SigmoidDerivative(act, x);
            } else if (this_ptr->type == ACTIVATION_RELU) {
                d = act->pVTbl->ReLUDerivative(act, x);
            } else if (this_ptr->type == ACTIVATION_LEAKY_RELU) {
                d = act->pVTbl->LeakyReLUDerivative(act, x, this_ptr->alpha);
            }
            grad_input->data[i] = grad_output->data[i] * d;
        }
        grad_input->size = this_ptr->size;
    }
    return 0;
}

// [ИЗМЕНЕНО]: VTbl для нейронного слоя
static IEcoNeuralLayerVTbl g_x42209322VTbl = {
    CEcoNeuralLayer_QueryInterface,
    CEcoNeuralLayer_AddRef,
    CEcoNeuralLayer_Release,
    CEcoNeuralLayer_Init,
    CEcoNeuralLayer_Forward,
    CEcoNeuralLayer_Backward
};

IEcoNeuralLayer* GetNeuralLayerComponent(void) {
    CEcoNeuralLayer* layer = (CEcoNeuralLayer*)malloc(sizeof(CEcoNeuralLayer));
    if (!layer) return NULL;

    layer->pVTbl = &g_x42209322VTbl;
    layer->ref_count = 1;
    layer->cached_input.data = NULL;
    layer->cached_output.data = NULL;
    layer->is_forward_done = 0;

    return (IEcoNeuralLayer*)layer;
}
