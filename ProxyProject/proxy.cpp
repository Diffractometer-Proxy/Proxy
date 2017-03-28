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
*
*
*Sina Mayr
*Technische Universitaet Muenchen
*Physik-Department E21
*James-Franck-Str. 1
*85748 Garching
*Germany
*
*sina.mayr@frm2.tum.de
*
*
*Wolfgang Kreuzpaintner
*Technische Universitaet Muenchen
*Physik-Department E21
*James-Franck-Str. 1
*85748 Garching
*Germany
*
*wolfgang.kreuzpaintner@frm2.tum.de
********************************************************************************/

/********************************************************************************
    Copyright (C) 2016  Christian Randau, Sina Mayr, Wolfgang Kreuzpaintner

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
#include "proxy.h"

Proxy::Proxy(QObject *parent)
	: QObject(parent)
{

}

Proxy::~Proxy()
{

}

void Proxy::run()
{
	//Print version info
	std::cout << "Used Libraries:" << std::endl;
	std::cout << "\tCore-Utilities: Core-Debug version " << Core::getDebugVersion() << std::endl;
	std::cout << "\tCore-Utilities: Core-ASCII-Parser version " << Core::getDebugVersion() << std::endl;
	std::cout << "\tProxyCommunication version " << getProxyCommunicationVersion() << std::endl;
	std::cout << std::endl;

	//Init Debug File
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	std::stringstream ss_path;
	ss_path << "./" << 1900+tm.tm_year << "-" << tm.tm_mon << "-" << tm.tm_mday << "_" << tm.tm_hour << "-" << tm.tm_min << "-" << tm.tm_sec << "_logFile.txt";
	std::cout << "Debug information would be written into the \n\"" << ss_path.str() << "\" file." << std::endl;
	std::cout << std::endl;
	//Set Debug Level
	Core::Debug::setDebugLevel(Core::Debug::eDebugLevel_Warning);
	Core::Debug::writeErrorInfosToFile(ss_path.str());
	std::cout << std::endl;

	//Create global status Table
	GlobalStatus *statusTable = new GlobalStatus();
	statusTable->setToleranz(eTwoTheta, 0.00101);
	statusTable->setToleranz(eTheta, 0.00101);
	statusTable->setToleranz(eChi, 0.00101);
	statusTable->setToleranz(ePhi, 0.00101);
	//statusTable->loadParameterIniFile("parameterAbsorber.ini");

	std::string pathClt = "parameterClientD500.ini";
	ProxyCommunicationClientD500 *communicationObjCltD500 = new ProxyCommunicationClientD500(pathClt);
	communicationObjCltD500->setGlobalStatusTable(statusTable);

	std::string pathSrv = "parameterServerD5000.ini";
	ProxyCommunicationServerD5000 *communicationObjSrvD5000 = new ProxyCommunicationServerD5000(pathSrv);
	communicationObjSrvD5000->setGlobalStatusTable(statusTable);
	communicationObjSrvD5000->setClientInterface(communicationObjCltD500);
}