#include "RDC.h"



RDC::RDC()
{
}

bool RDC::update_rdc_var(QString q1, QString q2, QString q3, QString q4, QString q5, QString q6, QString q7, QString q8)
{
	NAME = q1;
	DESCRIPTION = q2;
	OPERATOR = q3;
	STARTTIME = q4;
	ENDTIME = q5;
	DATE = q6;
	TIME = q7;
	CODE = q8;
	if (NAME.isEmpty() || DESCRIPTION.isEmpty() || OPERATOR.isEmpty() || STARTTIME.isEmpty() || ENDTIME.isEmpty() || DATE.isEmpty() || CODE.isEmpty() || TIME.isEmpty())
		return false;
	else
		return true;

}

void RDC::add_equipment_xml(Equipment * e)
{
	QFile xml_file(XML_PATH);
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
		if (ele.tagName() == "Equipments")
		{
			pdom = &ele;
			break;
		}
	}

	/*point the first platform tag*/
	QString e_code;
	QString e_labid;
	/*add new content to xml*/
	qDebug() << pdom->tagName();
	for (QDomNode n = pdom->firstChild(); !n.isNull(); n = n.nextSibling())
	{
		QDomElement ele = n.firstChild().nextSibling().toElement();
	
		if (ele.text() == e->LAB_ID)
		{
			pdom->removeChild(n);
			break;
		}

	}
	/*if new node exist ,remove old and add new*/
	QDomElement platform = d.createElement("Equipment");

	QDomElement code = d.createElement("code");
	QDomText txt_code = d.createTextNode(e->CODE);
	code.appendChild(txt_code);
	QDomElement p_name = d.createElement("EquipmentLabID");
	QDomText txt_name = d.createTextNode(e->LAB_ID);
	p_name.appendChild(txt_name);
	QDomElement type = d.createElement("EquipmentFactoryName");
	QDomText txt_type = d.createTextNode(e->FAC_NAME);
	type.appendChild(txt_type);
	QDomElement role = d.createElement("ewLibraryID");
	QDomText txt_role = d.createTextNode(e->LIB_ID);
	role.appendChild(txt_role);
	QDomElement f_name = d.createElement("ewLibraryVersion");
	QDomText txt_f_name = d.createTextNode(e->LIB_VERSION);
	f_name.appendChild(txt_f_name);


	QDomElement record = d.createElement("recordData");




	/*add data to plat*/

	pdom->appendChild(platform);
	platform.appendChild(code);
	platform.appendChild(p_name);
	platform.appendChild(type);
	platform.appendChild(role);
	platform.appendChild(f_name);
	platform.appendChild(record);



	for (int i = 0; i<e->list_data.size(); i++)
	{
	
		QDomElement data = d.createElement("data");
		QDomElement type = d.createElement("type");
		QDomText txt_type = d.createTextNode(e->list_data[i].first);
		QDomElement data_name = d.createElement("name");
		QDomText txt_data = d.createTextNode(e->list_data[i].second);
		record.appendChild(data);
		data.appendChild(type);
		data.appendChild(data_name);
		type.appendChild(txt_type);
		data_name.appendChild(txt_data);

	}

	if (!xml_file.open(QIODevice::Truncate | QIODevice::WriteOnly))
		return;
	QTextStream out(&xml_file);
	d.save(out, 4, QDomNode::EncodingFromTextStream);
	xml_file.close();


}

void RDC::rdc_find_node(QDomElement root)
{
	QDomNode node = root.firstChild();
	while (!node.isNull())
	{
		QDomElement element = node.toElement();
		if (element.isElement())
		{
			QString t = element.tagName();
			QString t1 = element.text();
			if (t == "Equipment")
			{
				Equipment * p = new Equipment();
				List_Equipment.append(p);
				IS_EQUIPMENT = true;
			}

			ParentNode = &element.parentNode().toElement();
			SibingNode = &ParentNode->firstChildElement().nextSibling().toElement();
			rdc_find_node(element);
			xml_var_ds(t, t1);
		}
		node = node.nextSibling();
	}


}

