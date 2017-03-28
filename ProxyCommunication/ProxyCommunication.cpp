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
#include "ProxyCommunication.h"

#include "VERSION.h"

#if defined(_MSC_VER)
#	pragma message("compile \"ProxyCommunication\" version: "VERSION)
#endif

std::string getProxyCommunicationVersion()
{
	std::string version = VERSION;
	return version;
}
int getProxyCommunicationVersionMajor()
{
	std::string version = VERSION;
	return atoi(version.substr(0, version.find_first_of('.')).c_str());
}
int getProxyCommunicationVersionMinor()
{
	std::string version = VERSION;
	return atoi(version.substr(version.find_first_of('.')+1, version.find_last_of('.')).c_str());
}
int getProxyCommunicationVersionPatch()
{
	std::string version = VERSION;
	return atoi(version.substr(version.find_last_of('.')+1).c_str());
}

GlobalStatus::GlobalStatus()
{
	this->_isScaning = false;

	this->_currentCountsPerSecond = 0;
	this->_currentExposureTime = 0;
	this->_absorberLevel = 0;

	this->_scanNumberOfRemainingMeasurementsPoints = 0;

	//Set Current and Target Values
	for (unsigned int i=0; i<eAxisNamesSize; i++)
	{
		this->_initializationStatus.push_back(GlobalStatus::eIsNotInitialized);
		this->_isMoving.push_back(false);
		this->_isPreparingMove.push_back(false);
		this->_targetValues.push_back(0);
		this->_currentValues.push_back(0);
		this->_toleranz.push_back(0);
		this->_zeroInitialValue.push_back(0);
		this->_isScaningAxis.push_back(false);
	}
}
void GlobalStatus::loadParameterOffsetValues(QString path)
{
	//Open Config File and Load Setup
	Core::ParserASCIIParameterTupleFiles configFile;
	configFile.loadParameterFile(path.toStdString());

	for (unsigned int i=eTwoTheta; i<=ePhi; i++)
	{
		std::string tempValue;
		if(configFile.getParameter(sAxisNames[i].toStdString(), tempValue))
			this->_zeroInitialValue[i] = QString(tempValue.c_str()).toDouble();
		else
			this->_zeroInitialValue[i] = 0;
	}
}
void GlobalStatus::saveParameterOffsetValues(QString path)
{
	//Open Config File and Save Setup
	Core::ParserASCIIParameterTupleFiles configFile;
	std::vector<Core::ParserASCIIParameterTupleFiles::ASCIIParameterTuple> parameterVec;

	for (unsigned int i=eTwoTheta; i<=ePhi; i++)
	{
		Core::ParserASCIIParameterTupleFiles::ASCIIParameterTuple parameter;
		parameter.parameterName = sAxisNames[i].toStdString();
		parameter.parameterValue = QString::number(this->_zeroInitialValue[i]).toStdString();
		parameterVec.push_back(parameter);
	}

	configFile.saveParameterFile(path.toStdString(), parameterVec);
}
bool GlobalStatus::isMoving()
{
	this->_ioMutex.lock();
	bool isMoving = false;
	for (unsigned int i=0;i<eAxisNamesSize; i++)
	{
		isMoving = isMoving | this->_isMoving[i];
	}
	this->_ioMutex.unlock();
	return isMoving;
}
bool GlobalStatus::isMoving(eAxisNames axis)
{
	this->_ioMutex.lock();
	bool isMoving = this->_isMoving[axis];
	this->_ioMutex.unlock();
	return isMoving;
}
bool GlobalStatus::isPreparingMoving()
{
	this->_ioMutex.lock();
	bool isPreparingMoving = false;
	for (unsigned int i=0;i<eAxisNamesSize; i++)
	{
		isPreparingMoving = isPreparingMoving | this->_isPreparingMove[i];
	}
	this->_ioMutex.unlock();
	return isPreparingMoving;
}
bool GlobalStatus::isPreparingMoving(eAxisNames axis)
{
	this->_ioMutex.lock();
	bool isPreparingMoving = this->_isPreparingMove[axis];
	this->_ioMutex.unlock();
	return isPreparingMoving;
}
bool GlobalStatus::isScaningAxis(eAxisNames axis)
	{
	this->_ioMutex.lock();
	bool isScaningAxis = this->_isScaningAxis[axis];
	this->_ioMutex.unlock();
	return isScaningAxis;
}
bool GlobalStatus::isScaning()
{
	this->_ioMutex.lock();
	bool isScaning = this->_isScaning;
	this->_ioMutex.unlock();
	return isScaning;
}
GlobalStatus::eInitializationStatus GlobalStatus::getInitializationStatus()
{
	this->_ioMutex.lock();
	GlobalStatus::eInitializationStatus initStatus = GlobalStatus::eIsInitialized;
	for (unsigned int i=0;i<eAxisNamesSize; i++)
	{
		if (this->_initializationStatus[i] == GlobalStatus::eIsInInitialization)
			initStatus = GlobalStatus::eIsInInitialization;
		if (this->_initializationStatus[i] == GlobalStatus::eIsNotInitialized)
		{
			initStatus = GlobalStatus::eIsNotInitialized;
			break;
		}
	}
	this->_ioMutex.unlock();
	return initStatus;
}
GlobalStatus::eInitializationStatus GlobalStatus::getInitializationStatus(eAxisNames axis)
{
	this->_ioMutex.lock();
	GlobalStatus::eInitializationStatus initStatus = this->_initializationStatus[axis];
	this->_ioMutex.unlock();
	return initStatus;
}
double GlobalStatus::getCurrentValue(eAxisNames axis)
{
	this->_ioMutex.lock();
	double currentValue = this->_currentValues[axis];
	this->_ioMutex.unlock();
	return currentValue;
}
double GlobalStatus::getTargetValue(eAxisNames axis)
{
	this->_ioMutex.lock();
	double targetValue = this->_targetValues[axis];
	this->_ioMutex.unlock();
	return targetValue;
}
double GlobalStatus::getToleranz(eAxisNames axis)
{
	this->_ioMutex.lock();
	double toleranz = this->_toleranz[axis];
	this->_ioMutex.unlock();
	return toleranz;
}
double GlobalStatus::getCurrentCountsPerSecond()
{
	this->_ioMutex.lock();
	double currentCountsPerSecond = this->_currentCountsPerSecond;
	this->_ioMutex.unlock();
	return currentCountsPerSecond;
}
double GlobalStatus::getCurrentExposureTime()
{
	this->_ioMutex.lock();
	double currentExposureTime = this->_currentExposureTime;
	this->_ioMutex.unlock();
	return currentExposureTime;	
}
double GlobalStatus::getZIValue(eAxisNames axis)
{
	this->_ioMutex.lock();
	double zeroInitialValue = this->_zeroInitialValue[axis];
	this->_ioMutex.unlock();
	return zeroInitialValue;	
}
double GlobalStatus::getUnlockedCoupledOffset()
{
	this->_ioMutex.lock();
	double unlockedCoupledOffset = this->_unlockedCoupledOffset;
	this->_ioMutex.unlock();
	return unlockedCoupledOffset;	
}
unsigned int GlobalStatus::getAbsorberLevel()
{
	this->_ioMutex.lock();
	unsigned int absorberLevel = this->_absorberLevel;
	this->_ioMutex.unlock();
	return absorberLevel;
}
void GlobalStatus::setMovingStatus(eAxisNames axis, bool isMoving)
{
	if (this->isMoving(axis) == isMoving)
		return;

	if (isMoving)
	{
		if (!this->isPreparingMoving())
		{
			this->writeDebugMessage("Attention old status was not isPreparingMove! Setting the current status to isMoving is critical!", Core::Debug::eDebugLevel_Error, "GlobalStatus", "setMovingStatus(eAxisNames axis, bool isMoving)");
		}
	}
	this->writeDebugMessage(QString("Axis: " + QString::number(axis) + " Set Moving Status to " + QString::number(isMoving)).toStdString() + "!", Core::Debug::eDebugLevel_Info, "GlobalStatus", "setMovingStatus(eAxisNames axis, bool isMoving)");
	this->_ioMutex.lock();
	this->_isMoving[axis] = isMoving;
	this->_ioMutex.unlock();
}
void GlobalStatus::setPreparingMovingStatus(eAxisNames axis, bool isPreparingMoving)
{
	if (this->isPreparingMoving(axis) == isPreparingMoving)
		return;

	if (isPreparingMoving)
	{
		if (this->isMoving())
		{
			this->writeDebugMessage("Attention the device is Moving, a new moving command is not allowed! The new status was not set!", Core::Debug::eDebugLevel_Error, "GlobalStatus", "setPreparingMovingStatus(eAxisNames axis, bool isPreparingMoving)");
			return;
		}
	}
	this->writeDebugMessage(QString("Axis: " + QString::number(axis) + " Set Preparing Moving Status to " + QString::number(isPreparingMoving)).toStdString() + "!", Core::Debug::eDebugLevel_Info, "GlobalStatus", "setPreparingMovingStatus(eAxisNames axis, bool isPreparingMoving)");
	this->_ioMutex.lock();
	this->_isPreparingMove[axis] = isPreparingMoving;
	this->_ioMutex.unlock();
}
void GlobalStatus::setScaningAxis(eAxisNames axis, bool isScaningAxis)
{
	if (isScaningAxis)
	this->writeDebugMessage(QString("Axis: " + QString::number(axis) + " Set Scaning Status to " + QString::number(isScaningAxis)).toStdString() + "!", Core::Debug::eDebugLevel_Info, "GlobalStatus", "setScaningAxis(eAxisNames axis, bool isScaningAxis)");
	this->_ioMutex.lock();
	this->_isScaningAxis[axis] = isScaningAxis;
	this->_ioMutex.unlock();
}
void GlobalStatus::setInitialize(eAxisNames axis, eInitializationStatus initializationStatus)
{
	if (this->getInitializationStatus(axis) == initializationStatus)
		return;

	//Attention it is not allowed to set the init state to isNotInit if the device is in initPreparation
	if ((this->getInitializationStatus(axis) == GlobalStatus::eIsInInitialization) && (initializationStatus == GlobalStatus::eIsNotInitialized))
		return;

	this->writeDebugMessage(QString("Axis: " + QString::number(axis) + " Set Initialization Status to " + QString::number((unsigned int)initializationStatus)).toStdString() + "!", Core::Debug::eDebugLevel_Info, "GlobalStatus", "setInitialize(eAxisNames axis, eInitializationStatus initializationStatus)");
	this->_ioMutex.lock();
	this->_initializationStatus[axis] = initializationStatus;
	this->_ioMutex.unlock();

	if (initializationStatus == GlobalStatus::eIsInitialized)
	{
		this->setMovingStatus(axis, false);
		this->setPreparingMovingStatus(axis, false);
	}
}
void GlobalStatus::setScaning(bool isScaning)
{
	if (this->isScaning() == isScaning)
		return;

	this->writeDebugMessage(QString("Set Scaning Status to " + QString::number(isScaning)).toStdString() + "!", Core::Debug::eDebugLevel_Info, "GlobalStatus", "setScaning(bool isScaning)");
	this->_ioMutex.lock();
	this->_isScaning = isScaning;
	this->_ioMutex.unlock();
}
void GlobalStatus::setCurrentCountsPerSecond(double countsPerSecond)
{
	this->_ioMutex.lock();
	this->_currentCountsPerSecond = countsPerSecond;
	this->_ioMutex.unlock();
}
void GlobalStatus::setCurrentExposureTime(double exposureTime)
{
	this->_ioMutex.lock();
	this->_currentExposureTime = exposureTime;
	this->_ioMutex.unlock();
}
void GlobalStatus::setAbsorberLevel(unsigned int absorberLevel)
{
	this->writeDebugMessage(QString("Set AbsorberLevel to " + QString::number(absorberLevel)).toStdString() + "!", Core::Debug::eDebugLevel_Info, "GlobalStatus", "setAbsorberLevel(unsigned int absorberLevel)");
	this->_ioMutex.lock();
	this->_absorberLevel = absorberLevel;
	this->_ioMutex.unlock();
}
void GlobalStatus::setCurrentValue(eAxisNames axis, double newValue)
{
	//Check if the axis is initialized
	if (this->getInitializationStatus(axis) != GlobalStatus::eIsInitialized)
		return;

	//Check if Value was Changed
	bool valueChanged = this->getCurrentValue(axis) != newValue;
	if (!valueChanged)
		return;

	//Set Current Value
	std::stringstream tempSS;
	tempSS << "Axis: " << axis << " Set new Current Value to: " << newValue << std::endl;
	this->writeDebugMessage(tempSS.str(), Core::Debug::eDebugLevel_Info, "GlobalStatus", "setCurrentValue(eAxisNames axis, double newValue)");
	this->_ioMutex.lock();
	this->_currentValues[axis] = newValue;
	this->_ioMutex.unlock();


		
	
	

	//Check if Movement was started
	if (this->isPreparingMoving(axis) && !this->isMoving(axis) && valueChanged)
	{
		std::stringstream tempSS;
		tempSS << "Movement is Started: Axis: " << axis << " changed status to isMoving and isNotPreparingMoving!" << std::endl;
		this->writeDebugMessage(tempSS.str(), Core::Debug::eDebugLevel_Info, "GlobalStatus", "setCurrentValue(eAxisNames axis, double newValue)");
		this->setMovingStatus(axis, true);
	}
	

	//Check if Movement is done
	if (this->isMoving(axis))
	{
		if (abs(this->getTargetValue(axis) - this->getCurrentValue(axis)) <= this->getToleranz(axis))
		{
			std::stringstream tempSS;
			tempSS << "Movement is Done: Axis: " << axis << " changed status to isNotMoving!" << std::endl;
			this->writeDebugMessage(tempSS.str(), Core::Debug::eDebugLevel_Info, "GlobalStatus", "setCurrentValue(eAxisNames axis, double newValue)");

			this->setMovingStatus(axis, false);
			this->setPreparingMovingStatus(axis, false);
		}
		
		if (this->isMoving(ePhi)) //Phi has only positive values, this is needed to perform scans from negative to positive values (-5 to 5 e.g.)
		{
			double targetValueCorrected = 360 - abs(this->getTargetValue(ePhi));
			if (abs(targetValueCorrected - this->getCurrentValue(axis)) <= this->getToleranz(axis))
			{
				std::stringstream tempSS;
				tempSS << "Movement is Done: Axis: " << ePhi << " changed status to isNotMoving!" << std::endl;
				this->writeDebugMessage(tempSS.str(), Core::Debug::eDebugLevel_Info, "GlobalStatus", "setCurrentValue(eAxisNames axis, double newValue)");

				this->setMovingStatus(ePhi, false);
				this->setPreparingMovingStatus(ePhi, false);
			}
		}
	}
}
void GlobalStatus::setTargetValue(eAxisNames axis, double newValue)
{
	//Set new TargetValue Value
	if (this->isMoving(axis))
	{
		std::stringstream tempSS;
		tempSS << "Axis: " << axis << " Device is moving, a new TargetValue is not allowed!" << std::endl;
		this->writeDebugMessage(tempSS.str(), Core::Debug::eDebugLevel_Error, "GlobalStatus", "setTargetValue(eAxisNames axis, double newValue)");
		return;
	}
	else
	{
		std::stringstream tempSS;
		tempSS << "Axis: " << axis << " Set new Target Value to: " << newValue << std::endl;
		this->writeDebugMessage(tempSS.str(), Core::Debug::eDebugLevel_Info, "GlobalStatus", "setTargetValue(eAxisNames axis, double newValue)");
		this->_ioMutex.lock();
		this->_targetValues[axis] = newValue;
		this->_ioMutex.unlock();

		//Change Status
		this->setPreparingMovingStatus(axis, true);
	}
}
void GlobalStatus::setToleranz(eAxisNames axis, double toleranz)
{
	this->_ioMutex.lock();
	this->_toleranz[axis] = toleranz;
	this->_ioMutex.unlock();
}
void GlobalStatus::setZIValue(eAxisNames axis, double zeroInitialValue)
{
	std::stringstream tempSS;
	tempSS << "Axis: " << axis << "Set ZI Value to " << zeroInitialValue << std::endl;
	this->writeDebugMessage(tempSS.str(), Core::Debug::eDebugLevel_Info, "GlobalStatus", "setZIValue(eAxisNames axis, double zeroInitialValue)");
	this->_ioMutex.lock();
	this->_zeroInitialValue[axis] = zeroInitialValue;
	this->_ioMutex.unlock();
}
void GlobalStatus::setUnlockedCoupledOffset(double unlockedCoupledOffset)
{
	this->writeDebugMessage(QString("Set unlockedCoupledOffset " + QString::number(unlockedCoupledOffset)).toStdString() + "!", Core::Debug::eDebugLevel_Info, "GlobalStatus", "setUnlockedCoupledOffset(double unlockedCoupledOffset)");
	this->_ioMutex.lock();
	this->_unlockedCoupledOffset = unlockedCoupledOffset;
	this->_ioMutex.unlock();
}
unsigned int GlobalStatus::getScanNumberOfCachedValues()
{
	return this->_scanCurrentCachedValues.size();
}
void GlobalStatus::setScanNewMeasuredValue(double intens)
{
	this->_scanCurrentCachedValues.append(intens);
}
double GlobalStatus::getScanOldestCachedValue()
{
	if (this->_scanCurrentCachedValues.empty())
		return -1;

	double value = this->_scanCurrentCachedValues.first();
	this->_scanCurrentCachedValues.removeFirst();
	return value;
}
void GlobalStatus::setScanNumberOfRemainingMeasurementsPoints(unsigned int number)
{
	this->_scanNumberOfRemainingMeasurementsPoints = number;
}
unsigned int GlobalStatus::getScanNumberOfRemainingMeasurementsPoints()
{
	return this->_scanNumberOfRemainingMeasurementsPoints;
}
void GlobalStatus::deleteAllScanValues()
{
	this->_scanCurrentCachedValues.clear();
}

