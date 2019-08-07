/*
 * PedalUtilityData.h
 *
 *  Created on: Nov 24, 2017
 *      Author: buildrooteclipse
 */

#ifndef PEDALUTILITYDATA_H_
#define PEDALUTILITYDATA_H_

#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <map>
#include <json/json.h>
#include <stdlib.h>
#include <algorithm>
#include <sstream>
#include <sys/stat.h>
#include <sys/mman.h>

#include "FileSystemInt.h"
#include "structs.h"
#include "IpcStructs.h"

class PedalUtilityData
{
private:

	Json::Reader pedalUtilityJsonReader;
	Json::Value pedalUtilityJson;
	Json::FastWriter pedalUtilityJsonWriter;
	FileSystemInt fsInt;
#if(1)
	int toProcessUtilityFD;
	char toProcessUtilityIPCPath[50];
	_ipcProcessUtilityStruct *toProcessUtilityMemory;
#endif
	ProcessUtility processUtil;
	JackUtility jackUtil;
	NoiseGateUtility gateUtil;
	EnvTriggerUtility triggerUtil;

	bool validateBoolData(bool utilityParameterValue, bool defaultValue);
	int validateIntData(int utilityParameterValue, UtilIntValue utilInt);
	double validateDoubleData(double utilityParameterValue, UtilDoubleValue utilDouble);
	string validateOptionData(string utilityParameterOption, UtilOption utilOption);
	int createProcessingUtilityIPC();
	int openProcessingUtilityIPC();
	int closeProcessingUtilityIPC();


public:
	PedalUtilityData();
	~PedalUtilityData();


	int readUtilityDataFromFile();
	int writeUtilityDataToFile();

	Json::Value getUtilityDataForPedalUi();

	int updateUtilityValue(string utilityParameterName, string utilityParameterValue);
	ProcessUtility getProcessUtility();
	JackUtility getJackUtility();
	NoiseGateUtility getNoiseGateUtility();
	EnvTriggerUtility getEnvTriggerUtility();

	/************* Process Utility functions *****************/
#if(1)
	int sendIpcProcessUtility();
	int sendIpcNoiseGateUtility();
	int sendIpcEnvTriggerUtility();
#endif


};
#endif /* PEDALUTILITYDATA_H_ */
