#ifndef __ID_ECO_LAB_2_H__
#define __ID_ECO_LAB_2_H__

#include "IEcoBase1.h"
#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"
#include "IEcoNeuralLayer.h" // Используется как интерфейс EcoLab1 из предыдущей работы

/* EcoLab2 CID = {00000000-0000-0000-0000-00000000EC02} */
#ifndef __CID_EcoLab2
static const UGUID CID_EcoLab2 = { 0x01, 0x10, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEC, 0x02} };
#endif /* __CID_EcoLab2 */

#ifdef ECO_DLL
ECO_EXPORT IEcoComponentFactory* ECOCALLMETHOD GetIEcoComponentFactoryPtr(void);
#elif ECO_LIB
extern struct IEcoComponentFactory* GetIEcoComponentFactoryPtr_0000000000000000000000000000EC02;
#endif

#endif /* __ID_ECO_LAB_2_H__ */