//*****************************
//*** Proxy IO Read Thread ***
//*****************************
ProxyIOThread::ProxyIOThread()
 {
	 this->_serialPort = NULL;
	 this->_deviceName = "";

	 this->_doStop = false;
	 this->_waitTimeForAnswer_ms = 500;
}
ProxyIOThread::~ProxyIOThread()
{

}
void ProxyIOThread::setParameter(QextSerialPort *serialPort, QString deviceName)
{
	this->_serialPort = serialPort;
	this->_deviceName = deviceName;
}
void ProxyIOThread::stop()
{
	this->_doStop = true;
}
void ProxyIOThread::sendDataWord(QString dataWord)
{
	QMutexLocker locker(&this->_sendFIFOMutex);

	this->_sendFIFO.push_back(dataWord);
}
QString ProxyIOThread::getNextReceivedDataWord()
{
	QMutexLocker locker(&this->_receiveFIFOMutex);

	if (this->_receiveFIFO.isEmpty())
		return "";

	//Extract first data word
	QString currentDataWord = this->_receiveFIFO.at(0);
	this->_receiveFIFO.removeAt(0);

	return currentDataWord;
}
void ProxyIOThread::run()
{
	bool sendDataWordAgain = false;
	//Main Loop
	while(!this->_doStop)
	{
		//Check if the port open
		if (this->_serialPort == NULL)
		{
				this->writeDebugMessage("Attention Port does not exist!", Core::Debug::eDebugLevel_Error, "ProxyCommunication", "run", this->_deviceName.toStdString());
				return;
		}

		if (!this->_serialPort->isOpen())
		{
			//Open Connection
			this->_serialPort->open(QIODevice::ReadWrite);
			if (!this->_serialPort->isOpen())
			{
				this->writeDebugMessage("Attention Port has not been opened!", Core::Debug::eDebugLevel_Error, "ProxyCommunication", "run", this->_deviceName.toStdString());
				return;
			}
		}

		//*********************
		//*** send DataWord ***
		//*********************
		if (!sendDataWordAgain)	//DataWord was successfully sent
		{
			if (!this->_sendFIFO.empty())
			{
				this->_sendFIFOMutex.lock();
				this->_tempCurrentSentDataWord = this->_sendFIFO.at(0);
				this->_sendFIFO.removeAt(0);
				this->_sendFIFOMutex.unlock();
				sendDataWordAgain = true;
			}
			else
			{
				//Attention there is nothing to send. It is not necessary to read the input buffer
				this->msleep(50);	//wait 50ms before we check the send FIFO again
				continue;
			}
		}
		if (sendDataWordAgain)
		{
			this->writeDebugMessage(QString(QString("Send DataWord: (") + this->_serialPort->portName() + ("): ") + this->_tempCurrentSentDataWord).toStdString(), 5, "ProxyCommunication", "_sendDataWord", this->_deviceName.toStdString());
			if (this->_serialPort->write(this->_tempCurrentSentDataWord.toStdString().c_str(), this->_tempCurrentSentDataWord.size()) == -1)
				this->writeDebugMessage("It was not possible to send the data within 50ms.", Core::Debug::eDebugLevel_Warning, "ProxyCommunication", "_sendDataWord", this->_deviceName.toStdString());
			sendDataWordAgain = false;
		}


		//************************
		//*** Receive DataWord ***
		//************************
		bool tempReceiveSuccessful = false;
		QTime tempTime;
		tempTime.start();
		do
		{
			this->msleep(10);	//wait 10 ms before we check the input buffer

			//Poll the Serial Buffer
			int numberOfBytes = this->_serialPort->bytesAvailable();
			if (numberOfBytes > 0)
			{
				//Read as much as possible
				char *tempChar = new char[numberOfBytes + 1];
				this->_serialPort->read(tempChar, numberOfBytes);
				tempChar[numberOfBytes] = '\0';
				QString tempString(tempChar);
				delete tempChar;
				this->writeDebugMessage(QString(QString("Read Buffer: ") + tempString).toStdString(), Core::Debug::eDebugLevel_Debug, "ProxyCommunication", "run", this->_deviceName.toStdString());
				this->_tempInputBuffer += tempString;
		
				//Check current Buffer for new Datawords
				for(int indexOfTerminator = this->_tempInputBuffer.indexOf(QChar(COMMUNICATION_TERMINATOR_SIGN)); indexOfTerminator >= 0; indexOfTerminator = this->_tempInputBuffer.indexOf(QChar(COMMUNICATION_TERMINATOR_SIGN)))
				{
					QString dataWord = this->_tempInputBuffer.left(indexOfTerminator + 1);
					this->_tempInputBuffer.remove(0, indexOfTerminator + 1);
			
					//Put new DataWord into FIFO
					this->_receiveFIFOMutex.lock();
					this->_receiveFIFO.push_back(dataWord);
					this->_receiveFIFOMutex.unlock();

					//Create Debug Msg
					std::stringstream tempSS;
					for (unsigned int i=0; i<dataWord.size(); i++)
					{
						tempSS << "0x" << std::hex << (int)dataWord.at(i).toLatin1() << " ";
					}
					this->writeDebugMessage(QString(QString("New DataWord: ") + QString(tempSS.str().c_str())).toStdString(), Core::Debug::eDebugLevel_Debug, "ProxyCommunication", "run", this->_deviceName.toStdString());

					//Emit Signal
					emit this->signalNewDataWord();
					tempReceiveSuccessful = true;
				}
			}
		} while(!tempReceiveSuccessful && (tempTime.elapsed() <= this->_waitTimeForAnswer_ms));	//Check Time to wait or receive was successful
		if (!tempReceiveSuccessful)	//We got no answer, it is necessary to send the DataWord again!!!
		{
			this->_serialPort->flush();
			this->writeDebugMessage(QString(QString("Attention: There was no answer within ") + QString::number(this->_waitTimeForAnswer_ms) + QString("ms from the DACO. The app sent the request again.")).toStdString(), Core::Debug::eDebugLevel_Warning, "ProxyIOThread", "run", this->_deviceName.toStdString());
			sendDataWordAgain = true;
		}
	}
}

