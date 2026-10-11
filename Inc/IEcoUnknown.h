#ifndef __I_ECO_UNKNOWN_H__
#define __I_ECO_UNKNOWN_H__

#include <stdint.h>
#include <stddef.h>


typedef struct UGUID {
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint8_t  Data4[8];
} UGUID;

typedef int32_t ACOM_RESULT;
#define ACOM_OK          0
#define ACOM_E_FAIL     -1
#define ACOM_E_POINTER  -2
#define ACOM_E_INVALIDARG -3

typedef struct IEcoUnknown {
    ACOM_RESULT (*QueryInterface)(struct IEcoUnknown* me, const UGUID* riid, void** ppv);
    uint32_t (*AddRef)(struct IEcoUnknown* me);
    uint32_t (*Release)(struct IEcoUnknown* me);
} IEcoUnknown, *IEcoUnknownPtr;


/* Объявления IID для компонентов ACOM */
#ifndef IID_IEcoUnknown_DEFINED
#define IID_IEcoUnknown_DEFINED
static const UGUID IID_IEcoUnknown = { 0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAA} };
#endif

#ifndef IID_IEcoActivationFunctions_DEFINED
#define IID_IEcoActivationFunctions_DEFINED
/* Уникальный UGUID для интерфейса функций активации */
static const UGUID IID_IEcoActivationFunctions = { 0x01109322, 0x2248, 0x4742, {0xAE, 0x06, 0x82, 0x81, 0x94, 0x47, 0x84, 0x3D} };
#endif


#endif /* __I_ECO_UNKNOWN_H__ */
