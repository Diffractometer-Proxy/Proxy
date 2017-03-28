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
#ifndef PROXYCOMMUNICATION_H
#define PROXYCOMMUNICATION_H

#include "proxycommunication_global.h"

#include <QExtSerialPort.h>

#include <Core-Debug.h>
#include <Core-ASCII-Parser.h>

#define COMMUNICATION_START_SIGN 0x01
#define COMMUNICATION_SEND_REQUEST_SIGN 0x05
#define COMMUNICATION_TERMINATOR_SIGN 0x0D
static const char COMMUNICATION_START_SIGN_C = 0x01;
static const char COMMUNICATION_SEND_REQUEST_SIGN_C = 0x05;
static const char COMMUNICATION_TERMINATOR_SIGN_C = 0x0D;

PROXYCOMMUNICATION_EXPORT std::string getProxyCommunicationVersion();
PROXYCOMMUNICATION_EXPORT int getProxyCommunicationVersionMajor();
PROXYCOMMUNICATION_EXPORT int getProxyCommunicationVersionMinor();
PROXYCOMMUNICATION_EXPORT int getProxyCommunicationVersionPatch();

//Class GlobalStatus
#define eAxisNamesSize 17
enum eAxisNames {eTwoTheta=1, eTheta=2, eChi=3, ePhi=4, eTranslationX=5, eTranslationY=6, eRotSampleChanger=7, eVarSlit1=8, eVarSlit2=9, eSlitChanger=10, 
				 eSampleChangerStorage=11, eHV1=12, eHV2=13, eXRayGenerator=14, eTubeWindow=15, eReserved=16};
const QList<QString> sAxisNames = QList<QString>() << "_" << "TwoTheta" << "Theta" << "Chi" << "Phi" << "TranslationX" << "TranslationY" << "RotSampleChanger" 
												   << "VarSlit1" << "VarSlit2" << "SlitChanger" << "SampleChangerStorage" << "HV1" << "HV2" 
												   << "XRayGenerator" << "TubeWindow" << "Reserved";

class PROXYCOMMUNICATION_EXPORT/* Definition for DLL Export */ GlobalStatus : public QObject, public Core::Debug
{
	//Class that stores all main parameters needed by the proxy, as well as their set and get functions. 
	Q_OBJECT

public:
	enum eInitializationStatus {eIsNotInitialized, eIsInInitialization, eIsInitialized};
	enum eScanMode {eStep=0, eContinuous=1};

public:
	GlobalStatus();
	~GlobalStatus() {}

	void loadParameterIniFile(QString path);
	void loadParameterOffsetValues(QString path); //Offset Parameters for the Zero Initial command are stored in a file so they can be accessed also when the proxy was stopped in the meantime.
	void saveParameterOffsetValues(QString path); //They have to be the same as in the configuration file of the Diffractometer control software.

	bool isMoving();
	bool isMoving(eAxisNames axis);
	bool isPreparingMoving();
	bool isPreparingMoving(eAxisNames axis);
	bool isScaningAxis(eAxisNames axis);
	bool isScaning();
	eInitializationStatus getInitializationStatus();
	eInitializationStatus getInitializationStatus(eAxisNames axis);
	double getCurrentValue(eAxisNames axis);
	double getTargetValue(eAxisNames axis);
	double getToleranz(eAxisNames axis);
	double getCurrentCountsPerSecond();
	double getCurrentExposureTime();
	double getZIValue(eAxisNames axis);
	double getUnlockedCoupledOffset();
	unsigned int getAbsorberLevel();

	void setMovingStatus(eAxisNames axis, bool isMoving);
	void setPreparingMovingStatus(eAxisNames axis, bool isPreparingMoving);
	void setInitialize(eAxisNames axis, eInitializationStatus initializationStatus);
	void setScaning(bool isScaning);
	void setScaningAxis(eAxisNames axis, bool isScaningAxis);
	void setCurrentCountsPerSecond(double countsPerSecond);
	void setCurrentExposureTime(double exposureTime);
	void setCurrentValue(eAxisNames axis, double newValue);
	void setTargetValue(eAxisNames axis, double newValue);
	void setToleranz(eAxisNames axis, double toleranz);
	void setAbsorberLevel(unsigned int absorberLevel);
	void setZIValue(eAxisNames axis, double zeroInitialValue);
	void setUnlockedCoupledOffset(double unlockedCoupledOffset);