//***************************
//*** ProxyCommunication ***
//***************************
ProxyCommunication::ProxyCommunication(std::string pathOfIniFile) : QObject(), Core::Debug()
{
	//Init Members
	this->_currentStatusTable = NULL;

	//Open Parameter File and Load Setup
	this->_parameterFile.loadParameterFile(pathOfIniFile);

	unsigned int dataBits = 0;
	unsigned int stopBits = 0;
	unsigned int baudRate = 0;
	std::string parity;

	std::string parameterValue;
	if(!this->_parameterFile.getParameter("DeviceName", parameterValue))
		this->_deviceName = "Unknown";
	else
		this->_deviceName = QString(parameterValue.c_str());

	if (!this->_parameterFile.getParameter("Port", parameterValue))
		this->_serialPort.setPortName("COM1");
	else
		this->_serialPort.setPortName(QString(parameterValue.c_str()));

	if (!this->_parameterFile.getParameter("Baud", parameterValue))
	{
		this->_serialPort.setBaudRate(BAUD9600);
		baudRate = 9600;
	}
	else
	{
		switch (QString(parameterValue.c_str()).toInt())
		{
		case 1200:
			this->_serialPort.setBaudRate(BAUD1200);
			baudRate = 1200;
			break;
		case 2400:
			this->_serialPort.setBaudRate(BAUD2400);
			baudRate = 2400;
			break;
		case 4800:
			this->_serialPort.setBaudRate(BAUD4800);
			baudRate = 4800;
			break;
		case 9600:
			this->_serialPort.setBaudRate(BAUD9600);
			baudRate = 9600;
			break;
		case 19200:
			this->_serialPort.setBaudRate(BAUD19200);
			baudRate = 19200;
			break;
		case 38400:
			this->_serialPort.setBaudRate(BAUD38400);
			baudRate = 38400;
			break;
		case 115200:
			this->_serialPort.setBaudRate(BAUD115200);
			baudRate = 115200;
			break;
		case 128000:
			this->_serialPort.setBaudRate(BAUD128000);
			baudRate = 128000;
			break;
		case 256000:
			this->_serialPort.setBaudRate(BAUD256000);
			baudRate = 256000;
			break;
		default:
			std::stringstream tempSS;
			tempSS << "The given Baud Rate is not supported!\nGiven BaudRate: " << QString(parameterValue.c_str()).toInt() << "! The BaudRate was changed to 9600Baud!" << std::endl;
			std::string tempMsg = tempSS.str();
			this->writeDebugMessage(tempMsg, Core::Debug::eDebugLevel_Error, "ProxyCommunication", "Constructor", this->_deviceName.toStdString());
			this->_serialPort.setBaudRate(BAUD9600);
			baudRate = 9600;
			break;
		}
	}

	if (!this->_parameterFile.getParameter("DataBit", parameterValue))
	{
		this->_serialPort.setDataBits(DATA_8);
		dataBits = 8;
	}
	else
	{
		switch (QString(parameterValue.c_str()).toInt())
		{
		case 5:
			this->_serialPort.setDataBits(DATA_5);
			dataBits = 5;
			break;
		case 6:
			this->_serialPort.setDataBits(DATA_6);
			dataBits = 6;
			break;
		case 7:
			this->_serialPort.setDataBits(DATA_7);
			dataBits = 7;
			break;
		case 8:
			this->_serialPort.setDataBits(DATA_8);
			dataBits = 8;
			break;
		default:
			std::stringstream tempSS;
			tempSS << "The given number of DataBits is not supported!\nGiven DataBit: " << QString(parameterValue.c_str()).toInt() << "! The number of DataBits was changed to 8Bit!" << std::endl;
			std::string tempMsg = tempSS.str();
			this->writeDebugMessage(tempMsg, Core::Debug::eDebugLevel_Error, "ProxyCommunication", "Constructor", this->_deviceName.toStdString());
			this->_serialPort.setDataBits(DATA_8);
			dataBits = 8;
			break;
		}
	}

	if (!this->_parameterFile.getParameter("StopBit", parameterValue))
	{
		this->_serialPort.setStopBits(STOP_1);
		stopBits = 1;
	}
	else
	{
		switch (QString(parameterValue.c_str()).toInt())
		{
		case 1:
			this->_serialPort.setStopBits(STOP_1);
			stopBits = 1;
			break;
		case 2:
			this->_serialPort.setStopBits(STOP_2);
			stopBits = 2;
			break;
		default:
			std::stringstream tempSS;
			tempSS << "The given number of StopBits is not supported!\nGiven StopBit: " << QString(parameterValue.c_str()).toInt() << "! The number of StopBits was changed to 1Bit!" << std::endl;
			std::string tempMsg = tempSS.str();
			this->writeDebugMessage(tempMsg, Core::Debug::eDebugLevel_Error, "ProxyCommunication", "Constructor", this->_deviceName.toStdString());
			this->_serialPort.setStopBits(STOP_1);
			stopBits = 1;
			break;
		}
	}

	if (!this->_parameterFile.getParameter("Parity", parameterValue))
	{
		this->_serialPort.setParity(PAR_NONE);
		parity = "none";
	}
	else
	{
		if (parameterValue.compare("even") == 0)
		{
			this->_serialPort.setParity(PAR_EVEN);
			parity = "even";
		}
		else if (parameterValue.compare("odd") == 0)
		{
			this->_serialPort.setParity(PAR_ODD);
			parity = "odd";
		}
		else if (parameterValue.compare("none") == 0)
		{
			this->_serialPort.setParity(PAR_NONE);
			parity = "none";
		}
		else
		{
			std::stringstream tempSS;
			tempSS << "The given Parity is not supported!\nGiven Parity: " << parameterValue << "! The Parity was changed to None!" << std::endl;
			std::string tempMsg = tempSS.str();
			this->writeDebugMessage(tempMsg, Core::Debug::eDebugLevel_Error, "ProxyCommunication", "Constructor", this->_deviceName.toStdString());
			this->_serialPort.setParity(PAR_NONE);
			parity = "none";
		}
	}

	//Deactivate Hardware Serial FlowCtrl and activate XON / XOFF
	this->_serialPort.setFlowControl(FLOW_XONXOFF);

	//Set Timeout to 1ms
	this->_serialPort.setTimeout(0, 1);

	//Output of the current Serial Configuration
	std::stringstream tempSS;
	tempSS << "Current init parameters for the serial interface!" << std::endl;
	tempSS << "Port: " << this->_serialPort.portName().toStdString() << std::endl;
	tempSS << "Baud: " << baudRate << std::endl;
	tempSS << "Data Bits: " << dataBits << std::endl;
	tempSS << "Stop Bits: " << stopBits << std::endl;
	tempSS << "Parity: " << parity << std::endl;
	std::string tempMsg = tempSS.str();
	this->writeDebugMessage(tempMsg, Core::Debug::eDebugLevel_Error, "ProxyCommunication", "Constructor", this->_deviceName.toStdString());
	std::cout << tempMsg << std::endl;
}
ProxyCommunication::~ProxyCommunication()
{
}
void ProxyCommunication::setGlobalStatusTable(GlobalStatus *globalStatusTable)
{
	this->_currentStatusTable = globalStatusTable;
}

