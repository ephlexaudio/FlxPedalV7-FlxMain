/*
 * structs.h
 *
 *  Created on: Jul 16, 2016
 *      Author: mike
 */

#ifndef IPCSTRUCTS_H_
#define IPCSTRUCTS_H_




#include "config.h"
#define AVE_ARRAY_SIZE 16




/**********************************************************************************************/
using namespace std;




struct _ipcConnector{
	char parentObjectName[20]; // connector "Parent".  Was "process", needed something more generic to cover "effect" and "control"
	char connectorName[20];
	int connectorIndex; // position of connector WRT other like connectors (input,output, parameter)
	int connectedBufferIndex; // index of connection to pub/sub data structure arrays, if applicable
};



// KEEP AS STRUCT
struct _ipcProcessSignalBuffer{
	_ipcConnector srcProcessOutputConnector; // connects to "srcProcessOutput"
	_ipcConnector destProcessInputConnectorArray[5]; //connects to "destProcessInputs"
	//double buffer[BUFFER_SIZE]; // omitted to save space on IPC shared mapped memory
};

struct _ipcProcessParameterControlBuffer{
	_ipcConnector srcControlOutputConnector;
	_ipcConnector destProcessParameterConnector;
	int parameterValueIndex;
};





//CLASS "PROCESS" (BASE OR "INTERFACE" CLASS FOR CHILD PROCESSES: DELAY, WAVESHAPER, ETC)
struct _ipcProcessObjectData{
	int processTypeInt;  //used to identify process type, not position in processing sequence
	int processSequenceIndex;
	char processName[20];
	int footswitchNumber;
	int parameterCount;
	struct{
		int internalIndexValue;
		int paramContBufferIndex;
		bool controlConnected;
		char parameterName[20];
	}parameterArray[10];
	int processInputCount;
	int processOutputCount;
	int inputConnectedBufferIndexArray[5];
	int outputConnectedBufferIndexArray[5];
	int bufferSize;
	int inputCouplingMode;
	int antiAliasingNumber;
	int waveshaperMode;

};



//CLASS "CONTROL" (BASE OR "INTERFACE" CLASS FOR CHILD CONTROLS: Normal, EnvelopeGenerator, LowFreqOsc)
struct _ipcControlObjectData{
	char controlName[20];
	int controlTypeInt;
	int parameterCount;
	struct {
		int value;
		bool cvEnabled;
		char parameterName[20];
	}parameterArray[10];
							// Normal: 	parameter[0].value=parameter value

							// EnvGen: 	parameter[0].value=attack time
							//			parameter[1].value=decay time
							//			parameter[2].value=sustain time
							//			parameter[3].value=release time
							//			parameter[4].value=peak value index;
							//			parameter[5].value=sustain value index;

							// LFO:		parameter[0].value=frequency
							//			parameter[1].value=amplitude
							//			parameter[2].value=offset


	int outputConnectionCount;
	int outputInvConnectionCount;
	int outputToParamControlBufferIndex[5];
	int outputInvToParamControlBufferIndex[5];
	bool envTrigger;
};



struct _ipcComboStruct{
	bool loadCombo;
	bool comboLoaded;
	bool processingEnabled;
	char name[20];
	_ipcProcessObjectData processSequenceData[20]; // array of Process child classes
	_ipcControlObjectData controlSequenceData[20]; // array of Control child classes
	_ipcProcessSignalBuffer processSignalBufferArray[60]; // keep this as struct
	_ipcProcessParameterControlBuffer processParamControlBufferArray[60]; // keep this as struct
	int footswitchStatus[10];
	int inputSystemBufferIndex[2];
	int outputSystemBufferIndex[2];
	int processCount;
	int controlCount;
	int processSignalBufferCount;
	int processParamControlBufferCount;
	bool controlVoltageEnabled;
	int bufferSize;
};


/*************************************************************************
 * 		FLX UTILITY IPC STRUCT
 *
 *************************************************************************/

struct _ipcProcessUtilityStruct
{
	bool loadProcessUtility;
	bool processUtilityLoaded;
	struct
	{
		double highThres;
		double lowThres;
	}envTrigger;
	struct
	{
		double closeThres;
		double gain;
		double openThres;
	}noiseGate;
	struct
	{
		int antiAliasingNumber;
		int bufferSize;
		int inputCouplingMode;
		int waveshaperMode;
	}process;
	struct
	{
		double highThres;
		double lowThres;
	}trigger;


};









#endif /* STRUCTS_H_ */