	//Member Functions for Scans
	unsigned int getScanNumberOfCachedValues();
	void setScanNewMeasuredValue(double intens);
	double getScanOldestCachedValue();
	void setScanNumberOfRemainingMeasurementsPoints(unsigned int number);
	unsigned int getScanNumberOfRemainingMeasurementsPoints();
	void deleteAllScanValues();

private:
	QMutex _ioMutex;

	QList<GlobalStatus::eInitializationStatus> _initializationStatus;
	QList<bool> _isMoving;
	QList<bool> _isPreparingMove;
	QList<bool> _isScaningAxis;
	bool _isScaning;
	
	unsigned int _absorberLevel;  

	double _currentCountsPerSecond;
	double _currentExposureTime;
	double _unlockedCoupledOffset;

	QList<double> _targetValues;
	QList<double> _currentValues;
	QList<double> _toleranz;
	QList<double> _zeroInitialValue;

	//Member for Scans
	QList<double> _scanCurrentCachedValues;
	unsigned int _scanNumberOfRemainingMeasurementsPoints;
};

//************************
//*** Proxy IO Thread ***
//************************
class PROXYCOMMUNICATION_EXPORT ProxyIOThread : public QThread, public Core::Debug
{
	//Class that controls the communication over the serial port to and from the Daco/Diffractometer.
	//It sends and reads data asynchronously in an own thread and does not have its own event loop.
	
	Q_OBJECT

public:
	ProxyIOThread();
	~ProxyIOThread();

public:
	void setParameter(QextSerialPort *serialPort, QString deviceName);
	void stop();

	QString getNextReceivedDataWord();
	void sendDataWord(QString dataWord);


protected:
	void run();

signals:
	void signalNewDataWord();

private:
	QextSerialPort *_serialPort;
	QString _deviceName;

	bool _doStop;
	double _waitTimeForAnswer_ms;

	//Übergabe Variablen
	QMutex _receiveFIFOMutex;
	QList<QString> _receiveFIFO;
	QMutex _sendFIFOMutex;
	QList<QString> _sendFIFO;

	QString _tempInputBuffer;
	QString _tempCurrentSentDataWord;
};

//***************************
//*** ProxyCommunication ***
//***************************
class PROXYCOMMUNICATION_EXPORT ProxyCommunication : public QObject, public Core::Debug
{
	//Abstract communication class. It serves the GlobalStatus for data exchange.
	//Also some unified parameters are served, e.g. device name and serial port.
	
	Q_OBJECT

public:
	ProxyCommunication(std::string pathOfIniFile);
	virtual ~ProxyCommunication();

public:
	void setGlobalStatusTable(GlobalStatus *globalStatusTable);

protected:
	Core::ParserASCIIParameterTupleFiles _parameterFile;
	
	//Current status table for synchronisation with other devices
	GlobalStatus *_currentStatusTable;

	QString _deviceName;

	//Variables only for the polling thread
	QextSerialPort _serialPort;
};

//*********************************
//*** ProxyCommunicationServer ***
//*********************************
class ProxyCommunicationClient;
class PROXYCOMMUNICATION_EXPORT ProxyCommunicationServer : public ProxyCommunication
{
	//This class has the basic functions needed to analyze controls by the diffractometer control software and to send answers, that are specialized in derived classes.
	//It is the srv (abstract) for the diffractometer control software.
	//For the proxy use, this srv is used instead of the real DACO.
	
	Q_OBJECT

public:
	ProxyCommunicationServer(std::string pathOfIniFile);
	virtual ~ProxyCommunicationServer();

public:
	void setClientInterface(ProxyCommunicationClient *clientCommunicationObj) {this->_clientCommunicationObj = clientCommunicationObj;}
	virtual void sendAnswer(QString dataWord) = 0;

signals:
	void signalDataWord();

protected:
	virtual QString _createStatusMsg() = 0;
	virtual QString _createScanStatusMsg() = 0;
	virtual QString _createScanDataMsg(unsigned int numberOfAskedValues) = 0;

protected slots:
	virtual void _slotAnalyzeNewCommand() = 0;

protected:
	void _sendDataWord(QString dataWord);

protected slots:
	void _slotPollingSerialInputBuffer();

protected:
	ProxyCommunicationClient *_clientCommunicationObj;

protected:
	//Transfer variables
	QMutex _inputFIFOMutex;
	QList<QString> _inputFIFO;