void RDC::xml_var_ds(QString tag, QString str)
{
	if (ParentNode->isNull())
		return;

	if (IS_EQUIPMENT)
	{
		if (tag == "code")
			List_Equipment.last()->CODE = str;
		else if (tag == "EquipmentLabID")
		{
			List_Equipment.last()->LAB_ID = str;
			List_Equipment.last()->UNIQ_CODE = QString("%1-%2").arg(List_Equipment.last()->CODE).arg(str);
		}
		else if (tag == "EquipmentFactoryName")
			List_Equipment.last()->FAC_NAME = str;
		else if (tag == "ewLibraryID")
			List_Equipment.last()->LIB_ID = str;
		else if (tag == "ewLibraryVersion")
			List_Equipment.last()->LIB_VERSION = str;
		else if (tag == "type")
		{
			List_Equipment.last()->list_data.append(qMakePair(str, SibingNode->text()));
		}

	}
	else
	{
		
		if (tag == "name")
		{
			NAME = str;
		}
		else if (tag == "description")
		{
			DESCRIPTION = str;
		}
		else if (tag == "OPERATOR")
		{
			OPERATOR = str;
		}
		else if (tag == "seconds" && ParentNode->tagName()=="startTime")
		{
			STARTTIME = str;
		}
		else if (tag == "seconds" && ParentNode->tagName() == "endTime")
		{
			ENDTIME = str;
		}
	
	}




}

void RDC::update_rdc_ds(QString scode, QString sdes, QString sname, QString sdate, QString sopera, QString stime, QString sstime, QString setime,  QString ecode, QString labid, QString facname, QString libversion, QString libid, QString data_date, QString data_time,QString data_type)
{
	
	NAME = sname;
	DESCRIPTION = sdes;
	OPERATOR = sopera;
	STARTTIME = sstime;
	ENDTIME = setime;
	CODE = scode;



	QFile xml_file(XML_PATH);
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

			if (t == "Equipments")
			{
				not_find_plat = false;
			}
			if (not_find_plat == true)
			{
				QString st = element.parentNode().toElement().tagName();
				if (t == "name")
					node.replaceChild(d.createTextNode(NAME), node.firstChild());
				else if (t == "description")
					node.replaceChild(d.createTextNode(DESCRIPTION), node.firstChild());
				else if (t == "OPERATOR")
					node.replaceChild(d.createTextNode(OPERATOR), node.firstChild());
				else if (t == "startTime")
					node.firstChildElement().replaceChild(d.createTextNode(STARTTIME), node.firstChildElement().firstChild());
				else if (t == "endTime")
					node.firstChildElement().replaceChild(d.createTextNode(ENDTIME), node.firstChildElement().firstChild());

			}

		}
		node = node.nextSibling();
	}
	if (!xml_file.open(QIODevice::Truncate | QIODevice::WriteOnly))
		return;
	QTextStream out(&xml_file);
	d.save(out, 4, QDomNode::EncodingFromTextStream);
	xml_file.close();

	Equipment *pp =NULL;

	for (int i = 0; i<List_Equipment.size(); i++)
	{
		if (List_Equipment[i]->LAB_ID == labid)
		{
			pp = List_Equipment[i];
		}

	}
	if (pp == NULL)
	{
		pp = new Equipment(ecode, labid);
		List_Equipment.append(pp);
	}

	pp->CODE = ecode;
	pp->LAB_ID = labid;
	pp->LIB_ID = libid;
	pp->FAC_NAME = facname;
	pp->LIB_VERSION = libversion;
	pp->data_date = data_date;
	pp->data_time = data_time;
	pp->UNIQ_CODE = QString("%1-%2").arg(pp->CODE).arg(pp->LAB_ID);
	S_NAME.sprintf("%s-%s-%s", scode.toStdString().data(),sdate.toStdString().data(), stime.toStdString().data());
	

	//待用代码
	for (int i = 0; i < List_Equipment.size(); i++)
	{
		UNIQ_EQUIPMENT.append(QString("%1-%2-%3").arg(S_NAME).arg(List_Equipment[i]->CODE).arg(List_Equipment[i]->LAB_ID));
		
	}
	

	//add platform file
	if (IS_ADD)
	{
		QString short_type;
		if (data_type == "Track")
			short_type = "TRK";
		else if (data_type == "Trackidentification")
			short_type = "ID";
		else if (data_type == "TrackPRI")
			short_type = "PRI";
		else if (data_type == "TrackPW")
			short_type = "PW";
		else if (data_type == "TrackRF")
			short_type = "RF";
		else if (data_type == "Event")
			short_type = "EVE";

		QString data_name = (QString("%1-%2-%3-%4.csv").arg(short_type).arg(labid).arg(data_date).arg(data_time));
		for (int i = 0; i < List_Equipment.size(); i++)
		{
			if (List_Equipment[i]->LAB_ID ==labid )
			{
				for (int k = 0; k < List_Equipment[i]->list_data.size(); k++)
				{
					if (List_Equipment[i]->list_data[k] == qMakePair(data_type, data_name))
					{
						QMessageBox::about(NULL, "warning", "can't accept the same data");
						IS_ADD = false;
						return;
					}
				}
				List_Equipment[i]->list_data.append(qMakePair(data_type, data_name));

				break;
			}
		}
		IS_ADD = false;
	}

	if (rdc_del_item != NULL)
	{

		QString name = rdc_del_item->text();
		QString type = name.mid(0, name.toStdString().find_first_of('-'));
		QString total_type;
		if (type == "TRK")
			total_type = "Track";
		else if (type == "ID")
			total_type = "Trackidentification";
		else if (type ==  "PRI")
			total_type = "TrackPRI";
		else if (type == "PW")
			total_type = "TrackPW";
		else if (type == "RF")
			total_type = "TrackRF";
		else if (type == "EVE")
			total_type = "Event";
	
		QString data_path =(QString("%1/%2").arg(S_PATH).arg(name));

		for (int i = 0; i < List_Equipment.size(); i++)
		{
			if (List_Equipment[i]->LAB_ID == labid)
			{
				
				List_Equipment[i]->list_data.removeOne(qMakePair(total_type, name));
				//ui.listWidget->removeItemWidget(del_item);
				rdc_del_item = NULL;
				QFile::remove(data_path);
					

			}
		}
	}
	
	
	add_equipment_xml(pp);
}

