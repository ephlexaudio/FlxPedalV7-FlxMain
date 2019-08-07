/*
 * ComboDatasInt.cpp
 *
 *  Created on: Dec 26, 2015
 *      Author: mike
 */
#include "config.h"
#include "ComboDataInt.h"
#include "utilityFunctions.h"

#define JSON_BUFFER_LENGTH 32000

using namespace std;


	ComboDataInt::ComboDataInt()
	{
		this->comboFD = 0;
		this->comboStruct.processSignalBufferCount = 0;
		this->controlCount = 0;
		this->effectCount = 0;
		this->processCount = 0;
		this->inputSystemBufferIndex[0] = 0;
		this->inputSystemBufferIndex[1] = 1;
		this->outputSystemBufferIndex[0] = 0;
		this->outputSystemBufferIndex[1] = 1;
		this->comboStruct.processParamControlBufferCount = 0;
	}


	ComboDataInt::ComboDataInt(Json::Value comboJson)
	{
		this->loadIndexMappedComboData2(comboJson);
	}

	ComboDataInt::~ComboDataInt()
	{

	}

	void ComboDataInt::printProcessList(bool sorted, vector<Process> processVector)
	{

		{
			if(sorted) cout << "SORTED PROCESSES" << endl;
			else cout << "UNSORTED PROCESSES" << endl;

			for(auto & process : processVector)
			{
				cout << process.processName << endl;
			}
		}
	}

	void ComboDataInt::printProcessStructVector(bool sorted, vector<Process> processVector)
	{

		{
			if(sorted) cout << "SORTED PROCESS STRUCTS" << endl;
			else cout << "UNSORTED PROCESS STRUCTS" << endl;

			for(auto & process : processVector)
			{
				cout << process.processName << endl;
				cout << "inputs" << endl;
				for(auto & input : process.inputVector)
				{
					cout << "\t" << input.parentObjectName << ":" << input.connectorName << endl;
				}
				cout << "outputs" << endl;
				for(auto & output : process.outputVector)
				{
					cout << "\t" << output.parentObjectName << ":" << output.connectorName << endl;
				}
				cout << "parameters" << endl;
				for(auto & parameter : process.paramVector)
				{
					cout << "\t" << parameter.paramConnector.parentObjectName << ":" << parameter.paramConnector.connectedBufferIndex;
					cout << "\t" << parameter.valueIndex << endl;
				}
			}
		}
	}

	void ComboDataInt::printProcessSignalConnectionList(bool sorted, vector<ProcessSignalConnection> processSignalConnectionVector)
	{

		{
			if(sorted) cout << "SORTED PROCESS SIGNAL CONNECTIONS: " << endl;
			else cout << "UNSORTED PROCESS SIGNAL CONNECTIONS: " << endl;

			int i = 0;
			for(auto & procConnection : processSignalConnectionVector)
			{
				cout << "ProcessSignalConnectionStructVector[" << i << "]: " << procConnection.srcProcessOutputConnector.parentObjectName << ":";
				cout << procConnection.srcProcessOutputConnector.connectorName << ">" << procConnection.destProcessInputConnector.parentObjectName << ":";
				cout << procConnection.destProcessInputConnector.connectorName << endl;
				i++;
			}
		}
	}

	void ComboDataInt::printDataReadyList(void)
	{

		{
			cout << "DATA READY LIST:" << endl;

			for(auto & dataReady : this->dataReadyVector)
			{
				cout <<  dataReady.parentObjectName << ":" << dataReady.connectorName << endl;
			}
		}

	}

	void ComboDataInt::printControlConnectionList(bool sorted, vector<ProcessParameterControlConnection> procParamContConnectVector)
	{

		{
			if(sorted) cout << "SORTED CONTROL CONNECTION LIST: " << endl;
			else cout << "UNSORTED CONTROL CONNECTION LIST: " << endl;
		}


		for(auto & controlConnection : procParamContConnectVector)
		{

			{
				cout << "controlConnection: " << controlConnection.destProcessParameterConnector.parentObjectName << ":";
				cout << controlConnection.srcControlOutputConnector.connectorName << ">";
				cout << controlConnection.destProcessParameterConnector.parentObjectName << ":";
				cout << controlConnection.destProcessParameterConnector.connectorName << endl;
			}
		}
		 cout << endl;
	}

	void ComboDataInt::printProcBufferList(void)
	{

		{
			cout << "PROCESS BUFFER LIST:" << endl;
			int i = 0;
			for(i = 0; i < this->comboStruct.processSignalBufferCount; i++)
			{
				cout << "buffer[" << i << "]: " << endl;
				ProcessSignalBuffer procSigBuffer = this->comboStruct.processSignalBufferArray[i];
				if(procSigBuffer.srcProcessOutputConnector.parentObjectName.empty() == false)
				{
					cout << procSigBuffer.srcProcessOutputConnector.parentObjectName << ":" << procSigBuffer.srcProcessOutputConnector.connectorName << endl;
					cout << "destProcessInputConnectorVector: " << endl;
					for(auto & destProcessInputConnector : procSigBuffer.destProcessInputConnectorVector)
					{
						cout << "\t\t" << destProcessInputConnector.parentObjectName;
						cout << ":" << destProcessInputConnector.connectorName << endl;
					}
				}
				else break;
				i++;
			}
			cout << endl;
		}
	}

#define dbg 0
	void ComboDataInt::printContBufferList(void)
	{

		{
			cout << "CONTROL BUFFER LIST:" << endl;
			int i = 0;
			for(auto & procParamContBuffer : this->comboStruct.processParamControlBufferArray)
			{
				cout << "buffer[" << i << "]: " << endl;
				cout << procParamContBuffer.srcControlOutputConnector.parentObjectName << ":" << procParamContBuffer.srcControlOutputConnector.connectorName << endl;
				cout << "destProcessParamVector: " << endl;
				cout << "\t\t" << procParamContBuffer.destProcessParameterConnector.parentObjectName;
				cout << ":" << procParamContBuffer.destProcessParameterConnector.connectorName << endl;

				i++;
			}
			cout << endl;
		}


	}

#define dbg 1
	void ComboDataInt::debugPrintParamContBufferListWithConnections()
	{
#if(dbg >= 1)

		{
			cout << " ENTERING ComboDataInt::" << __func__ << endl;
			bool exit = false;
			int paramContBufferIndex = 0;
			for(auto & processParamControlBuffer : this->comboStruct.processParamControlBufferArray)
			{
				Connector controlConn = processParamControlBuffer.srcControlOutputConnector;
				Connector paramConn = processParamControlBuffer.destProcessParameterConnector;
				string controlConnector;
				string processParamConnector;

				controlConnector = controlConn.parentObjectName + ":" + paramConn.connectorName;
				processParamConnector = paramConn.parentObjectName + ":" + paramConn.connectorName;

				cout << "[" <<paramContBufferIndex << "] control: " << controlConnector << "\t\t\t\t process: " << processParamConnector << "\t\tvalueIndex: " /*<< valueIndex*/ << endl;

				paramContBufferIndex++;
			}
			cout << " EXITING ComboDataInt::" << __func__ << endl;
		}
#endif
	}

#define dbg 1
	void ComboDataInt::printIndexMappedProcessData()
	{
#if(dbg >= 1)
		cout << "*****ENTERING ComboDataInt::" << __func__ << endl;
		cout << "PROCESS INDEX MAPPING" << endl;
		cout << "size: " << this->comboStruct.processIndexMap.size() << endl;
		for (auto &  processIndexing : this->comboStruct.processIndexMap)
		{

			cout << "PROCESS:" << processIndexing.second.processName << endl;
			cout << "name: " << processIndexing.second.processName << endl;
			cout << "processSequenceIndex: " << processIndexing.second.processSequenceIndex << endl;
			cout << "parentEffect: " << processIndexing.second.parentEffect << endl;

			cout << "paramIndexMap size: " << processIndexing.second.paramIndexMap.size() << endl;
			for(auto &  processParamIndexing : processIndexing.second.paramIndexMap)
			{
				cout << "\t\t parameter name: " << processParamIndexing.second.paramName << endl;
				cout << "\t\t parameter index: " << processParamIndexing.second.paramIndex << endl;
				cout << "\t\t parentProcess: " << processParamIndexing.second.parentProcess << endl;
				cout << "\t\t parameter paramControlBufferIndex: " << processParamIndexing.second.processParamControlBufferIndex << endl;
			}
			cout << "**********************************************" << endl;
		}
		cout << "***** EXITING ComboDataInt::" << __func__  << endl;
#endif

	}

#define dbg 1
	void ComboDataInt::printIndexMappedControlData()
	{
#if(dbg >= 1)

		{
			cout << "*****ENTERING ComboDataInt::" << __func__ << endl;
			cout << "CONTROL INDEX MAPPING" << endl;

			for (auto &  controlIndexing : this->comboStruct.controlIndexMap)
			{
				cout << "CONTROL:" << controlIndexing.second.controlName << endl;
				cout << "name: " << controlIndexing.second.controlName << endl;;
				cout << "index: " << controlIndexing.second.controlIndex << endl;
				cout << "parentEffect: " << controlIndexing.second.parentEffect << endl;
				cout << "conType: " << controlIndexing.second.controlTypeInt << endl;


				cout << "paramIndexMap size: " << controlIndexing.second.paramIndexMap.size() << endl;

				for(auto &  contParamIndexing : controlIndexing.second.paramIndexMap)
				{
					cout << "\t\t contParam.name: " << contParamIndexing.second.contParamName << endl;
					cout << "\t\t contParam.index: " << contParamIndexing.second.contParamIndex << endl;
					cout << "\t\t contParam.parentControl: " << contParamIndexing.second.parentControl << endl;
				}
			}

			cout << "***** EXITING ComboDataInt::" << __func__  << endl;
		}


#endif
	}

#define dbg 1
	void ComboDataInt::printIndexMappedComboData(void)
	{
#if(dbg >= 1)

			cout << "*****ENTERING ComboDataInt::" << __func__ << endl;
			cout << "COMBO INDEX MAPPING" << endl;
			this->printIndexMappedProcessData();

			this->printIndexMappedControlData();
			cout << "***** EXITING ComboDataInt::" << __func__ << endl;
#endif

	}

#define dbg 0
	void ComboDataInt::printPedalUIData()
	{
#if(dbg >= 1)

			cout << "*****ENTERING ComboDataInt::printPedalUIData" << endl;


			cout << getCompactedJSONData(this->pedalUiJson) << endl;
			cout << "***** EXITING ComboDataInt::printPedalUIData " << endl;
#endif

	}

	void ComboDataInt::printEffectComboJsonProcessParameters(Json::Value combo)
	{
		for(auto & effect : combo["effectArray"])
		{
			for(auto & process : effect["processArray"])
			{
				for(auto & parameter : process["paramArray"])
				{
					cout << effect["name"].asString() << ":" << process["name"].asString();
					cout << ":" << parameter["name"].asString() << "=" << parameter["value"].asInt();
					cout << "\t\t\t" << parameter["paramContType"].asString()<< endl;
				}

			}

		}

	}


	void ComboDataInt::printEffectComboJsonControlParameters(Json::Value combo)
	{
		for(auto & effect : combo["effectArray"])
		{
			for(auto & control : effect["controlArray"])
			{
				for(auto & parameter : control["conParamArray"])
				{
					cout << effect["name"].asString() << ":" << control["name"].asString();
					cout << ":" << parameter["alias"].asString() << "=" << parameter["value"].asInt() << endl;
				}
			}
		}
	}






	/*********************** Load Unsorted Struct Vectors ****************************************************/

