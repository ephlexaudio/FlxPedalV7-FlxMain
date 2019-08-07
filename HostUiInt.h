/*
 * HostUiInt.h
 *
 *  Created on: Sep 27, 2016
 *      Author: buildrooteclipse
 */

#ifndef HOSTUIINT_H_
#define HOSTUIINT_H_

#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include "utilityFunctions.h"
#include "FileSystemInt.h"
#include "structs.h"
#include "SocketInt.h"


class HostUiInt
{
private:
	SocketInt *sockIntPtr;
	std::thread sockThread;
	FileSystemInt fsInt;
public:

	HostUiInt();
	~HostUiInt();
	// Files are retrieved from the directory and sent straight to the host
	int openPort();
	int closePort();
	string getUserRequest(void);
	bool isFlxEditorRunning(void);
	bool isUsbCableConnected(void);
	bool isPortOpen();
	int sendResponse(string responseHeader, string responseBody);
	int sendControlTypeData(vector<string>controlTypeDataVector);
	int sendComponentData(vector<string> componentDataVector);
	int sendComboList(string comboLists);
	int sendComboToHost(string comboName);
	string getComboFromHost(string comboData);
	int sendCurrentStatus(string currentStatus);

};

#endif /* HOSTUIINT_H_ */