//*********************************
//*** ProxyCommunicationServer ***
//*********************************
ProxyCommunicationServer::ProxyCommunicationServer(std::string pathOfIniFile) : ProxyCommunication(pathOfIniFile)
{
	//Worker Thread
	this->_workerThread.start();
	this->moveToThread(&this->_workerThread);

	//Polling Timer
	this->_pollingTimer.setInterval(10);
	this->_pollingTimer.setSingleShot(false);
	this->_pollingTimer.start();
	this->_pollingTimer.moveToThread(&this->_workerThread);
	QObject::connect(&this->_pollingTimer, SIGNAL(timeout()), this, SLOT(_slotPollingSerialInputBuffer()), Qt::DirectConnection);

	//Connect Slots
	QObject::connect(this, SIGNAL(signalDataWord()), this, SLOT(_slotAnalyzeNewCommand()), Qt::DirectConnection);
}
ProxyCommunicationServer::~ProxyCommunicationServer() 
{
	//Polling Timer
	this->_pollingTimer.stop();

	//Worker Thread
	this->_workerThread.exit();
	this->_workerThread.wait();
}
void ProxyCommunicationServer::_slotPollingSerialInputBuffer()
{
	//Check if the port is open
	if (!this->_serialPort.isOpen())
	{
		//Open Connection
		this->_serialPort.open(QIODevice::ReadWrite);
		if (!this->_serialPort.isOpen())
		{
			this->writeDebugMessage("Attention Port has not been opened!", Core::Debug::eDebugLevel_Error, "ProxyCommunication", "run", this->_deviceName.toStdString());
			return;
		}
	}

	//Poll the Serial Buffer
	int numberOfBytes = this->_serialPort.bytesAvailable();
	if (numberOfBytes > 0)
	{
		//Read as much as possible
		char *tempChar = new char[numberOfBytes + 1];
		this->_serialPort.read(tempChar, numberOfBytes);
		tempChar[numberOfBytes] = '\0';
		QString tempString(tempChar);
		delete tempChar;
		this->writeDebugMessage(QString(QString("Read Buffer: ") + tempString).toStdString(), Core::Debug::eDebugLevel_Debug, "ProxyCommunication", "run", this->_deviceName.toStdString());
		this->_tempBufferString += tempString;
		
		//Check current Buffer for new Datawords
		for(int indexOfTerminator = this->_tempBufferString.indexOf(QChar(COMMUNICATION_TERMINATOR_SIGN)); indexOfTerminator >= 0; indexOfTerminator = this->_tempBufferString.indexOf(QChar(COMMUNICATION_TERMINATOR_SIGN)))
		{
			QString dataWord = this->_tempBufferString.left(indexOfTerminator + 1);
			this->_tempBufferString.remove(0, indexOfTerminator + 1);
			
			this->_inputFIFOMutex.lock();
			this->_inputFIFO.push_back(dataWord);
			this->_inputFIFOMutex.unlock();

			std::stringstream tempSS;
			for (unsigned int i=0; i<dataWord.size(); i++)
			{
				tempSS << "0x" << std::hex << (int)dataWord.at(i).toAscii() << " ";
			}
			this->writeDebugMessage(QString(QString("New DataWord: ") + QString(tempSS.str().c_str())).toStdString(), Core::Debug::eDebugLevel_Debug, "ProxyCommunication", "run", this->_deviceName.toStdString());
			emit this->signalDataWord();
		}
	}
}
void ProxyCommunicationServer::_sendDataWord(QString dataWord)
{
	if (this->_serialPort.isOpen())
	{
		this->writeDebugMessage(QString(QString("Send DataWord: (") + this->_serialPort.portName() + ("): ") + dataWord).toStdString(), Core::Debug::eDebugLevel_Debug, "ProxyCommunication", "_sendDataWord", this->_deviceName.toStdString());
		this->_serialPort.write(dataWord.toStdString().c_str(), dataWord.size());
		this->_serialPort.flush();
	}
}
// ProxyCommunicationServerD5000 Class
ProxyCommunicationServerD5000::ProxyCommunicationServerD5000(std::string pathOfIniFile) : ProxyCommunicationServer(pathOfIniFile)
{
}
ProxyCommunicationServerD5000::~ProxyCommunicationServerD5000()
{
}
void ProxyCommunicationServerD5000::sendAnswer(QString dataWord)
{
	this->_sendSerialPortMutex.lock();

	//Put Terminator to the end of the string
	std::stringstream tempSS;
	tempSS << dataWord.toStdString();
	tempSS << COMMUNICATION_TERMINATOR_SIGN_C << '\0';		//Terminator (0x0D)
	dataWord = QString(tempSS.str().c_str());

	this->_sendDataWord(dataWord);

	this->_sendSerialPortMutex.unlock();
}
void ProxyCommunicationServerD5000::_slotAnalyzeNewCommand()
{
	//Check if more than one data word in the FIFO???
	//Emit an Error if it is more than one data word => Communication problems are possible
	this->_inputFIFOMutex.lock();
	if (this->_inputFIFO.size() > 1)
	{
		this->writeDebugMessage("Attention more than one data word is in the FIFO! This is a critical status! All read DataWords are erased!", 1, "ProxyCommunicationServerD5000", "_slotAnalyzeNewCommand", this->_deviceName.toStdString());
		this->_inputFIFO.clear();
		this->_inputFIFOMutex.unlock();
		return;
	}
	this->_inputFIFOMutex.unlock();

	if (this->_inputFIFO.isEmpty())
		return;

	//Extract first data word
	this->_inputFIFOMutex.lock();
	QString currentDataWord = this->_inputFIFO.at(0);
	this->_inputFIFO.removeAt(0);
	this->_inputFIFOMutex.unlock();

	this->writeDebugMessage(QString("New DataWord: " + currentDataWord).toStdString(), Core::Debug::eDebugLevel_Debug, "ProxyCommunicationServerD5000", "_slotAnalyzeNewCommand()", this->_deviceName.toStdString());

	//Remove COMMUNICATION_SEND_REQUEST_SIGN (0x05)
	currentDataWord.remove(currentDataWord.indexOf(QChar(COMMUNICATION_SEND_REQUEST_SIGN)), 1);

	//Analyze Data Word
	if (currentDataWord.toStdString().compare(1, 4, "ST25") == 0) //ST25: Number of stored Measurement values requested
	{
		//It is a scan status request
		this->writeDebugMessage("DataWord was a scan status request!", Core::Debug::eDebugLevel_Debug, "ProxyCommunicationServerD5000", "_slotAnalyzeNewCommand", this->_deviceName.toStdString());
		this->sendAnswer(this->_createScanStatusMsg());
	}
	else if (currentDataWord.toStdString().compare(1, 4, "ST12") == 0) //ST12: Count Rate
	{
		//It is a ratemeter status request
		this->writeDebugMessage("DataWord was a ratemeter status request!", Core::Debug::eDebugLevel_Debug, "ProxyCommunicationServerD5000", "_slotAnalyzeNewCommand", this->_deviceName.toStdString());
		this->_clientCommunicationObj->doRate();
		
		this->sendAnswer(this->_createRatemeterMsg());
	}
	else if (currentDataWord.toStdString().compare(1, 2, "ST") == 0) //Is a normal status request
	{
		
		this->writeDebugMessage("DataWord was a status request!", Core::Debug::eDebugLevel_Debug, "ProxyCommunicationServerD5000", "_slotAnalyzeNewCommand", this->_deviceName.toStdString());
		this->sendAnswer(this->_createStatusMsg());
	}
	else if (currentDataWord.toStdString().compare(1, 2, "IN") == 0)	//Initialize
	{
		unsigned int axisNumber = currentDataWord.mid(3, currentDataWord.indexOf(QChar(COMMUNICATION_TERMINATOR_SIGN_C)) - 3).toInt();
		if (axisNumber == 0)
		{
			this->writeDebugMessage("Initialization: IN0 was sent. No Initialization was started!", Core::Debug::eDebugLevel_Error, "ProxyCommunicationServerD5000", "_slotAnalyzeNewCommand", this->_deviceName.toStdString());
			
			QString ErrorMessage;

			std::stringstream tempSS;

			tempSS << COMMUNICATION_START_SIGN_C << '?' << '1';		// ErrorMessage 1: Invalid Command
			ErrorMessage = QString(tempSS.str().c_str());
			this->sendAnswer(ErrorMessage);

			std::cout << "Error! IN0 was sent! To use the D500 IN0 command enter 'D500_IN0' "<< std::endl;
		}
		else
		{
			this->_clientCommunicationObj->doInit(axisNumber);
			this->sendAnswer(QString(COMMUNICATION_START_SIGN_C));
		}
		
	}
	else if (currentDataWord.toStdString().compare(1, 2, "ZI") == 0)	//Zero Initial
	{
		double newZero = currentDataWord.mid(3, currentDataWord.indexOf(QChar(',')) - 3).toDouble();
		currentDataWord.remove(0, currentDataWord.indexOf(QChar(',')) + 1);
		unsigned int axisNumber = currentDataWord.mid(0, currentDataWord.indexOf(QChar(COMMUNICATION_TERMINATOR_SIGN_C))).toInt();

		this->_clientCommunicationObj->doZeroInitial((eAxisNames)axisNumber, newZero);
		this->sendAnswer(QString(COMMUNICATION_START_SIGN_C));

	}
	else if (currentDataWord.toStdString().compare(1, 2, "GO") == 0)	//Move
	{
		double targetValue = currentDataWord.mid(3, currentDataWord.indexOf(QChar(',')) - 3).toDouble();
		currentDataWord.remove(0, currentDataWord.indexOf(QChar(',')) + 1);
		unsigned int axisNumber = currentDataWord.mid(0, currentDataWord.indexOf(QChar(COMMUNICATION_TERMINATOR_SIGN_C))).toInt();
		this->_clientCommunicationObj->doMove((eAxisNames)axisNumber, targetValue);
		this->sendAnswer(QString(COMMUNICATION_START_SIGN_C));
	}
	else if (currentDataWord.toStdString().compare(1, 2, "RC") == 0)	//RemoteControl
	{
		if (currentDataWord.toStdString().compare(3, 1, "0") == 0)
			this->_clientCommunicationObj->setRemoteControl(false);
		else if (currentDataWord.toStdString().compare(3, 1, "1") == 0)
			this->_clientCommunicationObj->setRemoteControl(true);
		else
			return;

		this->sendAnswer(QString(COMMUNICATION_START_SIGN_C));
	}
	else if (currentDataWord.toStdString().compare(1, 2, "HT") == 0)	//Hold
	{
		unsigned int axisNumber = currentDataWord.mid(3, currentDataWord.indexOf(QChar(COMMUNICATION_TERMINATOR_SIGN_C)) - 3).toInt();
		this->_clientCommunicationObj->doHold(axisNumber);

		this->sendAnswer(QString(COMMUNICATION_START_SIGN_C));
	}
	else if (currentDataWord.toStdString().compare(1, 2, "SL") == 0)	//Slit
	{
		if (currentDataWord.toStdString().compare(3, 1, "0") == 0)   //Absorber in
			this->_clientCommunicationObj->setAbsorberDown();

		else if (currentDataWord.toStdString().compare(3, 1, "1") == 0)    //Absorber out
			this->_clientCommunicationObj->setAbsorberUp();
		
		this->sendAnswer(QString(COMMUNICATION_START_SIGN_C));
	}
	
	else if (currentDataWord.toStdString().compare(1, 3, "OC2") == 0)  //Manual control for second (unused) absorber
	{
		if (currentDataWord.toStdString().compare(5, 1, "0") == 0)   //Absorber in
		{
			this->_clientCommunicationObj->setUnusedAbsorberDown();
		}
		else if (currentDataWord.toStdString().compare(5, 1, "1") == 0)    //Absorber out
		{
			this->_clientCommunicationObj->setUnusedAbsorberUp();
		}
		this->sendAnswer(QString(COMMUNICATION_START_SIGN_C));
	}
	else if (currentDataWord.toStdString().compare(1, 2, "TW") == 0)	//TubeWindow (Shutter)
	{
		if (currentDataWord.toStdString().compare(3, 1, "0") == 0)
			this->_clientCommunicationObj->doCloseShutter();
		else if (currentDataWord.toStdString().compare(3, 1, "1") == 0)
			this->_clientCommunicationObj->doOpenShutter();
		else
			return;

		this->sendAnswer(QString(COMMUNICATION_START_SIGN_C));
	}
	else if (currentDataWord.toStdString().compare(1, 2, "HV") == 0)	//HighVoltage
	{
		this->writeDebugMessage("High Voltage is currently not implemented!", Core::Debug::eDebugLevel_Warning, "ProxyCommunicationServerD5000", "_slotAnalyzeNewCommand", this->_deviceName.toStdString());
		this->sendAnswer(QString(COMMUNICATION_START_SIGN_C));
	}
	else if (currentDataWord.toStdString().compare(1, 8, "D500_IN0") == 0)	//IN0 Command for D500 (sets current values of 2Theta and Theta to 0)
	{
		this->_clientCommunicationObj->doOldZero();
		this->sendAnswer(QString(COMMUNICATION_START_SIGN_C));
	}
	else if (currentDataWord.toStdString().compare(1, 12, "D500_ALL_IN0") == 0)	//IN0,0,0,0 Command for D500 (sets current values of all axes to 0)
	{
		this->_clientCommunicationObj->doOldZeroAll();
		this->sendAnswer(QString(COMMUNICATION_START_SIGN_C));
	}
	else if (currentDataWord.toStdString().compare(1, 2, "AG") == 0)	//AmplifierGain
	{
		this->writeDebugMessage("Amplifier Gain is currently not implemented!", Core::Debug::eDebugLevel_Warning, "ProxyCommunicationServerD5000", "_slotAnalyzeNewCommand", this->_deviceName.toStdString());

		this->sendAnswer(QString(COMMUNICATION_START_SIGN_C));
	}
	else if (currentDataWord.toStdString().compare(1, 2, "DI") == 0)	//Discriminator
	{
		this->writeDebugMessage("Discriminator is currently not implemented!", Core::Debug::eDebugLevel_Warning, "ProxyCommunicationServerD5000", "_slotAnalyzeNewCommand", this->_deviceName.toStdString());

		this->sendAnswer(QString(COMMUNICATION_START_SIGN_C));
	}
	else if (currentDataWord.toStdString().compare(1, 2, "IG") == 0)	//Integrator
	{
		this->writeDebugMessage("Integrator is currently not implemented!", Core::Debug::eDebugLevel_Warning, "ProxyCommunicationServerD5000", "_slotAnalyzeNewCommand", this->_deviceName.toStdString());

		this->sendAnswer(QString(COMMUNICATION_START_SIGN_C));
	}
	else if (currentDataWord.toStdString().compare(1, 2, "OS") == 0)	//Oscillation
	{
		this->writeDebugMessage("Oszillation is currently not implemented!", Core::Debug::eDebugLevel_Warning, "ProxyCommunicationServerD5000", "_slotAnalyzeNewCommand", this->_deviceName.toStdString());

		this->sendAnswer(QString(COMMUNICATION_START_SIGN_C));
	}
	else if (currentDataWord.toStdString().compare(1, 2, "RO") == 0)	//Rotation Sample Changer
	{
		this->writeDebugMessage("Rotation Sample Changer is currently not implemented!", Core::Debug::eDebugLevel_Warning, "ProxyCommunicationServerD5000", "_slotAnalyzeNewCommand", this->_deviceName.toStdString());

		this->sendAnswer(QString(COMMUNICATION_START_SIGN_C));
	}
	else if (currentDataWord.toStdString().compare(1, 2, "SS") == 0)	//StepScan
	{
		this->writeDebugMessage("StepScan Command was detected!", Core::Debug::eDebugLevel_Info, "ProxyCommunicationServerD5000", "_slotAnalyzeNewCommand", this->_deviceName.toStdString());

		currentDataWord.remove(0, 3);	//Erase .SS
		unsigned int numberOfSteps = currentDataWord.mid(0, currentDataWord.toStdString().find_first_not_of(".-1234567890")).toUInt();
		currentDataWord.remove(0, currentDataWord.toStdString().find_first_not_of(".-1234567890") + 1);
		double measurementTime = currentDataWord.mid(0, currentDataWord.toStdString().find_first_not_of(".-1234567890")).toDouble();
		currentDataWord.remove(0, currentDataWord.toStdString().find_first_not_of(".-1234567890") + 1);
		double stepSize = currentDataWord.mid(0, currentDataWord.toStdString().find_first_not_of(".-1234567890")).toDouble();
		currentDataWord.remove(0, currentDataWord.toStdString().find_first_not_of(".-1234567890") + 1);
		unsigned int axisNumber = currentDataWord.mid(0, currentDataWord.toStdString().find_first_not_of(".-1234567890")).toUInt();
		currentDataWord.remove(0, currentDataWord.toStdString().find_first_not_of(".-1234567890") + 1);
		unsigned int mode = currentDataWord.mid(0, currentDataWord.toStdString().find_first_not_of(".-1234567890")).toUInt();
		

		//Attention: A measurementTime less than 0.8s is not allowed!
		//For shorter measurement times the communication might be too slow so that an HT command for the automatic absorber is sent too late and the detector is already in saturation.
		if (measurementTime < 0.8)
		{
			
			this->writeDebugMessage("StepScan: the MeasurementTime was less than 0.8s! No measurement started!", Core::Debug::eDebugLevel_Error, "ProxyCommunicationServerD5000", "_slotAnalyzeNewCommand", this->_deviceName.toStdString());
			
			QString ErrorMessage;

			std::stringstream tempSS;

			tempSS << COMMUNICATION_START_SIGN_C << '?' << '3';		// ErrorMessage 3: Incorrect Argument
			ErrorMessage = QString(tempSS.str().c_str());
			this->sendAnswer(ErrorMessage);

			std::cout << "Error! Measurement time too short! Please select at least 0.8 sec!" << std::endl;
		}
		
		else
		{
			this->_clientCommunicationObj->doScan(axisNumber, numberOfSteps, stepSize, measurementTime, mode);
			this->_currentStatusTable->setScaningAxis((eAxisNames)axisNumber, true);
			this->sendAnswer(QString(COMMUNICATION_START_SIGN_C));
		}
	}
	else if (currentDataWord.toStdString().compare(1, 2, "SC") == 0)	//ContinuousScan
	{
		this->writeDebugMessage("ContinuousScan Command was detected!", Core::Debug::eDebugLevel_Info, "ProxyCommunicationServerD5000", "_slotAnalyzeNewCommand", this->_deviceName.toStdString());

		currentDataWord.remove(0, 3);	//Erase .SC
		unsigned int numberOfSteps = currentDataWord.mid(0, currentDataWord.toStdString().find_first_not_of(".-1234567890")).toUInt();
		currentDataWord.remove(0, currentDataWord.toStdString().find_first_not_of(".-1234567890") + 1);
		double measurementTime = currentDataWord.mid(0, currentDataWord.toStdString().find_first_not_of(".-1234567890")).toDouble();
		currentDataWord.remove(0, currentDataWord.toStdString().find_first_not_of(".-1234567890") + 1);
		double stepSize = currentDataWord.mid(0, currentDataWord.toStdString().find_first_not_of(".-1234567890")).toDouble();
		currentDataWord.remove(0, currentDataWord.toStdString().find_first_not_of(".-1234567890") + 1);
		unsigned int axisNumber = currentDataWord.mid(0, currentDataWord.toStdString().find_first_not_of(".-1234567890")).toUInt();
		currentDataWord.remove(0, currentDataWord.toStdString().find_first_not_of(".-1234567890") + 1);
		unsigned int mode = currentDataWord.mid(0, currentDataWord.toStdString().find_first_not_of(".-1234567890")).toUInt();

		//Attention: A measurementTime less than 0.8s is not allowed!
		//For shorter measurement times the communication might be too slow so that an HT command for the automatic absorber is sent too late and the detector is already in saturation.
		if (measurementTime < 0.8)
		{
			
			this->writeDebugMessage("StepScan: the MeasurementTime was less than 0.8s! No measurement started!", Core::Debug::eDebugLevel_Error, "ProxyCommunicationServerD5000", "_slotAnalyzeNewCommand", this->_deviceName.toStdString());
			
			QString ErrorMessage;

			std::stringstream tempSS;

			tempSS << COMMUNICATION_START_SIGN_C << '?' << '3';		// ErrorMessage 3: Incorrect Argument
			ErrorMessage = QString(tempSS.str().c_str());
			this->sendAnswer(ErrorMessage);

			std::cout << "Error! Measurement time too short! Please select at least 0.8 sec!" << std::endl;
		}

		else
		{
			this->_clientCommunicationObj->doScan(axisNumber, numberOfSteps, stepSize, measurementTime, mode);
			this->_currentStatusTable->setScaningAxis((eAxisNames)axisNumber, true);
			this->sendAnswer(QString(COMMUNICATION_START_SIGN_C));
		}
	}

	else if (currentDataWord.toStdString().compare(1, 2, "MV") == 0)   //Measurement Values
	{
		currentDataWord.remove(0, 3);	//Erase .MV
		unsigned int numberOfAskedValues = currentDataWord.mid(0, currentDataWord.toStdString().find_first_not_of("1234567890")).toUInt();
		this->writeDebugMessage("DataWord was a Measurement Value request!", Core::Debug::eDebugLevel_Info, "ProxyCommunicationServerD5000", "_slotAnalyzeNewCommand", this->_deviceName.toStdString());
		this->sendAnswer(this->_createScanDataMsg(numberOfAskedValues));
		
		if (this->_currentStatusTable->getScanNumberOfRemainingMeasurementsPoints() <= 0)
		{
			this->_currentStatusTable->setScaning(false);
			
			for (unsigned int i=eTwoTheta; i<=ePhi; i++) 
			{
				if (this->_currentStatusTable->isScaningAxis((eAxisNames)i))
					this->_currentStatusTable->setScaningAxis((eAxisNames)i, false);
			}
			if (this->_currentStatusTable->isScaningAxis((eAxisNames)10))
				this->_currentStatusTable->setScaningAxis((eAxisNames)10, false);
		}
	}
	else if (currentDataWord.toStdString().compare(1, 2, "SU") == 0)	//Starts SetUp mode
	{
		this->sendAnswer(QString(COMMUNICATION_START_SIGN_C));
	}
	else if (currentDataWord.toStdString().compare(1, 2, "QU") == 0)	//Quit: ends SetUp mode
	{
		this->sendAnswer(QString(COMMUNICATION_START_SIGN_C));
	}
	else
	{
		this->writeDebugMessage("Unknown Command! ToDo!!!", Core::Debug::eDebugLevel_Error, "ProxyCommunicationServerD5000", "_slotAnalyzeNewCommand", this->_deviceName.toStdString());
	}
	
	
}
QString ProxyCommunicationServerD5000::_createStatusMsg()
{
	QString statusMsg;

	std::stringstream tempSS;
	tempSS << COMMUNICATION_START_SIGN_C << 'S' << 'T';		//.ST
	//Current Value of TwoTheta
	if ((this->_currentStatusTable->getInitializationStatus(eTwoTheta) == GlobalStatus::eIsNotInitialized) || (this->_currentStatusTable->getInitializationStatus(eTwoTheta) == GlobalStatus::eIsInInitialization))
		tempSS << "--.---" << ",";
	else
		tempSS << QString::number(this->_currentStatusTable->getCurrentValue(eTwoTheta), 'f', 3).toStdString() << ",";
	//Current Value of Theta
	if ((this->_currentStatusTable->getInitializationStatus(eTheta) == GlobalStatus::eIsNotInitialized) || (this->_currentStatusTable->getInitializationStatus(eTheta) == GlobalStatus::eIsInInitialization))
		tempSS << "--.---" << ",";
	else
		tempSS << QString::number(this->_currentStatusTable->getCurrentValue(eTheta), 'f', 3).toStdString() << ",";
	//Current Value of Chi
	if ((this->_currentStatusTable->getInitializationStatus(eChi) == GlobalStatus::eIsNotInitialized) || (this->_currentStatusTable->getInitializationStatus(eChi) == GlobalStatus::eIsInInitialization))
		tempSS << "--.---" << ",";
	else
		tempSS << QString::number(this->_currentStatusTable->getCurrentValue(eChi), 'f', 3).toStdString() << ",";
	//Current Value of Phi
	if ((this->_currentStatusTable->getInitializationStatus(ePhi) == GlobalStatus::eIsNotInitialized) || (this->_currentStatusTable->getInitializationStatus(ePhi) == GlobalStatus::eIsInInitialization))
		tempSS << "--.---" << ",";
	else
		tempSS << QString::number(this->_currentStatusTable->getCurrentValue(ePhi), 'f', 3).toStdString() << ",";
	tempSS << ",";	//Current Value of x
	tempSS << ",";	//Current Value of y
	tempSS << ",";	//Current Value of SampleChanger
	tempSS << ",";	//Current Value of Slit 1
	tempSS << ",";	//Current Value of Slit 2
	tempSS << QString::number((int)this->_currentStatusTable->getAbsorberLevel()).toStdString() << ","; //Current status of the slit changer
	tempSS << ",";	//Current Value of 40 Sample Magzine
	tempSS << QString::number(this->_currentStatusTable->getCurrentCountsPerSecond(), 'f', 3).toStdString() << ",";	//Count Rate in measuring electronics 1
	tempSS << QString::number(this->_currentStatusTable->getCurrentExposureTime(), 'f', 3).toStdString() << ",";	//Measuring time in measuring electronics 1
	tempSS << "0" << ",";	//Pulse Counter in measuring electronics 1
	tempSS << ",";	//Count Rate in measuring electronics 2
	tempSS << ",";	//Measuring time in measuring electronics 2
	tempSS << ",";	//Pulse Counter in measuring electronics 2

	//Create ReadyMessages (Fertigmeldungswert)
	int ReadyMessages = 0;
	for (unsigned int i=1; i<eAxisNamesSize; i++)
	{
		if (!this->_currentStatusTable->isPreparingMoving((eAxisNames)i) && !this->_currentStatusTable->isMoving((eAxisNames)i) && (this->_currentStatusTable->getInitializationStatus((eAxisNames)i) == GlobalStatus::eIsInitialized))
			ReadyMessages += pow((double)2, (int)(i-1));
	}
	if (ReadyMessages == 0)
		ReadyMessages = 16896;  //Slit (Absorber) and Shutter in desired position. If not, the GUI will lock when the device is not initialized.
	tempSS << QString::number(ReadyMessages).toStdString() << ",";	//Ready messages (Fertigmeldungswert)
	tempSS << "0" << ",";	//Warnings
	tempSS << "0" << ",";	//Alarms
	tempSS << "0" << ",";	//Trap register for alarms when the X-ray generator is switched off (Fangregister für Alarme bei Abschaltung)
	tempSS << "2" << ",";	//Mode control word (Betriebsartsteuerwort)
	if (this->_currentStatusTable->isScaning())
		tempSS << "4" << ",";	//Measuring sequence control word (Messablaufsteuerwort): a scan is executed
	else
		tempSS << "0" << ",";	//Measuring sequence control word (Messablaufsteuerwort): no scan is executed
	tempSS << QString::number(this->_currentStatusTable->getScanNumberOfRemainingMeasurementsPoints()).toStdString() << ",";	//Number of remaining steps for Step Scan (Zahl der Restschritte beim Step-Scan)
	tempSS << QString::number(this->_currentStatusTable->getScanNumberOfCachedValues()).toStdString() << ",";	//Number of stored measurement values (Zahl der gespeicherten Messwerte)
	tempSS << ",";			//Reason for abortion of a measurement (Abbruchursache bei Abbruch einer Messung)
	tempSS << ",";			//Missing ready messages upon abortion of a measurement (Fehlende Fertimeldungen bei Abbruch einer Messung)
	tempSS << ",";			//Pending alarms upon abortion of a measurement (Anstehende Alarme bei Abbruch einer Messung)

	statusMsg = QString(tempSS.str().c_str());

	return statusMsg;
}
QString ProxyCommunicationServerD5000::_createScanStatusMsg()
{
	QString statusMsg;

	std::stringstream tempSS;

	tempSS << COMMUNICATION_START_SIGN_C << 'S' << 'T';		//.ST
	tempSS << QString::number(this->_currentStatusTable->getScanNumberOfCachedValues()).toStdString();

	statusMsg = QString(tempSS.str().c_str());

	return statusMsg;
}
QString ProxyCommunicationServerD5000::_createScanDataMsg(unsigned int numberOfAskedValues)
{
	QString dataMsg;

	std::stringstream tempSS;

	tempSS << COMMUNICATION_START_SIGN_C << 'M' << 'V';		//.MV

	for (unsigned int i=0; i<numberOfAskedValues; i++)
	{
		if 	(this->_currentStatusTable->getScanNumberOfCachedValues() == 0)
			break;
		tempSS << QString::number(this->_currentStatusTable->getScanOldestCachedValue(), 'f', 4).toStdString();
		tempSS << ',';
	}
	dataMsg = QString(tempSS.str().c_str());
	//erase the last comma
	dataMsg.remove(dataMsg.length()-1, 1);

	return dataMsg;
}
QString ProxyCommunicationServerD5000::_createRatemeterMsg()
{
	QString rateMsg;
		
	std::stringstream tempSS;

	tempSS << COMMUNICATION_START_SIGN_C << 'S' << 'T';		//.ST
	tempSS << QString::number(this->_currentStatusTable->getCurrentCountsPerSecond()).toStdString();

	rateMsg = QString(tempSS.str().c_str());

	return rateMsg;
}