	//Thread Member
	QThread _workerThread;

	//Mutex for Sending
	QMutex _sendSerialPortMutex;

private:
	QTimer _pollingTimer;

private:
	//Variables only for the polling thread
	QString _tempBufferString;
};

class PROXYCOMMUNICATION_EXPORT ProxyCommunicationServerD5000 : public ProxyCommunicationServer
{
	//This class has the specific functions to analyze software commands for a D5000 and to send D5000 answers to the software.
	//A typical implementation of a srv (for a D5000).
	
	Q_OBJECT

public:
	ProxyCommunicationServerD5000(std::string pathOfIniFile);
	virtual ~ProxyCommunicationServerD5000();
	void sendAnswer(QString dataWord);

protected: 
	//different types of messages as answers to software requests
	QString _createStatusMsg();
	QString _createScanStatusMsg();
	QString _createRatemeterMsg();
	QString _createScanDataMsg(unsigned int numberOfAskedValues);

protected slots:
	void _slotAnalyzeNewCommand();  //all commands from the software are analyzed and processed
};

//*********************************
//*** ProxyCommunicationClient ***
//*********************************
class PROXYCOMMUNICATION_EXPORT ProxyCommunicationClient : public ProxyCommunication
{
	//This class has the basic functions needed to control a diffractometer, that are specialized in derived classes.
	//It is the clt (abstract) for the real diffractometer (DACO).
	//For the proxy use, this clt is used instead of the XRDCommander.
	
	Q_OBJECT

public:
	ProxyCommunicationClient(std::string pathOfIniFile);
	virtual ~ProxyCommunicationClient();

	virtual void sendCommand(QString commandString) = 0;

//Control Commands
public:
	void setRemoteControl(bool doRemote);
	void doHold(unsigned int axisNumber);
	void doInit(unsigned int axisNumber);
	void doZeroInitial(eAxisNames axis, double newZero);
	virtual void setAbsorberUp() = 0;
	virtual void setAbsorberDown() = 0;
	virtual void setUnusedAbsorberUp() = 0;
	virtual void setUnusedAbsorberDown() = 0;
	unsigned int getAbsorberElements();
	unsigned int getAbsorberCurrentActiveElement();
	void doOpenShutter();
	void doCloseShutter();
	void setHighVoltage(unsigned int voltage, unsigned int deviceNumber);
	void setAmplifierGain(unsigned int amplifierGain, unsigned int deviceNumber);
	void setDiscriminator(double discriminatorLowBorder, double discriminatorWidth, unsigned int deviceNumber);
	void setIntegrator(double timeConstant, unsigned int controlLimit, unsigned int deviceNumber);
	void doMove(eAxisNames axis, double targetValue);
	void doScan(unsigned int axisNumber, unsigned int numberOfSteps, double stepSize, double measurementTime, unsigned int mode);
	void doRate();
	void doOldZero();
	void doOldZeroAll();

	bool getPointErrorInfoOfTheLastMeasuredPoint() {return this->_scaningCurrentPointError;}

protected slots:
	virtual void _slotSetRemoteControl(bool doRemote) = 0;
	virtual void _slotDoHold(unsigned int axisNumber) = 0;
	virtual void _slotDoInit(unsigned int axisNumber) = 0;
	virtual void _slotDoZeroInitial(unsigned int axis, double newZero) = 0;
	virtual void _slotSetSlitL(bool doActive) = 0;
	virtual void _slotSetSlitR(bool doActive) = 0;
	virtual void _slotDoOpenShutter() = 0;
	virtual void _slotDoCloseShutter() = 0;
	virtual void _slotSetHighVoltage(unsigned int voltage, unsigned int deviceNumber) = 0;
	virtual void _slotSetAmplifierGain(unsigned int amplifierGain, unsigned int deviceNumber) = 0;
	virtual void _slotSetDiscriminator(double discriminatorLowBorder, double discriminatorWidth, unsigned int deviceNumber) = 0;
	virtual void _slotSetIntegrator(double timeConstant, unsigned int controlLimit, unsigned int deviceNumber) = 0;
	virtual void _slotDoMove(unsigned int axis, double targetValue) = 0;
	virtual void _slotScan() = 0;
	virtual void _slotDoRate() = 0;
	virtual void _slotDoOldZero() = 0;
	virtual void _slotDoOldZeroAll() = 0;

protected slots:
	virtual void _slotAnalyzeNewStatus() = 0;
	virtual void _slotSendStatusRequest() = 0;
	virtual void _slotStartMovementTimerDone() = 0;

protected:
	void _setIsDetectingData(bool isDetecting) {this->_isDetectingData = isDetecting;}
	bool _getIsDetectingData() {return this->_isDetectingData;}

private:
	QTimer _pollingTimerSendStatusRequest;
	std::ofstream _output();

