/*
 * ProcessingControl.h
 *
 *  Created on: Jun 25, 2016
 *      Author: mike
 */

#ifndef PROCESSINGCONTROL_H_
#define PROCESSINGCONTROL_H_
#include <iostream>
#include <map>


#include <vector>
#include <string>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <jack/jack.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "GPIOClass.h"
#include "config.h"
#include "structs.h"
#include "IpcStructs.h"

class ProcessingControl
{
private:
	NoiseGateUtility gateUtil;
	EnvTriggerUtility triggerUtil;
	JackUtility jackUtil;
	bool justPoweredUp;
	bool inputsSwitched;
	/********** COMBO STRUCT DATA ******************/
	string name;
	ProcessObjectData processSequenceData[20]; // array of Process child classes
	ControlObjectData controlSequenceData[20]; // array of Control child classes
	ProcessSignalBuffer processSignalBufferArray[60]; // keep this as struct
	ProcessParameterControlBuffer processParamControlBufferArray[60]; // keep this as struct
	map<string, ProcessIndexing>  processIndexMap;
	map<string, ControlIndexing>  controlIndexMap;
	int inputSystemBufferIndex[2];
	int outputSystemBufferIndex[2];
	int processCount;
	int controlCount;
	int processSignalBufferCount;
	int processParamControlBufferCount;
	bool controlVoltageEnabled;
	/*************************************************/
	vector<string> componentSymbolVector;
	vector<string> controlSymbolVector;
	int toComboFD;
	char toComboIPCPath[50];
	_ipcComboStruct *toComboMemory;

	void loadComponentSymbolVector();
	void loadControlSymbolVector();

	int createProcessingComboIPC();
	int openProcessingComboIPC();
	int closeProcessingComboIPC();

public:
	ProcessingControl();
	~ProcessingControl();

	/**************** Combo Change Functions*****************/
	int loadComboStruct(ComboStruct comboStruct);
	ComboStruct getComboStruct();
	int sendIpcComboStruct();
	/************** JACK Functions ***************/
	void setJackUtility(JackUtility jackUtil);
	int startJack(void);
	int stopJack(void);
	/*************** Combo Processing Functions ********************/
	int startComboProcessing();
	void stopComboProcessing();

	/***************** Enable AudioOutput functions****************/
	void enableAudioOutput();
	void disableAudioOutput();
	/******************* Process/Control parameter functions ******************/
	int updateProcessParameter(string parentProcess, string parameter, int parameterValue);
	int updateControlParameter(string parentControl, string parameter, int parameterValue);
	/*************** Symbol functions*****************/
	void loadSymbols();
	vector<string> getComponentSymbols();
	vector<string> getControlSymbols();
};



#endif /* PROCESSINGCONTROL_H_ */
