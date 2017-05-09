#ifndef __MLOGIC_FACTORY_H__
#define __MLOGIC_FACTORY_H__

#include "CyaObject.h"

class IMLogicClient;
class ProcessFactory
{
	DECLAR_FETCH_OBJ(ProcessFactory)
public:
	IMLogicClient* Instance(UINT16 cmdCode);
	void DeleteInstance(IMLogicClient* pObj);
private:
	ProcessFactory();
	~ProcessFactory();
};


#endif