#define dbg 0
	vector<Process>  ComboDataInt::loadProcessStructVectorFromJsonProcessArray(int parentEffectIndex, Json::Value processArray)
	{
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::" << __func__ << endl;
#endif
#if(dbg >= 3)
		cout << getCompactedJSONData(processArray) << endl;
#endif
		int status = 0;
		string causeOfException;
		int processArraySize = processArray.size();

		vector<Process> processVector(processArraySize);	// need temporary array to use indexing.

		try
		{
			for(auto & process : processArray)
			{

				int processEffectIndex = process["index"].asUInt();
				processVector[processEffectIndex].processEffectIndex = processEffectIndex;
				processVector[processEffectIndex].processName = process["name"].asString();
				if(process["type"].asString().empty() == false)
				{
					processVector[processEffectIndex].processTypeString = process["type"].asString();
				}
				else
				{
					processVector[processEffectIndex].processTypeString = process["procType"].asString();
				}

				if(processVector[processEffectIndex].processTypeString.compare("delayb") == 0)
					processVector[processEffectIndex].processTypeInt = 0;
				else if(processVector[processEffectIndex].processTypeString.compare("filter3bb") == 0)
					processVector[processEffectIndex].processTypeInt = 1;
				else if(processVector[processEffectIndex].processTypeString.compare("filter3bb2") == 0)
					processVector[processEffectIndex].processTypeInt = 2;
				else if(processVector[processEffectIndex].processTypeString.compare("lohifilterb") == 0)
					processVector[processEffectIndex].processTypeInt = 3;
				else if(processVector[processEffectIndex].processTypeString.compare("mixerb") == 0)
					processVector[processEffectIndex].processTypeInt = 4;
				else if(processVector[processEffectIndex].processTypeString.compare("volumeb") == 0)
					processVector[processEffectIndex].processTypeInt = 5;
				else if(processVector[processEffectIndex].processTypeString.compare("waveshaperb") == 0)
					processVector[processEffectIndex].processTypeInt = 6;

				processVector[processEffectIndex].parentEffect = process["parentEffect"].asString();
				processVector[processEffectIndex].parentEffectIndex = parentEffectIndex;
#if(dbg >= 2)
				cout << "processEffectIndex: " << processEffectIndex << endl;
				cout << "parentEffect: " << processVector[processEffectIndex].parentEffect << endl;
				cout << "parentEffectIndex: " << processVector[processEffectIndex].parentEffectIndex << endl;
				cout << "processEffectIndex: " << processVector[processEffectIndex].processEffectIndex << endl;
				cout << "processName: " << processVector[processEffectIndex].processName << endl;
				cout << "processTypeString: " << processVector[processEffectIndex].processTypeString << endl;
				cout << "processTypeInt: " << processVector[processEffectIndex].processTypeInt << endl;
#endif


				try
				{
					processVector[processEffectIndex].footswitchNumber = process["footswitchNumber"].asInt();
				}
				catch(std::exception &e)
				{
					processVector[processEffectIndex].footswitchNumber =  static_cast<int>(strtol(process["footswitchNumber"].asString().c_str(),NULL,10));
				}


				processVector[processEffectIndex].footswitchType = process["footswitchType"].asString();
#if(dbg >= 2)
				processVector[processEffectIndex].footswitchType = process["footswitchType"].asString();
					cout << "footswitchNumber: " << processVector[processEffectIndex].footswitchNumber << endl;
#endif


				Json::Value inputArray = process["inputArray"];
				int inputArraySize = inputArray.size();
				processVector[processEffectIndex].inputCount = inputArraySize;
				vector<Connector> inputVector(inputArraySize); // need temporary array to use indexing.
				for(auto & input : inputArray)
				{
					causeOfException = "input index";
					int inputArrayIndex = input["index"].asUInt();
					inputVector[inputArrayIndex].parentObjectName = process["name"].asString();
					inputVector[inputArrayIndex].connectorName = input["name"].asString();
					inputVector[inputArrayIndex].connectorIndex = inputArrayIndex;
					inputVector[inputArrayIndex].connectedBufferIndex = 58; // dummy input buffer
					processVector[processEffectIndex].inputVector = inputVector;
#if(dbg >= 2)
					cout << "inputArrayIndex: " << inputArrayIndex << endl;
					cout << "parentObjectName: " << inputVector[inputArrayIndex].parentObjectName << endl;
					cout << "connectorIndex: " << inputVector[inputArrayIndex].connectorIndex << endl;
					cout << "connectedBufferIndex: " << inputVector[inputArrayIndex].connectedBufferIndex << endl; // dummy input buffer
					cout << "input name: " << inputVector[inputArrayIndex].connectorName << endl;
#endif
				}
#if 0
				for(auto & input : tempInputArray)
				{
					processVector[processEffectIndex].inputVector.push_back(input);
				}
#endif
				Json::Value outputArray = process["outputArray"];
				int outputArraySize = outputArray.size();
				processVector[processEffectIndex].outputCount = outputArraySize;
				vector<Connector> outputVector(outputArraySize);
				for(auto & output : outputArray)
				{
					causeOfException = "output index";
					int outputArrayIndex = output["index"].asUInt();
					outputVector[outputArrayIndex].parentObjectName = process["name"].asString();
					outputVector[outputArrayIndex].connectorName =  output["name"].asString();
					outputVector[outputArrayIndex].connectorIndex = outputArrayIndex;
					outputVector[outputArrayIndex].connectedBufferIndex = 59; // dummy output buffer
					processVector[processEffectIndex].outputVector = outputVector;
#if(dbg >= 2)
					cout << "outputArrayIndex: " << outputArrayIndex << endl;
					cout << "parentObjectName: " << outputVector[outputArrayIndex].parentObjectName << endl;
					cout << "connectorName: " << outputVector[outputArrayIndex].connectorName << endl;
					cout << "connectorIndex: " << outputVector[outputArrayIndex].connectorIndex << endl;
					cout << "connectedBufferIndex: " << outputVector[outputArrayIndex].connectedBufferIndex << endl;
					cout << "connectorName: " << outputVector[outputArrayIndex].connectorName << endl;
#endif
				}
#if 0
				for(auto & output : tempOutputArray)
				{
					processVector[processEffectIndex].outputVector.push_back(output);
				}
#endif
				Json::Value paramArray = process["paramArray"];
				int paramArraySize = paramArray.size();
				processVector[processEffectIndex].paramCount = paramArraySize;
				vector<ProcessParameter> paramVector(paramArraySize);
				for(auto & parameter : paramArray)
				{
					causeOfException = "parameter index";
					int paramArrayIndex = parameter["index"].asUInt();
					paramVector[paramArrayIndex].paramConnector.connectorName = parameter["name"].asString();
					paramVector[paramArrayIndex].paramConnector.parentObjectName = process["name"].asString();

					paramVector[paramArrayIndex].paramConnector.connectorIndex = paramArrayIndex;
					paramVector[paramArrayIndex].paramConnector.connectedBufferIndex = 59; //"dummy" buffer index for

					paramVector[paramArrayIndex].paramControlType = parameter["paramContType"].asString();
					if(parameter["type"].asString().empty() == false)
					{
						try
						{
							paramVector[paramArrayIndex].paramType = parameter["type"].asInt();
						}
						catch(std::exception &e)
						{
							paramVector[paramArrayIndex].paramType =  atoi(parameter["type"].asString().c_str());
						}
					}
					else
					{

						try
						{
							paramVector[paramArrayIndex].paramType = parameter["paramType"].asInt();
						}
						catch(std::exception &e)
						{
							paramVector[paramArrayIndex].paramType =  atoi(parameter["paramType"].asString().c_str());
						}
					}


					try
					{
						paramVector[paramArrayIndex].valueIndex = parameter["value"].asInt();
					}
					catch(std::exception &e)
					{
						paramVector[paramArrayIndex].valueIndex =  atoi(parameter["value"].asString().c_str());
					}
#if(dbg >= 2)
					cout << "paramArrayIndex: " << paramArrayIndex << endl;
					cout << "paramType: " << paramVector[paramArrayIndex].paramType  << endl;
					cout << "connectorName: " << paramVector[paramArrayIndex].paramConnector.connectorName << endl;
					cout << "parentObjectName: " << paramVector[paramArrayIndex].paramConnector.parentObjectName << endl;

					cout << "connectorIndex: " << paramVector[paramArrayIndex].paramConnector.connectorIndex << endl;
					cout << "connectedBufferIndex: " << paramVector[paramArrayIndex].paramConnector.connectedBufferIndex << endl;

					cout << "paramControlType: " << paramVector[paramArrayIndex].paramControlType << endl;
					cout << "valueIndex: " << paramVector[paramArrayIndex].valueIndex << endl;
#endif
					processVector[processEffectIndex].paramVector = paramVector;
				}
#if 0
				for(auto & parameter : tempParamArray)
				{
					processVector[processEffectIndex].paramVector.push_back(parameter);

				}
#endif
			}
		}
		catch(exception &e)
		{
			 cout << "exception in ComboDataInt::" << __func__ << ": " << causeOfException << ":" << e.what() <<  endl;
			status = -1;
		}
#if(dbg >= 3)
			this->printProcessStructVector(false,processVector);
#endif

#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::" << __func__ << ": " << status << endl;
#endif

		return processVector;
	}



#define dbg 0
	vector<ProcessSignalConnection>  ComboDataInt::loadProcessConnectionStructVectorFromJsonConnectionArray(Json::Value connectionArray)
	{
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::" << __func__ << endl;
#endif

		int status = 0;


#if(dbg >= 3)
		getCompactedJSONData(connectionArray);
#endif
		vector<ProcessSignalConnection> processConnVector;

		try
		{
			for(auto & connection : connectionArray)
			{
				ProcessSignalConnection tempProcConnection;

				tempProcConnection.srcProcessOutputConnector.parentObjectName = connection["src"]["object"].asString();
				tempProcConnection.srcProcessOutputConnector.connectorName = connection["src"]["port"].asString();
				tempProcConnection.srcProcessOutputConnector.connectorIndex = 0;
				tempProcConnection.srcProcessOutputConnector.connectedBufferIndex = 59;
				tempProcConnection.destProcessInputConnector.parentObjectName = connection["dest"]["object"].asString();
				tempProcConnection.destProcessInputConnector.connectorName = connection["dest"]["port"].asString();
				tempProcConnection.destProcessInputConnector.connectorIndex = 0;
				tempProcConnection.destProcessInputConnector.connectedBufferIndex = 59;
				processConnVector.push_back(tempProcConnection);
#if(dbg >= 2)
				cout << tempProcConnection.srcProcessOutputConnector.parentObjectName << ":";
				cout << tempProcConnection.srcProcessOutputConnector.connectorName << ">";
				cout << tempProcConnection.destProcessInputConnector.parentObjectName << ":";
				cout << tempProcConnection.destProcessInputConnector.connectorName << endl;
#endif
			}

		}
		catch(exception &e)
		{
			 cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() <<  endl;
			status = -1;
		}

#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::" << __func__ << ": " << status << endl;
#endif

		return processConnVector;
	}

#define dbg 0
	vector<Control>  ComboDataInt::loadControlStructVectorFromJsonControlArray(int parentEffectIndex, string parentEffectAbbr, Json::Value controlArray)
	{
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::" << __func__ << endl;
#endif

#if(dbg >= 3)
		getCompactedJSONData(controlArray);
#endif
		int status = 0;
		vector<Control> controlVector(controlArray.size());	// need temporary array to use indexing.

		try
		{



			for(auto & control : controlArray)
			{


				int controlEffectIndex = control["index"].asUInt();
				controlVector[controlEffectIndex].name = control["name"].asString();
				controlVector[controlEffectIndex].controlEffectIndex = controlEffectIndex;
				controlVector[controlEffectIndex].parentEffect = control["parentEffect"].asString();
				controlVector[controlEffectIndex].parentEffectAbbr = parentEffectAbbr;
				controlVector[controlEffectIndex].parentEffectIndex = parentEffectIndex;
				controlVector[controlEffectIndex].conType = control["type"].asString();
				if(control["type"].asString().empty() == false)
				{
					controlVector[controlEffectIndex].conType = control["type"].asString();
				}
				else
				{
					controlVector[controlEffectIndex].conType = control["procType"].asString();
				}

				if(controlVector[controlEffectIndex].conType.compare("norm") == 0)
					controlVector[controlEffectIndex].conTypeInt = 0;
				else if(controlVector[controlEffectIndex].conType.compare("env") == 0)
					controlVector[controlEffectIndex].conTypeInt = 1;
				else if(controlVector[controlEffectIndex].conType.compare("lfo") == 0)
					controlVector[controlEffectIndex].conTypeInt = 2;


				Json::Value output = control["outputArray"][0];
				controlVector[controlEffectIndex].output.connectorName = output["name"].asString();
				controlVector[controlEffectIndex].output.connectorIndex = output["index"].asUInt();
				controlVector[controlEffectIndex].output.connectedBufferIndex = 59;

				Json::Value outputInv = control["outputArray"][1];
				controlVector[controlEffectIndex].outputInv.connectorName = outputInv["name"].asString();
				controlVector[controlEffectIndex].outputInv.connectorIndex = outputInv["index"].asUInt();
				controlVector[controlEffectIndex].outputInv.connectedBufferIndex = 59;


				vector<ControlParameter> paramVector;
				Json::Value contParamArray = control["conParamArray"];
				int contParamCount = contParamArray.size();
				vector<ControlParameter> controlParamVector(contParamCount);

				for(auto & controlParam : contParamArray)
				{
					int contParamIndex = controlParam["index"].asUInt();

					controlParamVector[contParamIndex].index = contParamIndex;
					controlParamVector[contParamIndex].name = controlParam["alias"].asString();
					controlParamVector[contParamIndex].alias = controlParam["alias"].asString();
					controlParamVector[contParamIndex].abbr = controlParam["abbr"].asString();

					if(controlParam["type"].asString().empty() == false)
					{
						controlParamVector[contParamIndex].paramType = controlParam["type"].asUInt();
					}
					else
					{
						controlParamVector[contParamIndex].paramType = controlParam["paramType"].asUInt();
					}
					controlParamVector[contParamIndex].controlledParamType = controlParam["controlledParamType"].asUInt();
					controlParamVector[contParamIndex].inheritControlledParamType = controlParam["inheritControlledParamType"].asBool();
					controlParamVector[contParamIndex].cvEnabled = controlParam["controlVoltageEnabled"].asBool();
					controlParamVector[contParamIndex].valueIndex = controlParam["value"].asInt();

				}
				controlVector[controlEffectIndex].paramVector = controlParamVector;
#if 0
				for(auto & controlParam : tempContParamVector)
				{
					controlVector[controlEffectIndex].paramVector.push_back(controlParam);
				}
#endif
			}
#if 0
			for(auto & control : controlVector)
			{
#if(dbg >= 2)

#endif
				controlVector.push_back(control);
			}
#endif
		}
		catch(exception &e)
		{
			 cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() <<  endl;
			status = -1;
		}
#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::" << __func__ << ": " << status << endl;
#endif

		return controlVector;
	}

#define dbg 0
	vector<ProcessParameterControlConnection>  ComboDataInt::loadControlConnectionStructVectorFromJsonControlConnectionArray(Json::Value controlConnectionArray)
	{
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::" << __func__ << endl;
#endif
		int status = 0;
#if(dbg >= 3)
		getCompactedJSONData(controlConnectionArray);
#endif
		vector<ProcessParameterControlConnection> controlConnVector;

		try
		{
			for(auto & controlConnection : controlConnectionArray)
			{
				ProcessParameterControlConnection tempContConnection;

				tempContConnection.srcControlOutputConnector.parentObjectName = controlConnection["src"]["object"].asString();
				tempContConnection.srcControlOutputConnector.connectorName = controlConnection["src"]["port"].asString();
				tempContConnection.srcControlOutputConnector.connectorIndex = 0;
				tempContConnection.srcControlOutputConnector.connectedBufferIndex = 59;
				tempContConnection.destProcessParameterConnector.parentObjectName = controlConnection["dest"]["object"].asString();
				tempContConnection.destProcessParameterConnector.connectorName = controlConnection["dest"]["port"].asString();
				tempContConnection.destProcessParameterConnector.connectorIndex = 0;
				tempContConnection.destProcessParameterConnector.connectedBufferIndex = 59;


				controlConnVector.push_back(tempContConnection);
			}
		}
		catch(exception &e)
		{
			 cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() <<  endl;
			status = -1;

		}

#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::" << __func__ << ": " << status << endl;
#endif

		return controlConnVector;
	}

	/****************** Loading struct vectors from effectComboJson arrays *************************************/