void RDC::delete_equipment_xml(Equipment * e)
{
	QFile xml_file(XML_PATH);
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
		if (ele.tagName() == "Equipments")
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
		QDomElement sut_id = ele.nextSibling().toElement();
		if (sut_id.text()== e->LAB_ID)
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







Equipment* RDC::is_equipment_exist(Equipment * e)
{
	for (int i = 0; i<List_Equipment.size(); i++)
	{
		if (List_Equipment[i]->LAB_ID == e->LAB_ID)
		{
			return List_Equipment[i];
		}

	}
	return NULL;

}

void RDC::write_sim_xml()
{
	QString path_xml;
	QString xml_name;
	if (XML_PATH == NULL)
		return;
	QFile xml_file(XML_PATH);
	QDomDocument qdoc;
	QDomElement root;

	if (xml_file.exists() == false)
	{

		xml_file.open(QIODevice::WriteOnly | QIODevice::Text);
		root = qdoc.createElement("Simulation");
		qdoc.appendChild(root);
	}

	QDomElement item2 = qdoc.createElement("name");
	QDomText domtext2 = qdoc.createTextNode(NAME);
	QDomElement item3 = qdoc.createElement("description");
	QDomText domtext3 = qdoc.createTextNode(DESCRIPTION);
	QDomElement item4 = qdoc.createElement("OPERATOR");
	QDomText domtext4 = qdoc.createTextNode(OPERATOR);
	/*starttime tag */
	QDomElement time_s = qdoc.createElement("startTime");
	QDomElement sec = qdoc.createElement("seconds");
	QDomElement msec = qdoc.createElement("microseconds");
	QDomText text_time = qdoc.createTextNode(STARTTIME);
	QDomText text_mtime = qdoc.createTextNode("0");
	time_s.appendChild(sec);
	time_s.appendChild(msec);
	sec.appendChild(text_time);
	msec.appendChild(text_mtime);
	/*endtime tag */
	QDomElement time_e = qdoc.createElement("endTime");
	QDomElement esec = qdoc.createElement("seconds");
	QDomElement emsec = qdoc.createElement("microseconds");
	QDomText text_etime = qdoc.createTextNode(ENDTIME);
	QDomText text_emtime = qdoc.createTextNode("0");
	time_e.appendChild(esec);
	time_e.appendChild(emsec);
	esec.appendChild(text_etime);
	emsec.appendChild(text_emtime);

	
	item2.appendChild(domtext2);
	item3.appendChild(domtext3);
	item4.appendChild(domtext4);
	root.appendChild(item2);
	root.appendChild(item3);
	root.appendChild(item4);
	root.appendChild(time_s);
	root.appendChild(time_e);

	QDomElement equipment = qdoc.createElement("Equipments");
	root.appendChild(equipment);

	QTextStream out(&xml_file);
	qdoc.save(out, 4, QDomNode::EncodingFromTextStream);
	xml_file.close();
	cout << "call write xml end" << endl;

}






RDC::~RDC()
{
}
