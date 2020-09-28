#include "otherFunctions.h"

#include <windows.h>
#include <Lmcons.h>
#include <QStringList>
#include <QProcessEnvironment>
#include <QDir>


///////////////////////////////////////////////////////////////////////////////
//  GET WINDOWS USER NAME
//  GET TEMP DIR PATH
///////////////////////////////////////////////////////////////////////////////


// GET WINDOWS USER NAME //////////////////////////////////////////////////////
QString getWindowsUserName()
{
	TCHAR username[UNLEN+1];
	DWORD size = UNLEN+1;
	GetUserName((TCHAR*)username,&size);
	
	QString str;
	for (unsigned int i=0; i<size-1; ++i) {str += username[i];}
	return str;
}

// GET TEMP DIR PATH //////////////////////////////////////////////////////////
QString getTempDirPath()
{
	QProcessEnvironment envt = QProcessEnvironment::systemEnvironment();
	
	if (envt.contains("TEMP"))
	{
		QString tempDirPathStr = envt.value("TEMP");
		//tempDirPathStr.replace('\\','/');
		if (QDir{tempDirPathStr}.exists()) {return tempDirPathStr;}
	}
	
	if (envt.contains("TMP"))
	{
		QString tempDirPathStr = envt.value("TMP");
		//tempDirPathStr.replace('\\','/');
		if (QDir{tempDirPathStr}.exists()) {return tempDirPathStr;}
	}
	
	return {};
}