#define dbg 0
	int ComboDataInt::loadEffectComboJsonFromFile(string comboName)
	{
#if(dbg >= 1)
		cout << "*****ENTERING ComboDataInt::" << __func__ << endl;
		cout << "comboName: " << comboName << endl;
#endif
		int status = 0;
		bool boolStatus = true;
		string effectString;
		string currentEffectString;
		string procString;
		string paramString;
		this->comboName = comboName;
#if(dbg >= 2)
		cout << "reading JSON file into jsonString." << endl;
#endif

		try
		{
			clearBuffer(this->jsonBuffer,JSON_BUFFER_LENGTH);
			clearBuffer(this->fileNameBuffer,20);
			// open combo file
			strncpy(this->fileNameBuffer, comboName.c_str(), 19);
			sprintf(this->fileNamePathBuffer,"/home/Combos/%s.txt", this->fileNameBuffer);
			this->comboFD = open(fileNamePathBuffer,O_RDONLY);
			// read file into temp string
			if(this->comboFD >= 0)
			{
				if(read(this->comboFD, this->jsonBuffer, JSON_BUFFER_LENGTH) >= 0)
				{
#if(dbg >= 1)
					cout << "parsing jsonString in effectComboJson" << endl;

#endif
					int result = validateJsonBuffer(this->jsonBuffer);
					if(result == 0) // file needed to cleaned, so replacing file
					{
#if(dbg >= 3)
						cout << "file needed cleaning, so replacing with cleaned file" << endl;
						cout << "new file: " << this->jsonBuffer << endl;
#endif
						close(this->comboFD);

						this->comboFD = open(fileNamePathBuffer, O_WRONLY|O_CREAT|O_TRUNC, 0666);

						if(write(this->comboFD,this->jsonBuffer,strlen(this->jsonBuffer)) == -1)
						{
							clearBuffer(this->jsonBuffer,JSON_BUFFER_LENGTH);
							cout << "error writing jsonBufferString back to combo file." << endl;
						}
					}
					if(result >= 0)
					{
						if(this->effectComboJson.empty() == false)
						{
							this->effectComboJson.clear();
						}

						boolStatus = this->comboReader.parse(this->jsonBuffer, this->effectComboJson);
#if(dbg >= 2)
						cout << "getting combo index" << endl;
#endif

#if(dbg >= 2)
						getCompactedJSONData(this->effectComboJson);
#endif

						if(boolStatus == false)
						{
							status = -1;
							cout << "JSON parse failed." << endl;
						}
					}
					else
					{
						status = -1;
						cout << "JSON parse failed." << endl;
					}
				}
				else
				{
					cout << "failed to read file: " << fileNamePathBuffer << endl;
					status = -1;
				}
			}
			else
			{
				cout << "failed to open file: " << fileNamePathBuffer << endl;
				status = -1;
			}

			if(this->comboFD >= 0)
			{
				close(this->comboFD);
			}
		}
		catch(exception &e)
		{
			cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() <<  endl;
			status = -1;
		}


#if(dbg >= 1)
		cout << "***** EXITING ComboDataInt::" << __func__ << ": " << status << endl;
#endif


		return status;
	}


	int ComboDataInt::loadEffectComboJson(string comboData)
	{
#if(dbg >= 1)
		cout << "*****ENTERING ComboDataInt::" << __func__ << endl;

#endif
		int status = 0;
		bool boolStatus = true;
		string effectString;
		string currentEffectString;
		string procString;
		string paramString;

#if(dbg >= 2)
		cout << "reading JSON file into jsonString." << endl;
#endif

		try
		{
			strncpy(this->jsonBuffer,comboData.c_str(),JSON_BUFFER_LENGTH);
#if(dbg >= 1)
			cout << "parsing jsonString in effectComboJson" << endl;

#endif
			int result = validateJsonBuffer(this->jsonBuffer);
			if(result == 1)
			{
				if(this->effectComboJson.empty() == false)
				{
					this->effectComboJson.clear();
				}

				boolStatus = this->comboReader.parse(this->jsonBuffer, this->effectComboJson);
#if(dbg >= 2)
				cout << "getting combo index" << endl;
#endif
				this->comboName = this->effectComboJson["name"].asString();
				cout << "comboName: " << this->comboName << endl;
#if(dbg >= 2)
				getCompactedJSONData(this->effectComboJson);
#endif

				if(boolStatus == false)
				{
					status = -1;
					cout << "JSON parse failed." << endl;
				}

			}
			else if(result == 0) // file needed to cleaned, so replacing file
			{
#if(dbg >= 3)
				cout << "file needed cleaning, so replacing with cleaned file" << endl;
				cout << "new file: " << this->jsonBuffer << endl;
#endif
				close(this->comboFD);

				this->comboFD = open(fileNamePathBuffer, O_WRONLY|O_CREAT|O_TRUNC, 0666);

				if(write(this->comboFD,this->jsonBuffer,strlen(this->jsonBuffer)) == -1)
				{
					clearBuffer(this->jsonBuffer,JSON_BUFFER_LENGTH);
					cout << "error writing jsonBufferString back to combo file." << endl;
				}
			}
			else
			{
					status = -1;
					cout << "JSON parse failed." << endl;

			}
		}
		catch(exception &e)
		{
			cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() <<  endl;
			status = -1;
		}


#if(dbg >= 1)
		cout << "***** EXITING ComboDataInt::" << __func__ << ": " << status << endl;
#endif


		return status;

	}


#define dbg 0
	int  ComboDataInt::loadComboJsonFileStructFromEffectComboJson(void)
	{
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::" << __func__ << endl;
#endif
		int status = 0;

		try
		{
			this->comboFileStruct.name = this->effectComboJson["name"].asString();
			if(this->comboFileStruct.name.empty() == false)
			{
				this->comboName = this->comboFileStruct.name;
			}

#if(dbg >= 1)
			cout << "comboName: " << this->comboName << endl;
#endif
			vector<Control> tempContStructVector;
			vector<ProcessParameterControlConnection> tempContConnStructVector;
			Json::Value effectArray = this->effectComboJson["effectArray"];
			for(auto & effect : this->effectComboJson["effectArray"]) // effects having indexing, but effect order has never been an issue
			{

				int effectIndex = effect["index"].asInt();
				this->comboFileStruct.effectArray[effectIndex].name = effect["name"].asString();
				this->comboFileStruct.effectArray[effectIndex].abbr = effect["abbr"].asString();
				this->comboFileStruct.effectArray[effectIndex].index = effectIndex;

				Json::Value processArray = effect["processArray"];
				this->comboFileStruct.effectArray[effectIndex].processVector =
								this->loadProcessStructVectorFromJsonProcessArray(effectIndex,processArray);

				Json::Value connectionArray = effect["connectionArray"];
				this->comboFileStruct.effectArray[effectIndex].processConnectionVector =
								this->loadProcessConnectionStructVectorFromJsonConnectionArray(connectionArray);

				Json::Value controlArray = effect["controlArray"];
				this->comboFileStruct.effectArray[effectIndex].processParamControlVector =
								this->loadControlStructVectorFromJsonControlArray(effectIndex,
										effect["abbr"].asString(),controlArray);


				Json::Value controlConnectionArray = effect["controlConnectionArray"];
				this->comboFileStruct.effectArray[effectIndex].processParamControlConnectionVector =
								this->loadControlConnectionStructVectorFromJsonControlConnectionArray(controlConnectionArray);
			}



			Json::Value effectConnectionArray = this->effectComboJson["effectConnectionArray"];

			for(auto & effectConnect : effectConnectionArray)
			{
				EffectConnection effectConnection;
				effectConnection.srcEffectOutputConnector.effectName = effectConnect["src"]["object"].asString();
				effectConnection.srcEffectOutputConnector.connectorName = effectConnect["src"]["port"].asString();

				effectConnection.destEffectInputConnector.effectName = effectConnect["dest"]["object"].asString();
				effectConnection.destEffectInputConnector.connectorName = effectConnect["dest"]["port"].asString();


				this->comboFileStruct.effectConnectionVector.push_back({effectConnection.srcEffectOutputConnector, effectConnection.destEffectInputConnector});
			}
		}
		catch(exception &e)
		{
			 cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() <<  endl;
			status = -1;
		}
#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::" << __func__ << ": " << status << endl;
#endif


		return status;
	}

#define dbg 0
	int ComboDataInt::loadUnsortedProcessStructVectorFromComboJsonFileStruct(void)
	{
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::" << __func__ << endl;
#endif
		int status = 0;
		this->unsortedProcessStructVector.clear();

		try
		{

			for(auto & effect : this->comboFileStruct.effectArray)
			{
				for(auto & process : effect.processVector)
				{
					this->unsortedProcessStructVector.push_back(process);
				}
			}
#if(dbg >= 2)
			this->printProcessStructVector(false,this->unsortedProcessStructVector);
#endif
		}
		catch(exception &e)
		{
			 cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() <<  endl;
			status = -1;
		}
#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::" << __func__ << ": " << status << endl;
#endif
		return status;
	}

#define dbg 0
	int ComboDataInt::loadUnmergedSubconnectionStructVectorFromComboJsonFileStruct(void)
	{
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::" << __func__ << endl;
#endif
		int status = 0;
		this->unmergedProcessSubconnectionStructVector.clear();

		try
		{

			for(auto & effect : this->comboFileStruct.effectArray)
			{
				vector<ProcessSignalConnection> procConnVector =
								effect.processConnectionVector;
				for(auto & processSubConnection : procConnVector)
				{
					this->unmergedProcessSubconnectionStructVector.push_back(processSubConnection);
				}

				vector<EffectConnection> effectConnVector = this->comboFileStruct.effectConnectionVector;

				for(auto & effectConnection : effectConnVector)
				{
					ProcessSignalConnection effectConnAdaptor;
					string effectName; // connector "Parent".  Was "process", needed something more generic to cover "effect" and "control"
					string connectorName;
					int connectorIndex; // position of connector WRT other like connectors (input,output, parameter)

					effectConnAdaptor.srcProcessOutputConnector.parentObjectName =
							effectConnection.srcEffectOutputConnector.effectName;
					effectConnAdaptor.srcProcessOutputConnector.connectorName =
							effectConnection.srcEffectOutputConnector.connectorName;
					effectConnAdaptor.srcProcessOutputConnector.connectorIndex =
							effectConnection.srcEffectOutputConnector.connectorIndex;

					effectConnAdaptor.destProcessInputConnector.parentObjectName =
							effectConnection.destEffectInputConnector.effectName;
					effectConnAdaptor.destProcessInputConnector.connectorName =
							effectConnection.destEffectInputConnector.connectorName;
					effectConnAdaptor.destProcessInputConnector.connectorIndex =
							effectConnection.destEffectInputConnector.connectorIndex;


					this->unmergedProcessSubconnectionStructVector.push_back(effectConnAdaptor);
				}
			}

		}
		catch(exception &e)
		{
			 cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() <<  endl;
			status = -1;
		}

#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::" << __func__ << ": " << status << endl;
#endif
		return status;
	}

	/************************* Internal data processing *********************************/


#define dbg 0
	int ComboDataInt::mergeUnmergedSubconnectionsAndLoadIntoUnsortedConnectionStructVector(void)
	{

#if(dbg >= 1)
		 cout << "***** ENTERING: ComboDataInt::" << __func__ << endl;
#endif
		int status = 0;

		struct _procInput {
			string procName;
			string procInput;
		};


		if(this->effectComboJson.isNull() == false)
		{
			try
			{
				this->unsortedProcessConnectionStructVector.clear();

				std::vector<ProcessSignalConnection> procConnectionsStruct;
				std::vector<Connector> procInputs;

				std::vector<ProcessSignalConnection> mergedConnections;
				ProcessSignalConnection tempProcConnection;  // used for merging subconnections which are then transferred
				// to mergedConnections.
				mergedConnections.clear();
				procInputs.clear();

				Effect tempEffects[2] = this->comboFileStruct.effectArray;
				this->effectCount = 2;

				// ASSUMPTION: EACH PROCESS INPUT WILL BE CONNECTED TO ONLY ONE OUTPUT.


				// ************ Gather inputs for all processes ********************
				Connector tempProcInput;
				tempProcInput.parentObjectName = string("system");
				tempProcInput.connectorName = string("playback_1");
				procInputs.push_back(tempProcInput);
				tempProcInput.parentObjectName = string("system");
				tempProcInput.connectorName = string("playback_2");
				procInputs.push_back(tempProcInput);


				for(auto & effect : this->comboFileStruct.effectArray)
				{
					vector<Process> tempProcVector = effect.processVector;
					for(auto & process : effect.processVector)
					{
						vector<Connector> tempInputVector = process.inputVector;
						for(auto & processInput : process.inputVector)
						{
							tempProcInput.parentObjectName = process.processName;
							tempProcInput.connectorName = processInput.connectorName;
							procInputs.push_back(tempProcInput);
						}
					}
				}


				for(auto & input : procInputs)
				{
#if(dbg >=3)
					 cout << "procInput[" << procInputIndex << "]: " << procInputs[procInputIndex].parentObjectName << ":" << procInputs[procInputIndex].connectorName << endl;
					 cout << "PROCESS CONNECTIONS:" << endl;
#endif
					Json::Value inputConn;
					// For each input, find the connection that directly feeds into it.

					for(auto & unmergedConnection : this->unmergedProcessSubconnectionStructVector)
					{
						if(this->compareConnectors(false, unmergedConnection.destProcessInputConnector, input) == true)
						{
							tempProcConnection = unmergedConnection;
							bool exit = false;

#if(dbg >= 3)
							 cout << "BASE CONNECTION FOUND....." << endl;
							 cout << tempProcConnection.srcProcessOutputConnector.parentObjectName << ":";
							 cout << tempProcConnection.srcProcessOutputConnector.connectorName << ">";
							 cout << tempProcConnection.destProcessInputConnector.parentObjectName << ":";
							 cout << tempProcConnection.destProcessInputConnector.connectorName << endl;
							 cout << "************************************" << endl;
#endif
							for(int loopNum = 0; loopNum < 5 && exit == false; loopNum++) // loop multiple times to make sure
								// all subconnections were merged
							{
								size_t connIndex = 0;
								for(auto & unmergedConnection : this->unmergedProcessSubconnectionStructVector)
								{
#if(dbg >= 3)
									 cout << this->unmergedProcessSubconnectionStructVector[connIndex].srcProcessOutputConnector.parentObjectName << ":";
									 cout << this->unmergedProcessSubconnectionStructVector[connIndex].srcProcessOutputConnector.connectorName << ">";
									 cout << this->unmergedProcessSubconnectionStructVector[connIndex].destProcessInputConnector.parentObjectName << ":";
									 cout << this->unmergedProcessSubconnectionStructVector[connIndex].destProcessInputConnector.connectorName << endl;
#endif

									if(this->compareConnectors(false, unmergedConnection.destProcessInputConnector, tempProcConnection.srcProcessOutputConnector) == true)
									{
										tempProcConnection = this->mergeConnections(unmergedConnection,tempProcConnection);
										exit = (tempProcConnection.srcProcessOutputConnector.parentObjectName.find("(") == string::npos) || (tempProcConnection.srcProcessOutputConnector.parentObjectName.compare("system") == 0); // object is process output or "system"
										if(exit)
										{
											this->unsortedProcessConnectionStructVector.push_back(tempProcConnection);
										}
#if(dbg >= 3)
										 cout << "MATCH....." << endl;
										 cout << tempProcConnection.srcProcessOutputConnector.parentObjectName << ":";
										 cout << tempProcConnection.srcProcessOutputConnector.connectorName << ">";
										 cout << tempProcConnection.destProcessInputConnector.parentObjectName << ":";
										 cout << tempProcConnection.destProcessInputConnector.connectorName << endl;
#endif

										break;
									}
									else if((tempProcConnection.srcProcessOutputConnector.parentObjectName.find("(") == string::npos) || (tempProcConnection.srcProcessOutputConnector.parentObjectName.compare("system") == 0))
										// original connection can't be merged further
									{
										this->unsortedProcessConnectionStructVector.push_back(tempProcConnection);
										exit = true;
										break;
									}
									else if(connIndex == this->unmergedProcessSubconnectionStructVector.size()-1) // no matches found, so no connections exist
									{
										exit = true;
										break;
									}
									connIndex++;
								}
							}
							break;
						}
					}
				}

#if(dbg >= 2)
				this->printProcessSignalConnectionList(false,this->unsortedProcessConnectionStructVector);
#endif

			}
			catch (exception &e)
			{
				 cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() <<  endl;
				status = -1;
			}
		}
		else status = -1;

#if(dbg >= 1)
		 cout << "***** EXITING: ComboDataInt::" << __func__ << ": " << status << endl;
#endif
		return status;
	}


