/********************************************************************************
* written by 
*
* Christian Randau
* Georg-August-Universitaet Goettingen
* Fakultaet fuer Geowissenschaften und Geologie
* Abteilung Isotopengeologie Aussenstelle MLZ (FRM II)
* Lichtenbergstr. 1
* 85747 Garching
* 
* christian.randau@frm2.tum.de
********************************************************************************/

/********************************************************************************
    Copyright (C) 2016  Christian Randau

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************************/
#ifndef CORE_DEBUG_H
#define CORE_DEBUG_H

#include "Core-Debug_Global.h"
#ifndef _MSC_VER
#	include "stdafx.h"
#endif

namespace Core
{

COREDEBUG_EXPORT std::string getDebugVersion();
COREDEBUG_EXPORT int getDebugVersionMajor();
COREDEBUG_EXPORT int getDebugVersionMinor();
COREDEBUG_EXPORT int getDebugVersionPatch();

typedef struct
{
	std::string message;
	std::string objectName;
	std::string className;
	std::string functionName;
	unsigned int debugLevel;
	std::string timeAndDate;
}ErrorMessage;

class DebugSingletonEvent;

class COREDEBUG_EXPORT Debug
{
public:
	enum eDebugLevel{eDebugLevel_NoMessage=0, eDebugLevel_Error=1, eDebugLevel_Warning=2, eDebugLevel_Info=3, eDebugLevel_Debug=4};

public:
	Debug(void);
	~Debug(void);

//Satic Functions
public:
	static void setDebugLevel(unsigned int debugLevel);
	static unsigned int getDebugLevel();
	static bool writeErrorInfosToFile(std::string path);
	static void writeErrorInfosToCerr();

	static DebugSingletonEvent* getEventSingletonObject() {return &Debug::__singletonEventObject;}

private:
	static std::string getDateAndTime();

//Static Member
private:
	//Current Debug Level
	static unsigned int __activeDebugLevel;

	//Mutex to protect against fragmentation of the messages
	static QMutex __mutex;

	//OutStreams
	static std::ostream __outStream;

	//Singleton Event Object
	static DebugSingletonEvent __singletonEventObject;

protected:
	static void writeDebugMessage(std::string message, unsigned int debugLevel=1, std::string className="", std::string functionName="", std::string objectName="");
	static void writeDebugMessage(ErrorMessage errorMessage);

private:
	static void prepareMessage(ErrorMessage &errorMessage);
	static std::string getMsgKindText(ErrorMessage *errorMessage);
};

class COREDEBUG_EXPORT DebugSingletonEvent : public QObject
{
	Q_OBJECT

	friend class Debug;

private:
	DebugSingletonEvent(void);
	~DebugSingletonEvent(void);

private:
	void _sendNewErrorEvent(ErrorMessage message);

signals:
	void _signalNewErrorMessage(Core::ErrorMessage);
};

} // End of NameSpace Core

#endif //CORE_DEBUG_H
