/*
 * HostUiInt.cpp
 *
 *  Created on: Sep 27, 2016
 *      Author: buildrooteclipse
 */

#include "HostUiInt.h"
#include "config.h"


using namespace std;


#define dbg 1
HostUiInt::HostUiInt()
{
#if(dbg >= 1)
	 cout << "***** ENTERING: HostUiInt::Constructor" << endl;
#endif
	 this->sockIntPtr = new SocketInt();


#if(dbg >= 1)
	 cout << "***** EXITING: HostUiInt::Constructor" << endl;
#endif

}

HostUiInt::~HostUiInt()
{
	// TODO Auto-generated destructor stub
#if(dbg >= 1)
	 cout << "***** ENTERING: HostUiInt::Destructor" << endl;
#endif
	 if(this->sockIntPtr != NULL)
	 {
		 this->closePort();
		 this->sockIntPtr = NULL;
	 }
#if(dbg >= 1)
	 cout << "***** EXITING: HostUiInt::Destructor" << endl;
#endif
}


int HostUiInt::openPort()
{
	int status = 0;

#if(dbg >= 1)
	 cout << "***** ENTERING: HostUiInt::openPort" << endl;
#endif
	 try
	 {
		 // Putting tcpLoop in a seperate thread to prevent it from recv from
		 // blocking the rest of the FlxMain
		 this->sockThread = std::thread(&SocketInt::tcpLoop, this->sockIntPtr);
	 }
	 catch(exception &e)
	 {
		 cout << "exception in HostUiInt::openPort: " << e.what() << endl;
	 }
#if(dbg >= 1)
	 cout << "***** EXITING: HostUiInt::openPort" << endl;
#endif
	 return status;
}

int HostUiInt::closePort()
{
	int status = 0;

#if(dbg >= 1)
	 cout << "***** ENTERING: HostUiInt::closePort" << endl;
#endif
	 try
	 {
		 this->sockIntPtr->exitTcpLoop();
		 this->sockThread.join();

	 }
	 catch(exception &e)
	 {
		 cout << "exception in HostUiInt::openPort: " << e.what() << endl;
	 }

#if(dbg >= 1)
	 cout << "***** EXITING: HostUiInt::closePort" << endl;
#endif
	 return status;
}

bool HostUiInt::isFlxEditorRunning(void)
{
	return this->sockIntPtr->isFlxEditorRunning();
}

bool HostUiInt::isUsbCableConnected(void)
{
	return this->sockIntPtr->isUsbCableConnected();
}

bool  HostUiInt::isPortOpen()
{
	return this->sockIntPtr->isSocketOpen();
}




#define dbg 2
int HostUiInt::sendResponse(string responseHeader, string responseBody)
{
#if(dbg >= 1)
	 cout << "***** ENTERING: HostUiInt::" << __func__ << endl;
#endif

	int status = 0;
	string hostUiReponseString;

	hostUiReponseString = "<"+responseHeader+">" + responseBody + "</"+responseHeader+">";
#if(dbg >= 2)
	 cout << "hostUiReponseString: " << hostUiReponseString << endl;
#endif
	this->sockIntPtr->writeData(hostUiReponseString);
#if(dbg >= 1)
	 cout << "***** EXITING: HostUiInt::" << __func__  << endl;
#endif

	return status;

}

#define dbg 0
string HostUiInt::getUserRequest(void)
{
#if(dbg >= 1)
	 cout << "***** ENTERING: HostUiInt::getUserRequest" << endl;
#endif
	string newData = this->sockIntPtr->readNewData();
#if(dbg >= 1)
	 cout << "***** EXITING: HostUiInt::getUserRequest" << newData << endl;
#endif
	return newData;
}


#define dbg 1
int HostUiInt::sendComboList(string comboLists)
{

	int status = 0;
#if(dbg >= 1)
	 cout << "***** ENTERING: HostUiInt::sendComboList" << endl;
#endif
	 this->sendResponse("ComboList",comboLists);
#if(dbg >= 1)
	 cout << "***** EXITING: HostUiInt::sendComboList: " << status << endl;
#endif


	return status;
}



