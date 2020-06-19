#include "QtGuiApplication.h"

using namespace std;
QtGuiApplication::QtGuiApplication(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	MDC_Init();
	RDC_Init();
	ui.treeWidget->installEventFilter(this);
	ui.RTree->installEventFilter(this);
}
void QtGuiApplication::RDC_Init()
{
	//按钮回调函数
	connect(ui.Rbtn_new,&QPushButton::clicked,this,&QtGuiApplication::save_rdc);
	connect(ui.Rbtn_open,&QPushButton::clicked,this,&QtGuiApplication::open_rdc);
	connect(ui.btn_add,&QPushButton::clicked,this,&QtGuiApplication::create_equiment);
	connect(ui.btn_delete,&QPushButton::clicked,this,&QtGuiApplication::rdc_delete_data);
	connect(ui.RListData, SIGNAL(itemPressed(QListWidgetItem *)), this, SLOT(rdc_del_data(QListWidgetItem *)));
	connect(ui.btn_modify, &QPushButton::clicked, this, &QtGuiApplication::rdc_save_current);
	connect(ui.btn_modify_2, &QPushButton::clicked, this, &QtGuiApplication::rdc_save_current);
	connect(ui.btn_modify_3, &QPushButton::clicked, this, &QtGuiApplication::rdc_save_current);
	connect(ui.btn_zip_2,&QPushButton::clicked,this,&QtGuiApplication::rdc_zip);
	connect(ui.delete_equip, &QPushButton::clicked, this, &QtGuiApplication::delete_euipment);
	//当选择的equipment变化时，刷新界面
	connect(ui.cm_equipment, QOverload<int>::of(&QComboBox::currentIndexChanged),
		[=](int index) {
		ui.btn_delete->setDisabled(true);
		for (int i = 0; i < RDC_FILE.List_Equipment.size(); i++)
		{
			if (ui.cm_equipment->currentText() == RDC_FILE.List_Equipment[i]->LAB_ID)
			{
				ui.REditcode->setText(RDC_FILE.List_Equipment[i]->CODE);
				ui.comboBox->setCurrentText(RDC_FILE.List_Equipment[i]->LAB_ID);
				ui.REditFacName->setText(RDC_FILE.List_Equipment[i]->FAC_NAME);
				ui.REditLibID->setText(RDC_FILE.List_Equipment[i]->LIB_ID);
				ui.REditLibVersion->setText(RDC_FILE.List_Equipment[i]->LIB_VERSION);

				ui.RListData->clear();
				for (int j = 0; j < RDC_FILE.List_Equipment[i]->list_data.size(); j++)
				{
					ui.RListData->addItem(RDC_FILE.List_Equipment[i]->list_data[j].second);
				}
				//如果没有任何csv数据则返回
				if (RDC_FILE.List_Equipment[i]->list_data.isEmpty())
				{
					ui.REditLibID_2->setText("0");
					ui.REditLibID_3->setText("0");
					return;
				}
				//根据csv数据的时间填写在对应的控件上
				QString sub_str = RDC_FILE.List_Equipment[i]->list_data.first().second;
				if (sub_str.isEmpty())
					break;
				sub_str.remove(".csv");
				QStringList ql = sub_str.split('-');
				for (int i = 0; i < ql.size(); i++)
				{
					if (ql[i].size() == 8)
					{
						ui.REditLibID_2->setText(ql[i]);
					}
					else if (ql[i].size() == 6)
						ui.REditLibID_3->setText(ql[i]); 
				}
				break;
			}

		}
	});
	//设置删除按钮失能
	ui.btn_delete->setDisabled(true);
	RDC_FILE.rdc_del_item = NULL;
	RDC_FILE.IS_EQUIPMENT = false;


}
void QtGuiApplication::save_rdc()
{
	QDir qd;
	QString path;
	RDC_FILE.List_Equipment.clear();//清除list_equipment

	//检查输入控件不为空
	if (!RDC_FILE.update_rdc_var(ui.REditName_2->text(), ui.REditDescrip->text(), ui.REditOperator->text(), ui.REditSTime->text(), ui.REditETime->text(), ui.REditLibID_5->text(), ui.REditLibID_4->text(), ui.REditCode->text()))
	{
		QMessageBox::about(this, "warning", "input can't be empty!");
		return;
	}
	//选择文件保存路径
	path = QFileDialog::getSaveFileName(this, "save", "../", "all(*)");
	if (path == NULL)
		return;
	//路径预处理
	int len = path.toStdString().find_last_of('/');
	path = QString::fromStdString(path.toStdString().substr(0, len + 1));

	//获取rdc文件名和路径
	RDC_FILE.S_NAME.sprintf("%s-%s-%s",ui.REditCode->text().toStdString().data(),ui.REditLibID_5->text().toStdString().data(),ui.REditLibID_4->text().toStdString().data());
	path.append(RDC_FILE.S_NAME);
	RDC_FILE.S_PATH = path;
	RDC_FILE.XML_NAME = "SUTRecDataCatalog.xml";
	RDC_FILE.XML_PATH = path.append(QString("/%1").arg(RDC_FILE.XML_NAME));

	//创建rdc文件夹
	qDebug() << RDC_FILE.S_PATH << endl;
	if (!qd.exists(RDC_FILE.S_PATH))
	{
		if (qd.mkdir(RDC_FILE.S_PATH))
		{
		}
		else
			return;
	}
	//写rdc的simulation属性到xml
	RDC_FILE.IS_ADD = false;
	RDC_FILE.write_sim_xml();

}




