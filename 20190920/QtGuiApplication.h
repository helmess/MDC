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
	
	void save_file(); //保存mission 文件
	void write_mission_xml();//写mission信息到xml文件
	void open_file();//打开mission的xml文件
	void bind_str(QString tag,QString str);//读 xml数据到list_platform数据结构里保存
	void find_node(QDomElement root);//遍历xml所有节点信息
	void create_plat();//创建platform文件，添加new platform到list_plat
	void add_xml_platform(PlatForm *p);//根据platform的信息写到xml文件
	void m_member();//misson属性赋值
	void data2ui();//ui控件读取数据结构数据
	void ini_ui();//初始化combox等
	void ZipFile();//压缩mission文件
	void Update_DS();//更新数据结构
	void delete_plat();//删除paltform
	void delete_xml_platform(PlatForm *p);//删除platform在xml里的节点
	void del_process();//删除csv文件

	void MDC_Init();//mission初始化
	QFileInfoList find_file_node(QTreeWidgetItem*root ,QString path);//显示路径下所有文件和目录
	void tree_display(QTreeWidget *,QString path);//树控件显示
	PlatForm* is_plat_exist(PlatForm* p);//新建一个platform并判断是否在list_platform里是否存在
protected:
	bool eventFilter(QObject *obj, QEvent *event);
	void mousePressEvent(QMouseEvent *event);//鼠标点击跟新树控件
	public slots:
	void del_data(QListWidgetItem *);//删除mission 的csv文件属于del_process的调用
	void rdc_del_data(QListWidgetItem *);//删除rdc的scv文件


private:
	Ui::QtGuiApplicationClass ui;
	QList < PlatForm*> List_pform;//PlatForm数据结构
	PlatForm* mdc_delete_plat;//待删除的PlatForm
	QDomElement * ParentNode;//xml里的父节点指针
	QDomElement * SibingNode;//xml里的兄弟节点指针
	QListWidgetItem * del_item;//mission里list widget待删除的item
	QString OPEN_XML_PATH;//打开的xml文件路径
	QString CURRENT_XML_PATH;//当前xml文件路径
	bool XML_OF_OPEN;//xml是否打开的标志
	QString XML_PATH;//xml路径
	QString XML_NAME;//xml 名字
	QString M_PATH;//MISSION文件的路径
	QString M_NAME;//mission文件的名字
	QString P_PATH;//platform文件的路径
	
	//mission 属性
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
	void RDC_Init();//RDC初始化
	void save_rdc();//保存rdc文件
	void open_rdc();//打开rdc的xml文件
	void create_equiment();//创建equipment
	void rdc_ds_ui();//把equipment的数据结构信息填到对应ui控件里边
	void delete_euipment();//删除equipment以及对应的csv文件
	void rdc_delete_data();//delete_euipment的调用
	//跟新rdc所有属性
	void update_rdc_all(QString scode, QString sdes, QString sname, QString sdate, QString sopera, QString stime, QString sstime, QString setime, QString ecode, QString labid, QString facname, QString libversion, QString libid, QString data_date, QString data_time,QString data_type);
	//跟新rdc的ui界面
	void update_rdc_ui();
	void rdc_save_current();//保存rdc界面的修改
	void rdc_zip();//压缩rdc文件

private:	
	RDC RDC_FILE;//创建rdc对象


};
