#pragma once

#include <QtWidgets/QMainWindow>
#include "PlatForm.h"
#include "ui_QtGuiApplication.h"
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
#include"RDC.h"
#include "Equipment.h"
#include "QMouseEvent"

class QtGuiApplication : public QMainWindow
{
	Q_OBJECT

public:
	QtGuiApplication(QWidget *parent = Q_NULLPTR);
	
	void save_file(); //����mission �ļ�
	void write_mission_xml();//дmission��Ϣ��xml�ļ�
	void open_file();//��mission��xml�ļ�
	void bind_str(QString tag,QString str);//�� xml���ݵ�list_platform���ݽṹ�ﱣ��
	void find_node(QDomElement root);//����xml���нڵ���Ϣ
	void create_plat();//����platform�ļ������new platform��list_plat
	void add_xml_platform(PlatForm *p);//����platform����Ϣд��xml�ļ�
	void m_member();//misson���Ը�ֵ
	void data2ui();//ui�ؼ���ȡ���ݽṹ����
	void ini_ui();//��ʼ��combox��
	void ZipFile();//ѹ��mission�ļ�
	void Update_DS();//�������ݽṹ
	void delete_plat();//ɾ��paltform
	void delete_xml_platform(PlatForm *p);//ɾ��platform��xml��Ľڵ�
	void del_process();//ɾ��csv�ļ�

	void MDC_Init();//mission��ʼ��
	QFileInfoList find_file_node(QTreeWidgetItem*root ,QString path);//��ʾ·���������ļ���Ŀ¼
	void tree_display(QTreeWidget *,QString path);//���ؼ���ʾ
	PlatForm* is_plat_exist(PlatForm* p);//�½�һ��platform���ж��Ƿ���list_platform���Ƿ����
protected:
	bool eventFilter(QObject *obj, QEvent *event);
	void mousePressEvent(QMouseEvent *event);//������������ؼ�
	public slots:
	void del_data(QListWidgetItem *);//ɾ��mission ��csv�ļ�����del_process�ĵ���
	void rdc_del_data(QListWidgetItem *);//ɾ��rdc��scv�ļ�


private:
	Ui::QtGuiApplicationClass ui;
	QList < PlatForm*> List_pform;//PlatForm���ݽṹ
	PlatForm* mdc_delete_plat;//��ɾ����PlatForm
	QDomElement * ParentNode;//xml��ĸ��ڵ�ָ��
	QDomElement * SibingNode;//xml����ֵܽڵ�ָ��
	QListWidgetItem * del_item;//mission��list widget��ɾ����item
	QString OPEN_XML_PATH;//�򿪵�xml�ļ�·��
	QString CURRENT_XML_PATH;//��ǰxml�ļ�·��
	bool XML_OF_OPEN;//xml�Ƿ�򿪵ı�־
	QString XML_PATH;//xml·��
	QString XML_NAME;//xml ����
	QString M_PATH;//MISSION�ļ���·��
	QString M_NAME;//mission�ļ�������
	QString P_PATH;//platform�ļ���·��
	
	//mission ����
	QString code;
	QString des;
	QString name;
	QString stime;
	QString etime;
	QString ato;
	QString date;
	QString time;
	bool is_plat;
public:
	void RDC_Init();//RDC��ʼ��
	void save_rdc();//����rdc�ļ�
	void open_rdc();//��rdc��xml�ļ�
	void create_equiment();//����equipment
	void rdc_ds_ui();//��equipment�����ݽṹ��Ϣ���Ӧui�ؼ����
	void delete_euipment();//ɾ��equipment�Լ���Ӧ��csv�ļ�
	void rdc_delete_data();//delete_euipment�ĵ���
	//����rdc��������
	void update_rdc_all(QString scode, QString sdes, QString sname, QString sdate, QString sopera, QString stime, QString sstime, QString setime, QString ecode, QString labid, QString facname, QString libversion, QString libid, QString data_date, QString data_time,QString data_type);
	//����rdc��ui����
	void update_rdc_ui();
	void rdc_save_current();//����rdc������޸�
	void rdc_zip();//ѹ��rdc�ļ�

private:	
	RDC RDC_FILE;//����rdc����


};
