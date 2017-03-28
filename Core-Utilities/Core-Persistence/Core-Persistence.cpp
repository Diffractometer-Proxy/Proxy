#include "stdafx.h"
#include "Core-Persistence.h"

//Definitions
unsigned int MaxDataLength = 1000000;

using namespace Core;

#include "VERSION.h"

#if defined(_MSC_VER)
#	pragma message("compile \"Core-Persistence\" version: "VERSION)
#endif

std::string Core::getPersistenceVersion()
{
	std::string version = VERSION;
	return version;
}
int Core::getPersistenceVersionMajor()
{
	std::string version = VERSION;
	return atoi(version.substr(0, version.find_first_of('.')).c_str());
}
int Core::getPersistenceVersionMinor()
{
	std::string version = VERSION;
	return atoi(version.substr(version.find_first_of('.')+1, version.find_last_of('.')).c_str());
}
int Core::getPersistenceVersionPatch()
{
	std::string version = VERSION;
	return atoi(version.substr(version.find_last_of('.')+1).c_str());
}

Persistence::Persistence()
{
	this->_programVersion = "x.xx";
}

Persistence::~Persistence()
{

}

void Persistence::setProgramName(QString name)
{
	this->_programName = name;
}
void Persistence::setProgramVersion(QString version)
{
	this->_programVersion = version;
}
void Persistence::setProgramVersion(double version)
{
	this->_programVersion = QString::number(version);
}
QString Persistence::getProgramName()
{
	return this->_programName;
}
QString Persistence::getProgramVersion()
{
	return this->_programVersion;
}

bool Persistence::storeInformation(const QString &information, QString fileName)
{
	QByteArray byteArray;
	byteArray.append(information);
	return this->storeByteArray(byteArray, fileName);
}
bool Persistence::storeByteArray(const QByteArray &data, QString fileName)
{
	QString filePath = this->getPath(fileName);
	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly))
	{
		//Can't create the File
		//ToDo Debug Message
		return  false;
	}
	
	file.write(data);

	file.close();

	return true;
}
bool Persistence::loadInforamtion(QString &information, QString fileName)
{
	QByteArray byteArray;
	bool returnValue = this->loadByteArray(byteArray, fileName);
	information = QString(byteArray);

	return returnValue;
}
bool Persistence::loadByteArray(QByteArray &data, QString fileName)
{
	QString filePath = this->getPath(fileName);
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly))
	{
		//Can't read this File, maybe it dosn't exist
		//ToDo Debug Message
		return false;
	}
	
	data = file.read(MaxDataLength);

	file.close();

	return true;
}
QString Persistence::getPath(QString fileName)
{
	QString filePath;

	filePath = this->getDirectory();
	filePath = filePath  + "/" + fileName;
	
	return filePath;
}
QString Persistence::getDirectory()
{
	QString path;

#if defined(Q_OS_WIN)
	//For Windows
	path = QString::fromLocal8Bit(getenv("APPDATA")) + "/" + this->_programName + "/" + this->_programVersion;
#else
	//For Linux
	path = QDir::homePath() + "/." + this->_programName + "/" + this->_programVersion;
#endif	

	//Check if the Folder exist
	QDir dir(path);
	if (!dir.exists())
	{
		//Create the Folders
		dir.mkpath(path);
	}

	return path;
}