#define dbg 0
	int ComboDataInt::getTargetProcessIndex(string processName)
	{
#if(dbg >= 1)
		 cout << "***** ENTERING: ComboDataInt::" << __func__ << endl;
		 cout << "processName: " << processName << endl;
#endif
		int targetProcessIndex = 0;
#if(dbg >=2)
		 cout << "process to search for index of: " << processName << endl;
#endif
		try
		{
			size_t processIndex = 0;
			for(auto & unsortedProcess : this->unsortedProcessStructVector)
			{
#if(dbg >=2)
				 cout << "comparing: " << processName << " & " << this->unsortedProcessStructVector[processIndex].paramName << endl;
#endif

				if(processName.compare(unsortedProcess.processName) == 0)
				{
					targetProcessIndex = processIndex;
					break;
				}
				if(processIndex == this->unsortedProcessStructVector.size() - 1) //end of connection array reach, but no match found
				{
					targetProcessIndex = -1;
				}
				processIndex++;
			}
		}
		catch(std::exception &e)
		{
			 cout << "exception in ComboDataInt::getTargetProcessIndex: " << e.what() <<  endl;
		}
#if(dbg >= 2)
		if(targetProcessIndex >= 0)
			 cout << "target process index: " << targetProcessIndex << ":" << this->unsortedProcessStructVector[targetProcessIndex].paramName << endl;
#endif

#if(dbg >= 1)
		 cout << "***** EXITING: ComboDataInt::" << __func__ << ": " << targetProcessIndex << endl;
#endif
		return targetProcessIndex;
	}

#define dbg 0
	std::vector<string> ComboDataInt::getProcessInputs(string processName)
	{
#if(dbg >= 1)
		 cout << "***** ENTERING: ComboDataInt::" << __func__ << endl;
		 cout << "processName: " << processName << endl;
#endif
		std::vector<string> inputs;
		int  targetIndex = this->getTargetProcessIndex(processName);

		for(auto & input : this->unsortedProcessStructVector[targetIndex].inputVector)
		{
			inputs.push_back(input.connectorName);
		}

#if(dbg >= 2)
		for(auto & effectConnection : effectConnVector)
		{
			 cout << "inputs[" << i <<"]: " << inputs[i] << endl;
		}
#endif

#if(dbg >= 1)
		 cout << "***** EXITING: ComboDataInt::" << __func__ << ": " << inputs.size() << endl;
#endif

		return inputs;
	}

#define dbg 0
	std::vector<string> ComboDataInt::getFirstProcesses()
	{
#if(dbg >= 1)
		 cout << "***** ENTERING: ComboDataInt::" << __func__ << endl;
#endif
		Connector start;
		start.parentObjectName = "system";
		start.connectorName = "capture_1";

		string firstProcess;
		std::vector<string> firstProcesses;

		try
		{
			for(auto & unsortedProcConn : this->unsortedProcessConnectionStructVector)
			{
	#if(dbg >= 2)
				 cout << "comparing: ";
				 cout << start.parentObjectName << " vs " << unsortedProcConn.srcProcessOutputConnector.parentObjectName;
				 cout << "\t and \t";
				 cout << start.connectorName << " vs " << unsortedProcConn.srcProcessOutputConnector.connectorName << endl;
	#endif
				if((start.parentObjectName.compare(unsortedProcConn.srcProcessOutputConnector.parentObjectName) == 0)  &&
								(start.connectorName.compare(unsortedProcConn.srcProcessOutputConnector.connectorName) == 0))
				{
					firstProcess = unsortedProcConn.destProcessInputConnector.parentObjectName;
					if(std::find(firstProcesses.begin(),firstProcesses.end(),firstProcess)==firstProcesses.end()) // procName not in nextProcesses
					{
						firstProcesses.push_back(firstProcess);
					}

				}
			}

	#if(dbg >= 2)
			for(std::vector<Connector>::size_type i = 0; i < this->dataReadyVector.size(); i++)
			{
				 cout << "dataReadyVector[" << i <<"]: " << dataReadyVector[i].parentObjectName << ":" << dataReadyVector[i].connectorName << endl;
			}
	#endif

		}
		catch(exception &e)
		{
			cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() << endl;
		}


#if(dbg >= 1)
		 cout << "***** EXITING: ComboDataInt::" << __func__;
		for(std::vector<string>::size_type i = 0; i < firstProcesses.size(); i++)
		{
			 cout << firstProcesses[i] << ", ";
		}
		 cout << endl;

#endif

		return firstProcesses;
	}

#define dbg 0
	std::vector<string> ComboDataInt::getNextProcesses()
	{
#if(dbg >= 1)
		 cout << "***** ENTERING: ComboDataInt::" << __func__ << endl;
#endif
		std::vector<string> nextProcesses;
		bool foundNextProcesses = false;

		try
		{
			for(int inputSatisfactionLevel = 2; (inputSatisfactionLevel > 0) && (foundNextProcesses == false); inputSatisfactionLevel--)
			{
				for(auto & unsortedProcess : this->unsortedProcessStructVector)
				{
					string procName = unsortedProcess.processName;
					if(this->areAllProcessInputsSatisfied(procName) == inputSatisfactionLevel) // all inputs satisfied.  Check for this first.
					{
						if(std::find(nextProcesses.begin(),nextProcesses.end(),procName)==nextProcesses.end()) // procName not in nextProcesses
						{
							nextProcesses.push_back(procName);
							foundNextProcesses = true;
						}
					}
				}
			}

	#if(dbg >= 2)
			int i = 0;
			for(auto & unsortedProcess this->unsortedProcessStructVector)
			{
				 cout << "unsortedProcessStructVector[" << i <<"]: " << unsortedProcess.name << endl;
				i++;
			}

	#endif

		}
		catch(exception &e)
		{
			cout << "exception in ComboDataInt::getNextProcesses: " << e.what() << endl;
		}


#if(dbg >= 1)

		{
			cout << "***** EXITING: ComboDataInt::getNextProcesses: ";
			for(auto & nextProcess : nextProcesses)
			{
				cout << nextProcess << ", ";
			}
			cout << endl;
		}

#endif

		return nextProcesses;
	}



#define dbg 0
	int ComboDataInt::areAllProcessInputsSatisfied(string processName)
	{
		/*	status = 2: all inputs satisfied
		 * status = 1: some inputs satisfied
		 * status = 0: no inputs satisfied
		 */
#if(dbg >= 1)
		 cout << "***** ENTERING: ComboDataInt::" << __func__ << endl;
		 cout << "processName: " << processName << endl;
#endif

		int status = 0;
		try
		{
			std::vector<string> inputs = getProcessInputs(processName);
			std::vector<ProcessSignalConnection> connections;
			int inputConnectorCount = inputs.size();
			int inputConnectorSatisfiedCount = 0;
#if(dbg >= 2)
		this->printUnsortedConnectionList();
#endif
		// Get all connections connecting to the inputs of process "processName"
		for(auto & input : inputs)
		{
#if(dbg >= 2)
			 cout << "input connector: " << processName << ":" << inputs[inputConnectorIndex] << endl;
#endif

			for(auto & unsortedProcConnect : this->unsortedProcessConnectionStructVector)
			{

				if((unsortedProcConnect.destProcessInputConnector.parentObjectName.compare(processName) == 0) &&
								(unsortedProcConnect.destProcessInputConnector.connectorName.compare(input) == 0))
				{
#if(dbg >= 2)
					Connection conn = this->unsortedProcessConnectionStructVector[connIndex];


					{
						cout << "unsorted connection: " << conn.srcProcessOutputConnector.parentObjectName << ":" << conn.srcProcessOutputConnector.connectorName;
						cout << ">" << conn.destProcessInputConnector.parentObjectName << ":" << conn.destProcessInputConnector.connectorName << endl;
					}

#endif
					connections.push_back(unsortedProcConnect);
				}
			}
		}

		for(auto & connection : connections)
		{
			for(auto & dataReadyConn : this->dataReadyVector)
			{
#if(dbg >= 2)
				 cout << "comparing: " << connections[connectionsIndex].srcProcessOutputConnector.parentObjectName << ":" << connections[connectionsIndex].srcProcessOutputConnector.connectorName << " to " << this->dataReadyVector[dataReadyListIndex].parentObjectName << ":" << this->dataReadyVector[dataReadyListIndex].connectorName << endl;
#endif

				if(this->compareConnectors(false, dataReadyConn, connection.srcProcessOutputConnector) == true)
				{
#if(dbg >= 2)
					 cout << "inputConnectorSatisfied: " << connections[connectionsIndex].srcProcessOutputConnector.parentObjectName << ":" << connections[connectionsIndex].srcProcessOutputConnector.connectorName << endl;
#endif
					inputConnectorSatisfiedCount++;
				}
			}
		}

		if(inputConnectorSatisfiedCount == inputConnectorCount) status = 2;
		else if(inputConnectorSatisfiedCount > 0) status = 1;

		}
		catch(exception &e)
		{
			cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() << endl;
		}

#if(dbg >= 1)
		 cout << "***** EXITING: ComboDataInt::" << __func__ << ": " << status << endl;
#endif

		return status;
	}

#define dbg 0
	int ComboDataInt::addOutputConnectionsToDataReadyVector(vector<string> processNames)
	{

#if(dbg >= 1)

		{
			cout << "***** ENTERING: ComboDataInt::" << __func__ << endl;
			for(auto & name : processNames)
			{
				cout << name << ", ";
			}
			cout << endl;
		}
#endif
		int status = 0;

		try
		{
			for(auto & processName : processNames)
			{
				int procIndex = this->getTargetProcessIndex(processName);

				if(procIndex >= 0)
				{
					for(auto & output : this->unsortedProcessStructVector[procIndex].outputVector)
					{
						Connector tempConn;
						tempConn.parentObjectName = processName;
						tempConn.connectorName = output.connectorName;
						if(this->isOutputInDataReadyVector(tempConn) == false)
							this->dataReadyVector.push_back(tempConn);
					}
				}
				else
				{
					status = -1;
					break;
				}
			}
		}
		catch(std::exception &e)
		{
			 cout << "exception in getTargetProcessIndex(processName) section " << e.what() <<  endl;
			status = -1;
		}

#if(dbg >= 2)
		this->printDataReadyList();
#endif

#if(dbg >= 1)
		 cout << "***** EXITING: ComboDataInt::" << __func__ << ": " << status << endl;
#endif
		return status;
	}



	int ComboDataInt::transferProcessStructsToSortedProcessStructVector(vector<string> processNames)
	{
#if(dbg >= 1)

		{

		}
		cout << "***** ENTERING: ComboDataInt::" << __func__ << endl;
		for(auto & name : processNames)
		{
			cout << name << ", ";
		}
#endif
		int status = 0;

		try
		{
			for(auto & processName : processNames)
			{
				int  targetProcessIndex = 0;
				// get index for target process
				targetProcessIndex = this->getTargetProcessIndex(processName);
				if(targetProcessIndex >= 0)
				{
					this->sortedProcessStructVector.push_back(this->unsortedProcessStructVector[targetProcessIndex]);
					this->unsortedProcessStructVector.erase(this->unsortedProcessStructVector.begin() + targetProcessIndex);
				}
				else
				{
					status = -1;
					break;
				}
#if(dbg >= 2)

				{
					cout << "targetProcessIndex: " << targetProcessIndex << endl;
					cout << "unsorted processes: " << endl;
					for(auto & unsortedProcess : unsortedProcessStructVector)
					{
						cout << unsortedProcess.name << endl;
					}

					cout << "sorted processes: " << endl;
					for(auto & sortedProcess : sortedProcessStructVector)
					{
						cout << sortedProcess.name << endl;
					}

				}

#endif
			}
		}
		catch(std::exception &e)
		{
			 cout << "exception in " << __func__ << ": " << e.what() <<  endl;
			status = -1;
		}

#if(dbg >= 1)
		 cout << "***** EXITING: ComboDataInt::" << __func__ << ": " << status << endl;
#endif
		return status;
	}


#define dbg 0
	bool ComboDataInt::isUnsortedProcessStructVectorEmpty()
	{
		bool isListEmpty;

#if(dbg >= 1)
		 cout << "***** ENTERING: ComboDataInt::" << __func__ << endl;
#endif

		if(this->unsortedProcessStructVector.size() == 0) isListEmpty = true;
		else isListEmpty = false;
#if(dbg >= 2)
		 cout << "unsequenced processes: " << endl;
		this->printUnsequencedProcessList();
#endif

#if(dbg >= 1)
		 cout << "***** EXITING: ComboDataInt::" << __func__ << ": " << isListEmpty << endl;
#endif
		return isListEmpty;
	}

