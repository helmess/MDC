#pragma once
#include"Equipment.h"
#include"qlist.h"
#include "qdialog.h"
#include "qdebug.h"
#include "iostream"
#include "qstringlist.h"
#include <QGridLayout> 
#include "qtextedit.h"
#include "qdir.h"
#include "qfile.h"
#include "qfiledialog.h"
#include "qfileinfo.h"
#include "qmessagebox"
#include <QDomDocument>
#include <QtXml>
#include <QXmlStreamReader>
#include <QMap>
#include "qvector"
#include "qset.h"
#include <QEvent> 
#include <QListWidget>
#include <QProcess>

using namespace std;
class RDC
{
public:
	RDC();
	void write_sim_xml();
	bool update_rdc_var(QString, QString, QString, QString, QString, QString, QString, QString);
	void add_equipment_xml(Equipment* e);
	void rdc_find_node(QDomElement root);
	void xml_var_ds (QString tag, QString str);
	void update_rdc_ds(QString scode, QString sdes, QString sname, QString sdate, QString sopera, QString stime, QString sstime, QString setime, QString ecode, QString labid, QString facname, QString libversion, QString libid, QString data_date, QString data_time,QString data_type );
	void delete_equipment_xml(Equipment* e);

	Equipment* is_equipment_exist(Equipment* e);

	~RDC();
public:
	
	QString NAME;
	QString DESCRIPTION;
	QString OPERATOR;
	QString STARTTIME;
	QString ENDTIME;
	QString CODE;

	QString DATE;
	QString TIME;

	QList<Equipment*>List_Equipment;

	Equipment* delete_rdc_item;
	QString S_PATH;
	QString S_NAME;
	QString	XML_NAME;
	QString XML_PATH;
	QString EQUIPMENT_NAME;
	QString EQUIPMENT_PATH;
	QStringList UNIQ_EQUIPMENT;
	
	

	QString UNIQ_CODE;
	QString CUR_CODE;
	QDomElement * ParentNode;
	QDomElement * SibingNode;
	QListWidgetItem * rdc_del_item;
	bool IS_EQUIPMENT;
	bool IS_ADD;
	
};

