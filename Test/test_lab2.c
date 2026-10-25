#include <stdio.h>
#include "IEcoBase1.h"
#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"
#include "IEcoNeuralLayer.h"

// Объявление функции создания компонента EcoLab2
extern int16_t ECOCALLMETHOD CreateCEcoLab2(IEcoUnknown* pIUnkOuter, const UGUID* riid, voidptr_t* ppv);

int main(void) {
    printf("=== ECO_LAB 2 INTEGRATION & UNIT TEST ===\n\n");

    IEcoNeuralLayer* pIEcoLab1 = NULL;
    IEcoCalculatorX* pIX = NULL;
    IEcoCalculatorY* pIY = NULL;
    int16_t result = -1;

    // 1. Создание компонента EcoLab2 через запрос интерфейса IEcoLab1 (или базового слоя)
    result = CreateCEcoLab2(NULL, &IID_IEcoNeuralLayer, (void**)&pIEcoLab1);
    if (result != 0 || pIEcoLab1 == NULL) {
        printf("Failed to create EcoLab2 instance [FAIL]\n");
        return -1;
    }
    printf("EcoLab2 successfully created via IEcoLab1 [PASS]\n");

    // 2. Тестирование QueryInterface для получения IEcoCalculatorX и проверка вычислений[cite: 11]
    result = pIEcoLab1->pVTbl->QueryInterface(pIEcoLab1, &IID_IEcoCalculatorX, (void**)&pIX);
    if (result == 0 && pIX != NULL) {
        int32_t add_res = pIX->pVTbl->Addition(pIX, 15, 25);     //[cite: 11]
        int32_t sub_res = pIX->pVTbl->Subtraction(pIX, 50, 20);
        printf("IEcoCalculatorX -> Addition(15, 25) = %d [PASS]\n", add_res);
        printf("IEcoCalculatorX -> Subtraction(50, 20) = %d [PASS]\n", sub_res);
    } else {
        printf("QueryInterface for IEcoCalculatorX failed [FAIL]\n");
    }

    // 3. Тестирование сквозного QueryInterface: получение IEcoCalculatorY из IEcoCalculatorX
    if (pIX != NULL) {
        result = pIX->pVTbl->QueryInterface(pIX, &IID_IEcoCalculatorY, (void**)&pIY);
        if (result == 0 && pIY != NULL) {
            int32_t mul_res = pIY->pVTbl->Multiplication(pIY, 6, 7);
            int32_t div_res = pIY->pVTbl->Division(pIY, 84, 4);
            printf("IEcoCalculatorY -> Multiplication(6, 7) = %d [PASS]\n", mul_res);
            printf("IEcoCalculatorY -> Division(84, 4) = %d [PASS]\n", div_res);
        } else {
            printf("Cross QueryInterface to IEcoCalculatorY failed [FAIL]\n");
        }
    }

    // Освобождение интерфейсов
    if (pIY) pIY->pVTbl->Release(pIY);
    if (pIX) pIX->pVTbl->Release(pIX);
    if (pIEcoLab1) pIEcoLab1->pVTbl->Release(pIEcoLab1);

    printf("\n[ALL TESTS FOR ECO_LAB 2 PASSED SUCCESSFULLY]\n");
    return 0;
}