#define dbg 0
	bool ComboDataInt::isOutputInDataReadyVector(Connector output)
	{
		bool inList = false;
#if(dbg >= 1)
		 cout << "***** ENTERING: ComboDataInt::" << __func__ << endl;
		 cout << "Connector process: " << output.parentObjectName << "\t connector: " << output.connectorName << endl;
#endif
			try
			{
				for(auto & dataReadyConn : this->dataReadyVector)
				{
					string listedOutputProcess = dataReadyConn.parentObjectName;
					string listedOutputConnector = dataReadyConn.connectorName;
					if(output.parentObjectName.compare(listedOutputProcess) == 0 && output.connectorName.compare(listedOutputConnector) == 0)
					{
						inList = true;
						break;
					}
				}

			}
			catch(exception &e)
			{
				cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() << endl;
			}


#if(dbg >= 1)
		 cout << "***** EXITING: ComboDataInt::" << __func__ << ": " << inList << endl;
#endif

		return inList;
	}


#define dbg 0
	ProcessSignalConnection ComboDataInt::mergeConnections(ProcessSignalConnection srcConn, ProcessSignalConnection destConn)
	{
		ProcessSignalConnection mergedConnection;
#if(dbg >= 1)

		{
			cout << "ComboDataInt::" << __func__ << endl;
			cout << "srcConn: " << srcConn.srcProcessOutputConnector.parentObjectName << ":" << srcConn.srcProcessOutputConnector.connectorName << ">"
							<< srcConn.destProcessInputConnector.parentObjectName << ":" << srcConn.destProcessInputConnector.connectorName << " & ";
			cout << "destConn: " << destConn.srcProcessOutputConnector.parentObjectName << ":" << destConn.srcProcessOutputConnector.connectorName << ">"
							<< destConn.destProcessInputConnector.parentObjectName << ":" << destConn.destProcessInputConnector.connectorName << endl;
		}
#endif

		mergedConnection.srcProcessOutputConnector.parentObjectName = srcConn.srcProcessOutputConnector.parentObjectName;
		mergedConnection.srcProcessOutputConnector.connectorName = srcConn.srcProcessOutputConnector.connectorName;

		mergedConnection.destProcessInputConnector.parentObjectName = destConn.destProcessInputConnector.parentObjectName;
		mergedConnection.destProcessInputConnector.connectorName = destConn.destProcessInputConnector.connectorName;

#if(dbg >= 1)

		{
			cout << "***** EXITING: ComboDataInt::" << __func__ << ": ";
			cout << "mergeConnection: " << mergedConnection.srcProcessOutputConnector.parentObjectName << ":" << mergedConnection.srcProcessOutputConnector.connectorName << ">"
							<< mergedConnection.destProcessInputConnector.parentObjectName << ":" << mergedConnection.destProcessInputConnector.connectorName << endl;
		}
#endif
		return mergedConnection;
	}

#define dbg 0
	bool ComboDataInt::compareConnectors(bool display, Connector conn1, Connector conn2)
	{
		bool result = (conn1.parentObjectName.compare(conn2.parentObjectName) == 0 && conn1.connectorName.compare(conn2.connectorName) == 0);
		if(display)
		{
			cout << "comparing: " << conn1.parentObjectName << ":" << conn1.connectorName << " with ";
			cout << conn2.parentObjectName << ":" << conn2.connectorName << "\t\tresult: " << result << endl;
		}
		return result;
	}



	/****************** Sort unsorted struct-vectors into sorted struct-vectors***********************************/

#define dbg 0
	int ComboDataInt::sortUnsortedProcessStructsIntoSortedProcessStructVector()
	{
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::" << __func__ << endl;
#endif
		int status = 0;
		int  breakLoopCount = 0;
		bool sequencingStart = true;
		string process;
		vector<string> processes;
		string effectString;
		string currentEffectString;

		string procString;
		string paramString;

		try
		{
#if(dbg>=2)
			 cout << "loadUnsortedProcessStructVectorFromComboJsonFileStruct" << endl;
#endif
			this->dataReadyVector.clear();

			Connector tempConn;
			tempConn.parentObjectName = string("system");
			tempConn.connectorName = string("capture_1");
			this->dataReadyVector.push_back(tempConn);
			tempConn.parentObjectName = string("system");
			tempConn.connectorName = string("capture_2");
			this->dataReadyVector.push_back(tempConn);
			this->loadUnsortedProcessStructVectorFromComboJsonFileStruct();
		}
		catch(std::exception &e)
		{
			 cout << "exception in fillUnsequencedProcessVector section: " << e.what() <<  endl;
			status = -1;
		}


		this->sortedProcessStructVector.clear();
		try
		{
			breakLoopCount = 0;
			while(this->isUnsortedProcessStructVectorEmpty() == false)
			{
				if(sequencingStart)
				{
					processes = this->getFirstProcesses();
					if(processes.empty() == true)
					{
						status = -1;
						 cout << "first process string vector is blank." << endl;
						break;
					}
					else
					{
						sequencingStart = false;
					}
				}
				else
				{
					processes = this->getNextProcesses();
					if(processes.empty() == true)
					{
						status = -1;
						 cout << "next process string vector is empty." << endl;
						break;
					}
				}
				if(this->addOutputConnectionsToDataReadyVector(processes) != 0)
				{
					status = -1;
					 cout << "addOutputConnectionsToDataReadyVector failed." << endl;
					break;
				}

				if(this->transferProcessStructsToSortedProcessStructVector(processes) != 0)
				{
					status = -1;
					break;
				}

				if(breakLoopCount++ >= 100)
				{
					status = -1;
					 cout << "Couldn't sort processes." << endl;
					break;
				}
			}

			if(status == 0)
			{
				this->processCount = this->sortedProcessStructVector.size();
			}

#if(dbg >= 2)
			this->printProcessList(true, this->sortedProcessStructVector);

#endif
		}
		catch(std::exception &e)
		{
			 cout << "exception in " << __func__ << ": " << e.what() <<  endl;
			return -1;
		}

		// Create IndexedProcessParameter vector here
#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::" << __func__ << ": " << status << endl;
#endif

		return status;
	}


#define dbg 0
	int ComboDataInt::sortUnsortedConnectionStructsIntoSortedConnectionStructVector()
	{
		int status = 0;
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::" << __func__ << endl;
#endif

		try
		{
			while(this->unsortedProcessConnectionStructVector.empty() == false)
			{
				int dataReadyIndex = 0;
				for(auto & dataReadyConn : this->dataReadyVector)
				{
					int connIndex = 0;
					for(auto connection = this->unsortedProcessConnectionStructVector.begin();
									connection <  this->unsortedProcessConnectionStructVector.end(); connection++)
					{
						if(this->compareConnectors(false, dataReadyConn,connection->srcProcessOutputConnector) == true)
						{
							this->sortedProcessConnectionStructVector.push_back(*connection);
							this->unsortedProcessConnectionStructVector.erase(connection);
						}
						connIndex++;
					}
				}
				dataReadyIndex++;
			}
			this->comboStruct.processSignalBufferCount =
					this->sortedProcessConnectionStructVector.size();
#if(dbg >= 2)
	this->printProcessSignalConnectionList(true,this->sortedProcessConnectionStructVector);
#endif

		}
		catch(std::exception &e)
		{
			 cout << "exception in " << __func__ << ": " << e.what() <<  endl;
			status = -1;
		}
#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::" << __func__ << ": " << status << endl;
#endif
		return status;
	}

#define dbg 0
	int ComboDataInt::loadSortedControlStructVectorFromComboJsonFileStruct(void)
	{
		int status = 0;
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::" << __func__ << endl;
#endif
		int absControlIndex = 0;
		int effectIndex = 0;
		int controlEffectIndex = 0;
		this->sortedControlStructVector.clear();
		try
		{

			for(auto & effect : this->comboFileStruct.effectArray)
			{
				string effectName = effect.name;
				controlEffectIndex = 0;
				for(auto & processParamControl : effect.processParamControlVector)
				{
					processParamControl.controlSequenceIndex = absControlIndex;
					this->sortedControlStructVector.push_back(processParamControl);
					absControlIndex++;
				}
			}

			this->controlCount = this->sortedControlStructVector.size();
#if(dbg >= 2)
			this->printSortedControlList();
#endif
		}
		catch(exception &e)
		{
			 cout << "exception in " << __func__ << ": " << e.what() <<  endl;
			status = -1;
		}

#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::" << __func__ << ": " << status << endl;
#endif
		return status = 0;
	}


#define dbg 0
	int ComboDataInt::loadSortedControlConnectionStructVectorFromComboJsonFileStruct(void)
	{
		int status = 0;
#if(dbg >= 1)
		 cout << "***** ENTERING: ComboDataInt::" << __func__ << endl;
#endif
		string process;
		string effectString;
		string currentEffectString;

		string procString;
		string paramString;


		this->sortedControlConnectionStructVector.clear();
		try
		{
#if(dbg >= 1)
			 cout << "getting effects" << endl;
#endif
			for(auto & effect : this->comboFileStruct.effectArray)
			{
				for(auto & processParamControlConnection : effect.processParamControlConnectionVector)
				{
					this->sortedControlConnectionStructVector.push_back(processParamControlConnection);
				}
			}
			this->comboStruct.processParamControlBufferCount =
					this->sortedControlConnectionStructVector.size();
		}
		catch(exception &e)
		{
			 cout << "exception in " << __func__ << ": " << e.what() <<  endl;
			status = -1;
		}

#if(dbg >= 2)
		this->printControlConnectionList();
#endif

#if(dbg >= 1)
		 cout << "***** EXITING: ComboDataInt::" << __func__ << ": " << status << endl;
#endif
		return status;
	}


	/******************** Set interconnections ***********************************/

#define dbg 0
	int ComboDataInt::setProcBufferArrayOutputAndInputConnectorStructsUsingSortedConnectionStructVectorAndDataReadyVector()
	{
#if(dbg >= 1)
		 cout << "***** ENTERING: ComboDataInt::" << __func__ << endl;
#endif
		int status = 0;
		try
		{
	#if(dbg >= 3)
			this->printDataReadyList();
			this->printSortedConnectionList();
	#endif
			int procSigBufferIndex = 0;
			for(auto & dataReady : this->dataReadyVector)
			{
				this->comboStruct.processSignalBufferArray[procSigBufferIndex].srcProcessOutputConnector = dataReady;
				procSigBufferIndex++;
			}

			for(auto & procSignalBuffer : this->comboStruct.processSignalBufferArray)
			{

				for(auto & processConnection : this->sortedProcessConnectionStructVector)
				{
					if(compareConnectors(false, procSignalBuffer.srcProcessOutputConnector,
							processConnection.srcProcessOutputConnector) == true)
					{
	#if(dbg >= 4)
						cout << "match found: " <<  this->comboStruct.processSignalBufferArray[procSignalBufferIndex].srcProcessOutputConnector.parentObjectName;
						cout << ":" << this->comboStruct.processSignalBufferArray[procSignalBufferIndex].srcProcessOutputConnector.connectorName << endl;
						cout << "pushing " << this->sortedProcessConnectionStructVector[connIndex].destProcessInputConnector.parentObjectName;
						cout << ":" << this->sortedProcessConnectionStructVector[connIndex].destProcessInputConnector.connectorName;
						cout << " into procBufferArray[bufferIndex].destProcessInputConnectorVector" << endl;
	#endif
	#if(dbg >= 3)
						cout << "pushing " << processConnection.destProcessInputConnector.parentObjectName;
						cout << "into  procSignalBuffer.destProcessInputConnectorVector" << procSignalBuffer.srcProcessOutputConnector.parentObjectName  << endl;
	#endif
						procSignalBuffer.destProcessInputConnectorVector.push_back(processConnection.destProcessInputConnector);

					}
				}
			}

	#if(dbg >= 2)
			this->printProcBufferList();
	#endif

		}
		catch(exception &e)
		{
			cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() << endl;
		}


#if(dbg >= 1)
		 cout << "***** EXITING: ComboDataInt::" << __func__ << ": " << status << endl;
#endif

		return status;
	}