void QtGuiApplication::open_rdc()
{
	//获取rdc的 xml文件路径
	QString oldpath = QFileDialog::getOpenFileName(this, "Select", "../", tr("xml(*.xml)"));
	if (oldpath.isEmpty())
		return;
	QFile xml_file(oldpath);
	int len = oldpath.toStdString().find_last_of('/');
	RDC_FILE.S_PATH = QString::fromStdString(oldpath.toStdString().substr(0,len));
	RDC_FILE.XML_PATH = oldpath;
	//若多次打开文件则要设置为false
	RDC_FILE.IS_EQUIPMENT = false;
	
	xml_file.open(QIODevice::ReadWrite);
	RDC_FILE.rdc_del_item = NULL;
	RDC_FILE.IS_ADD = false;
	//预处理获取RDC文件的CODE
	int len_1 = RDC_FILE.S_PATH.toStdString().find_first_of('-');
	int len_2 = RDC_FILE.S_PATH.toStdString().find_last_of('/');
	RDC_FILE.CODE = QString::fromStdString(RDC_FILE.S_PATH.toStdString().substr(len_2+1, len_1-len_2-1));
	//预处理根据xml文件路径获得对应的创建时间
	QStringList ql = RDC_FILE.S_PATH.split('-');
	for (int i = 0; i < ql.size(); i++)
	{
		if (ql[i].size() == 8)
		{
			RDC_FILE.DATE = ql[i];
		}
		else if (ql[i].size() == 6)
			RDC_FILE.TIME = ql[i];
	}


	//获得xml句柄
	QDomDocument d;
	if (!d.setContent(&xml_file))
	{
		xml_file.close();
		return;
	}
	//没打开一次就清掉上次的数据结构
	RDC_FILE.List_Equipment.clear();

	xml_file.close();
	//依次读取xml节点信息并动态创建对应的platform
	QDomElement root = d.documentElement();//根节点
	QString nodeName = root.nodeName();

	RDC_FILE.rdc_find_node(root);
	
	rdc_ds_ui();
	RDC_FILE.S_NAME.sprintf("%s-%s-%s", ui.REditCode->text().toStdString().data(), ui.REditLibID_5->text().toStdString().data(), ui.REditLibID_4->text().toStdString().data());
	// 添加数据结构里所有的LABID到对应控件上
	ui.cm_equipment->clear();
	for (int i = 0; i < RDC_FILE.List_Equipment.size(); i++)
	{
		ui.cm_equipment->addItem(RDC_FILE.List_Equipment[i]->LAB_ID);
	}


}

void QtGuiApplication::create_equiment()
{
	//选择要添加的csv文件
	QString cu_text;
	cu_text.sprintf("%s(*.csv)", ui.Rcm_data->currentText().toStdString().data());
	QString fp = QFileDialog::getOpenFileName(this, "select", "../", cu_text);
	//取消选择就返回
	if (fp.isEmpty())
	{
		return;
	}
	//设置待删除csv文件的指针为NULL
	RDC_FILE.rdc_del_item = NULL;
	//scv文件的类型的简写转换根据文档要求
	QString short_type;
	if (ui.Rcm_data->currentText() == "Track")
		short_type = "TRK";
	else if (ui.Rcm_data->currentText() == "Trackidentification")
		short_type = "ID";
	else if (ui.Rcm_data->currentText() == "TrackPRI")
		short_type = "PRI";
	else if (ui.Rcm_data->currentText() == "TrackPW")
		short_type = "PW";
	else if (ui.Rcm_data->currentText() == "TrackRF")
		short_type = "RF";
	else if (ui.Rcm_data->currentText() == "Event")
		short_type = "EVE";
	//清楚csv文件的日期
	ui.REditLibID_2->clear();
	ui.REditLibID_3->clear();
	//如果创建的plat存在未填写的内容就提示并返回
	if (ui.REditcode->text().isEmpty()|| ui.REditFacName->text().isEmpty() || ui.REditLibVersion->text().isEmpty() || ui.REditLibID->text().isEmpty() || ui.REditLibID_2->text().isEmpty() || ui.REditLibID_3->text().isEmpty())
	{
		QMessageBox::about(NULL,"warning","input is empty still exist");
		return;
	}
	//设置添加csv标识为true
	RDC_FILE.IS_ADD = true;
	//更新数据结构和xml
	update_rdc_all(ui.REditCode->text(), ui.REditDescrip->text(), ui.REditName_2->text(), ui.REditLibID_5->text(), ui.REditOperator->text(), ui.REditLibID_4->text(), ui.REditSTime->text(), ui.REditETime->text(), ui.REditcode->text(), ui.comboBox->currentText(), ui.REditFacName->text(), ui.REditLibVersion->text(), ui.REditLibID->text(), ui.REditLibID_2->text(),ui.REditLibID_3->text(), ui.Rcm_data->currentText());
	//跟新ui界面
	QString data_name = (QString("%1-%2-%3-%4.csv").arg(short_type).arg(ui.comboBox->currentText()).arg(ui.REditLibID_2->text()).arg(ui.REditLibID_3->text()));
	QString data_path;
	data_path = QString("%1/%2").arg(RDC_FILE.S_PATH).arg(data_name);
	ui.RListData->clear();
	for (int j = 0; j < RDC_FILE.List_Equipment.size(); j++)
	{
		if (RDC_FILE.List_Equipment[j]->LAB_ID ==   ui.cm_equipment->currentText())
		{	
			for(int i = 0;i< RDC_FILE.List_Equipment[j]->list_data.size();i++)
			ui.RListData->addItem(RDC_FILE.List_Equipment[j]->list_data[i].second);
			RDC_FILE.CUR_CODE = RDC_FILE.List_Equipment[j]->LAB_ID;
			break;
		}
	}
	ui.cm_equipment->clear();
	for (int i = 0; i < RDC_FILE.List_Equipment.size(); i++)
	{
		ui.cm_equipment->addItem(RDC_FILE.List_Equipment[i]->LAB_ID);
	}
	ui.cm_equipment->setCurrentText(RDC_FILE.CUR_CODE);
	//创建csv文件到对应路径
	QFile old_file(fp);
	if (!QFile::exists(data_path))
	{
		if (!old_file.copy(data_path))
			return;
	}

}

