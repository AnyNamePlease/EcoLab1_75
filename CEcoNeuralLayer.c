#include "IEcoNeuralLayer.h"
#include "IEcoActivationFunctions.h"
#include <stdlib.h> // Для динамического выделения памяти

extern IEcoActivationFunctions* GetActivationFunctionsComponent(void);

/* Внутренняя структура компонента, наследующая интерфейс IEcoNeuralLayer */
typedef struct CEcoNeuralLayer {
    /* Таблица функций (в данном интерфейсе они объявлены прямо в структуре) */
    ACOM_RESULT (*QueryInterface)(struct IEcoNeuralLayer* me, const UGUID* riid, void** ppv);
    uint32_t (*AddRef)(struct IEcoNeuralLayer* me);
    uint32_t (*Release)(struct IEcoNeuralLayer* me);
    ACOM_RESULT (*Init)(struct IEcoNeuralLayer* me, ActivationType type, double alpha, size_t size);
    ACOM_RESULT (*Forward)(struct IEcoNeuralLayer* me, const EcoVector* input, EcoVector* output);
    ACOM_RESULT (*Backward)(struct IEcoNeuralLayer* me, const EcoVector* grad_output, EcoVector* grad_input);

    /* Скрытые данные экземпляра (состояние слоя) */
    uint32_t ref_count;
    ActivationType type;
    double alpha;
    size_t size;

    EcoVector cached_input;
    EcoVector cached_output;
    int is_forward_done;
} CEcoNeuralLayer;

static ACOM_RESULT CEcoNeuralLayer_QueryInterface(struct IEcoNeuralLayer* me, const UGUID* riid, void** ppv) {
    (void)riid; /* Place the suppression statement inside the function body */

    if (me == NULL || ppv == NULL) {
        return ACOM_E_POINTER;
    }

    *ppv = me;
    me->AddRef(me);
    return ACOM_OK;
}

static uint32_t CEcoNeuralLayer_AddRef(struct IEcoNeuralLayer* me) {
    CEcoNeuralLayer* this_ptr = (CEcoNeuralLayer*)me;
    if (me == NULL) return (uint32_t)-1;
    return ++this_ptr->ref_count;
}

static uint32_t CEcoNeuralLayer_Release(struct IEcoNeuralLayer* me) {
    CEcoNeuralLayer* this_ptr = (CEcoNeuralLayer*)me;
    if (me == NULL) return (uint32_t)-1;

    --this_ptr->ref_count;
    if (this_ptr->ref_count == 0) {
        /* Освобождение динамической памяти при уничтожении компонента */
        if (this_ptr->cached_input.data) free(this_ptr->cached_input.data);
        if (this_ptr->cached_output.data) free(this_ptr->cached_output.data);
        free(this_ptr);
        return 0;
    }
    return this_ptr->ref_count;
}

static ACOM_RESULT CEcoNeuralLayer_Init(struct IEcoNeuralLayer* me, ActivationType type, double alpha, size_t size) {
    if (me == NULL) return ACOM_E_POINTER;
    CEcoNeuralLayer* this_ptr = (CEcoNeuralLayer*)me;

    this_ptr->type = type;
    this_ptr->alpha = alpha;
    this_ptr->size = size;

    /* Динамическое выделение памяти для кэша без ограничения в 128 элементов */
    if (this_ptr->cached_input.data) free(this_ptr->cached_input.data);
    if (this_ptr->cached_output.data) free(this_ptr->cached_output.data);

    this_ptr->cached_input.data = (double*)malloc(size * sizeof(double));
    this_ptr->cached_output.data = (double*)malloc(size * sizeof(double));

    if (!this_ptr->cached_input.data || !this_ptr->cached_output.data) {
        return ACOM_E_FAIL; /* Ошибка выделения памяти */
    }

    this_ptr->cached_input.capacity = size;
    this_ptr->cached_input.size = 0;
    this_ptr->cached_output.capacity = size;
    this_ptr->cached_output.size = 0;
    this_ptr->is_forward_done = 0;

    return ACOM_OK;
}

