#pragma once
#include "qstring"
#include "qvector"
class PlatForm
{
public:
	PlatForm();
	PlatForm(QString code);
	~PlatForm();
public:
	QString code;
	QString id;
	QString p_name;
	QString f_name;
	QString type;
	QString role;
	QString task;
	QString category;
	QString entityid;
	QVector< QPair<QString,QString> > q_data;

};

