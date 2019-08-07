/*
 * ComboDataInt.h
 *
 *  Created on: Dec 24, 2015
 *      Author: mike
 */

#ifndef COMBODATAINT_H_
#define COMBODATAINT_H_


#include <fcntl.h>
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <map>
#include <strings.h>
#include <json/json.h>
#include <algorithm>
#include "FileSystemInt.h"
#include "structs.h"
#include "utilityFunctions.h"
#include "IpcStructs.h"

#define JSON_BUFFER_LENGTH 32000




class ComboDataInt
{
public:
	void printProcessList(bool sorted, vector<Process> processVector);
	void printProcessStructVector(bool sorted, vector<Process> processVector);
	void printControlStructVector(bool sorted, vector<Control> controlVector);
	void printDataReadyList(void);
	void printProcessSignalConnectionList(bool sorted, vector<ProcessSignalConnection> processSignalConnectionVector);
	void printControlConnectionList(bool sorted, vector<ProcessParameterControlConnection> procParamContConnectVector);
	void printProcBufferList(void);
	void printContBufferList(void);

	void debugPrintParamContBufferListWithConnections();
	void printIndexMappedProcessData();
	void printIndexMappedControlData();
	void printIndexMappedComboData(void);
	void printPedalUIData();
	void printEffectComboJsonProcessParameters(Json::Value combo);
	void printEffectComboJsonControlParameters(Json::Value combo);


private:


	int comboFD;
	Json::Reader comboReader;
	Json::FastWriter comboWriter;
	char jsonBuffer[JSON_BUFFER_LENGTH];
	char fileNameBuffer[20];
	char fileNamePathBuffer[40];
	 int inputSystemBufferIndex[2];
	 int outputSystemBufferIndex[2];
	ComboJsonFileStruct comboFileStruct;
	std::vector<Connector> dataReadyVector;
	string comboName;
	Json::Value effectComboJson;
	Json::Value pedalUiJson;
	FileSystemInt fsInt;
	std::vector<Process> unsortedProcessStructVector;

	std::vector<ProcessSignalConnection> unmergedProcessSubconnectionStructVector;
	std::vector<ProcessSignalConnection> unsortedProcessConnectionStructVector;

	std::vector<Process> sortedProcessStructVector; // processes here are in order
	std::vector<ProcessSignalConnection> sortedProcessConnectionStructVector;
	std::vector<Control> sortedControlStructVector;
	std::vector<ProcessParameterControlConnection> sortedControlConnectionStructVector;


	vector<Process>::size_type processCount;
	int effectCount;
	vector<Control>::size_type controlCount;

	// **************** CENTRAL DATA MODEL ************************/
	ComboStruct comboStruct;
/*********************** Load Unsorted Struct Vectors ****************************************************/
	vector<Process>  loadProcessStructVectorFromJsonProcessArray(int parentEffectIndex, Json::Value processArray);
	vector<ProcessSignalConnection>  loadProcessConnectionStructVectorFromJsonConnectionArray(Json::Value connectionArray);
	vector<Control>  loadControlStructVectorFromJsonControlArray(int parentEffectIndex, string parentEffectAbbr, Json::Value controlArray);
	vector<ProcessParameterControlConnection>  loadControlConnectionStructVectorFromJsonControlConnectionArray(Json::Value controlConnectionArray);


	int loadEffectComboJsonFromFile(string comboName); //get JSON data from file and parse into effectComboJson
	int loadEffectComboJson(string comboData); //get JSON data from file and parse into effectComboJson
	int loadComboJsonFileStructFromEffectComboJson(void);
	int loadUnsortedProcessStructVectorFromComboJsonFileStruct(void);
	int loadUnmergedSubconnectionStructVectorFromComboJsonFileStruct(void);