void QtGuiApplication::rdc_ds_ui()
{
	//打开xml文件后填写simulation的属性到相应控件
	ui.REditCode->setText(RDC_FILE.CODE);
	ui.REditDescrip->setText(RDC_FILE.DESCRIPTION);
	ui.REditName_2->setText(RDC_FILE.NAME);
	ui.REditOperator->setText(RDC_FILE.OPERATOR);
	ui.REditSTime->setText(RDC_FILE.STARTTIME);
	ui.REditETime->setText(RDC_FILE.ENDTIME);
	ui.REditLibID_5->setText(RDC_FILE.DATE);
	ui.REditLibID_4->setText(RDC_FILE.TIME);

	ui.REditcode->setText(RDC_FILE.List_Equipment.first()->CODE);
	ui.comboBox->setCurrentText(RDC_FILE.List_Equipment.first()->LAB_ID);
	ui.REditLibID->setText(RDC_FILE.List_Equipment.first()->LIB_ID);
	ui.REditFacName->setText(RDC_FILE.List_Equipment.first()->FAC_NAME);
	ui.REditLibVersion->setText(RDC_FILE.List_Equipment.first()->LIB_VERSION);
	//如果没有csv文件返回
	if (RDC_FILE.List_Equipment.first()->list_data.isEmpty())
		return;
	//填写csv文件的创建时间到指定控件
	QString data_name = RDC_FILE.List_Equipment.first()->list_data.first().second;
	data_name = QString::fromStdString(data_name.toStdString().substr(0, data_name.toStdString().find_last_of('.')));
	
	int len = data_name.toStdString().find_last_of('-');
	int max_len = data_name.toStdString().length();
	QString str1 = QString::fromStdString(data_name.toStdString().substr(0,len ));
	QString time = QString::fromStdString(data_name.toStdString().substr(len+1,max_len));
	len = str1.toStdString().find_last_of('-');
	max_len = str1.toStdString().length();
	QString date = QString::fromStdString(str1.toStdString().substr(len+1, max_len));
	ui.REditLibID_2->setText(date);
	ui.REditLibID_3->setText(time);
	//更新ui界面的显示csv的列表控件
	ui.RListData->clear();

	for (int i = 0; i < RDC_FILE.List_Equipment.first()->list_data.size(); i++)
	{

		ui.RListData->addItem(RDC_FILE.List_Equipment.first()->list_data[i].second);
	}


}

void QtGuiApplication::delete_euipment()
{
	//遍历equipment链表找到要删除的equipment
	for (int i = 0; i < RDC_FILE.List_Equipment.size(); i++)
	{
		if (ui.cm_equipment->currentText() == RDC_FILE.List_Equipment[i]->LAB_ID)
		{
			RDC_FILE.delete_rdc_item = RDC_FILE.List_Equipment[i];
			break;
		}
		RDC_FILE.delete_rdc_item = NULL;
	}

	if (RDC_FILE.delete_rdc_item != NULL)
	{
		//在xml里删除对应的Equipment节点，并从当前数据结构里删除掉
		RDC_FILE.delete_equipment_xml(RDC_FILE.delete_rdc_item);
		RDC_FILE.List_Equipment.removeOne(RDC_FILE.delete_rdc_item);

		
		ui.cm_equipment->clear();
		for (int i = 0; i < RDC_FILE.List_Equipment.size(); i++)
		{
			ui.cm_equipment->addItem(RDC_FILE.List_Equipment[i]->LAB_ID);
		}
		//删除对应文件
		QDir qdir(RDC_FILE.S_PATH);
		QFileInfoList list_file = qdir.entryInfoList();
		for (int i = 0; i < list_file.size(); i++)
		{

			QFileInfo qf = list_file[i];
			for (int j = 0; j < RDC_FILE.delete_rdc_item->list_data.size(); j++)
			{
				if (qf.fileName() == RDC_FILE.delete_rdc_item->list_data[j].second)
				{
					QFile::remove(qf.absoluteFilePath());
				}
			}
		}
		//释放内存
		free(RDC_FILE.delete_rdc_item);
		RDC_FILE.delete_rdc_item = NULL;

	}
	return;


}

void QtGuiApplication::rdc_del_data(QListWidgetItem *P)
{
	//设按钮为使能，可以点击触发删除事件
	ui.btn_delete->setDisabled(false);
	//得到待删除csv的指针
	RDC_FILE.rdc_del_item = P;
}