static ACOM_RESULT CEcoNeuralLayer_Forward(struct IEcoNeuralLayer* me, const EcoVector* input, EcoVector* output) {
    if (me == NULL || input == NULL || output == NULL) return ACOM_E_POINTER;
    CEcoNeuralLayer* this_ptr = (CEcoNeuralLayer*)me;
    if (input->size != this_ptr->size) return ACOM_E_INVALIDARG;

    IEcoActivationFunctions* act = GetActivationFunctionsComponent();

    for (size_t i = 0; i < input->size; ++i) {
        this_ptr->cached_input.data[i] = input->data[i];
    }
    this_ptr->cached_input.size = input->size;

    if (this_ptr->type == ACTIVATION_SOFTMAX) {
        act->Softmax(act, input, output);
    } else {
        for (size_t i = 0; i < input->size; ++i) {
            double x = input->data[i];
            if (this_ptr->type == ACTIVATION_SIGMOID) {
                output->data[i] = act->Sigmoid(act, x);
            } else if (this_ptr->type == ACTIVATION_RELU) {
                output->data[i] = act->ReLU(act, x);
            } else if (this_ptr->type == ACTIVATION_LEAKY_RELU) {
                output->data[i] = act->LeakyReLU(act, x, this_ptr->alpha);
            }
        }
        output->size = input->size;
    }

    for (size_t i = 0; i < output->size; ++i) {
        this_ptr->cached_output.data[i] = output->data[i];
    }
    this_ptr->cached_output.size = output->size;
    this_ptr->is_forward_done = 1;

    return ACOM_OK;
}

static ACOM_RESULT CEcoNeuralLayer_Backward(struct IEcoNeuralLayer* me, const EcoVector* grad_output, EcoVector* grad_input) {
    if (me == NULL || grad_output == NULL || grad_input == NULL) return ACOM_E_POINTER;
    CEcoNeuralLayer* this_ptr = (CEcoNeuralLayer*)me;
    if (!this_ptr->is_forward_done) return ACOM_E_FAIL;

    IEcoActivationFunctions* act = GetActivationFunctionsComponent();

    if (this_ptr->type == ACTIVATION_SOFTMAX) {
        return act->SoftmaxVectorGrad(act, &this_ptr->cached_input, grad_output, grad_input);
    } else {
        for (size_t i = 0; i < this_ptr->size; ++i) {
            double x = this_ptr->cached_input.data[i];
            double d = 0.0;
            if (this_ptr->type == ACTIVATION_SIGMOID) {
                d = act->SigmoidDerivative(act, x);
            } else if (this_ptr->type == ACTIVATION_RELU) {
                d = act->ReLUDerivative(act, x);
            } else if (this_ptr->type == ACTIVATION_LEAKY_RELU) {
                d = act->LeakyReLUDerivative(act, x, this_ptr->alpha);
            }
            grad_input->data[i] = grad_output->data[i] * d;
        }
        grad_input->size = this_ptr->size;
    }
    return ACOM_OK;
}

/* Фабричный метод (заглушка) для создания экземпляра слоя.
   В полноценной ОС на базе АОМК создание происходит через QueryComponent и Class Factory. */
IEcoNeuralLayer* GetNeuralLayerComponent(void) {
    CEcoNeuralLayer* layer = (CEcoNeuralLayer*)malloc(sizeof(CEcoNeuralLayer));
    if (!layer) return NULL;

    layer->QueryInterface = CEcoNeuralLayer_QueryInterface;
    layer->AddRef = CEcoNeuralLayer_AddRef;
    layer->Release = CEcoNeuralLayer_Release;
    layer->Init = CEcoNeuralLayer_Init;
    layer->Forward = CEcoNeuralLayer_Forward;
    layer->Backward = CEcoNeuralLayer_Backward;

    layer->ref_count = 1;
    layer->cached_input.data = NULL;
    layer->cached_output.data = NULL;
    layer->is_forward_done = 0;

    return (IEcoNeuralLayer*)layer;
}