#define dbg 0
int HostUiInt::sendComponentData(vector<string> componentDataVector)
{
	int status = 0;
#if(dbg >= 1)
	 cout << "***** ENTERING: HostUiInt::sendComponentData" << endl;
#endif
	string componentDataString;
	size_t componentIndex = 0;
	for(auto & component : componentDataVector)
	{
		componentDataString += component;
		if(componentIndex < componentDataVector.size()-1)
			componentDataString += ",";
		componentIndex++;
	}
	this->sendResponse("ComponentData",string("["+componentDataString+"]"));


#if(dbg >= 1)
	 cout << "***** EXITING: HostUiInt::sendComponentData:" << status << endl;
#endif

	return status;
}

#define dbg 0
int HostUiInt::sendControlTypeData(vector<string>controlTypeDataVector)
{
	int status = 0;
#if(dbg >= 1)
	 cout << "***** ENTERING: HostUiInt::sendControlTypeData" << endl;
#endif
	string controlTypeDataString;
	size_t controlTypeIndex = 0;
	for(auto & controlType : controlTypeDataVector)
	{
		controlTypeDataString += controlType;
		if(controlTypeIndex < controlTypeDataVector.size()-1)
			controlTypeDataString += ",";
		controlTypeIndex++;
	}
	this->sendResponse("ControlTypeData",string("["+controlTypeDataString+"]"));

#if(dbg >= 1)
	 cout << "***** EXITING: HostUiInt::sendControlTypeData:" << status << endl;
#endif

	return status;
}


#define dbg 0
int HostUiInt::sendComboToHost(string comboName)
{
	#if(dbg >= 1)
		 cout << "******************************************" << endl;
		 cout << "***** ENTERING: HostUiInt::sendComboToHost" << endl;
		 cout << "comboName: " << comboName << endl;
		 cout << "******************************************" << endl;
		startTimer();
	#endif

	int status = 0;
	string comboString;
	string hostUiResponse;

	comboString = fsInt.loadComboDataFromFileSystemToHost(comboName);
	if(comboString.empty() == false)
	{
		int checkSum = 0;
		for(auto & c : comboString)
		{
			if(c > 31)
			{
				checkSum += c;
			}
		}

		this->sendResponse("ComboData",comboString);
	}
	else
	{
		status = -1;
		 cout << "error reading data" << endl;
	}


#if(dbg >= 1)
	 cout << "***** EXITING: HostUiInt::sendComboToHost: " << status << endl;
#endif

	return status;
}

#define dbg 2
string HostUiInt::getComboFromHost(string hostComboJson)
{
	#if(dbg >= 1)
		 cout << "***** ENTERING: HostUiInt::getComboFromHost" << endl;
	#endif
	string hostUiRequest;
	string comboData;
	string comboName;
	bool done = false;
	string tcpData;
	unsigned newDataSize = 0;
	comboData.clear();
	comboName.clear();
	usleep(1000);

	comboName = fsInt.saveComboDataFromHostToFileSystem(hostComboJson);
	if(comboName.empty() == true)
	{
		 cout << "error saving combo." << endl;
	}


#if(dbg >= 2)
	 cout << "combo data: " << comboData << endl;
#endif

#if(dbg >= 1)
	 cout << "EXITING: HostUiInt::getComboFromHost: " << comboName << endl;
#endif

	return comboName;
}

#define dbg 0
int HostUiInt::sendCurrentStatus(string currentStatus)
{
	int status = 0;

#if(dbg >= 1)
	 cout << "***** ENTERING: HostUiInt::sendCurrentStatus" << endl;
#endif
	 this->sendResponse("CurrentStatus",currentStatus);

#if(dbg >= 1)
	 cout << "***** EXITING: HostUiInt::sendCurrentStatus: " << status << endl;
#endif

	return status;
}