void QtGuiApplication::rdc_delete_data()
{
	//提示是否要删除？
	if (QMessageBox::warning(this, "warning", "are you sure to delete data item?", QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Cancel)
		return;
	//删除以后更新数据结构并把删除按钮设为失能
	update_rdc_all(ui.REditCode->text(), ui.REditDescrip->text(), ui.REditName_2->text(), ui.REditLibID_5->text(), ui.REditOperator->text(), ui.REditLibID_4->text(), ui.REditSTime->text(), ui.REditETime->text(), ui.REditcode->text(), ui.comboBox->currentText(), ui.REditFacName->text(), ui.REditLibVersion->text(), ui.REditLibID->text(), ui.REditLibID_2->text(), ui.REditLibID_3->text(),ui.Rcm_data->currentText());
	ui.btn_delete->setDisabled(true);
	
}

void QtGuiApplication::update_rdc_all(QString scode, QString sdes, QString sname, QString sdate, QString sopera, QString stime, QString sstime, QString setime, QString ecode, QString labid, QString facname, QString libversion, QString libid, QString data_date, QString data_time, QString data_type)
{
	//当对rdc的某一属性进行修改时或添加新的equipment更新数据结构
	RDC_FILE.update_rdc_ds( scode,  sdes,  sname,  sdate,  sopera,  stime,  sstime,  setime,  ecode,  labid,  facname,  libversion,  libid,  data_date,  data_time,data_type);
	//同时跟新rdc的界面
	update_rdc_ui();
}

void QtGuiApplication::update_rdc_ui()
{
	//遍历链表找到与ui界面对应的LAB_ID
	for (int i = 0; i < RDC_FILE.List_Equipment.size(); i++)
	{
		if (RDC_FILE.List_Equipment[i]->LAB_ID == ui.cm_equipment->currentText())
		{
			
			for (int j = 0; j < RDC_FILE.List_Equipment[i]->list_data.size(); j++)
			{
				ui.RListData->addItem(RDC_FILE.List_Equipment[i]->list_data[j].second);
			}

		}
	}

//更新选择equipment的控件
ui.cm_equipment->clear();
QStringList qsl;
if (RDC_FILE.List_Equipment.isEmpty())
return;
RDC_FILE.CUR_CODE = QString("%1").arg(ui.comboBox->currentText()); 

for (int i = 0; i < RDC_FILE.List_Equipment.size(); i++)
{
	qsl.append(RDC_FILE.List_Equipment[i]->LAB_ID);

}
ui.cm_equipment->addItems(qsl);
//保持当前界面仍显示修改前的equipment
ui.cm_equipment->setCurrentText(RDC_FILE.CUR_CODE);





}
//更新rdc的数据结构
void QtGuiApplication::rdc_save_current()
{

	update_rdc_all(ui.REditCode->text(), ui.REditDescrip->text(), ui.REditName_2->text(), ui.REditLibID_5->text(), ui.REditOperator->text(), ui.REditLibID_4->text(), ui.REditSTime->text(), ui.REditETime->text(), ui.REditcode->text(), ui.comboBox->currentText(), ui.REditFacName->text(), ui.REditLibVersion->text(), ui.REditLibID->text(), ui.REditLibID_2->text(), ui.REditLibID_3->text(), ui.Rcm_data->currentText());
}

//rdc 压缩文件
void QtGuiApplication::rdc_zip()
{
	QProcess p(0);
	QFile mfile("../WinRAR.exe");
	QString filename;

	//压缩的文件是rdc文件路径
	filename = RDC_FILE.S_PATH;
	//指令是调用winrar
	QString command = mfile.fileName();
	//选择要生成的压缩文件格式，取消则返回
	QString filepath = QFileDialog::getSaveFileName(this, "Select", "../", "zip(*.zip);;rar(*.rar);;tar(*.tar)");
	if (filepath == NULL)
		return;
	
	QStringList args;
	args.append("a");
	args.append("-k");
	args.append("-r");
	args.append("-s");
	args.append("-m1");
	args.append(filepath);
	args.append(filename);
	p.execute(command, args);
	mfile.close();
}

void QtGuiApplication::MDC_Init()
{
	//按钮对应槽函数的响应
	connect(ui.btn_save, &QPushButton::clicked, this, &QtGuiApplication::save_file);
	connect(ui.btn_open, &QPushButton::clicked, this, &QtGuiApplication::open_file);
	connect(ui.data_3, &QPushButton::clicked, this, &QtGuiApplication::create_plat);
	connect(ui.btn_create, &QPushButton::clicked, this, &QtGuiApplication::Update_DS);
	connect(ui.btn_create_2, &QPushButton::clicked, this, &QtGuiApplication::Update_DS);
	connect(ui.btn_create_3, &QPushButton::clicked, this, &QtGuiApplication::Update_DS);
	connect(ui.PLAT_DELETE, &QPushButton::clicked, this, &QtGuiApplication::delete_plat);
	connect(ui.pushButton, &QPushButton::clicked, this, &QtGuiApplication::del_process);
	connect(ui.btn_zip, &QPushButton::clicked, this, &QtGuiApplication::ZipFile);
	connect(ui.listWidget, SIGNAL(itemPressed(QListWidgetItem *)), this, SLOT(del_data(QListWidgetItem *)));
	//当combox选择变化时更新ui界面
	connect(ui.cm_plat, QOverload<int>::of(&QComboBox::currentIndexChanged),
		[=](int index) {
		ui.pushButton->setDisabled(true);
		for (int i = 0; i < List_pform.size(); i++)
		{
			if (ui.cm_plat->itemText(index) == List_pform[i]->code)
			{
				ui.linecode_4->setText(List_pform[i]->code);
				ui.platform_name_3->setText(List_pform[i]->p_name);
				ui.lineEdit_12->setText(List_pform[i]->f_name);
				ui.lineEdit_11->setText(List_pform[i]->id);
				ui.cm_type_3->setCurrentText(List_pform[i]->type);
				ui.cm_role_3->setCurrentText(List_pform[i]->role);
				ui.cm_task_3->setCurrentText(List_pform[i]->task);
				ui.lineEdit_9->setText(List_pform[i]->category);
				ui.lineEdit_10->setText(List_pform[i]->entityid);
				ui.listWidget->clear();
				for (int j = 0; j < List_pform[i]->q_data.size(); j++)
				{
					ui.listWidget->addItem(List_pform[i]->q_data[j].second);
				}
			}

		}
	});
	//初始化ui
	ini_ui();
	ui.pushButton->setDisabled(true);
	is_plat = false;//if plat is not be find;
	del_item = NULL;
	
}

QFileInfoList QtGuiApplication::find_file_node(QTreeWidgetItem * root, QString path)
{
	//找到当前path下所有的目录和文件，并添加到ui的树控件里来显示
	QDir dir(path);
	QDir dir_file(path);
	dir_file.setFilter(QDir::Files| QDir::Hidden |QDir::NoSymLinks);
	dir_file.setSorting(QDir::Time);
	QFileInfoList list_file = dir_file.entryInfoList();//获取所有的文件
	for(int i=0;i<list_file.size();i++)
	{
		QFileInfo fileinfo = list_file.at(i);
		QString name = fileinfo.fileName();
		QTreeWidgetItem *child = new QTreeWidgetItem(QStringList()<<name);//挂载子文件到root
		root->addChild(child);
	}
	//递归遍历所有目录的子文件和目录
	QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	QFileInfoList foder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot );

	for (int i = 0; i != foder_list.size(); i++)
	{

		QString namepath = foder_list.at(i).absoluteFilePath();
		QFileInfo file_info = foder_list.at(i);
		QString name = file_info.fileName();
		QTreeWidgetItem *child = new QTreeWidgetItem(QStringList()<<name);
		root->addChild(child);
		QFileInfoList child_file_list = find_file_node(child,namepath);
		file_list.append(child_file_list);
		file_list.append(name);

	}
	return file_list;


}
//ui树控件显示文件
void QtGuiApplication::tree_display(QTreeWidget* tree, QString path)
{
	QTreeWidgetItem *root_node = new QTreeWidgetItem(QStringList() << path);
	tree->addTopLevelItem(root_node);
	find_file_node(root_node, path);

}