#define dbg 0
	int ComboDataInt::setConnectedBufferIndexesInSortedProcessStructProcessIOVectorsUsingProcBufferArray()
	{
#if(dbg >= 1)
		 cout << "***** ENTERING: ComboDataInt::" << __func__ << endl;
#endif
		int status = 0;
		// loop through sortedProcessStructVector
		try
		{
			Connector systemPlayback[2];
			systemPlayback[0].parentObjectName = "system";
			systemPlayback[1].parentObjectName = "system";
			systemPlayback[0].connectorName = "playback_1";
			systemPlayback[1].connectorName = "playback_2";
			Connector systemCapture[2];
			systemCapture[0].parentObjectName = "system";
			systemCapture[1].parentObjectName = "system";
			systemCapture[0].connectorName = "capture_1";
			systemCapture[1].connectorName = "capture_2";
			// ****************  Connect system capture outputs **************************
			int processSignalBufferIndex = 0;

			for(auto & processSignalBuffer : this->comboStruct.processSignalBufferArray)
			{
				if(this->compareConnectors(false, systemCapture[0],processSignalBuffer.srcProcessOutputConnector) == true)
				{
					this->inputSystemBufferIndex[0] = processSignalBufferIndex;
#if(dbg >= 2)
					cout << "system:capture_1 connected to buffer: " << this->inputSystemBufferIndex[0] << endl;
#endif
				}
				else if(this->compareConnectors(false, systemCapture[1],processSignalBuffer.srcProcessOutputConnector) == true)
				{
					this->inputSystemBufferIndex[1] = processSignalBufferIndex;
#if(dbg >= 2)
					cout << "system:capture_2 connected to buffer: " << this->inputSystemBufferIndex[1] << endl;
#endif
				}
				processSignalBufferIndex++;
			}

			for(auto & sortedProcess : this->sortedProcessStructVector)
			{
				processSignalBufferIndex = 0;
				for(auto & signalProcessBuffer : this->comboStruct.processSignalBufferArray)
				{
					if(signalProcessBuffer.srcProcessOutputConnector.parentObjectName == "") break;

					for(auto & procInput : sortedProcess.inputVector)
					{
						for(auto & bufferDestInConn : signalProcessBuffer.destProcessInputConnectorVector)
						{
							if(this->compareConnectors(false,procInput, bufferDestInConn) == true)
							{
								procInput.connectedBufferIndex = processSignalBufferIndex;
#if(dbg >= 2)
								cout << procInput.parentObjectName << ":" << procInput.connectorName;
								cout << " connected to buffer[" << processSignalBufferIndex << "]. "  << endl;
#endif
							}
						}
					}

					for(auto & procOutput : sortedProcess.outputVector)
					{
						if(this->compareConnectors(false, procOutput,
								signalProcessBuffer.srcProcessOutputConnector) == true)
						{
							procOutput.connectedBufferIndex = processSignalBufferIndex;
#if(dbg >= 2)
							cout << procOutput.parentObjectName << ":" << procOutput.connectorName;
							cout << " connected to buffer[" << processSignalBufferIndex << "]. "  << endl;
#endif
						}
					}
					processSignalBufferIndex++;
				}
			}

			// ****************  Connect system playback inputs ***********************************************
			processSignalBufferIndex = 0;
			for(auto & processSignalBuffer : this->comboStruct.processSignalBufferArray)
			{
				for(auto & bufferInConn : processSignalBuffer.destProcessInputConnectorVector)
				{
					if(this->compareConnectors(false, systemPlayback[0],bufferInConn) == true)
					{
						this->outputSystemBufferIndex[0] = processSignalBufferIndex;
	#if(dbg >= 2)
						cout << "system:playback_1 connected to buffer: " << this->outputSystemBufferIndex[0] << endl;
	#endif
					}
					else if(this->compareConnectors(false, systemPlayback[1],bufferInConn) == true)
					{
						this->outputSystemBufferIndex[1] = processSignalBufferIndex;
	#if(dbg >= 2)
						cout << "system:playback_2 connected to buffer: " << this->outputSystemBufferIndex[1] << endl;
	#endif
					}
				}
				processSignalBufferIndex++;
			}
		}
		catch(exception &e)
		{
			cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() << endl;
		}

#if(dbg >= 3)
		this->printProcessStructVector(true, this->sortedProcessStructVector);
#endif
#if(dbg >= 1)
		 cout << "***** EXITING: ComboDataInt::" << __func__ << ": " << status << endl;
#endif
		return status;
	}






#define dbg 0
	int ComboDataInt::setControlOutputAndProcessParameterControlBufferIndexesUsingParamContBufferArray()
	{
#if(dbg >= 1)
		 cout << "***** ENTERING: ComboDataInt::" << __func__ << endl;
#endif
		int status = 0;
		try
		{
			for(auto & procParamContBuffer : this->comboStruct.processParamControlBufferArray)
			{
				procParamContBuffer.destProcessParameterConnector.parentObjectName = "empty";
				procParamContBuffer.destProcessParameterConnector.connectorName = "empty";
				procParamContBuffer.destProcessParameterConnector.connectedBufferIndex = 59;
				procParamContBuffer.srcControlOutputConnector.parentObjectName = "empty";
				procParamContBuffer.srcControlOutputConnector.connectorName = "empty";
				procParamContBuffer.srcControlOutputConnector.connectedBufferIndex = 59;
			}

			// First set dest side Connectors to include all parameters for all process
			// and set sortedProcessStruct:param:paramContBufferIndex  to paramContBufferIndex

			int processParamControlBufferIndex = 0;

			for(auto & sortedProcess : this->sortedProcessStructVector)
			{
				for(auto & parameter : sortedProcess.paramVector)
				{
					this->comboStruct.processParamControlBufferArray[processParamControlBufferIndex].destProcessParameterConnector.parentObjectName =
									sortedProcess.processName;
					this->comboStruct.processParamControlBufferArray[processParamControlBufferIndex].destProcessParameterConnector.connectorName =
									parameter.paramConnector.connectorName;
					this->comboStruct.processParamControlBufferArray[processParamControlBufferIndex].destProcessParameterConnector.connectedBufferIndex =
									processParamControlBufferIndex;
					parameter.paramConnector.connectedBufferIndex =
									processParamControlBufferIndex;

					this->comboStruct.processIndexMap[sortedProcess.processName].paramIndexMap[parameter.paramConnector.connectorName].processParamControlBufferIndex =
									processParamControlBufferIndex;
					processParamControlBufferIndex++;
				}
			}

			this->comboStruct.processParamControlBufferCount = processParamControlBufferIndex;


			processParamControlBufferIndex = 0; // reset buffer index

			for(auto & processParamControlConnection : this->comboStruct.processParamControlBufferArray)
			{
				for(auto & sortedControlConnection : this->sortedControlConnectionStructVector)
				{
					if(this->compareConnectors(false, processParamControlConnection.destProcessParameterConnector
											   ,sortedControlConnection.destProcessParameterConnector) == true)
					{
						processParamControlConnection.srcControlOutputConnector = sortedControlConnection.srcControlOutputConnector;
						int controlIndex = 0;
						for(auto & sortedControlStruct : this->sortedControlStructVector)
						{
							Connector controlOut, controlOutInv;
							controlOut.parentObjectName = sortedControlStruct.name;
							controlOut.connectorName = sortedControlStruct.output.connectorName;
							controlOutInv.parentObjectName = sortedControlStruct.name;
							controlOutInv.connectorName = sortedControlStruct.outputInv.connectorName;
							if(this->compareConnectors(false, controlOut,processParamControlConnection.srcControlOutputConnector) == true)
							{
								Connector outputDest = processParamControlConnection.destProcessParameterConnector;
								outputDest.connectedBufferIndex = processParamControlBufferIndex;
								sortedControlStruct.targetProcessParamVector.push_back(outputDest);
								processParamControlConnection.srcControlOutputConnector.connectedBufferIndex = processParamControlBufferIndex;
	#if(dbg >= 2)
								cout << "this->sortedControlStructVector[" << controlIndex <<"].targetProcessParamVector.connectedBufferIndex: " << outputDest.parentObjectName << ":" << outputDest.connectorName << "=" << outputDest.connectedBufferIndex << endl;
	#endif
							}
							if(this->compareConnectors(false, controlOutInv,processParamControlConnection.srcControlOutputConnector) == true)
							{
								Connector outputInvDest = processParamControlConnection.destProcessParameterConnector;
								outputInvDest.connectedBufferIndex = processParamControlBufferIndex;
								sortedControlStruct.targetProcessParamInvVector.push_back(outputInvDest);
								processParamControlConnection.srcControlOutputConnector.connectedBufferIndex = processParamControlBufferIndex;
	#if(dbg >= 2)
								cout << "sortedControlStructVector[" << controlIndex <<"].targetProcessParamVector.connectedBufferIndex: " << outputInvDest.parentObjectName << ":" << outputInvDest.connectorName << "=" << outputInvDest.connectedBufferIndex << endl;
	#endif
							}
							controlIndex++;
						}
					}
				}
				processParamControlBufferIndex++;
			}


	#if(dbg >= 2)
			this->debugPrintParamContBufferListWithConnections();
			this->printContBufferList();
			this->printControlStructVector(true,this->sortedControlStructVector);
	#endif
		}
		catch(exception &e)
		{
			cout << "exception in ComboDataInt" << __func__ << ": " << e.what() << endl;
		}

#if(dbg >= 1)
		 cout << "***** EXITING: ComboDataInt::" << __func__ << ": " << status << endl;
#endif

		return status;
	}


	/******************* Set other data ****************************************/

	int ComboDataInt::setProcessAndControlTypeIntsInSortedStructVectors()
	{
		int status = 0;
#if(dbg >= 1)
		 cout << "***** ENTERING: ComboDataInt::" << __func__ << endl;
#endif
		try
		{
			int processIndex = 0;
			for(auto & sortedProcess : this->sortedProcessStructVector)
			{
				this->sortedProcessStructVector[processIndex].processSequenceIndex = processIndex;
				this->comboStruct.processIndexMap[sortedProcess.processName].processSequenceIndex =
						processIndex;
				processIndex++;
			}


			for(auto & sortedControlStruct : this->sortedControlStructVector)
			{
				if(sortedControlStruct.conType.compare("norm") == 0)
				{
					sortedControlStruct.conTypeInt = 0;
				}
				else if(sortedControlStruct.conType.compare("env") == 0)
				{
					sortedControlStruct.conTypeInt = 1;
				}
				else if(sortedControlStruct.conType.compare("lfo") == 0)
				{
					sortedControlStruct.conTypeInt = 2;
				}

			}
		}
		catch(exception &e)
		{
			cout << "exception in : " << e.what() << endl;
			status = -1;
		}

#if(dbg >= 1)
		 cout << "***** EXITING: ComboDataInt::" << __func__ << ": " << status << endl;
#endif

		return status;
	}






#define dbg 0
	int  ComboDataInt::loadProcessIndexMapFromSortedProcessVector()
	{

#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::" << __func__ << endl;
#endif
		int status = 0;
		string causeOfException;

		try
		{
			for(auto & process : this->sortedProcessStructVector)
			{
				ProcessIndexing procIndexing;

				procIndexing.processName = process.processName;
				procIndexing.processSequenceIndex = process.processSequenceIndex;
				procIndexing.processTypeInt = process.processTypeInt;
				procIndexing.parentEffect = process.parentEffect;


				for(auto & parameter : process.paramVector)
				{
					causeOfException = "parameter index";
					ProcessParameterIndexing procParamIndexing;
					//pair<map<string,ProcessParameterIndexing>::iterator,bool> result;
					procParamIndexing.paramName = parameter.paramConnector.connectorName;
					procParamIndexing.paramIndex = parameter.paramConnector.connectorIndex;
					procParamIndexing.parentProcess = process.processName;
					procParamIndexing.processParamControlBufferIndex =
							parameter.paramConnector.connectedBufferIndex;

					procIndexing.paramIndexMap.insert(pair<string,ProcessParameterIndexing>(procParamIndexing.paramName, procParamIndexing) );
				}
				this->comboStruct.processIndexMap.insert(pair<string,ProcessIndexing>(procIndexing.processName,procIndexing));

			}
		}
		catch(exception &e)
		{
			 cout << "exception in ComboDataInt::" << __func__ << ": " << causeOfException << ":" << e.what() <<  endl;
			status = -1;
		}

#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::" << __func__ << ": " << status << endl;
#endif

		 return status;
	}


#define dbg 0
	int  ComboDataInt::loadControlIndexMapFromSortedControlVector()
	{
		map<string, ControlIndexing> controlIndexMap;
		int status = 0;
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::" << __func__ << endl;
#endif

		try
		{
			for(auto & control : this->sortedControlStructVector)
			{
				ControlIndexing contIndexing;

				contIndexing.controlName = control.name;
				contIndexing.controlIndex = control.controlSequenceIndex;
				contIndexing.parentEffect = control.parentEffect;
				contIndexing.controlTypeInt = control.conTypeInt;

				map<string,ProcessControlParameterIndexing> paramIndexMap;

				for(auto & controlParameter : control.paramVector)
				{

					ProcessControlParameterIndexing contParamIndexing;
					contParamIndexing.contParamName = controlParameter.alias;
					contParamIndexing.contParamIndex = controlParameter.index;
					contParamIndexing.parentControl = contIndexing.controlName;
					contIndexing.paramIndexMap.insert(pair<string,ProcessControlParameterIndexing>
					(contParamIndexing.contParamName,contParamIndexing));
				}
				this->comboStruct.controlIndexMap.insert(pair<string,ControlIndexing>(contIndexing.controlName,contIndexing));
			}
		}
		catch(exception &e)
		{
			 cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() <<  endl;
			status = -1;
		}


#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::" << __func__ << ": " << status << endl;
#endif
		 return status;
	}


#define dbg 0
	int ComboDataInt::loadIndexMappedComboDataIntoComboStructFromSortedVectors(void)
	{
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::" << __func__ << endl;
#endif
		int status = 0;

		try
		{
			this->loadProcessIndexMapFromSortedProcessVector();
			this->loadControlIndexMapFromSortedControlVector();
		}
		catch(exception &e)
		{
			 cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() <<  endl;
			status = -1;
		}
#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::" << __func__ << ": " << status << endl;
#endif

		return status;
	}




#define dbg 3
	int ComboDataInt::setPedalUi(void)
	{
		int status = 0;
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::" << __func__ << endl;
#endif

		string effectString;
		string currentEffectString;
		string controlString;

		vector<Control>::size_type controlCount = this->comboStruct.controlCount;
		string paramString;
		vector<ControlParameter>::size_type  paramIndex = 0;
		int effectParamArrayIndex = 0;

		int absParamArrayIndex = 0;

		try
		{
			if(this->effectComboJson.isNull() == false)
			{
				this->pedalUiJson = Json::Value(); // clear any previous data

				this->pedalUiJson["title" ] = this->comboFileStruct.name;
	#if(dbg>=2)
				 cout << "title: " << this->pedalUiJson["title"].asString() << endl;
	#endif

				int effectIndex = 0;
				int currentEffectIndex = 0;
				effectParamArrayIndex = 0;

#if(dbg >= 2)
				cout << "control count: " << this->comboStruct.controlCount << endl;
#endif
				for(int i = 0; i < this->comboStruct.controlCount; i++)
				{
					ControlObjectData control = this->comboStruct.controlSequenceData[i];
					int effectIndex = control.parentEffectIndex;

					if(currentEffectIndex != effectIndex)
					{
						effectParamArrayIndex = 0;
						currentEffectIndex = effectIndex;
					}
					this->pedalUiJson["effects"][effectIndex]["abbr"] =
							control.parentEffectAbbr;
					this->pedalUiJson["effects"][effectIndex]["name"] =
							control.parentEffectName;
#if(dbg>=2)
				 cout << "\teffect: " << this->pedalUiJson["effects"][effectIndex]["abbr"].asString();
				 cout << "\t" << this->pedalUiJson["effects"][effectIndex]["name"].asString() << endl;
#endif
					for(int j = 0; j < control.parameterCount; j++)
					{
						ControlObjectParameter parameter = control.parameterArray[j];
							this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["name"] =
											parameter.parameterName;
							this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["abbr"] =
											parameter.parameterAbbr;
							this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["value"] =
											parameter.value;
							this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["parentControl"] =
											control.controlName;
							this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["paramType"] =
											parameter.paramType;
#if(dbg>=2)
							cout << "\t\t" << this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["name"].asString() << ":";
							cout << "\t\t" << this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["abbr"].asString() << ":";
							cout << "\t\t" << this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["value"].asInt() << ":";
							cout << "\t\t" << this->pedalUiJson["effects"][effectIndex]["params"][effectParamArrayIndex]["parentControl"].asString() << endl;
#endif
							effectParamArrayIndex++;
					}
				}

	#if(dbg>=2)
				 cout << "param count: " << this->pedalUiJson["effects"][0]["params"].size() << endl;
	#endif
				status = 0;

			}
			else
			{
				status = -1;
				cout << "effectComboJson is NULL" << endl;
			}
		}
		catch(exception &e)
		{
			cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() << endl;
		}

#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::" << __func__ << ": " << status << endl;
#endif
#if(dbg >= 2)
		 cout << getCompactedJSONData(this->pedalUiJson) << endl;
#endif
		return status;
	}


