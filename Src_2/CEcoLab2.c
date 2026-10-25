#include "IEcoBase1.h"
#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"
#include "IEcoNeuralLayer.h"
#include "IEcoMemoryAllocator1.h"
#include "IdEcoLab2.h"
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


typedef struct CEcoLab2 {
    struct IEcoNeuralLayerVTbl* m_pVTblIEcoLab1; // Интерфейс EcoLab1 (нейронный слой)
    struct IEcoCalculatorXVTbl* m_pVTblCalculatorX; // Интерфейс калькулятора X
    struct IEcoCalculatorYVTbl* m_pVTblCalculatorY; // Интерфейс калькулятора Y

    uint32_t m_cRef;
    IEcoUnknown* m_pIUnknownOuter; // Указатель на внешнего владельца для агрегирования
    IEcoMemoryAllocator1* m_pIMem; // Системный аллокатор памяти
} CEcoLab2;

/* --- Реализация интерфейса IEcoLab1 / IEcoNeuralLayer --- */
static int16_t ECOCALLMETHOD CEcoLab2_EcoLab1_QueryInterface(IEcoNeuralLayerPtr_t me, const UGUID* riid, voidptr_t* ppv) {
    CEcoLab2* pCMe = (CEcoLab2*)((uint8_t*)me - offsetof(CEcoLab2, m_pVTblIEcoLab1));

    if (ppv == NULL) return -2;
    *ppv = NULL;

    // Делегирование внешнему объекту при агрегировании
    if (pCMe->m_pIUnknownOuter != NULL) {
        return pCMe->m_pIUnknownOuter->pVTbl->QueryInterface(pCMe->m_pIUnknownOuter, riid, ppv);
    }

    if (IsEqualUGUID(riid, &IID_IEcoUnknown) || IsEqualUGUID(riid, &IID_IEcoNeuralLayer)) {
        *ppv = &pCMe->m_pVTblIEcoLab1;
    } else if (IsEqualUGUID(riid, &IID_IEcoCalculatorX)) {
        *ppv = &pCMe->m_pVTblCalculatorX;
    } else if (IsEqualUGUID(riid, &IID_IEcoCalculatorY)) {
        *ppv = &pCMe->m_pVTblCalculatorY;
    } else {
        return -1; // ACOM_E_NOINTERFACE
    }

    ((IEcoUnknown*)*ppv)->pVTbl->AddRef((IEcoUnknown*)*ppv);
    return 0;
}

static uint32_t ECOCALLMETHOD CEcoLab2_EcoLab1_AddRef(IEcoNeuralLayerPtr_t me) {
    CEcoLab2* pCMe = (CEcoLab2*)((uint8_t*)me - offsetof(CEcoLab2, m_pVTblIEcoLab1));
    return ++pCMe->m_cRef;
}

static uint32_t ECOCALLMETHOD CEcoLab2_EcoLab1_Release(IEcoNeuralLayerPtr_t me) {
    CEcoLab2* pCMe = (CEcoLab2*)((uint8_t*)me - offsetof(CEcoLab2, m_pVTblIEcoLab1));
    --pCMe->m_cRef;
    if (pCMe->m_cRef == 0) {
        if (pCMe->m_pIMem != NULL) {
            pCMe->m_pIMem->pVTbl->Free(pCMe->m_pIMem, pCMe);
        } else {
            free(pCMe);
        }
        return 0;
    }
    return pCMe->m_cRef;
}

/* --- Реализация интерфейса IEcoCalculatorX --- */
static int16_t ECOCALLMETHOD CEcoLab2_CalcX_QueryInterface(IEcoCalculatorXPtr_t me, const UGUID* riid, voidptr_t* ppv) {
    CEcoLab2* pCMe = (CEcoLab2*)((uint8_t*)me - offsetof(CEcoLab2, m_pVTblCalculatorX));
    return pCMe->m_pVTblIEcoLab1->QueryInterface((IEcoNeuralLayerPtr_t)&pCMe->m_pVTblIEcoLab1, riid, ppv);
}

static uint32_t ECOCALLMETHOD CEcoLab2_CalcX_AddRef(IEcoCalculatorXPtr_t me) {
    CEcoLab2* pCMe = (CEcoLab2*)((uint8_t*)me - offsetof(CEcoLab2, m_pVTblCalculatorX));
    return pCMe->m_pVTblIEcoLab1->AddRef((IEcoNeuralLayerPtr_t)&pCMe->m_pVTblIEcoLab1);
}

static uint32_t ECOCALLMETHOD CEcoLab2_CalcX_Release(IEcoCalculatorXPtr_t me) {
    CEcoLab2* pCMe = (CEcoLab2*)((uint8_t*)me - offsetof(CEcoLab2, m_pVTblCalculatorX));
    return pCMe->m_pVTblIEcoLab1->Release((IEcoNeuralLayerPtr_t)&pCMe->m_pVTblIEcoLab1);
}

