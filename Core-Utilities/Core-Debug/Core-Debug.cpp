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

#include "stdafx.h"
#include "Core-Debug.h"

using namespace std;
using namespace Core;

//Init Error Channel
unsigned int Debug::__activeDebugLevel = 3;
QMutex Debug::__mutex;
ostream Debug::__outStream(cerr.rdbuf());
DebugSingletonEvent Debug::__singletonEventObject;

#include "VERSION.h"

#if defined(_MSC_VER)
#	pragma message("compile \"Core-Debug\" version: "VERSION)
#endif

std::string Core::getDebugVersion()
{
	std::string version = VERSION;
	return version;
}
int Core::getDebugVersionMajor()
{
	std::string version = VERSION;
	return atoi(version.substr(0, version.find_first_of('.')).c_str());
}
int Core::getDebugVersionMinor()
{
	std::string version = VERSION;
	return atoi(version.substr(version.find_first_of('.')+1, version.find_last_of('.')).c_str());
}
int Core::getDebugVersionPatch()
{
	std::string version = VERSION;
	return atoi(version.substr(version.find_last_of('.')+1).c_str());
}

Debug::Debug(void)
{
}
Debug::~Debug(void)
{
}

//*******************************
//*** Static Member Functions ***
//*******************************
void Debug::setDebugLevel(unsigned int debugLevel)
{
	Debug::__activeDebugLevel = debugLevel;
}
unsigned int Debug::getDebugLevel()
{
	return Debug::__activeDebugLevel;
}
bool Debug::writeErrorInfosToFile(string path)
{
	ostream tempOut(Debug::__outStream.rdbuf());
	filebuf *fileBuffer = new filebuf();
	fileBuffer->open(path.c_str(), ios::out);
	if (!fileBuffer->is_open())
	{
		//Error
		tempOut << "Can't open the Error Output File!" << endl;
		tempOut << "Path: " << path << endl;
		return false;
	}

	Debug::__outStream.rdbuf(fileBuffer);
	tempOut.rdbuf(fileBuffer);
	tempOut << "*******************************\n   " << Debug::getDateAndTime() << "*******************************" << endl;
	tempOut << endl;
	return true;
}
void Debug::writeErrorInfosToCerr()
{
	//define Error Channel
	Debug::__outStream.rdbuf(cerr.rdbuf());
}
std::string Debug::getDateAndTime()
{
	time_t tempTime;
	time(&tempTime);
	string timeString = ctime(&tempTime);
	timeString.erase(timeString.end()-1, timeString.end());
	return timeString;
}

//************************
//*** Member Functions ***
//************************
void Debug::writeDebugMessage(string message, unsigned int debugLevel/*=1*/, string className/*=""*/, string functionName/*=""*/, string objectName/*=""*/)
{
	ErrorMessage errorMessage;

	errorMessage.className = className;
	errorMessage.functionName = functionName;
	errorMessage.objectName = objectName;
	errorMessage.debugLevel = debugLevel;
	errorMessage.message = message;
	errorMessage.timeAndDate = "";

	Debug::writeDebugMessage(errorMessage);
}
void Debug::writeDebugMessage(ErrorMessage errorMessage)
{
	ostream tempOut(Debug::__outStream.rdbuf());
	
	if (Debug::__activeDebugLevel >= errorMessage.debugLevel)
	{
		Debug::prepareMessage(errorMessage);

		Debug::__mutex.lock();
		//Write Date and Time
		tempOut << "*** " << Debug::getDateAndTime() << " ***" << endl;

		if (errorMessage.objectName.compare("") != 0)
			tempOut << "ObjectName: " << errorMessage.objectName << endl;
		if (errorMessage.className.compare("") != 0)
			tempOut << "ClassName: " << errorMessage.className << endl;
		if (errorMessage.functionName.compare("") != 0)
			tempOut << "FunctionName: " << errorMessage.functionName << endl;
		if (errorMessage.message.compare("") != 0)
			tempOut << Debug::getMsgKindText(&errorMessage) << ": " << errorMessage.message << endl;
		else
			tempOut << Debug::getMsgKindText(&errorMessage) << ": " << "No Message!" << endl;
		tempOut << endl;

		errorMessage.timeAndDate = Debug::getDateAndTime();
		Debug::__mutex.unlock();
		Debug::__singletonEventObject._sendNewErrorEvent(errorMessage);
	}
}
void Debug::prepareMessage(ErrorMessage &errorMessage)
{
	//Erase the RETURN to the end of the Msg.
	size_t pos = errorMessage.message.find_last_of("\n");
	if (pos != std::string::npos)
	{
		if (pos+1 >= errorMessage.message.size())
		{
			errorMessage.message.erase(pos);
		}
	}
	pos = errorMessage.objectName.find_last_of("\n");
	if (pos != std::string::npos)
	{
		if (pos+1 >= errorMessage.objectName.size())
		{
			errorMessage.objectName.erase(pos);
		}
	}
	pos = errorMessage.className.find_last_of("\n");
	if (pos != std::string::npos)
	{
		if (pos+1 >= errorMessage.className.size())
		{
			errorMessage.className.erase(pos);
		}
	}
	pos = errorMessage.functionName.find_last_of("\n");
	if (pos != std::string::npos)
	{
		if (pos+1 >= errorMessage.functionName.size())
		{
			errorMessage.functionName.erase(pos);
		}
	}
}
std::string Debug::getMsgKindText(ErrorMessage *errorMessage)
{
	std::string text;

	if (errorMessage->debugLevel <= 1)
		text = "ErrorMsg";
	else if (errorMessage->debugLevel == 2)
		text = "WarningMsg";
	else if (errorMessage->debugLevel == 3)
		text = "InfoMsg";
	else if (errorMessage->debugLevel >= 4)
		text = "DebugMsg";

	return text;
}


DebugSingletonEvent::DebugSingletonEvent(void)
{}
DebugSingletonEvent::~DebugSingletonEvent(void)
{}
void DebugSingletonEvent::_sendNewErrorEvent(ErrorMessage message)
{
	emit this->_signalNewErrorMessage(message);
}