//******************* ComboStruct Sub-Functions ******************

/****************************** ComboStructProcess Set******************************************/

#define dbg 0
void ComboDataInt::setComboStructProcessInputBufferIndexes(int sequenceIndex, Process process)
{
	int j = 0;
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::" << __func__ << ": " << process.processName << endl;
#endif



		try
		{
			for(auto & input : process.inputVector)
			{
				this->comboStruct.processSequenceData[sequenceIndex].inputConnectedBufferIndexArray[j] =
						input.connectedBufferIndex;

		#if(dbg >= 2)
				 cout << "this->comboStruct.processSequenceData[" << sequenceIndex << "].inputConnectedBufferIndexArray[" << j << "]: " << this->comboStruct.processSequenceData[sequenceIndex].inputConnectedBufferIndexArray[j] << endl;
		#endif
				j++;
			}
		}
		catch(exception &e)
		{
			cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() << endl;

		}
#if(dbg >= 1)
	cout << "***** EXITING ComboDataInt::" << __func__ << ": " << endl;
#endif
}

void ComboDataInt::setComboStructProcessOutputBufferIndexes(int sequenceIndex, Process process)
{
	int j = 0;
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::" << __func__ << ": " << process.processName << endl;
#endif

	try
	{
		for(auto & output : process.outputVector)
		{

			this->comboStruct.processSequenceData[sequenceIndex].outputConnectedBufferIndexArray[j] =
					output.connectedBufferIndex;
	#if(dbg >= 2)
			 cout << "this->comboStruct.processSequenceData[" << sequenceIndex << "].outputProcessSignalBufferArray[" << j << "]: " << this->comboStruct.processSequenceData[sequenceIndex].outputConnectedBufferIndexArray[j] << endl;
	#endif
			j++;
		}
	}
	catch(exception &e)
	{
		cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() << endl;

	}

#if(dbg >= 1)
	cout << "***** EXITING ComboDataInt::" << __func__ << endl;
#endif
}

#define dbg 0
void ComboDataInt::setComboStructProcessParameters(int sequenceIndex, Process process)
{
	int j = 0;
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::" << __func__ << ": " << process.processName << endl;
#endif

		try
		{
			for(auto & parameter : process.paramVector)
			{
				this->comboStruct.processSequenceData[sequenceIndex].parameterArray[j].internalIndexValue =
						parameter.valueIndex;
				this->comboStruct.processSequenceData[sequenceIndex].parameterArray[j].paramContBufferIndex =
						parameter.paramConnector.connectedBufferIndex;
				this->comboStruct.processSequenceData[sequenceIndex].parameterArray[j].parameterName =
						parameter.paramConnector.connectorName;
				this->comboStruct.processSequenceData[sequenceIndex].parameterArray[j].internalIndexValue =
						parameter.valueIndex;
				this->comboStruct.processSequenceData[sequenceIndex].parameterArray[j].index =
						parameter.paramConnector.connectorIndex;

				if(parameter.paramControlType.compare("none") != 0) //parameter is controlled
				{
					this->comboStruct.processSequenceData[sequenceIndex].parameterArray[j].controlConnected = true;
				}
				else this->comboStruct.processSequenceData[sequenceIndex].parameterArray[j].controlConnected = false;
		#if(dbg >= 1)
				cout << "\t\tparameterName: " << this->comboStruct.processSequenceData[sequenceIndex].parameterArray[j].parameterName << endl;
				cout << "\t\tvalue: " << this->comboStruct.processSequenceData[sequenceIndex].parameterArray[j].internalIndexValue << endl;
		#endif
				j++;
			}
			for(; j < 10; j++)
			{
				this->comboStruct.processSequenceData[sequenceIndex].parameterArray[j].internalIndexValue = 0;
				this->comboStruct.processSequenceData[sequenceIndex].parameterArray[j].controlConnected = false;
				j++;
			}
		}
		catch(exception &e)
		{
			cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() << endl;

		}

#if(dbg >= 1)
	cout << "***** EXITING ComboDataInt::" << __func__ << endl;
#endif

}

void ComboDataInt::setComboStructProcessDataObject(int sequenceIndex, Process process)
{
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::" << __func__ << ": " << process.processName << endl;
#endif

	try
	{
		this->comboStruct.processSequenceData[sequenceIndex].processName = process.processName;
		this->comboStruct.processSequenceData[sequenceIndex].processTypeInt = process.processTypeInt;
		this->comboStruct.processSequenceData[sequenceIndex].processEffectIndex = process.processEffectIndex;
		this->comboStruct.processSequenceData[sequenceIndex].parentEffectIndex = process.parentEffectIndex;

		this->comboStruct.processSequenceData[sequenceIndex].footswitchNumber = process.footswitchNumber;
		this->comboStruct.processSequenceData[sequenceIndex].processSequenceIndex = process.processSequenceIndex;
	#if(dbg >= 1)
		 cout << "\tprocessName: " << this->comboStruct.processSequenceData[sequenceIndex].processName << endl;
	#endif

		//***************** Get input count and store values **************
		 this->comboStruct.processSequenceData[sequenceIndex].processInputCount = process.inputCount;
		this->setComboStructProcessInputBufferIndexes(sequenceIndex,process);
		//***************** Get output count and store values  **************
		this->comboStruct.processSequenceData[sequenceIndex].processOutputCount = process.outputCount;
		this->setComboStructProcessOutputBufferIndexes(sequenceIndex,process);
		//***************** Get parameter count and store values **************
		this->comboStruct.processSequenceData[sequenceIndex].parameterCount = process.paramCount;
		this->setComboStructProcessParameters(sequenceIndex,process);

		this->comboStruct.processSequenceData[sequenceIndex].bufferSize = 256;
		this->comboStruct.processSequenceData[sequenceIndex].inputCouplingMode = 1;
		this->comboStruct.processSequenceData[sequenceIndex].antiAliasingNumber = 1;
		this->comboStruct.processSequenceData[sequenceIndex].waveshaperMode = 0;
	}
	catch(exception &e)
	{
		cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() << endl;

	}

#if(dbg >= 1)
	cout << "***** EXITING ComboDataInt::" << __func__ << endl;
#endif

}


/****************************** ComboStructControl Set******************************************/


void ComboDataInt::setComboStructControlOutputs(int sequenceIndex, Control control)
{
	int j = 0;
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::" << __func__ << ": " << control.name << endl;
#endif


	try
	{
		for(auto & outputTarget : control.targetProcessParamVector)
		{
			this->comboStruct.controlSequenceData[sequenceIndex].outputToParamControlBufferIndex[j] =
					outputTarget.connectedBufferIndex;
	#if(dbg >= 2)
			 cout << "this->comboStruct.controlSequenceData[" << sequenceIndex << "].outputToParamControlBufferIndex: " << this->comboStruct.controlSequenceData[sequenceIndex].outputToParamControlBufferIndex[j] << endl;
	#endif
			j++;
		}
	}
	catch(exception &e)
	{
		cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() << endl;

	}
#if(dbg >= 1)
	cout << "***** EXITING ComboDataInt::" << __func__ << endl;
#endif

}


void ComboDataInt::setComboStructControlOutputInvs(int sequenceIndex, Control control)
{
	int j = 0;
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::" << __func__ << ": " << control.name << endl;
#endif



	try
	{
		for(auto & outputInvTarget : control.targetProcessParamInvVector)
		{
			this->comboStruct.controlSequenceData[sequenceIndex].outputInvToParamControlBufferIndex[j] =
					outputInvTarget.connectedBufferIndex;
	#if(dbg >= 2)
			 cout << "this->comboStruct.controlSequenceData[" << sequenceIndex << "].outputInvToParamControlBufferIndex: " << this->comboStruct.controlSequenceData[sequenceIndex].outputInvToParamControlBufferIndex[j] << endl;
	#endif
			j++;
		}
	}
	catch(exception &e)
	{
		cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() << endl;

	}
#if(dbg >= 1)
	cout << "***** EXITING ComboDataInt::" << __func__ << endl;
#endif
}


void ComboDataInt::setComboStructControlParameters(int sequenceIndex, Control control)
{
	int j = 0;
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::" << __func__ << ": " << control.name << endl;
#endif



	try
	{

		for(auto & parameter : control.paramVector)
		{
			this->comboStruct.controlSequenceData[sequenceIndex].parameterArray[j].parameterName = parameter.alias;
			this->comboStruct.controlSequenceData[sequenceIndex].parameterArray[j].parameterAbbr = parameter.abbr;
			this->comboStruct.controlSequenceData[sequenceIndex].parameterArray[j].value = parameter.valueIndex;
			this->comboStruct.controlSequenceData[sequenceIndex].parameterArray[j].cvEnabled = parameter.cvEnabled;
			this->comboStruct.controlSequenceData[sequenceIndex].parameterArray[j].index = parameter.index;

			if(parameter.inheritControlledParamType)
			{
				this->comboStruct.controlSequenceData[sequenceIndex].parameterArray[j].paramType =
						parameter.controlledParamType;
			}
			else
			{
				this->comboStruct.controlSequenceData[sequenceIndex].parameterArray[j].paramType =
						parameter.paramType;
			}
	#if(dbg >= 1)
			{
				cout << "\t\tparameterName: " << this->comboStruct.controlSequenceData[sequenceIndex].
						parameterArray[j].parameterName << endl;
				cout << "\t\tvalue: " << this->comboStruct.controlSequenceData[sequenceIndex].
						parameterArray[j].value << endl;
			}
	#endif
			j++;
		}
		for(; j < 10; j++)
		{
			this->comboStruct.controlSequenceData[sequenceIndex].parameterArray[j].value = 0;
			this->comboStruct.controlSequenceData[sequenceIndex].parameterArray[j].cvEnabled = false;
		}
	}
	catch(exception &e)
	{
		cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() << endl;

	}

#if(dbg >= 1)
	cout << "***** EXITING ComboDataInt::" << __func__ << endl;
#endif
}


void ComboDataInt::setComboStructControlDataObject(int sequenceIndex, Control control)
{
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::" << __func__ << ": " << control.name << endl;
#endif

	try
	{
		this->comboStruct.controlSequenceData[sequenceIndex].controlName = control.name;
		this->comboStruct.controlSequenceData[sequenceIndex].controlTypeInt = control.conTypeInt;
		this->comboStruct.controlSequenceData[sequenceIndex].controlEffectIndex = control.controlEffectIndex;
		this->comboStruct.controlSequenceData[sequenceIndex].parentEffectIndex = control.parentEffectIndex;
		this->comboStruct.controlSequenceData[sequenceIndex].parentEffectName = control.parentEffect;
		this->comboStruct.controlSequenceData[sequenceIndex].parentEffectAbbr = control.parentEffectAbbr;
		//***************** Get output count and store values **************
		this->comboStruct.controlSequenceData[sequenceIndex].outputConnectionCount =
				control.targetProcessParamVector.size();
		this->setComboStructControlOutputs(sequenceIndex,control);
		//***************** Get outputInv count and store values  **************
		this->comboStruct.controlSequenceData[sequenceIndex].outputInvConnectionCount =
				control.targetProcessParamInvVector.size();
		this->setComboStructControlOutputInvs(sequenceIndex,control);
		//***************** Get parameter count and store values **************
		this->comboStruct.controlSequenceData[sequenceIndex].parameterCount =
				control.paramVector.size();
		this->setComboStructControlParameters(sequenceIndex,control);
	}
	catch(exception &e)
	{
		cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() << endl;

	}

#if(dbg >= 1)
	cout << "***** EXITING ComboDataInt::" << __func__ << endl;
#endif

}



void ComboDataInt::setComboStructProcessParameterControlBufferArray(void)
{
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::" << __func__ << endl;
#endif


	int controlBufferIndex = 0;

	try
	{
		for(int i = 0; i < this->comboStruct.processCount; i++)
		{
			int paramCount = this->comboStruct.processSequenceData[i].parameterCount;
			string procName = this->comboStruct.processSequenceData[i].processName;
			for(int j = 0; j < paramCount; j++)
			{
				ProcessObjectParameter procParam = this->comboStruct.processSequenceData[i].parameterArray[j];

				this->comboStruct.processParamControlBufferArray[controlBufferIndex].
				destProcessParameterConnector.parentObjectName = procName;

				this->comboStruct.processParamControlBufferArray[controlBufferIndex].
				destProcessParameterConnector.connectorName = procParam.parameterName;

				this->comboStruct.processParamControlBufferArray[controlBufferIndex].parameterValueIndex =
						procParam.internalIndexValue;

			}
		}
	}
	catch(exception &e)
	{
		cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() << endl;

	}
#if(dbg >= 1)
	cout << "***** EXITING ComboDataInt::" << __func__ << endl;
#endif
}

void ComboDataInt::setComboStructProcessSignalBufferArray(void)
{
#if(dbg >= 1)
	cout << "*****ENTERING ComboDataInt::" << __func__ << endl;
#endif



	int i = 0;
	try
	{
		for(auto & sortedProcess : this->sortedProcessStructVector)
		{
			for(auto & output : sortedProcess.inputVector)
			{
				this->comboStruct.processSignalBufferArray[i].srcProcessOutputConnector = output;
				i++;
			}
		}
	}
	catch(exception &e)
	{
		cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() << endl;

	}

#if(dbg >= 1)
	cout << "***** EXITING ComboDataInt::" << __func__ << endl;
#endif

}





	//******************* Updates when saving Combo ******************