//*********************************
//*** ProxyCommunicationClient ***
//*********************************
ProxyCommunicationClient::ProxyCommunicationClient(std::string pathOfIniFile) : ProxyCommunication(pathOfIniFile)
{
	//Create serial IO Thread
	this->_serialIOThread.setParameter(&this->_serialPort, this->_deviceName);
	this->_serialIOThread.start();

	//Init Timer
	this->_pollingTimerSendStatusRequest.setInterval(500);		//0.5s
	this->_pollingTimerSendStatusRequest.setSingleShot(false);
	this->_pollingTimerSendStatusRequest.start();
	
	//Init scaning variables
	this->_scaningCurrentPointError = false;

	//Internal status parameter
	this->_isDetectingData = false;

	//Connect Slots
	QObject::connect(&this->_serialIOThread, SIGNAL(signalNewDataWord()), this, SLOT(_slotAnalyzeNewStatus()), Qt::DirectConnection);
	QObject::connect(&this->_pollingTimerSendStatusRequest, SIGNAL(timeout()), this, SLOT(_slotSendStatusRequest()), Qt::DirectConnection);

	//Wait Timer Runs in the Main Thread
	this->_waitTimerForMovement.setSingleShot(true);
	this->_waitTimerForMovement.setInterval(1000);			//1s
	QObject::connect(&this->_waitTimerForMovement, SIGNAL(timeout()), this, SLOT(_slotStartMovementTimerDone()), Qt::QueuedConnection);

	this->_waitTimerForScan.setSingleShot(true);
	this->_waitTimerForScan.setInterval(100);				//100ms
	QObject::connect(&this->_waitTimerForScan, SIGNAL(timeout()), this, SLOT(_slotScan()), Qt::QueuedConnection);
}
ProxyCommunicationClient::~ProxyCommunicationClient()
{
}
void ProxyCommunicationClient::setRemoteControl(bool doRemote)
{
	QMetaObject::invokeMethod(this, "_slotSetRemoteControl", Qt::QueuedConnection, Q_ARG(bool, doRemote));
	return;
}
void ProxyCommunicationClient::doHold(unsigned int axisNumber)
{
	if(this->_currentStatusTable->getScanNumberOfRemainingMeasurementsPoints() > 0)
		this->_currentStatusTable->setScanNumberOfRemainingMeasurementsPoints(0);
	if (this->_currentStatusTable->getScanNumberOfCachedValues() > 0)
		this->_currentStatusTable->deleteAllScanValues();
	if (this->_currentStatusTable->isScaning())
		this->_currentStatusTable->setScaning(false);
	for (unsigned int i=eTwoTheta; i<=ePhi; i++) 
	{
		if (this->_currentStatusTable->isScaningAxis((eAxisNames)i))
			this->_currentStatusTable->setScaningAxis((eAxisNames)i, false);
	}
	if (this->_currentStatusTable->isScaningAxis((eAxisNames)10))
		this->_currentStatusTable->setScaningAxis((eAxisNames)10, false);

	QMetaObject::invokeMethod(this, "_slotDoHold", Qt::QueuedConnection, Q_ARG(unsigned int, axisNumber));
	return;
}
void ProxyCommunicationClient::doInit(unsigned int axisNumber)
{
	this->_currentStatusTable->setPreparingMovingStatus((eAxisNames)axisNumber, true);
	QMetaObject::invokeMethod(this, "_slotDoInit", Qt::QueuedConnection, Q_ARG(unsigned int, axisNumber));
	return;
}
void ProxyCommunicationClient::doZeroInitial(eAxisNames axis,double newZero)
{
	QMetaObject::invokeMethod(this, "_slotDoZeroInitial", Qt::QueuedConnection, Q_ARG(unsigned int, axis), Q_ARG(double, newZero) );
	return;
}
unsigned int ProxyCommunicationClient::getAbsorberElements()
{
	return 1;
}
unsigned int ProxyCommunicationClient::getAbsorberCurrentActiveElement()
{
	return this->_currentStatusTable->getAbsorberLevel();
}
void ProxyCommunicationClient::doOpenShutter()
{
	QMetaObject::invokeMethod(this, "_slotDoOpenShutter", Qt::QueuedConnection);
	return;
}
void ProxyCommunicationClient::doCloseShutter()
{
	QMetaObject::invokeMethod(this, "_slotDoCloseShutter", Qt::QueuedConnection);
	return;
}
void ProxyCommunicationClient::setHighVoltage(unsigned int voltage, unsigned int deviceNumber)
{
	QMetaObject::invokeMethod(this, "_slotSetHighVoltage", Qt::QueuedConnection, Q_ARG(unsigned int, voltage), Q_ARG(unsigned int, deviceNumber));
	return;
}
void ProxyCommunicationClient::setAmplifierGain(unsigned int amplifierGain, unsigned int deviceNumber)
{
	QMetaObject::invokeMethod(this, "_slotSetAmplifierGain", Qt::QueuedConnection, Q_ARG(unsigned int, amplifierGain), Q_ARG(unsigned int, deviceNumber));
	return;
}
void ProxyCommunicationClient::setDiscriminator(double discriminatorLowBorder, double discriminatorWidth, unsigned int deviceNumber)
{
	QMetaObject::invokeMethod(this, "_slotSetDiscriminator", Qt::QueuedConnection, Q_ARG(double, discriminatorLowBorder), Q_ARG(double, discriminatorWidth), Q_ARG(unsigned int, deviceNumber));
	return;
}
void ProxyCommunicationClient::setIntegrator(double timeConstant, unsigned int controlLimit, unsigned int deviceNumber)
{
	QMetaObject::invokeMethod(this, "_slotSetIntegrator", Qt::QueuedConnection, Q_ARG(double, timeConstant), Q_ARG(unsigned int, controlLimit), Q_ARG(unsigned int, deviceNumber));
	return;
}
void ProxyCommunicationClient::doMove(eAxisNames axis, double targetValue)
{
	this->_currentStatusTable->setPreparingMovingStatus(axis, true);
	QMetaObject::invokeMethod(this, "_slotDoMove", Qt::QueuedConnection, Q_ARG(unsigned int, axis), Q_ARG(double, targetValue));
	return;
}
void ProxyCommunicationClient::doScan(unsigned int axisNumber, unsigned int numberOfSteps, double stepSize, double measurementTime, unsigned int mode)
{
	//Set temp scan parameter
	this->_axisNumber = axisNumber;
	this->_numberOfSteps = numberOfSteps;
	this->_stepSize = stepSize;
	this->_measurementTime = measurementTime;
	this->_mode = mode;

	QMetaObject::invokeMethod(this, "_slotScan", Qt::QueuedConnection);
	return;
}
void ProxyCommunicationClient::doRate()
{
	QMetaObject::invokeMethod(this, "_slotDoRate", Qt::QueuedConnection);
	return;
}
void ProxyCommunicationClient::doOldZero()
{
	QMetaObject::invokeMethod(this, "_slotDoOldZero", Qt::QueuedConnection);
	return;
}
void ProxyCommunicationClient::doOldZeroAll()
{
	QMetaObject::invokeMethod(this, "_slotDoOldZeroAll", Qt::QueuedConnection);
	return;
}
// ProxyCommunicationClientD500 Class
ProxyCommunicationClientD500::ProxyCommunicationClientD500(std::string pathOfIniFile) : ProxyCommunicationClient(pathOfIniFile)
{
	//Init Scan Flag Parameter
	for (unsigned int i=0; i<eScanFlagValuesSize; i++)		//Attention it is necessary to set all flag values to false. If the axis values are not false, they will be counted twice. 
	{
		this->_scanFlagValues.flagArray[i] = false;
	}

	this->_scanFlagValues.flagArray[eDiagramPlot] = false;
	this->_scanFlagValues.flagArray[eHeadingRemoval] = true;
	this->_scanFlagValues.flagArray[ePeakSearch] = false;
	this->_scanFlagValues.flagArray[ePlotLocalDiagram] = false;
	this->_scanFlagValues.flagArray[ePrintSSCLResoults] = false;
	this->_scanFlagValues.flagArray[eSampleRefPrompted] = false;
	this->_scanFlagValues.flagArray[eTableOfCountingValues] = true;
	this->_scanFlagValues.flagArray[eTransCPLocalMode] = false;
	this->_scanFlagValues.flagArray[ePeakTableDSpacingRestriction] = false;
	this->_scanFlagValues.flagArray[ePeakTableIntRestriction] = false;
	this->_scanFlagValues.flagArray[e24Bit] = false;
	this->_scanFlagValues.flagArray[eContinuous] = false;
	this->_scanFlagValues.useAxis = eTwoTheta;
}
ProxyCommunicationClientD500::~ProxyCommunicationClientD500()
{
}
void ProxyCommunicationClientD500::setAbsorberUp()
{
	this->setSlitR(false);
	if (this->getAbsorberCurrentActiveElement() == 1)
	this->_currentStatusTable->setAbsorberLevel(this->getAbsorberCurrentActiveElement() - 1);
}
void ProxyCommunicationClientD500::setUnusedAbsorberDown()
{
	this->setSlitL(true);
}
void ProxyCommunicationClientD500::setUnusedAbsorberUp()
{
	this->setSlitL(false);
}
void ProxyCommunicationClientD500::setAbsorberDown()
{
	this->setSlitR(true);
	if (this->getAbsorberCurrentActiveElement() == 0)
	this->_currentStatusTable->setAbsorberLevel(this->getAbsorberCurrentActiveElement() + 1);
}
void ProxyCommunicationClientD500::setSlitL(bool doActive)
{
	QMetaObject::invokeMethod(this, "_slotSetSlitL", Qt::QueuedConnection, Q_ARG(bool, doActive));
	return;
}
void ProxyCommunicationClientD500::setSlitR(bool doActive)
{
	QMetaObject::invokeMethod(this, "_slotSetSlitR", Qt::QueuedConnection, Q_ARG(bool, doActive));
	return;
}
void ProxyCommunicationClientD500::_slotAnalyzeNewStatus()
{

	QString currentDataWord = this->_serialIOThread.getNextReceivedDataWord();

	if (currentDataWord == "")
		return;

	this->writeDebugMessage(QString("New DataWord: " + currentDataWord).toStdString(), 5, "ProxyCommunicationClientD500", "_slotAnalyzeNewStatus()", this->_deviceName.toStdString());

	
	
	//No Normal Status Answer
	if ((currentDataWord.toStdString().compare(1, 1, " ") == 0) || (currentDataWord.toStdString().compare(1, 1, "-") == 0))
	{
		if (currentDataWord.length() < 14)
		{
			//Rate Request answer
			this->writeDebugMessage("DataWord is a ratemeter message!", Core::Debug::eDebugLevel_Debug, "ProxyCommunicationClientD500", "_slotAnalyzeNewStatus", this->_deviceName.toStdString());

			currentDataWord.remove(0, currentDataWord.toStdString().find_first_of(".-1234567890"));	//Erase . and spaces
			double value = currentDataWord.mid(0, currentDataWord.toStdString().find_first_not_of(".-1234567890")).toDouble();
			currentDataWord.remove(0, currentDataWord.toStdString().find_first_not_of(".-1234567890") + 1);
			this->_currentStatusTable->setCurrentCountsPerSecond(value);
			this->writeDebugMessage(QString("Ratemeter Answer: CurrentCountsPerSecond = " + QString::number(this->_currentStatusTable->getCurrentCountsPerSecond())).toStdString(), Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotAnalyzeNewStatus", this->_deviceName.toStdString());
			return;
		}
		else
		{
			//Scan Mode Answer
			//Client is currently scanning
			this->writeDebugMessage("DataWord is a scanning message!", Core::Debug::eDebugLevel_Debug, "ProxyCommunicationClientD500", "_slotAnalyzeNewStatus", this->_deviceName.toStdString());

			currentDataWord.remove(0, currentDataWord.toStdString().find_first_of(".-1234567890"));	//Erase . and spaces
			double angle = currentDataWord.mid(0, currentDataWord.toStdString().find_first_not_of(".-1234567890")).toDouble();
			currentDataWord.remove(0, currentDataWord.toStdString().find_first_not_of(".-1234567890") + 1);
			currentDataWord.remove(0, currentDataWord.toStdString().find_first_of(".-1234567890"));	//Erase spaces
			double value = currentDataWord.mid(0, currentDataWord.toStdString().find_first_not_of(".-1234567890")).toDouble();
			currentDataWord.remove(0, currentDataWord.toStdString().find_first_not_of(".-1234567890") + 1);

			//Set current value (counts of the scan) to CountsPerSecond
			this->_currentStatusTable->setCurrentCountsPerSecond(value / this->_currentStatusTable->getCurrentExposureTime());
			this->writeDebugMessage(QString("Scan Answer: CurrentCountsPerSecond = " + QString::number(this->_currentStatusTable->getCurrentCountsPerSecond())).toStdString(), Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotAnalyzeNewStatus", this->_deviceName.toStdString());

			//Put measured point to the data list
			this->_currentStatusTable->setScanNewMeasuredValue(value);

			//reduce the number of remaining measurement points
			if(this->_currentStatusTable->getScanNumberOfRemainingMeasurementsPoints() > 0)
				this->_currentStatusTable->setScanNumberOfRemainingMeasurementsPoints(this->_currentStatusTable->getScanNumberOfRemainingMeasurementsPoints()-1);


			//set the current position for the status info
			for (unsigned int i=eTwoTheta; i<=ePhi; i++)   
				{
					if (this->_currentStatusTable->isScaningAxis((eAxisNames)i))
						this->_currentStatusTable->setCurrentValue((eAxisNames)i, angle);
				}
			if (this->_currentStatusTable->isScaningAxis((eAxisNames)10))  //2Theta and Theta are scanning
			{
				this->_currentStatusTable->setCurrentValue(eTwoTheta, angle);
				double offsetAngle = 0;
				offsetAngle = (angle/2 - this->_currentStatusTable->getUnlockedCoupledOffset());
				this->_currentStatusTable->setCurrentValue(eTheta, offsetAngle);
			}

			
			if (this->_currentStatusTable->getScanNumberOfRemainingMeasurementsPoints() <= 0)
				this->_setIsDetectingData(false);

			return;
		}
	}
	//Normal Answer
	//Analyze Data Word First Part
	if (currentDataWord.toStdString().compare(1, 1, "V") == 0)
	{
		//Is a status report
		this->writeDebugMessage("DataWord is the first part of a status!", Core::Debug::eDebugLevel_Debug, "ProxyCommunicationClientD500", "_slotAnalyzeNewStatus", this->_deviceName.toStdString());

		//*** Set current Status ***
		//**************************
		currentDataWord = currentDataWord.remove(0, 2);		//Remove .V
		//Set Version
		unsigned int version = currentDataWord.left(2).toInt();
		currentDataWord = currentDataWord.remove(0, 2);
		//Set number of axis
		unsigned int numberOfAxis = currentDataWord.left(1).toInt();
		currentDataWord = currentDataWord.remove(0, 1);
		//Set diffractometer type
		int diffractometerType = currentDataWord.left(2).toInt();
		currentDataWord = currentDataWord.remove(0, 2);
		//Set sample changer info
		int sampleChangerInfo = currentDataWord.left(3).toInt();
		currentDataWord = currentDataWord.remove(0, 3);
		//Set Type of X-Ray electronics
		int xRayElectronicType = currentDataWord.left(3).toInt();
		currentDataWord = currentDataWord.remove(0, 3);

		//Set current status bit value
		currentDataWord = currentDataWord.remove(0, 4);
		int statusBitValue = currentDataWord.left(6).toInt();
		currentDataWord = currentDataWord.remove(0, 6);

		//Set error status bit value
		currentDataWord = currentDataWord.remove(0, 6);
		int errorBitValue = currentDataWord.left(4).toInt();
		currentDataWord = currentDataWord.remove(0, 4);

		//Set lable of current instruction
		currentDataWord = currentDataWord.remove(0, 4);
		int currentInstruction = currentDataWord.left(6).toInt();
		currentDataWord = currentDataWord.remove(0, 6);

		//Set sample changer position
		currentDataWord = currentDataWord.remove(0, 5);
		int sampleChangerPosition = currentDataWord.left(3).toInt();
		currentDataWord = currentDataWord.remove(0, 3);

		//Set sample changer slot is used
		currentDataWord = currentDataWord.remove(0, 4);
		int sampleChangerSlotIsUsed = currentDataWord.left(2).toInt();
		currentDataWord = currentDataWord.remove(0, 2);

		if ((statusBitValue & 11) != 0)
		{
		}
		else
		{
			//Is not Moving
			for (unsigned int i=0; i<eAxisNamesSize; i++)
			{
				//this->_currentStatusTable->setMovingStatus((eAxisNames)i, false);
				if (this->_currentStatusTable->getInitializationStatus((eAxisNames)i) == GlobalStatus::eIsInInitialization)
					this->_currentStatusTable->setInitialize((eAxisNames)i, GlobalStatus::eIsInitialized);
			}
		}
		if ((statusBitValue & 4) == 1)
		{
			//Setup Mode

			//ToDo
		}
		if ((statusBitValue & 32) == 1)
		{
			//Local Mode

			//ToDo
		}
		if ((statusBitValue & 64) == 1)
		{
			//Computer Mode

			//ToDo
		}
		if ((statusBitValue & 128) == 1)
		{
			//Program Mode

			//ToDo
		}
		if ((statusBitValue & 256) == 1)
		{
			//Puls counter is active

			//ToDo
		}
		if ((statusBitValue & 8192) == 1)
		{
			//local terminal used by DACO-MP

			//ToDo
		}

		if ((errorBitValue & 1) == 1)
		{
			//HV1 error

			//ToDo
		}
		if ((errorBitValue & 2) == 1)
		{
			//Sample Changer Error

			//ToDo
		}
		if ((errorBitValue & 4) == 1)
		{
			//HV2 error

			//ToDo
		}
		if ((errorBitValue & 8) == 1)
		{
			//Motor error

			//ToDo
		}
		if ((errorBitValue & 16) == 1)
		{
			//X-Ray generator off

			//ToDo
		}
		if ((errorBitValue & 32) == 1)
		{
			//Water error

			//ToDo
		}
		if ((errorBitValue & 64) == 1)
		{
			//Shutter closed

			//ToDo
		}
		if ((errorBitValue & 128) == 1)
		{
				//HV off

		//ToDo
		}

		return;
	}
	//Normal Answer
	//Analyze Data Word Second Part
	if (currentDataWord.toStdString().compare(1, 6, "2Theta") == 0)
	{
		//Is a status report
		this->writeDebugMessage("DataWord is the second part of a status!", Core::Debug::eDebugLevel_Debug, "ProxyCommunicationClientD500", "_slotAnalyzeNewStatus", this->_deviceName.toStdString());

		//*** Set current Status ***
		//**************************
		//Set 2Theta value
		currentDataWord = currentDataWord.remove(0, 7);		//Remove .2Theta
		if (currentDataWord.left(9).contains('*'))
		{
			//If axis is not initialized
			this->_currentStatusTable->setInitialize(eTwoTheta, GlobalStatus::eIsNotInitialized);
		}
		else
		{
			//If axis is initialized
			double twoTheta = currentDataWord.left(9).toDouble();
			this->_currentStatusTable->setCurrentValue(eTwoTheta, twoTheta);
		}
		currentDataWord = currentDataWord.remove(0, 9);

		//Set Theta value
		currentDataWord = currentDataWord.remove(0, 6);		//Remove Theta
		if (currentDataWord.left(9).contains('*'))
		{
			//If axis is not initialized
			this->_currentStatusTable->setInitialize(eTheta, GlobalStatus::eIsNotInitialized);
		}
		else
		{
			//If axis is initialized
			double theta = currentDataWord.left(9).toDouble();
			this->_currentStatusTable->setCurrentValue(eTheta, theta);
		}
		currentDataWord = currentDataWord.remove(0, 9);

		//Set Chi value
		currentDataWord = currentDataWord.remove(0, 4);		//Remove Chi
		if (currentDataWord.left(9).contains('*'))
		{
			//If axis is not initialized
			this->_currentStatusTable->setInitialize(eChi, GlobalStatus::eIsNotInitialized);
		}
		else
		{
			//If axis is initialized
			double chi = currentDataWord.left(9).toDouble();
			this->_currentStatusTable->setCurrentValue(eChi, chi);
		}
		currentDataWord = currentDataWord.remove(0, 9);

		//Set Phi value
		currentDataWord = currentDataWord.remove(0, 4);		//Remove Phi
		if (currentDataWord.left(9).contains('*'))
		{
			//If axis is not initialized
			this->_currentStatusTable->setInitialize(ePhi, GlobalStatus::eIsNotInitialized);
		}
		else
		{
			//If axis is initialized
			double phi = currentDataWord.left(9).toDouble();
			this->_currentStatusTable->setCurrentValue(ePhi, phi);
		}
		currentDataWord = currentDataWord.remove(0, 9);

		return;
	}
}
void ProxyCommunicationClientD500::_slotSendStatusRequest()
{
	if (this->_getIsDetectingData() == true)
	{
		//Client is currently scanning

		//Send 0x05 to read the scan status message
		this->_serialIOThread.sendDataWord(QChar(COMMUNICATION_SEND_REQUEST_SIGN_C));
	}
	else
	{
		//Normal Mode: the Client is not scanning now

		this->writeDebugMessage("Send Status Request to the Diffractometer!", Core::Debug::eDebugLevel_Debug, "ProxyCommunicationClientD500", "_slotSendStatusRequest", this->_deviceName.toStdString());
		std::stringstream tempSS;
		tempSS << COMMUNICATION_START_SIGN_C << 'S' << 'T';		//.ST. (0x01; 0x53; 0x54)
		this->sendCommand(QString(tempSS.str().c_str()));		//attach 0x0D and 0x05 inside sendCommand

		//Send the polling message to get both parts of the status message
		//****************************************************
		//Send 0x05; to read the first part of the answer!
		this->_serialIOThread.sendDataWord(QChar(COMMUNICATION_SEND_REQUEST_SIGN_C/*0x05*/));

		//Send 0x05; to read the second part of the answer!
		this->_serialIOThread.sendDataWord(QChar(COMMUNICATION_SEND_REQUEST_SIGN_C/*0x05*/));
	}
}
void ProxyCommunicationClientD500::sendCommand(QString commandString)
{
	commandString = QString(commandString + QChar(COMMUNICATION_TERMINATOR_SIGN_C) + QChar(COMMUNICATION_SEND_REQUEST_SIGN_C));
	this->_serialIOThread.sendDataWord(commandString);
}
void ProxyCommunicationClientD500::_slotSetRemoteControl(bool doRemote)
{
	this->writeDebugMessage("Enter: Sent RemoteControl Command! (\"RC\")", Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotSetRemoteControl(bool doRemote)", this->_deviceName.toStdString());
	std::stringstream tempSS;
	tempSS << COMMUNICATION_START_SIGN_C << 'R' << 'C';
	if (doRemote)
		tempSS << '0';
	else
		tempSS << '1';
	this->sendCommand(QString(tempSS.str().c_str()));
	this->writeDebugMessage(QString("Done: Sent RemoteControl Command! (\"" + QString(tempSS.str().c_str()) + "\")").toStdString(), Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotSetRemoteControl(bool doRemote)", this->_deviceName.toStdString());
}
void ProxyCommunicationClientD500::_slotDoHold(unsigned int axisNumber)
{
	this->_setIsDetectingData(false);

	this->writeDebugMessage("Enter: Sent Hold Command! (\"HT\")", Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotDoHold(unsigned int axisNumber)", this->_deviceName.toStdString());
	std::stringstream tempSS;
	tempSS << COMMUNICATION_START_SIGN_C << 'H' << 'T';
	this->sendCommand(QString(tempSS.str().c_str()));
	this->writeDebugMessage("Done: Sent Hold Command! (\"HT\")", Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotDoHold(unsigned int axisNumber)", this->_deviceName.toStdString());
}
void ProxyCommunicationClientD500::_slotDoInit(unsigned int axisNumber)
{
	if(this->_currentStatusTable->getInitializationStatus() == GlobalStatus::eIsInInitialization)
		return;

	this->writeDebugMessage("Enter: Sent Init Command! (\"IN\")", Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotDoInit(unsigned int axisNumber)", this->_deviceName.toStdString());

	//Set all axis to isPreparingMoving status
	for (unsigned int i=0; i<eAxisNamesSize; i++)
	{
		//Change Status
		this->_currentStatusTable->setInitialize((eAxisNames)i, GlobalStatus::eIsInInitialization);
		this->_currentStatusTable->setPreparingMovingStatus((eAxisNames)i, true);
	}

	std::stringstream tempSS;
	tempSS << COMMUNICATION_START_SIGN_C << 'I' << 'N';
	this->sendCommand(QString(tempSS.str().c_str()));
	this->writeDebugMessage("Done: Sent Init Command! (\"IN\")", Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotDoInit(unsigned int axisNumber)", this->_deviceName.toStdString());
}
void ProxyCommunicationClientD500::_slotDoZeroInitial(unsigned int axis, double newZero)
{
	//Load File
	this->_currentStatusTable->loadParameterOffsetValues("./offsetParameter.txt");

	this->writeDebugMessage("Enter: Sent ZeroInitial Command!  (\"IN0\")", Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotDoZeroInitial()", this->_deviceName.toStdString());


	std::stringstream tempSS;
	tempSS << COMMUNICATION_START_SIGN_C << 'I' << 'N';

	for (unsigned int i=eTwoTheta; i<=ePhi; i++)
	{
		if (i == axis)
		{
			tempSS << (newZero + this->_currentStatusTable->getCurrentValue((eAxisNames)i) - this->_currentStatusTable->getZIValue((eAxisNames)i));
			this->_currentStatusTable->setZIValue((eAxisNames)i, newZero);
		}
		else
			tempSS << this->_currentStatusTable->getCurrentValue((eAxisNames)i);
		tempSS << ",";
	}

	this->sendCommand(QString(tempSS.str().c_str()));
	this->writeDebugMessage("Done: Sent ZeroInitial Command!  (\"IN0\")", Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotDoZeroInitial()", this->_deviceName.toStdString());

	//Save File
	this->_currentStatusTable->saveParameterOffsetValues("./offsetParameter.txt");
}
void ProxyCommunicationClientD500::_slotSetSlitL(bool doActive)
{
	this->writeDebugMessage("Enter: Sent Left Slit Command!  (\"OC2,\")", Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotSetSlitL(bool doActive)", this->_deviceName.toStdString());

	std::stringstream tempSS;
	tempSS << COMMUNICATION_START_SIGN_C << 'O' << 'C' << '2' << ',';
	if (!doActive)
	{
		tempSS << '1';
	}
	else
	{
		tempSS << '0';
	}
	this->sendCommand(QString(tempSS.str().c_str()));
	this->writeDebugMessage(QString("Done: Sent Left Slit Command!  (\"" + QString(tempSS.str().c_str()) + "\")").toStdString(), 3, "ProxyCommunicationClientD500", "_slotSetSlitL(bool doActive)", this->_deviceName.toStdString());
}
void ProxyCommunicationClientD500::_slotSetSlitR(bool doActive)
{
	this->writeDebugMessage("Enter: Sent Right Slit Command!  (\"OC1,\")", Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotSetSlitR(bool doActive)", this->_deviceName.toStdString());
	
	std::stringstream tempSS;
	tempSS << COMMUNICATION_START_SIGN_C << 'O' << 'C' << '1' << ',';
	if (!doActive)
	{
		tempSS << '1';
	}
	else
	{
		tempSS << '0';
	}
	this->sendCommand(QString(tempSS.str().c_str()));
	this->writeDebugMessage(QString("Done: Sent Right Slit Command!  (\"" + QString(tempSS.str().c_str()) + "\")").toStdString(), Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotSetSlitR(bool doActive)", this->_deviceName.toStdString());
}
void ProxyCommunicationClientD500::_slotDoOpenShutter()
{
	this->writeDebugMessage("Enter: Sent OpenShutter Command!  (\"OC4,1\")", Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotDoOpenShutter()", this->_deviceName.toStdString());
	std::stringstream tempSS;
	tempSS << COMMUNICATION_START_SIGN_C << 'O' << 'C' << '4' << ',' << '1';
	this->sendCommand(QString(tempSS.str().c_str()));
	this->writeDebugMessage("Done: Sent OpenShutter Command!  (\"OC4,1\")", Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotDoOpenShutter()", this->_deviceName.toStdString());
}
void ProxyCommunicationClientD500::_slotDoCloseShutter()
{
	this->writeDebugMessage("Enter: Sent CloseShutter Command!  (\"OC4,0\")", Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotDoCloseShutter()", this->_deviceName.toStdString());
	std::stringstream tempSS;
	tempSS << COMMUNICATION_START_SIGN_C << 'O' << 'C' << '4' << ',' << '0';
	this->sendCommand(QString(tempSS.str().c_str()));
	this->writeDebugMessage("Done: Sent CloseShutter Command!  (\"OC4,0\")", Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotDoCloseShutter()", this->_deviceName.toStdString());
}
void ProxyCommunicationClientD500::_slotSetHighVoltage(unsigned int voltage, unsigned int deviceNumber)
{
	this->writeDebugMessage("HighVoltage for detector is not supported!", Core::Debug::eDebugLevel_Warning, "ProxyCommunicationClientD500", "setHighVoltage", this->_deviceName.toStdString());
}
void ProxyCommunicationClientD500::_slotSetAmplifierGain(unsigned int amplifierGain, unsigned int deviceNumber)
{
	this->writeDebugMessage("AmplifierGain for detector is not supported!", Core::Debug::eDebugLevel_Warning, "ProxyCommunicationClientD500", "setAmplifierGain", this->_deviceName.toStdString());
}
void ProxyCommunicationClientD500::_slotSetDiscriminator(double discriminatorLowBorder, double discriminatorWidth, unsigned int deviceNumber)
{
	this->writeDebugMessage("Discriminator for detector is not supported!", Core::Debug::eDebugLevel_Warning, "ProxyCommunicationClientD500", "setDiscriminator", this->_deviceName.toStdString());
}
void ProxyCommunicationClientD500::_slotSetIntegrator(double timeConstant, unsigned int controlLimit, unsigned int deviceNumber)
{
	this->writeDebugMessage("Integrator for detector is not supported!", Core::Debug::eDebugLevel_Warning, "ProxyCommunicationClientD500", "setIntegrator", this->_deviceName.toStdString());
}
void ProxyCommunicationClientD500::_slotDoMove(unsigned int axis, double targetValue)
{
	this->writeDebugMessage("Enter: Set Target Value inside the global status table and start the wait Counter, if the counter is not running!", 3, "ProxyCommunicationClientD500", "_slotDoMove(unsigned int axis, double targetValue)", this->_deviceName.toStdString());
	if(!(this->_currentStatusTable->getInitializationStatus() == GlobalStatus::eIsInitialized))
	{
		this->writeDebugMessage("Done: Movement not possible: the device is not initialized", Core::Debug::eDebugLevel_Error, "ProxyCommunicationClientD500", "_slotDoMove(unsigned int axis, double targetValue)", this->_deviceName.toStdString());
		return;
	}

	//Set TargetValue and Trigger the wait counter
	//Up to the end of the WaitCounter it is allowed to catch target values for other axes.
	this->_currentStatusTable->setTargetValue((eAxisNames)axis, targetValue);
	
	if (!this->_waitTimerForMovement.isActive())
	{
		this->_waitTimerForMovement.start();
	}
	this->writeDebugMessage("Done: Set Target Value inside the global status table and start the wait Counter, if the counter is not running!", Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotDoMove(unsigned int axis, double targetValue)", this->_deviceName.toStdString());
}
void ProxyCommunicationClientD500::_slotStartMovementTimerDone()
{
	this->writeDebugMessage("Enter: Sent Move Command! Wait Timer is Done!", Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotStartMovementTimerDone()", this->_deviceName.toStdString());
	if(!(this->_currentStatusTable->getInitializationStatus() == GlobalStatus::eIsInitialized))
	{
		this->writeDebugMessage("Movement not possible: the device is not initialized", Core::Debug::eDebugLevel_Error, "ProxyCommunicationClientD500", "_slotStartMovementTimerDone()", this->_deviceName.toStdString());
		return;
	}

	double offset = 0; //necessary to show current angles for (un)locked coupled scans on the GUI

	// Execute Movement
	std::stringstream tempSS;
	tempSS << COMMUNICATION_START_SIGN_C << 'G' << 'O';
	
	if ( (this->_currentStatusTable->isPreparingMoving(eTwoTheta)) && 
		 abs(this->_currentStatusTable->getTargetValue(eTwoTheta) - this->_currentStatusTable->getCurrentValue(eTwoTheta)) > this->_currentStatusTable->getToleranz(eTwoTheta)
	   )
	{
		tempSS << this->_currentStatusTable->getTargetValue(eTwoTheta);
		offset = (this->_currentStatusTable->getTargetValue(eTwoTheta))/2;
	}
	else
	{
		tempSS << this->_currentStatusTable->getCurrentValue(eTwoTheta);
		offset = (this->_currentStatusTable->getCurrentValue(eTwoTheta))/2;
		this->_currentStatusTable->setPreparingMovingStatus(eTwoTheta, false);   
	}
	tempSS << ',';
	if ( (this->_currentStatusTable->isPreparingMoving(eTheta)) && 
		 abs(this->_currentStatusTable->getTargetValue(eTheta) - this->_currentStatusTable->getCurrentValue(eTheta)) > this->_currentStatusTable->getToleranz(eTheta)
	   )
		{
			tempSS << this->_currentStatusTable->getTargetValue(eTheta);
			offset = offset - (this->_currentStatusTable->getTargetValue(eTheta));
		}
	else
	{
		tempSS << this->_currentStatusTable->getCurrentValue(eTheta);
		offset = offset - (this->_currentStatusTable->getTargetValue(eTheta));
		this->_currentStatusTable->setPreparingMovingStatus(eTheta, false);
	}
	tempSS << ',';
	if ( (this->_currentStatusTable->isPreparingMoving(eChi)) && 
		 abs(this->_currentStatusTable->getTargetValue(eChi) - this->_currentStatusTable->getCurrentValue(eChi)) > this->_currentStatusTable->getToleranz(eChi)
	   )
		{
			tempSS << this->_currentStatusTable->getTargetValue(eChi);
		}
	else
	{
		tempSS << this->_currentStatusTable->getCurrentValue(eChi);
		this->_currentStatusTable->setPreparingMovingStatus(eChi, false);
	}
	tempSS << ',';
	if ( (this->_currentStatusTable->isPreparingMoving(ePhi)) && 
		 abs(this->_currentStatusTable->getTargetValue(ePhi) - this->_currentStatusTable->getCurrentValue(ePhi)) > this->_currentStatusTable->getToleranz(ePhi)
	   )
		{
			tempSS << this->_currentStatusTable->getTargetValue(ePhi);
		}
	else
	{
		tempSS << this->_currentStatusTable->getCurrentValue(ePhi);
		this->_currentStatusTable->setPreparingMovingStatus(ePhi, false);
	}

	
	this->_currentStatusTable->setUnlockedCoupledOffset(offset);

	this->sendCommand(QString(tempSS.str().c_str()));
	this->writeDebugMessage("Done: Sent Move Command! Wait Timer is Done!", Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotStartMovementTimerDone()", this->_deviceName.toStdString());
}
void ProxyCommunicationClientD500::_slotScan()
{
	//Set Internal Status Info
	this->_setIsDetectingData(true);

	this->writeDebugMessage("Enter: Sent StepScan Command!", Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotStepScan()", this->_deviceName.toStdString());
	if(!(this->_currentStatusTable->getInitializationStatus() == GlobalStatus::eIsInitialized))
	{
		this->writeDebugMessage("StepScan not possible: the device is not initialized", Core::Debug::eDebugLevel_Error, "ProxyCommunicationClientD500", "_slotStepScan()", this->_deviceName.toStdString());
		return;
	}
	//Set Scan Axis and Scan Mode
	this->_scanFlagValues.useAxis = (eAxisNames)this->_axisNumber;
	
	if (this->_mode == GlobalStatus::eStep)
	{
		this->_scanFlagValues.flagArray[eContinuous] = false;
	}
	else
	{
		this->_scanFlagValues.flagArray[eContinuous] = true;
	}

	//Set Global Status Info
	this->_currentStatusTable->setCurrentExposureTime(this->_measurementTime);
	this->_currentStatusTable->setScaning(true);
	this->_currentStatusTable->setScanNumberOfRemainingMeasurementsPoints(this->_numberOfSteps+1);

	//Set Internal Status Info
	this->_setIsDetectingData(true);

	if (this->_currentStatusTable->getScanNumberOfCachedValues() > 0)    //Stored measurement values are erased
		this->_currentStatusTable->deleteAllScanValues();

	// Execute Movement
	std::stringstream tempSS;
	
	tempSS << COMMUNICATION_START_SIGN_C << 'S' << 'S';

	tempSS << this->_numberOfSteps+1 << ',';	//Send number of measurement points, not steps
	tempSS << this->_measurementTime << ',';
	tempSS << this->_stepSize << ',';
	tempSS << this->_getScanFlagValue();  //Defines axes and mode for the scan

	this->sendCommand(QString(tempSS.str().c_str()));
	this->writeDebugMessage("Done: Sent StepScan Command!", Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotStepScan()", this->_deviceName.toStdString());
}
void ProxyCommunicationClientD500::_slotDoRate()
{
	this->writeDebugMessage("Enter: Sent Ratemeter Command!  (\"RA\")", Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotDoRate()", this->_deviceName.toStdString());
	std::stringstream tempSS;
	tempSS << COMMUNICATION_START_SIGN_C << 'R' << 'A';
	this->sendCommand(QString(tempSS.str().c_str()));
	this->_serialIOThread.sendDataWord(QChar(COMMUNICATION_SEND_REQUEST_SIGN_C));  // second 0x05 necessary to get data
	this->writeDebugMessage("Done: Sent Sent Ratemeter Command!  (\"RA\")", Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotDoRate()", this->_deviceName.toStdString());
}
void ProxyCommunicationClientD500::_slotDoOldZero()
{
	this->writeDebugMessage("Enter: Sent Old Zero Initial Command!  (\"IN0\")", Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotDoOldZero()", this->_deviceName.toStdString());
	std::stringstream tempSS;
	tempSS << COMMUNICATION_START_SIGN_C << 'I' << 'N' << '0';
	this->sendCommand(QString(tempSS.str().c_str()));
	this->writeDebugMessage("Done: Sent Old Zero Initial Command!  (\"IN0\")", Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotDoOldZero()", this->_deviceName.toStdString());
}
void ProxyCommunicationClientD500::_slotDoOldZeroAll()
{
	this->writeDebugMessage("Enter: Sent Old Zero Initial All Command!  (\"IN0,0,0,0\")", Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotDoOldZeroAll()", this->_deviceName.toStdString());
	std::stringstream tempSS;
	tempSS << COMMUNICATION_START_SIGN_C << 'I' << 'N' << '0'<< ','<< '0'<< ','<< '0'<< ','<< '0';
	this->sendCommand(QString(tempSS.str().c_str()));
	this->writeDebugMessage("Done: Sent Old Zero Initial All Command!  (\"IN0,0,0,0\")", Core::Debug::eDebugLevel_Info, "ProxyCommunicationClientD500", "_slotDoOldZeroAll()", this->_deviceName.toStdString());
}
unsigned int ProxyCommunicationClientD500::_getScanFlagValue()
{
	unsigned int flagValue = 0;

	for (unsigned int i=0; i<eScanFlagValuesSize; i++)
	{
		if (this->_scanFlagValues.flagArray[i])
			flagValue += pow(2.0, (int)i);
	}

	if (this->_scanFlagValues.useAxis == eTwoTheta)
		flagValue += pow(2.0, eAxisSelectBit2Theta);
	if (this->_scanFlagValues.useAxis == eTheta)
		flagValue += pow(2.0, eAxisSelectBit1);
	if (this->_scanFlagValues.useAxis == eChi)
		flagValue += pow(2.0, eAxisSelectBit2);
	if (this->_scanFlagValues.useAxis == ePhi)
	{
		flagValue += pow(2.0, eAxisSelectBit1);
		flagValue += pow(2.0, eAxisSelectBit2);
	}
	

	return flagValue;
}
