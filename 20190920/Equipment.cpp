#include "Equipment.h"



Equipment::Equipment()
{
}
Equipment::Equipment(QString code, QString lab_id)
{
	CODE = code;
	LAB_ID = lab_id;
	UNIQ_CODE = QString("%1-%2").arg(CODE).arg(LAB_ID);

}

Equipment::~Equipment()
{
}