#define dbg 0
	int  ComboDataInt::loadComboStructDataBackIntoEffectComboJson()
	{
		int status = 0;
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::" << __func__ << endl;
#endif

		try
		{
#if(dbg >= 2)
					 cout << "Setting Processes: " << endl;
#endif
			for(int i = 0; i < this->comboStruct.processCount; i++)
			{
#if(dbg >= 2)
					 cout << "\tprocess: " << this->comboStruct.processSequenceData[i].processName << endl;
#endif
				for(int j = 0; j < this->comboStruct.processSequenceData[i].parameterCount; j++)
				{
					int effectIndex = this->comboStruct.processSequenceData[i].parentEffectIndex;
					int processIndex = this->comboStruct.processSequenceData[i].processEffectIndex;
					int paramIndex = this->comboStruct.processSequenceData[i].parameterArray[j].index;
					int value = this->comboStruct.processSequenceData[i].parameterArray[j].internalIndexValue;

					this->effectComboJson["effectArray"][effectIndex]["processArray"]\
						[processIndex]["paramArray"][paramIndex]["value"] = value;
#if(dbg >= 2)
					cout << "\t\t " << effectIndex << ":"<< processIndex << ":"<< paramIndex;
					 cout << "\tname: " << this->effectComboJson["effectArray"][effectIndex]\
							 ["processArray"][processIndex]["paramArray"][paramIndex]["name"];

					 cout << "\tvalue: " << this->effectComboJson["effectArray"][effectIndex]\
							 ["processArray"][processIndex]["paramArray"][paramIndex]["value"] << endl;
#endif
				}
			}

#if(dbg >= 2)
					 cout << "Setting Controls: " << endl;
#endif
			for(int i = 0; i < this->comboStruct.controlCount; i++)
			{
#if(dbg >= 2)
					 cout << "\tcontrol: " << this->comboStruct.controlSequenceData[i].controlName << endl;
#endif
				for(int j = 0; j < this->comboStruct.controlSequenceData[i].parameterCount; j++)
				{
					int effectIndex = this->comboStruct.controlSequenceData[i].parentEffectIndex;
					int controlIndex = this->comboStruct.controlSequenceData[i].controlEffectIndex;
					int paramIndex = this->comboStruct.controlSequenceData[i].parameterArray[j].index;
					int value = this->comboStruct.controlSequenceData[i].parameterArray[j].value;

					this->effectComboJson["effectArray"][effectIndex]["controlArray"]\
						[controlIndex]["conParamArray"][paramIndex]["value"] = value;
#if(dbg >= 2)
					 cout << "\t\t " << effectIndex << ":"<< controlIndex << ":"<< paramIndex;

					 cout << "\tname: " << this->effectComboJson["effectArray"][effectIndex]\
							 ["controlArray"][controlIndex]["conParamArray"][paramIndex]["alias"];
					 cout << "\tvalue: " << this->effectComboJson["effectArray"][effectIndex]\
							 ["controlArray"][controlIndex]["conParamArray"][paramIndex]["value"] << endl;
#endif
				}
			}

		}
		catch(exception &e)
		{
			cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() << endl;
			status = -1;
		}



#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::" << __func__ << ": " << status << endl;
#endif

		return status;
	}


	//***************************** PUBLIC *************************************

	void ComboDataInt::setProcessUtilityData(ProcessUtility procUtil)
	{
		this->processUtil.bufferSize = procUtil.bufferSize;
		this->processUtil.inputCouplingMode = procUtil.inputCouplingMode;
		this->processUtil.antiAliasingNumber = procUtil.antiAliasingNumber;
		this->processUtil.waveshaperMode = procUtil.waveshaperMode;
	}


	Json::Value ComboDataInt::getPedalUiJson(void)
	{
		return this->pedalUiJson;
	}

	string ComboDataInt::getName()
	{
		return this->effectComboJson["name"].asString();
	}


#define dbg 1
	void ComboDataInt::loadComboStruct()
	{
		int status = 0;
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::" << __func__ << endl;
		 cout << "comboName: " << this->comboName << endl;
#endif

		try
		{
			this->comboStruct.controlVoltageEnabled = false;
			this->comboStruct.processCount = this->processCount;
			this->comboStruct.controlCount = this->controlCount;

			this->comboStruct.name = this->comboName;
			 cout << this->comboStruct.name << ":" << this->comboStruct.processCount << ":";
			 cout << this->comboStruct.controlCount << ":" << this->comboStruct.processSignalBufferCount << endl;

			// set Processes
			int i = 0;
#if(dbg >= 1)
					 cout << "Setting Processes: " << endl;
#endif

			for(auto & process : this->sortedProcessStructVector)
			{

				this->setComboStructProcessDataObject(i,process);
				i++;
			}
			// set Controls
			i = 0;
#if(dbg >= 1)
					 cout << "Setting Controls: " << endl;
#endif
			for(auto & control : this->sortedControlStructVector)
			{
				this->setComboStructControlDataObject(i,control);
				i++;
			}

			i = 0;
			this->setComboStructProcessParameterControlBufferArray();

			this->comboStruct.inputSystemBufferIndex[0] = this->inputSystemBufferIndex[0];
#if(dbg >= 2)
			 cout << "this->comboStruct.inputSystemBufferIndex[0]: " << this->comboStruct.inputSystemBufferIndex[0] << endl;
#endif
			 this->comboStruct.inputSystemBufferIndex[1] = this->inputSystemBufferIndex[1];
#if(dbg >= 2)
			 cout << "this->comboStruct.inputSystemBufferIndex[1]: " << this->comboStruct.inputSystemBufferIndex[1] << endl;
#endif
			 this->comboStruct.outputSystemBufferIndex[0] = this->outputSystemBufferIndex[0];
#if(dbg >= 2)
			 cout << "this->comboStruct.outputSystemBufferIndex[0]: " << this->comboStruct.outputSystemBufferIndex[0] << endl;
#endif
			 this->comboStruct.outputSystemBufferIndex[1] = this->outputSystemBufferIndex[1];
#if(dbg >= 2)
			 cout << "this->comboStruct.outputSystemBufferIndex[1]: " << this->comboStruct.outputSystemBufferIndex[1] << endl;
#endif
		}
		catch(exception &e)
		{
			 cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() << endl;
			status = -1;

		}
#if(dbg >= 3)
		this->debugPrintParamContBufferListWithConnections();
		this->printIndexMappedComboData();
#endif
#if(dbg >= 1)
		 cout << "*****EXITING ComboDataInt::" << __func__ << endl;
#endif


	}

// This is used for saving combo settings to the ComboDataInt, so only parameter values need to be updated.
#define dbg 0
void ComboDataInt::setComboStructParamValues(ComboStruct combo)
{
	int status = 0;
	int i = 0;
	int j = 0;

#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::" << __func__ << endl;
		 cout << "comboName: " << combo.name << endl;
#endif

		try
		{
#if(dbg >= 2)
					 cout << "Setting Processes: " << endl;
#endif

			for(int i = 0; i < combo.processCount; i++)
			{

#if(dbg >= 2)
					 cout << "\tprocess: " << combo.processSequenceData[i].processName << endl;
					 cout << "\tparamCount: " << combo.processSequenceData[i].parameterCount << endl;
#endif

				//***************** Get parameter count and store values **************
				this->comboStruct.processSequenceData[i].parameterCount = combo.processSequenceData[i].parameterCount;

				for(j = 0; j < combo.processSequenceData[i].parameterCount; j++)
				{
					this->comboStruct.processSequenceData[i].parameterArray[j].internalIndexValue =
							combo.processSequenceData[i].parameterArray[j].internalIndexValue;
#if(dbg >= 2)
					 cout << "\tname: " << combo.processSequenceData[i].parameterArray[j].parameterName;
					 cout << "\tvalue: " << combo.processSequenceData[i].
							 parameterArray[j].internalIndexValue << endl;
#endif
				}

				for(; j < 10; j++)
				{
					this->comboStruct.processSequenceData[i].parameterArray[j].internalIndexValue = 0;
				}
			}
			// set Controls
			i = 0;

#if(dbg >= 2)
					 cout << "Setting Controls: " << endl;
#endif
			for(i = 0; i < combo.controlCount; i++)
			{
#if(dbg >= 2)
					 cout << "\tcontrol: " << combo.controlSequenceData[i].controlName << endl;
					 cout << "\tparamCount: " << combo.controlSequenceData[i].parameterCount << endl;
#endif
				this->comboStruct.controlSequenceData[i].parameterCount = combo.controlSequenceData[i].parameterCount;

				for(j = 0; j < combo.controlSequenceData[i].parameterCount; j++)
				{
					this->comboStruct.controlSequenceData[i].parameterArray[j].value =
							combo.controlSequenceData[i].parameterArray[j].value;
#if(dbg >= 2)
					 cout << "\tname: " << combo.controlSequenceData[i].parameterArray[j].parameterName;
					 cout << "\tvalue: " << combo.controlSequenceData[i].parameterArray[j].value << endl;
#endif
				}
				for(; j < 10; j++)
				{
					this->comboStruct.controlSequenceData[i].parameterArray[j].value = 0;
				}
			}

			// set parameterControl buffering
#if(dbg >= 3)
			 cout << "set parameterControl buffering: " << this->comboStruct.processParamControlBufferCount << endl;
#endif
			for(i = 0; i < combo.processParamControlBufferCount; i++)
			{
					this->comboStruct.processParamControlBufferArray[i].parameterValueIndex =
							combo.processParamControlBufferArray[i].parameterValueIndex;

			}


		}
		catch(exception &e)
		{
			 cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() << endl;
			status = -1;

		}
#if(dbg >= 3)
		this->debugPrintParamContBufferListWithConnections();
		this->printIndexMappedComboData();
#endif
#if(dbg >= 1)
		 cout << "*****EXITING ComboDataInt::" << __func__ << endl;
#endif

}


ComboStruct ComboDataInt::getComboStruct()
{
	return this->comboStruct;
}




	Json::Value ComboDataInt::getEffectComboJson()
	{
		return this->effectComboJson;
	}





#define dbg 0
	int ComboDataInt::loadIndexMappedComboData2(Json::Value comboJson)
	{
		int status = 0;
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::" << __func__ << endl;
#endif

#if(dbg >= 2)
		 cout << "comboName: " << comboJson["name"].asString() << endl;

#endif
			try
			{
				this->effectComboJson = comboJson;
				if(this->loadComboJsonFileStructFromEffectComboJson() >= 0)
				{
					if(this->loadUnsortedProcessStructVectorFromComboJsonFileStruct() >= 0)
					{
						if((this->loadUnmergedSubconnectionStructVectorFromComboJsonFileStruct() >= 0) &&
										(this->mergeUnmergedSubconnectionsAndLoadIntoUnsortedConnectionStructVector() >= 0))
						{
							if(this->sortUnsortedProcessStructsIntoSortedProcessStructVector() >= 0)
							{
								if(this->sortUnsortedConnectionStructsIntoSortedConnectionStructVector() >= 0)
								{
									if(this->loadSortedControlStructVectorFromComboJsonFileStruct() >= 0)
									{
										if(this->loadSortedControlConnectionStructVectorFromComboJsonFileStruct() >= 0)
										{
											this->loadProcessIndexMapFromSortedProcessVector();
											this->loadControlIndexMapFromSortedControlVector();

											this->setProcBufferArrayOutputAndInputConnectorStructsUsingSortedConnectionStructVectorAndDataReadyVector();
											this->setConnectedBufferIndexesInSortedProcessStructProcessIOVectorsUsingProcBufferArray();
											this->setControlOutputAndProcessParameterControlBufferIndexesUsingParamContBufferArray();
											this->setProcessAndControlTypeIntsInSortedStructVectors();
											this->loadComboStruct();

#if(dbg >= 2)
											this->printIndexMappedComboData();
#endif

											this->setPedalUi();
										}
										else
										{
											status = -1;
		#if(dbg >= 2)
											 cout << "loadSortedControlConnectionStructVectorFromComboJsonFileStruct failed." << endl;
		#endif
										}
									}
									else
									{
										status = -1;
		#if(dbg >= 2)
										 cout << "loadSortedControlStructVectorFromComboJsonFileStruct failed." << endl;
		#endif
									}
								}
								else
								{
									status = -1;
		#if(dbg >= 2)
									 cout << "sortUnsortedConnectionStructsIntoSortedConnectionStructVector failed." << endl;
		#endif
								}
							}
							else
							{
								status = -1;
		#if(dbg >= 2)
								 cout << "sortUnsortedProcessStructsIntoSortedProcessStructVector failed." << endl;
		#endif
							}
						}
						else
						{
							status = -1;

							{
		#if(dbg >= 2)
								 cout << "loadUnmergedSubconnectionStructVectorFromComboJsonFileStruct or" << endl;
								 cout << "mergeUnmergedSubconnectionsAndLoadIntoUnsortedConnectionStructVector or" << endl;
		#endif
							}
						}
					}
					else
					{
						status = -1;
		#if(dbg >= 2)
						 cout << "loadUnsortedProcessStructVectorFromComboJsonFileStruct failed." << endl;
		#endif
					}
				}
				else
				{
					status = -1;
		#if(dbg >= 2)

					 cout << "loadEffectComboJsonFromFile or loadComboStructFromName failed." << endl;
		#endif
				}
			}
			catch(exception &e)
			{
				cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() << endl;
			}


#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::" << __func__ << ": " << status << endl;
#endif

		return status;

	}

#define dbg 0
	void ComboDataInt::transferComboStructBackToEffectComboJson()
	{
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::" << __func__ << endl;
#endif
		int status = 0;

		try
		{
			this->setPedalUi();
			this->loadComboStructDataBackIntoEffectComboJson();
		}
		catch(exception &e)
		{
			cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() << endl;
			status = -1;
		}


#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::" << __func__ << ": " << status << endl;
#endif

	}

	void ComboDataInt::updateComboDataInt(ComboStruct combo)
	{
#if(dbg >= 1)
		 cout << "*****ENTERING ComboDataInt::" << __func__ << endl;
#endif
	try
	{
		this->setComboStructParamValues(combo);
		this->transferComboStructBackToEffectComboJson();
	}
	catch(exception &e)
	{
		cout << "exception in ComboDataInt::" << __func__ << ": " << e.what() << endl;
	}
#if(dbg >= 1)
		 cout << "***** EXITING ComboDataInt::" << __func__ << ": " << status << endl;
#endif
	}
