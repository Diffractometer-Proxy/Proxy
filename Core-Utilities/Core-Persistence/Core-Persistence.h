#ifndef COREPERSISTENCE_H
#define COREPERSISTENCE_H

#ifndef _MSC_VER
#	include "stdafx.h"
#endif

namespace Core
{

std::string getPersistenceVersion();
int getPersistenceVersionMajor();
int getPersistenceVersionMinor();
int getPersistenceVersionPatch();

class Persistence
{
public:
	Persistence();
	~Persistence();

	void setProgramName(QString name);
	void setProgramVersion(QString version);
	void setProgramVersion(double version);
	QString getProgramName();
	QString getProgramVersion();

	bool storeInformation(const QString &information, QString fileName);
	bool storeByteArray(const QByteArray &data, QString fileName);
	bool loadInforamtion(QString &information, QString fileName);
	bool loadByteArray(QByteArray &data, QString fileName);

	QString getPath(QString fileName);
	QString getDirectory();

private:
	QString _programName;
	QString _programVersion;
};

} //End Namespace Core

#endif // COREPERSISTENCE_H
