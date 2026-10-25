#ifndef __I_ECO_CALCULATOR_Y_H__
#define __I_ECO_CALCULATOR_Y_H__

#include "IEcoBase1.h"

#ifndef __IID_IEcoCalculatorY
static const UGUID IID_IEcoCalculatorY = { 0x00, 0x00, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02} };
#endif

typedef struct IEcoCalculatorY* IEcoCalculatorYPtr_t;

typedef struct IEcoCalculatorYVTbl {
    int16_t (ECOCALLMETHOD *QueryInterface)(IEcoCalculatorYPtr_t me, const UGUID* riid, voidptr_t* ppv);
    uint32_t (ECOCALLMETHOD *AddRef)(IEcoCalculatorYPtr_t me);
    uint32_t (ECOCALLMETHOD *Release)(IEcoCalculatorYPtr_t me);
    int32_t (ECOCALLMETHOD *Multiplication)(IEcoCalculatorYPtr_t me, int32_t a, int32_t b);
    int32_t (ECOCALLMETHOD *Division)(IEcoCalculatorYPtr_t me, int32_t a, int32_t b);
} IEcoCalculatorYVTbl;

interface IEcoCalculatorY {
    struct IEcoCalculatorYVTbl *pVTbl;
} IEcoCalculatorY;

#endif /* __I_ECO_CALCULATOR_Y_H__ */