static int32_t ECOCALLMETHOD CEcoLab2_CalcX_Addition(IEcoCalculatorXPtr_t me, int32_t a, int32_t b) {
    return a + b;
}

static int32_t ECOCALLMETHOD CEcoLab2_CalcX_Subtraction(IEcoCalculatorXPtr_t me, int32_t a, int32_t b) {
    return a - b;
}

/* --- Реализация интерфейса IEcoCalculatorY --- */
static int16_t ECOCALLMETHOD CEcoLab2_CalcY_QueryInterface(IEcoCalculatorYPtr_t me, const UGUID* riid, voidptr_t* ppv) {
    CEcoLab2* pCMe = (CEcoLab2*)((uint8_t*)me - offsetof(CEcoLab2, m_pVTblCalculatorY));
    return pCMe->m_pVTblIEcoLab1->QueryInterface((IEcoNeuralLayerPtr_t)&pCMe->m_pVTblIEcoLab1, riid, ppv);
}

static uint32_t ECOCALLMETHOD CEcoLab2_CalcY_AddRef(IEcoCalculatorYPtr_t me) {
    CEcoLab2* pCMe = (CEcoLab2*)((uint8_t*)me - offsetof(CEcoLab2, m_pVTblCalculatorY));
    return pCMe->m_pVTblIEcoLab1->AddRef((IEcoNeuralLayerPtr_t)&pCMe->m_pVTblIEcoLab1);
}

static uint32_t ECOCALLMETHOD CEcoLab2_CalcY_Release(IEcoCalculatorYPtr_t me) {
    CEcoLab2* pCMe = (CEcoLab2*)((uint8_t*)me - offsetof(CEcoLab2, m_pVTblCalculatorY));
    return pCMe->m_pVTblIEcoLab1->Release((IEcoNeuralLayerPtr_t)&pCMe->m_pVTblIEcoLab1);
}

static int32_t ECOCALLMETHOD CEcoLab2_CalcY_Multiplication(IEcoCalculatorYPtr_t me, int32_t a, int32_t b) {
    return a * b;
}

static int32_t ECOCALLMETHOD CEcoLab2_CalcY_Division(IEcoCalculatorYPtr_t me, int32_t a, int32_t b) {
    return (b != 0) ? (a / b) : 0;
}

/* --- Таблицы виртуальных методов (VTbl) --- */
static IEcoNeuralLayerVTbl g_x2209322VTblLab1 = {
    CEcoLab2_EcoLab1_QueryInterface,
    CEcoLab2_EcoLab1_AddRef,
    CEcoLab2_EcoLab1_Release,
    NULL, NULL, NULL // Заглушки Init, Forward, Backward для слоя, если не используются напрямую
};

static IEcoCalculatorXVTbl g_x2209322VTblCalcX = {
    CEcoLab2_CalcX_QueryInterface,
    CEcoLab2_CalcX_AddRef,
    CEcoLab2_CalcX_Release,
    CEcoLab2_CalcX_Addition,
    CEcoLab2_CalcX_Subtraction
};

static IEcoCalculatorYVTbl g_x2209322VTblCalcY = {
    CEcoLab2_CalcY_QueryInterface,
    CEcoLab2_CalcY_AddRef,
    CEcoLab2_CalcY_Release,
    CEcoLab2_CalcY_Multiplication,
    CEcoLab2_CalcY_Division
};

/* --- Функция создания компонента (Constructor / Factory function) --- */
int16_t ECOCALLMETHOD CreateCEcoLab2(IEcoUnknown* pIUnkOuter, const UGUID* riid, voidptr_t* ppv) {
    CEcoLab2* pCMe = NULL;
    int16_t result = -1;

    if (ppv == NULL) return -2;
    *ppv = NULL;

    // Выделение памяти под структуру компонента
    pCMe = (CEcoLab2*)malloc(sizeof(CEcoLab2));
    if (pCMe == NULL) return -1;

    pCMe->m_pVTblIEcoLab1 = &g_x2209322VTblLab1;
    pCMe->m_pVTblCalculatorX = &g_x2209322VTblCalcX;
    pCMe->m_pVTblCalculatorY = &g_x2209322VTblCalcY;
    pCMe->m_cRef = 1;
    pCMe->m_pIUnknownOuter = pIUnkOuter;
    pCMe->m_pIMem = NULL;

    // Проверка агрегирования: если pIUnkOuter задан, то базовым для AddRef/Release выступает pIUnkOuter
    result = pCMe->m_pVTblIEcoLab1->QueryInterface((IEcoNeuralLayerPtr_t)&pCMe->m_pVTblIEcoLab1, riid, ppv);

    // Декрементируем локальный счетчик (QueryInterface увеличивает его)
    pCMe->m_pVTblIEcoLab1->Release((IEcoNeuralLayerPtr_t)&pCMe->m_pVTblIEcoLab1);

    return result;
}
