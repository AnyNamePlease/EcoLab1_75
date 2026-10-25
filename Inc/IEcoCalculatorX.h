#ifndef __I_ECO_CALCULATOR_X_H__
#define __I_ECO_CALCULATOR_X_H__

#include "IEcoBase1.h"

#ifndef __IID_IEcoCalculatorX
static const UGUID IID_IEcoCalculatorX = { 0x00, 0x00, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01} };
#endif

typedef struct IEcoCalculatorX* IEcoCalculatorXPtr_t;

typedef struct IEcoCalculatorXVTbl {
    int16_t (ECOCALLMETHOD *QueryInterface)(IEcoCalculatorXPtr_t me, const UGUID* riid, voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(IEcoCalculatorXPtr_t me);
    uint32_t (ECOCALLMETHOD *Release)(IEcoCalculatorXPtr_t me);
    int32_t (ECOCALLMETHOD *Addition)(IEcoCalculatorXPtr_t me, int32_t a, int32_t b);
    int32_t (ECOCALLMETHOD *Subtraction)(IEcoCalculatorXPtr_t me, int32_t a, int32_t b);
} IEcoCalculatorXVTbl;

interface IEcoCalculatorX {
    struct IEcoCalculatorXVTbl *pVTbl;
} IEcoCalculatorX;

#endif /* __I_ECO_CALCULATOR_X_H__ */
