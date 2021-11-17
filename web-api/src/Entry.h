#ifndef ENTRY
#define ENTRY


#include <QString>
#include <QDateTime>
#include <QJsonObject>


struct Entry
{
	int id = 0;
	QString description = "";
	int number = 0;
	QDateTime last_modif = {};

	QJsonObject toJsonObject() const
	{
		QJsonObject obj;
		obj.insert("id",this->id);
		obj.insert("description",this->description);
		obj.insert("number",this->number);
		obj.insert("last_modif",this->last_modif.toString("yyyy-MM-dd hh:mm:ss"));
		return obj;
	};

	static Entry fromJsonObject(const QJsonObject &obj)
	{
		if (!obj.contains("id")          || !obj["id"].isDouble())          {return {};}
		if (!obj.contains("description") || !obj["description"].isString()) {return {};}
		if (!obj.contains("number")      || !obj["number"].isDouble())      {return {};}
		if (!obj.contains("last_modif")  || !obj["last_modif"].isString())  {return {};}
		
		Entry e;
		e.id = obj["id"].toInt();
		e.description = obj["description"].toString();
		e.number = obj["number"].toInt();
		e.last_modif = QDateTime::fromString(obj["last_modif"].toString(),"yyyy-MM-dd hh:mm:ss");
		return e;
	};
};


#endif