void QtGuiApplication::del_process()
{

	if (QMessageBox::warning(this, "warning", "are you sure to delete data item?", QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Cancel)
		return;
	//更新数据结构
	Update_DS();
	ui.pushButton->setDisabled(true);

}

void QtGuiApplication::ini_ui()
{	
	//初始化ui的combox信息
	QStringList ql;
	ql << "NAV" << "TRACK" << "PDM" << "ENV" << "EVE" << "RAW" << "MOP";
	ui.cm_data1_3->addItems(ql);
	ql.clear();
	ql << "unknown" << "air" << "surface" << "subsurface" << "ground";
	ui.cm_type_3->addItems(ql);
	ql.clear();
	ql << "reserved" << "hostile" << "suspect" << "unknown" << "pending" << "assume_friendly" << "neutral" << "friendly";
	ui.cm_role_3->addItems(ql);
	ql.clear();
	ql << "strike" << "ea" << "dca" << "isr" << "ej" << "esc" << "sead" << "soj";
	ui.cm_task_3->addItems(ql);
	ql.clear();
	ql << "SUT1" << "SUT2" << "SUT3";
	ui.comboBox->addItems(ql);
	ql.clear();
	ql << "Track" << "Trackidentification" << "TrackPRI" << "TrackPW" << "TrackRF" << "Event";
	ui.Rcm_data->addItems(ql);
}

void QtGuiApplication::save_file()
{
	//保存mission文件到指定位置
	QDir qd;
	QString path;
	path = QFileDialog::getSaveFileName(this,"sad","../","all(*)");
	if (path == NULL)
		return;
	XML_OF_OPEN = false;
	//mission数据绑定
	m_member();
	int len = path.toStdString().find_last_of('/');
	path =QString::fromStdString( path.toStdString().substr(0,len+1));
	qDebug() << path << endl;
	//路径预处理并根据路径创建文件	
	path.append(M_NAME);
	M_PATH = path;
	M_PATH.append('/');
	
	if (!qd.exists(path))
	{
		if (qd.mkdir(path))
		{
		}
		else
			return;
	}
	//写mission信息到xml
	write_mission_xml();
}
//判断paltform是否已经存在
PlatForm* QtGuiApplication::is_plat_exist(PlatForm* p)
{

	for (int i = 0; i<List_pform.size(); i++)
	{
		if (List_pform[i]->code == p->code)
		{
			return List_pform[i];
		}

	}
	return NULL;
}
//绑定mission数据
void QtGuiApplication::m_member()
{
	code = ui.linecode->text();
	des = ui.linedescrip->text();
	name = ui.linename->text();
	stime = ui.linestime->text();
	etime = ui.linetime->text();
	ato = ui.lineato->text();
	date = ui.linetime_2->text();
	time = ui.linetime_3->text();
	M_NAME.sprintf("%s-%s-%s", code.toStdString().data(), date.toStdString().data(), time.toStdString().data());

}

void QtGuiApplication::open_file()
{
	//打开xml文件并获取路径
	QString oldpath = QFileDialog::getOpenFileName(this, "Select", "../", "xml(*.xml)");
	if (oldpath == NULL)
	{
		return;
	}
	QFile qfile(oldpath);
	QString qfname = qfile.fileName();
	//判断xml文件是否正确
	if (qfname.contains( "SUTRecDataCatalog.xml"))
	{

		QMessageBox::about(this, "warining", "xml file read error,check xml file if correct");
		return;
	}
	//清掉上次的数据
	while(!List_pform.isEmpty())
	{
		List_pform.clear();
	}
	ui.cm_plat->clear();
	ui.cm_data1_3->clear();
	ini_ui();
	is_plat = false;

	//xml路径预处理
	OPEN_XML_PATH = oldpath;
	int len =OPEN_XML_PATH.toStdString().find_last_of('/');
	M_PATH = QString::fromStdString(OPEN_XML_PATH.toStdString().substr(0, len + 1));

	
	//tree_display(ui.treeWidget,M_PATH);

	QFile xml_file(oldpath);
	del_item = NULL;
	XML_OF_OPEN = true;
	//**get file type such as nav,raw/
	xml_file.open(QIODevice::ReadWrite);
	QDomDocument d;
	if (!d.setContent(&xml_file))
	{
		xml_file.close();
		return;
	}
	//根据xml名字添加参数到指定时间控件
	QString sub_str = xml_file.fileName();
	sub_str.remove(".xml");
	QStringList ql = sub_str.split('-');
	for (int i = 0; i < ql.size(); i++)
	{
		if (ql[i].size() == 8)
		{
			date = ql[i];
		}
		else if (ql[i].size() == 9)
			time = ql[i];
	}
	xml_file.close();
	QDomElement root = d.documentElement();//根节点
	QString nodeName = root.nodeName();
	find_node(root);//遍历xml节点
	data2ui();//填写数据到ui控件
	M_NAME.sprintf("%s-%s-%s", code.toStdString().data(), date.toStdString().data(), time.toStdString().data());
	


}

void QtGuiApplication::find_node(QDomElement root)
{
	//遍历xml所有节点
	QDomNode node = root.firstChild();
	while (!node.isNull())
	{
		QDomElement element = node.toElement();
		if (element.isElement())
		{
			QString t = element.tagName();
			QString t1 = element.text();
			if (t == "platform")
			{
				PlatForm * p = new PlatForm();
				List_pform.append(p);
				is_plat = true;
			}
			
			ParentNode = &element.parentNode().toElement();
			SibingNode = &ParentNode->firstChildElement().nextSibling().toElement();
			find_node(element);
			bind_str(t,t1);
		}
		node = node.nextSibling();
	}
	
}
void QtGuiApplication::bind_str(QString tag,QString str)
{
	//绑定xml数据到数据结构
	if (ParentNode->isNull())
		return;

	if (is_plat)
	{
		if (tag == "code")
			List_pform.last()->code = str;
		else if (tag == "platformName")
			List_pform.last()->p_name = str;
		else if (tag == "flightName")
			List_pform.last()->f_name = str;
		else if (tag == "category")
			List_pform.last()->category = str;
		else if (tag == "id")
			List_pform.last()->entityid = str;
		else if (tag == "platformType")
			List_pform.last()->type = str;
		else if (tag == "platformRole")
			List_pform.last()->role = str;
		else if (tag == "missionTask")
			List_pform.last()->task = str;
		else if (tag == "platformPlanId")
			List_pform.last()->id = str;
		else if (tag == "type")
		{
			List_pform.last()->q_data.append(qMakePair(str, SibingNode->text()));
		}

	}
	else
	{
		if (tag == "code")
		{
			code = str;
		}
		else if (tag == "name")
		{
			name = str;
		}
		else if (tag == "description")
		{
			des = str;
		}
		else if (tag == "ato")
		{
			ato = str;
		}
		else if (tag == "seconds" && ParentNode->tagName()=="startTime")
		{
			stime = str;
		}
		else if (tag == "seconds" && ParentNode->tagName() == "endTime")
		{
			etime = str;
		}
	
	}
	
}
void QtGuiApplication::data2ui()
{
	//把数据结构信息输出到ui控件
	QStringList qsl;
	if (List_pform.isEmpty())
		return;
		for (int i = 0; i < List_pform.size(); i++)
	{
		qsl.append(List_pform[i]->code);
		
	}
	ui.cm_plat->addItems(qsl);

	ui.linecode_4->setText(List_pform.first()->code);
	ui.platform_name_3->setText(List_pform.first()->p_name);
	ui.lineEdit_12->setText(List_pform.first()->f_name);
	ui.lineEdit_11->setText(List_pform.first()->id);
	ui.cm_type_3->setCurrentText(List_pform.first()->type);
	ui.cm_role_3->setCurrentText(List_pform.first()->role);
	ui.cm_task_3->setCurrentText(List_pform.first()->task);

	ui.lineEdit_9->setText(List_pform.first()->category);
	ui.lineEdit_10->setText(List_pform.first()->entityid);
	
	ui.listWidget->clear();
	for (int i = 0; i < List_pform.first()->q_data.size(); i++)
	{

		ui.listWidget->addItem(List_pform.first()->q_data[i].second);
	}
	/*mission text*/
	ui.linecode->setText(code);
	ui.linedescrip->setText(des);
	ui.linename->setText(name);
	ui.lineato->setText(ato);
	ui.linestime->setText(stime);
	ui.linetime->setText(etime);
	ui.linetime_2->setText(date);
	ui.linetime_3->setText(time);


}
void QtGuiApplication::del_data(QListWidgetItem * p)
{
	ui.pushButton->setDisabled(false);
	del_item = p;
}


void QtGuiApplication::create_plat()
{
	//增加csv文件到paltform
	QString cu_text;
	cu_text.sprintf("%s(*.csv)", ui.cm_data1_3->currentText().toStdString().data());
	QString fp = QFileDialog::getOpenFileName(this, "select", "../", cu_text);
	if (fp.isEmpty())
	{
		return;
	}
	XML_NAME.sprintf("%s-%s.csv", ui.cm_data1_3->currentText().toStdString().data(), M_NAME.toStdString().data());
	QString plat_path = M_PATH;
	QString plat_name;
	plat_name.sprintf("%s-%s", ui.linecode_4->text().toStdString().data(), M_NAME.toStdString().data());
	plat_path.append(plat_name);
	P_PATH = plat_path;
	P_PATH.append("/");
	//创建paltform的文件夹
	QDir qdp;
	if (!qdp.exists(plat_path))
	{
		qdp.mkdir(plat_path);

	}
	//添加完csv后更新数据结构
	Update_DS();

	//遍历某个platform的csv文件看有无重复，有则返回
		for (int i = 0; i < List_pform.size(); i++)
		{
			if (List_pform[i]->code == ui.linecode_4->text())
			{

				for (int k = 0; k < List_pform[i]->q_data.size() ; k++)
				{
					if (List_pform[i]->q_data[k] == qMakePair(ui.cm_data1_3->currentText(), XML_NAME))
					{
						QMessageBox::about(this, "warning", "can't accept the same data");
						return;
					}

				}

				List_pform[i]->q_data.append(qMakePair(ui.cm_data1_3->currentText(), XML_NAME));
				ui.listWidget->clear();
				for (int j = 0; j < List_pform[i]->q_data.size(); j++)
				{
					ui.listWidget->addItem(List_pform[i]->q_data[j].second);
				}
				break;
			}
		}
	

	//复制csv文件到指定platform 的文件夹
	QString data_path;
	data_path = P_PATH.append(XML_NAME);
	QFile old_file(fp);
	if (!QFile::exists(data_path))
	{
		if (!old_file.copy(data_path))
			return;
	}
	else
	{
		return;
	}
	
	



}




void QtGuiApplication::add_xml_platform(PlatForm *p)
{
	//根据数据结构信息写到xml里面 or 更新
	QFile xml_file((XML_OF_OPEN)?OPEN_XML_PATH: XML_PATH);
	xml_file.open(QIODevice::ReadOnly);
	QDomDocument d;
	QDomElement *pdom;
	QDomElement *pchild;
	if (!d.setContent(&xml_file))
	{
		return;
	}
	xml_file.close();
	QDomElement root = d.documentElement();

	/*find platform tag*/
	for (QDomNode n = root.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		QDomElement ele = n.toElement();
		if (ele.tagName() == "PLATFORMS")
		{
			pdom = &ele;
			break;
		}
	}

	/*point the first platform tag*/
	
	/*add new content to xml*/
	qDebug() << pdom->tagName();
	for (QDomNode n = pdom->firstChild(); !n.isNull(); n = n.nextSibling())
	{
		QDomElement ele = n.firstChild().toElement();
		if (ele.text() == p->code)
		{
			pdom->removeChild(n);
			break;
		}
	}
	/*if new node exist ,remove old and add new*/
	QDomElement platform = d.createElement("platform");
	
	QDomElement code = d.createElement("code");
	QDomText txt_code = d.createTextNode(p->code);
	code.appendChild(txt_code);
	QDomElement p_name = d.createElement("platformName");
	QDomText txt_name = d.createTextNode(p->p_name);
	p_name.appendChild(txt_name);
	QDomElement type = d.createElement("platformType");
	QDomText txt_type = d.createTextNode(p->type);
	type.appendChild(txt_type);
	QDomElement role = d.createElement("platformRole");
	QDomText txt_role = d.createTextNode(p->role);
	role.appendChild(txt_role);
	QDomElement f_name = d.createElement("flightName");
	QDomText txt_f_name = d.createTextNode(p->f_name);
	f_name.appendChild(txt_f_name);
	QDomElement task = d.createElement("missionTask");
	QDomText txt_task = d.createTextNode(p->task);
	task.appendChild(txt_task);
	QDomElement p_id = d.createElement("platformPlanId");
	QDomText txt_p_id = d.createTextNode(p->id);
	p_id.appendChild(txt_p_id);


	QDomElement entity = d.createElement("EntityType");
	QDomElement record = d.createElement("recordData");
	QDomElement cat = d.createElement("category");
	QDomText txt_cat = d.createTextNode(p->category);
	QDomElement entity_id = d.createElement("id");
	QDomText txt_id = d.createTextNode(p->entityid);

	
	/*add data to plat*/

	pdom->appendChild(platform);
	platform.appendChild(code);
	platform.appendChild(p_name);
	platform.appendChild(type);
	platform.appendChild(role);
	platform.appendChild(f_name);
	platform.appendChild(task);
	platform.appendChild(p_id);
	platform.appendChild(entity);
	platform.appendChild(record);



	entity.appendChild(cat);
	entity.appendChild(entity_id);
	cat.appendChild(txt_cat);
	entity_id.appendChild(txt_id);

	for (int i = 0; i<p->q_data.size(); i++)
	{
		QDomElement data = d.createElement("data");
		QDomElement type = d.createElement("type");
		QDomText txt_type = d.createTextNode(p->q_data[i].first);
		QDomElement data_name = d.createElement("name");
		QDomText txt_data = d.createTextNode(p->q_data[i].second);
		record.appendChild(data);
		data.appendChild(type);
		data.appendChild(data_name);
		type.appendChild(txt_type);
		data_name.appendChild(txt_data);

	}

	if (!xml_file.open(QIODevice::Truncate | QIODevice::WriteOnly))
		return;
	QTextStream out(&xml_file);
	d.save(out,4,QDomNode::EncodingFromTextStream);
	xml_file.close();
	
}





void QtGuiApplication::write_mission_xml()
{
	//根据mission属性内容写到xml
	QString path_xml;
	QString xml_name;
	path_xml = M_PATH;
	xml_name.sprintf("CAT-%s.xml",M_NAME.toStdString().data());
	path_xml.append(xml_name);
	QFile xml_file(path_xml);
	XML_PATH = path_xml;
	
	QDomDocument qdoc;
	QDomElement root;

	if (xml_file.exists()==false)
	{

		xml_file.open(QIODevice::WriteOnly | QIODevice::Text);
		root = qdoc.createElement("MISSION");
		qdoc.appendChild(root);
	}
	
	
		QDomElement item1 = qdoc.createElement("code");
		QDomText domtext1 = qdoc.createTextNode(code);
		QDomElement item2 = qdoc.createElement("name");
		QDomText domtext2 = qdoc.createTextNode(name);
		QDomElement item3 = qdoc.createElement("description");
		QDomText domtext3 = qdoc.createTextNode(des);
		QDomElement item4 = qdoc.createElement("ato");
		QDomText domtext4 = qdoc.createTextNode(ato);
		/*starttime tag */
		QDomElement time_s = qdoc.createElement("startTime");
		QDomElement sec = qdoc.createElement("seconds");
		QDomElement msec = qdoc.createElement("microseconds");
		QDomText text_time = qdoc.createTextNode(stime);
		QDomText text_mtime = qdoc.createTextNode("000");
		time_s.appendChild(sec);
		time_s.appendChild(msec);
		sec.appendChild(text_time);
		msec.appendChild(text_mtime);
		/*endtime tag */
		QDomElement time_e = qdoc.createElement("endTime");
		QDomElement esec = qdoc.createElement("seconds");
		QDomElement emsec = qdoc.createElement("microseconds");
		QDomText text_etime = qdoc.createTextNode(etime);
		QDomText text_emtime = qdoc.createTextNode("000");
		time_e.appendChild(esec);
		time_e.appendChild(emsec);
		esec.appendChild(text_etime);
		emsec.appendChild(text_emtime);

		item1.appendChild(domtext1);
		item2.appendChild(domtext2);
		item3.appendChild(domtext3);
		item4.appendChild(domtext4);
		root.appendChild(item1);
		root.appendChild(item2);
		root.appendChild(item3);
		root.appendChild(item4);
		root.appendChild(time_s);
		root.appendChild(time_e);

		QDomElement platform = qdoc.createElement("PLATFORMS");
		root.appendChild(platform);

		QTextStream out(&xml_file);
		qdoc.save(out,4,QDomNode::EncodingFromTextStream);
		xml_file.close();
		cout << "call write xml end" << endl;

}

void QtGuiApplication::Update_DS()
{
	//更新所有的数据结构信息
	PlatForm *pp =NULL;
	code = ui.linecode->text();
	des = ui.linedescrip->text();
	name = ui.linename->text();
	stime = ui.linestime->text();
	etime = ui.linetime->text();
	ato = ui.lineato->text();
	date = ui.linetime_2->text();
	time = ui.linetime_3->text();
	QFile xml_file((XML_OF_OPEN) ? OPEN_XML_PATH : XML_PATH);
	//**get file type such as nav,raw/
	xml_file.open(QIODevice::ReadWrite);
	QDomDocument d;
	if (!d.setContent(&xml_file))
	{
		xml_file.close();
		return;
	}
	xml_file.close();
	bool not_find_plat = true;
	QDomElement root = d.documentElement();//根节点
	QString nodeName = root.nodeName();
	QDomNode node = root.firstChild();
	while (!node.isNull())
	{
		QDomElement element = node.toElement();
		if (element.isElement())
		{
			QString t = element.tagName();
			QString t1 = element.text();

			if (t == "PLATFORMS")
			{
				not_find_plat = false;
			}
			if (not_find_plat == true)
			{
				QString st = element.parentNode().toElement().tagName();
				if (t == "code")
					node.replaceChild(d.createTextNode(code), node.firstChild());
				else if (t == "name" )
					node.replaceChild(d.createTextNode(name), node.firstChild());
				else if (t == "description")
					node.replaceChild(d.createTextNode(des), node.firstChild());
				else if (t == "ato")
					node.replaceChild(d.createTextNode(ato), node.firstChild());
				else if (t == "startTime")
					node.firstChildElement().replaceChild(d.createTextNode(stime), node.firstChildElement().firstChild());
				else if (t == "endTime")
					node.firstChildElement().replaceChild(d.createTextNode(etime), node.firstChildElement().firstChild());
				
			}
			
		}
		node = node.nextSibling();
	}
	if (!xml_file.open(QIODevice::Truncate | QIODevice::WriteOnly))
		return;
	QTextStream out(&xml_file);
	d.save(out, 4, QDomNode::EncodingFromTextStream);
	xml_file.close();


	//new platform若存在则返回已存在的platform
	for (int i = 0; i<List_pform.size(); i++)
	{
		if (List_pform[i]->code == ui.linecode_4->text())
		{
			pp = List_pform[i];
		}

	}
	if (pp == NULL)
	{
		pp = new PlatForm(ui.linecode_4->text());
		List_pform.append(pp);
	}
	pp->id = ui.lineEdit_11->text();
	pp->p_name = ui.platform_name_3->text();
	pp->f_name = ui.lineEdit_12->text();
	pp->type = ui.cm_type_3->currentText();
	pp->role = ui.cm_role_3->currentText();
	pp->task = ui.cm_task_3->currentText();
	pp->category = ui.lineEdit_9->text();
	pp->entityid = ui.lineEdit_10->text();

	QString plat_path = M_PATH;
	QString plat_name;
	plat_name.sprintf("%s-%s", ui.linecode_4->text().toStdString().data(), M_NAME.toStdString().data());
	plat_path.append(plat_name);
	P_PATH = plat_path;
	P_PATH.append("/");
	//创建plattfrom文件夹
	QDir qdp;
	if (!qdp.exists(plat_path))
	{
		qdp.mkdir(plat_path);

	}
	//如果有要删除的文件就删掉对应元素和物理文件
	if (del_item != NULL)
	{

		QString name = del_item->text();
		QString type = name.mid(0, name.toStdString().find_first_of('-'));
		QString data_path = P_PATH.append(name);

		for (int i = 0; i < List_pform.size(); i++)
		{
			if (List_pform[i]->code == ui.linecode_4->text())
			{
				List_pform[i]->q_data.removeOne(qMakePair(type, name));
				ui.listWidget->removeItemWidget(del_item);
				del_item = NULL;
				if (!QFile::remove(data_path))
					return;
				ui.listWidget->clear();
				for (int j = 0; j < List_pform[i]->q_data.size(); j++)
				{
					ui.listWidget->addItem(List_pform[i]->q_data[j].second);
				}
				
			}
		}
	}
	//add platform file
	add_xml_platform(pp);


	//更新ui界面
	ui.cm_plat->clear();
	QStringList qsl;
	if (List_pform.isEmpty())
		return;
	for (int i = 0; i < List_pform.size(); i++)
	{
		qsl.append(List_pform[i]->code);

	}
	ui.cm_plat->addItems(qsl);
	ui.cm_plat->setCurrentText(pp->code);

	



}
void QtGuiApplication::delete_plat()
{
	for (int i = 0; i < List_pform.size(); i++)
	{
		if (ui.cm_plat->currentText() == List_pform[i]->code)
		{
			mdc_delete_plat = List_pform[i];
			break;
		}
		mdc_delete_plat = NULL;
	}

	if (mdc_delete_plat != NULL)
	{
		//在xml里删除对应的plat节点，并从当前数据结构里删除掉
		delete_xml_platform(mdc_delete_plat);
		List_pform.removeOne(mdc_delete_plat);
		QString plat_path = QString("%1%2-%3").arg(M_PATH).arg(mdc_delete_plat->code).arg(M_NAME);
		ui.cm_plat->clear();
		for (int i = 0; i < List_pform.size(); i++)
		{
			ui.cm_plat->addItem(List_pform[i]->code);
		}
		//删除对应文件
		QDir qdir(plat_path);
		QFileInfoList list_file = qdir.entryInfoList();
		for (int i = 0; i < list_file.size(); i++)
		{
			QFileInfo qf = list_file[i];
			QFile::remove(qf.absoluteFilePath());
		}
		//删除对应目录
		
		QString m_path = QString::fromStdString(M_PATH.toStdString().substr(0, M_PATH.size() - 1));
		QDir mdir(m_path);
		QFileInfoList dir_list = mdir.entryInfoList();
		for (int i = 0; i < dir_list.size(); i++)
		{
			QFileInfo qf = dir_list[i];
			if (qf.absoluteFilePath() == plat_path)
			{
				mdir.rmdir(qf.fileName());
				break;
			}
		}



		free(mdc_delete_plat);
		mdc_delete_plat = NULL;

	}

	
	return;

}
void QtGuiApplication::delete_xml_platform(PlatForm * p)
{
	//获取xml文件
	QFile xml_file((XML_OF_OPEN) ? OPEN_XML_PATH : XML_PATH);
	xml_file.open(QIODevice::ReadOnly);
	QDomDocument d;
	QDomElement *pdom;
	QDomElement *pchild;
	if (!d.setContent(&xml_file))
	{
		return;
	}
	xml_file.close();
	QDomElement root = d.documentElement();

	/*find platform tag*/
	for (QDomNode n = root.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		QDomElement ele = n.toElement();
		if (ele.tagName() == "PLATFORMS")
		{
			pdom = &ele;
			break;
		}
	}

	/*point the first platform tag*/

	/*add new content to xml*/
	for (QDomNode n = pdom->firstChild(); !n.isNull(); n = n.nextSibling())
	{
		QDomElement ele = n.firstChild().toElement();
		if (ele.text() == p->code)
		{
			pdom->removeChild(n);
			break;
		}
	}
	if (!xml_file.open(QIODevice::Truncate | QIODevice::WriteOnly))
		return;
	QTextStream out(&xml_file);
	d.save(out, 4, QDomNode::EncodingFromTextStream);
	xml_file.close();




}
void QtGuiApplication::ZipFile()
{
	//压缩文件
	QProcess p(0);
	QString rar_path = QFileDialog::getOpenFileName(this, tr("Open File"),
		"../",
		tr("Winrar (*.exe)"));
	if (rar_path == NULL)
	{
		return;
	}

	QFile mfile(rar_path);
	QString filename;
	
	qDebug() << OPEN_XML_PATH << endl;
	filename = (XML_OF_OPEN)? (OPEN_XML_PATH ): M_PATH;
	if (filename == OPEN_XML_PATH)
	{
		
		int len = filename.toStdString().find_last_of('/');
		filename = QString::fromStdString( filename.toStdString().substr(0,len));
	}


	QString command = mfile.fileName();
	
	QString filepath = QFileDialog::getSaveFileName(this, "Select", "../", "zip(*.zip);;rar(*.rar);;tar(*.tar)");
	
	if (filepath == NULL)
		return;
	qDebug() << filepath << endl;
	QStringList args;
	args.append("a");
	args.append("-k");
	args.append("-r");
	args.append("-s");
	args.append("-m1");
	args.append(filepath);
	args.append(filename);
	p.execute(command,args);
	mfile.close();

}
//鼠标点击显示ui的树控件信息
void  QtGuiApplication::mousePressEvent(QMouseEvent *event)
{
	if (event->type() == QMouseEvent::MouseButtonPress)
	{
		if (!M_PATH.isEmpty())
		{
			ui.treeWidget->clear();
			tree_display(ui.treeWidget, M_PATH);
		}
		if (!RDC_FILE.S_PATH.isEmpty())
		{
			ui.RTree->clear();
			tree_display(ui.RTree, RDC_FILE.S_PATH);
		}
		qDebug() << "trigger" << endl;
		
	}
	
}
bool QtGuiApplication::eventFilter(QObject *obj, QEvent *event)
{
		
			return QObject::eventFilter(obj, event);
		
	
}