/*
 * structs.h
 *
 *  Created on: Jul 16, 2016
 *      Author: mike
 */

#ifndef STRUCTS_H_
#define STRUCTS_H_



#include <string>

#include <vector>
#include <map>
#include <sys/types.h>
#include <json/json.h>
#include "indexMapStructs.h"
#include "config.h"
#define AVE_ARRAY_SIZE 16

using namespace std;


struct Connector{
	string parentObjectName; // connector "Parent".  Was "process", needed something more generic to cover "effect" and "control"
	string connectorName;
	int connectorIndex; // position of connector WRT other like connectors (input,output, parameter)
	int connectedBufferIndex; // index of connection to pub/sub data structure arrays, if applicable
};

/*********************************************************************************************************/
/*********************************** ComboJsonFileStruct components **************************************/
/*********************************************************************************************************/

struct EffectConnector{
	string effectName; // connector "Parent".  Was "process", needed something more generic to cover "effect" and "control"
	string connectorName;
	int connectorIndex; // position of connector WRT other like connectors (input,output, parameter)
};

struct ProcessSignalConnection{
	Connector srcProcessOutputConnector; // source process output connector
	Connector destProcessInputConnector; // destination process input connector
};



struct ProcessParameterControlConnection{
	Connector srcControlOutputConnector; // source parameter control output connector
	Connector destProcessParameterConnector; // destination parameter connector
	int parameterValueIndex;
};




struct ProcessParameter{
	Connector paramConnector; // contains parent process, param name, param index, paramControlBuffer index.
	int paramType;   //  both ProcessParams type and ControlParameter type use the same type system.
	string paramControlType;
	int valueIndex;
};

struct ControlParameter{
	string name;
	string alias;  // alias and abbr are for Pedal UI
	string abbr;
	int index;
	string parentControl;  // create parent control index in getPedalUi
	int paramType;
	int controlledParamType;
	bool inheritControlledParamType;
	bool cvEnabled;
	int valueIndex;
};



struct Process{
	string processName;
	string parentEffect;
	int processEffectIndex;
	int parentEffectIndex;
	int processSequenceIndex;
	string processTypeString;
	int processTypeInt;
	string footswitchType;
	int footswitchNumber;
	int inputCount;
	int outputCount;
	int paramCount;
	vector<Connector> inputVector;	//

	vector<Connector> outputVector;  // use ComboDataInt::connectProcessOutputsToProcessOutputBuffersUsingProcBufferArray
									 // to put bufferArray index into outputVector[].connectedBufferIndex
	vector<ProcessParameter> paramVector;
};


struct Control{
	string name;  // make the name more descriptive (ie. control:mixerb_0:level1)
	string parentEffect;
	string parentEffectAbbr;
	int controlSequenceIndex;
	int parentEffectIndex;
	int controlEffectIndex;

	int controlTypeIndex;
	string conType;
	int conTypeInt;
	vector<ControlParameter> paramVector;
	Connector output;
	Connector outputInv;
	vector<Connector> targetProcessParamVector;
	vector<Connector> targetProcessParamInvVector;
};

struct Effect{
	string name;
	string abbr;
	int index;
	vector<Process> processVector;
	vector<ProcessSignalConnection> processConnectionVector;
	vector<Control> processParamControlVector;
	vector<ProcessParameterControlConnection> processParamControlConnectionVector;
};

struct EffectConnection{
	EffectConnector srcEffectOutputConnector;
	EffectConnector destEffectInputConnector;
};


struct ComboJsonFileStruct
{
	string name;
	Effect effectArray[2];
	vector<EffectConnection> effectConnectionVector;
};


/*********************************************************************************************************/
/************************** ComboStruct components *******************************************************/
/*********************************************************************************************************/


// KEEP AS STRUCT
struct ProcessSignalBuffer{
	Connector srcProcessOutputConnector;  // connects to "srcProcessOutput"
	vector<Connector> destProcessInputConnectorVector; //connects to "destProcessInputs"
};

struct ProcessParameterControlBuffer{
	Connector srcControlOutputConnector;
	Connector destProcessParameterConnector;
	int parameterValueIndex;
};

struct ProcessObjectParameter {
	int index;
	int internalIndexValue;
	int paramContBufferIndex;
	bool controlConnected;
	string parameterName;
};

struct ControlObjectParameter {
	int index;
	int value;
	bool cvEnabled;
	string parameterName;
	string parameterAbbr;
	int paramType;
};



//CLASS "PROCESS" (BASE OR "INTERFACE" CLASS FOR CHILD PROCESSES: DELAY, WAVESHAPER, ETC)
struct ProcessObjectData{
	int processTypeInt;  //used to identify process type, not position in processing sequence
	int processSequenceIndex;
	int processEffectIndex;
	int parentEffectIndex;
	string processName;
	int footswitchNumber;
	int parameterCount;
	ProcessObjectParameter parameterArray[10];
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
struct ControlObjectData{
	string controlName;
	string parentEffectName;
	string parentEffectAbbr;
	int controlEffectIndex;
	int parentEffectIndex;
	int controlTypeInt;
	int parameterCount;
	ControlObjectParameter parameterArray[10];
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



struct ComboStruct {
	string name;
	ProcessObjectData processSequenceData[20]; // array of Process child classes
	ControlObjectData controlSequenceData[20]; // array of Control child classes
	ProcessSignalBuffer processSignalBufferArray[60]; // keep this as struct
	ProcessParameterControlBuffer processParamControlBufferArray[60]; // keep this as struct
	map<string, ProcessIndexing>  processIndexMap;
	map<string, ControlIndexing>  controlIndexMap;
	int footswitchStatus[10];
	int inputSystemBufferIndex[2];
	int outputSystemBufferIndex[2];
	int processCount;
	int controlCount;
	int processSignalBufferCount;
	int processParamControlBufferCount;
	bool controlVoltageEnabled;
	int bufferSize;
	Json::Value pedalUi;
};







// **********  Utility value structs provide error checking data  ****************

struct UtilDoubleValue {
	double value;
	double minimum;
	double maximum;
};

struct UtilIntValue {
	int value;
	int minimum;
	int maximum;
};

struct UtilOption {
	string option;
	vector<string> validOptions;
};

//********************* Utility Structs *******************************************


struct UtilParam{
	string name;
	string abbr;
	int utilParamIndex;
	int paramType; //0=Double, 1=String Option
	UtilOption	option;
	UtilIntValue intValue;
	UtilDoubleValue doubleValue;
};

struct UtilType{
	string name;
	string abbr;
	vector<UtilParam> utilParams;
};

//********************** structs for FlxUtilityParams text file data **********************
struct ProcessUtility {
		UtilIntValue antiAliasingNumber;
		UtilOption inputCouplingMode;
		UtilOption waveshaperMode;
		UtilIntValue bufferSize;
};

struct JackUtility {
	UtilIntValue period;
	UtilIntValue buffer;
};

struct NoiseGateUtility {
	UtilDoubleValue closeThres;
	UtilDoubleValue openThres;
	UtilDoubleValue gain;
};

struct EnvTriggerUtility {
	UtilDoubleValue highThres;
	UtilDoubleValue lowThres;
};


//***************************** Change requests **********************************
struct PedalUtilityChange {
	string utility;
	string utilityParameter;
	union{
		int intValue;
		double doubleValue;
		char option[5];
	};
	bool error;
};

struct ProcessControlChange {
	string procContName;
	string parameter;
	int parameterValueIndex;
};








#endif /* STRUCTS_H_ */
