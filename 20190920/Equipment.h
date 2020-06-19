#pragma once
#include "qstring.h"
#include"qvector.h"
class Equipment
{
public:
	Equipment();
	Equipment(QString code, QString lab_id);
	~Equipment();


public:
	QString CODE;
	QString LAB_ID;
	QString UNIQ_CODE;
	QString LIB_ID;
	QString FAC_NAME;
	QString LIB_VERSION;
	QString data_date;
	QString data_time;
	QVector< QPair<  QString ,QString> > list_data;


};