	/************************* Internal data processing *********************************/
	int mergeUnmergedSubconnectionsAndLoadIntoUnsortedConnectionStructVector(void);
	int getTargetProcessIndex(string processName);
	std::vector<string> getProcessInputs(string processName);
	std::vector<string> getFirstProcesses();
	std::vector<string> getNextProcesses();

	int areAllProcessInputsSatisfied(string processName);
	int addOutputConnectionsToDataReadyVector(vector<string> processNames);
	int transferProcessStructsToSortedProcessStructVector(vector<string> processNames);
	bool isUnsortedProcessStructVectorEmpty();
	bool isOutputInDataReadyVector(Connector output);
	ProcessSignalConnection mergeConnections(ProcessSignalConnection srcConn, ProcessSignalConnection destConn);
	bool compareConnectors(bool display, Connector conn1, Connector conn2);

	/****************** Sort unsorted struct-vectors into sorted struct-vectors***********************************/

	int sortUnsortedProcessStructsIntoSortedProcessStructVector();
	int sortUnsortedConnectionStructsIntoSortedConnectionStructVector();
	int loadSortedControlStructVectorFromComboJsonFileStruct(void);
	int loadSortedControlConnectionStructVectorFromComboJsonFileStruct(void);

	/******************** Set interconnections ***********************************/

	int setProcBufferArrayOutputAndInputConnectorStructsUsingSortedConnectionStructVectorAndDataReadyVector();
	int setConnectedBufferIndexesInSortedProcessStructProcessIOVectorsUsingProcBufferArray();
	int setControlOutputAndProcessParameterControlBufferIndexesUsingParamContBufferArray();

	/******************* Set other data ****************************************/
	int  setProcessAndControlTypeIntsInSortedStructVectors();
	int  loadProcessIndexMapFromSortedProcessVector();
	int  loadControlIndexMapFromSortedControlVector();
	int loadIndexMappedComboDataIntoComboStructFromSortedVectors(void);


	int setPedalUi(void); // setup pedalUiJson using comboFileStruct

	//******************* ComboStruct Sub-Functions ******************



	void setComboStructProcessInputBufferIndexes(int sequenceIndex, Process);
	void setComboStructProcessOutputBufferIndexes(int sequenceIndex, Process);
	void setComboStructProcessParameters(int sequenceIndex, Process);
	//the above 3 functions go into this function
	void setComboStructProcessDataObject(int sequenceIndex, Process);



	void setComboStructControlOutputs(int sequenceIndex, Control);
	void setComboStructControlOutputInvs(int sequenceIndex, Control);
	void setComboStructControlParameters(int sequenceIndex, Control);
	//the above 3 functions go into this function
	void setComboStructControlDataObject(int sequenceIndex, Control);

	void setComboStructProcessParameterControlBufferArray(void);
	void setComboStructProcessSignalBufferArray(void);


	//******************* Updates when saving Combo ******************

	//this is for saving from pedal, only controls need to be updated.
	int updateControlParameterValuesInComboFileStruct();

	int loadComboFileStructDataBackIntoEffectComboJson();
	int loadComboStructDataBackIntoEffectComboJson();


	ProcessUtility processUtil;
	void transferComboStructBackToEffectComboJson();
	void setComboStructParamValues(ComboStruct combo);
	int loadIndexMappedComboData2(Json::Value comboJson);

public:
	ComboDataInt();
	ComboDataInt(Json::Value comboJson);
	virtual ~ComboDataInt();



	void setProcessUtilityData(ProcessUtility procUtil);
	Json::Value getPedalUiJson(void);
	string getName();
	void updateComboDataInt(ComboStruct combo);
	void loadComboStruct(void);
	ComboStruct getComboStruct();
	_ipcComboStruct getIpcComboStruct();
	Json::Value getEffectComboJson();
	ComboJsonFileStruct getComboJsonFileStruct(){return this->comboFileStruct;}
	string getComboStringData(string comboName);
	void setComboStringData(string comboData);

};




#endif /* COMBODATAINT_H_ */