	//Internal status parameter
	bool _isDetectingData;

protected:
	ProxyIOThread _serialIOThread;

protected:
	//Temp member to store the scan parameters
	unsigned int _axisNumber; 
	unsigned int _numberOfSteps;
	double _stepSize;
	double _measurementTime;
	unsigned int _mode;

protected:
	bool _scaningCurrentPointError;

protected:
	QTimer _waitTimerForMovement;		//This timer is not retriggerable.
	QTimer _waitTimerForScan;			//This timer is not retriggerable.
};

class PROXYCOMMUNICATION_EXPORT ProxyCommunicationClientD500 : public ProxyCommunicationClient
{
	//This class has the specific functions to control a D500 and to analyze D500 answers.
	//A typical implementation of a clt (for a D500).
	
	Q_OBJECT


	// Scan Flag Values encode the information for a scan (axes, mode,...) for a D500
	#define eScanFlagValuesSize 15
	enum eScanFlagValues{eDiagramPlot=0, eTableOfCountingValues=1, eTransCPLocalMode=2, ePeakSearch=3, eHeadingRemoval=4, ePrintSSCLResoults=5, ePeakTableIntRestriction=6, ePeakTableDSpacingRestriction=7, 
						 e24Bit=8, eSampleRefPrompted=9, eAxisSelectBit2Theta=10, eContinuous=11, ePlotLocalDiagram=12, eAxisSelectBit1=13, eAxisSelectBit2=14};
	typedef struct
	{
		bool flagArray[eScanFlagValuesSize];
		eAxisNames useAxis;
	}tScanFlagValues;

public:
	ProxyCommunicationClientD500(std::string pathOfIniFile);
	~ProxyCommunicationClientD500();

	void sendCommand(QString commandString);

//Interface from ProxyCommunicationClient
public:
	void setAbsorberUp();
	void setAbsorberDown();
	void setUnusedAbsorberDown();
	void setUnusedAbsorberUp();

//protected:
	void setSlitL(bool doActive);
	void setSlitR(bool doActive);

//Control Commands
public:
	void _slotSetRemoteControl(bool doRemote);
	void _slotDoHold(unsigned int axisNumber);  //Despite axisNumber for a D500 always all axes are stopped
	void _slotDoInit(unsigned int axisNumber);  //Despite axisNumber for a D500 always all axes are initialized
	void _slotDoZeroInitial(unsigned int axis, double newZero);
	void _slotSetSlitL(bool doActive);
	void _slotSetSlitR(bool doActive);
	void _slotDoOpenShutter();
	void _slotDoCloseShutter();
	void _slotSetHighVoltage(unsigned int voltage, unsigned int deviceNumber);
	void _slotSetAmplifierGain(unsigned int amplifierGain, unsigned int deviceNumber);
	void _slotSetDiscriminator(double discriminatorLowBorder, double discriminatorWidth, unsigned int deviceNumber);
	void _slotSetIntegrator(double timeConstant, unsigned int controlLimit, unsigned int deviceNumber);
	void _slotDoMove(unsigned int axis, double targetValue);
	void _slotScan();
	void _slotDoRate();
	void _slotDoOldZero();
	void _slotDoOldZeroAll();

protected slots:
	void _slotAnalyzeNewStatus();
	void _slotSendStatusRequest();
	void _slotStartMovementTimerDone();

private:
	unsigned int _getScanFlagValue(); //Defines axes and mode that are used for the scan

private:
	ProxyCommunicationClientD500::tScanFlagValues _scanFlagValues;
};

#endif // PROXYCOMMUNICATION_H
