/*
 * Processing.cpp
 *
 *  Created on: Jun 25, 2016
 *      Author: mike
 */




#include "ProcessingControl.h"
//#include "indexMapStructs.h"
#include "ControlSymbols.h"
#include "ComponentSymbols.h"


#include "GPIOClass.h"
#include "structs.h"



#define FS1_LED	30
#define FS2_LED	31
#define FS1_PIN	33 // 33 and 32 are switch around
#define FS2_PIN	32



#define CONTROL_PIN_0_NUMBER 41
#define CONTROL_PIN_1_NUMBER 42
#define CONTROL_PIN_2_NUMBER 43


using namespace std;

ProcessingControl::ProcessingControl()
{
	this->inputsSwitched = false;

	this->justPoweredUp = true;

	this->toComboMemory = NULL;
	this->createProcessingComboIPC();
	 this->openProcessingComboIPC();

}


ProcessingControl::~ProcessingControl()
{
	this->closeProcessingComboIPC();

}

/**********************************************************************************
 * **************** Private Functions ******************************************
 * ********************************************************************************/



#define dbg 1
int ProcessingControl::createProcessingComboIPC()
{
	int status = 0;
#if(dbg >= 1)
	 cout << "***** ENTERING: ProcessingControl::createProcessingIPC" << endl;
#endif
		sprintf(this->toComboIPCPath,"/ipc_toFlxProcessingComboInt");
		errno = 0;

		try
		{
			this->toComboFD = shm_open(this->toComboIPCPath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
			if(this->toComboFD == -1)
			{
				printf("toProdFD open failed.\n");
				printf("errno %d:%s\n", errno, strerror(errno));
			}
			else if(ftruncate(this->toComboFD,sizeof(_ipcComboStruct)) == -1)
			{
				printf("toProdFD ftruncate failed: %d\n", errno);
				errno = 0;
			}
			close(toComboFD);
		}
		catch(exception &e)
		{
			cout << "exception in ProcessingControl::createProcessingIPC: " << e.what();
			status = -1;
		}

#if(dbg >= 1)
	 cout << "***** EXITING: ProcessingControl::createProcessingIPC: " << status << endl;
#endif

	return status;

}

#define dbg 1
int ProcessingControl::openProcessingComboIPC()
{
	int status = 0;
#if(dbg >= 1)
	 cout << "***** ENTERING: ProcessingControl::openProcessingIPC" << endl;
#endif

		try
		{

			this->toComboFD = shm_open(this->toComboIPCPath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
			cout << "toComboFD: " << this->toComboFD << endl;
			if(this->toComboFD == -1)
			{
				printf("toProdFD open failed.\n");
			}
			else
			{
				this->toComboMemory = (_ipcComboStruct *)mmap (NULL, sizeof(_ipcComboStruct), PROT_WRITE | PROT_READ,
						MAP_SHARED, this->toComboFD, 0);
				memset(this->toComboMemory,0,sizeof(_ipcComboStruct));
			}

			this->toComboMemory->loadCombo = false;
			this->toComboMemory->comboLoaded = false;
			this->toComboMemory->processingEnabled = false;

			close(toComboFD);
		}
		catch(exception &e)
		{
			cout << "exception in ProcessingControl::openProcessingIPC: " << e.what();
			status = -1;
		}

#if(dbg >= 1)
	 cout << "***** EXITING: ProcessingControl::openProcessingIPC: " << status << endl;
#endif
	return status;
}

int ProcessingControl::closeProcessingComboIPC()
{
	int status = 0;
#if(dbg >= 1)
	 cout << "***** ENTERING: ProcessingControl::closeProcessingIPC" << endl;
#endif

	if(munmap(this->toComboMemory, this->toComboFD) == -1) status = -1;
	return status;

#if(dbg >= 1)
	 cout << "***** EXITING: ProcessingControl::closeProcessingIPC: " << status << endl;
#endif
	return status;
}


/**********************************************************************************
 * **************** Public Functions ******************************************
 * ********************************************************************************/


/************** JACK Functions ***************/

void ProcessingControl::setJackUtility(JackUtility jackUtil)
{
#if(dbg >= 1)
	 cout << "***** ENTERING: ProcessingControl::setJackUtility" << endl;
#endif

	try
	{
		this->jackUtil.buffer.value = jackUtil.buffer.value;
		this->jackUtil.period.value = jackUtil.period.value;
	}
	catch(exception &e)
	{
		cout << "exception in ProcessingControl::setJackUtility: " << e.what() << endl;
	}

#if(dbg >= 1)
	 cout << "***** EXITING: ProcessingControl::setJackUtility" << endl;
#endif
}

#define dbg 1
int ProcessingControl::startJack(void)
{
	int status = 0;
#if(DISABLE_AUDIO_THREAD == 0)

#if(dbg >= 1)
	 cout << "***** ENTERING: ProcessingControl::startJack" << endl;
#endif


	 cout << "ProcessingControl JACK period: " << this->jackUtil.period.value << "\tJACK buffer: " << this->jackUtil.buffer.value << endl;

	char jackInitString[100];
	sprintf(jackInitString, "jackd -d alsa -p %d -n %d &", this->jackUtil.period.value, this->jackUtil.buffer.value);
#if(dbg >= 2)
	cout << "jackInitString: " << jackInitString << endl;
#endif
	system(jackInitString);
	sleep(2);
	 cout << "setting up I2C connector..." << endl;
	uint8_t i2cInitSequence[][2] = 	{
			{0x12 ,0},		// setting to inactive
			{0x10 ,0},		// resetting
			{0x0E ,0x4A},	// setting DAIF
			{0x00 ,0x17},	// setting left line-in register
			{0x02 ,0x17},	// setting right line-in register
			{0x08 ,0x12},	// enabling DAC
			{0x0A ,0x07},	// disable DAC soft mute, disable HPF, enable de-emphasis
			{0x0C ,0x5F},	// disabling POWEROFF
			{0x0C ,0x5E},	// powering line inputs
			{0x0C ,0x5A},	// powering ADC
			{0x0C ,0x52},	// powering DAC
			{0x0C ,0x42},	// powering outputs
			{0x12 ,0x01}	// setting to active
	};
	uint8_t i2cInitLength = 13;

	int i2cDevFile;
	int i2cAdapter=1;
	char i2cDevFileName[20];


	ssize_t i2cStatus;


	//*************  Finish setting codec registers ************
	  snprintf(i2cDevFileName, 19, "/dev/i2c-%d", i2cAdapter);
	  i2cDevFile = open(i2cDevFileName, O_WRONLY);
	  if (i2cDevFile < 0)
	  {
	    // ERROR HANDLING; you can check errno to see what went wrong
#if(dbg>=2)
		cout << "error creating I2C device file" << endl;
#endif
		  status = -1;
	  }
	  int addr = 0x1a; // The I2C address

	  if((ioctl(i2cDevFile, I2C_SLAVE_FORCE, addr) < 0) && status == 0)
	  {
	    // ERROR HANDLING; you can check errno to see what went wrong
#if(dbg>=2)
		cout << "error opening I2C device file: " << errno << endl;
#endif
		  status = -1;
	  }

	  if(status == 0)
	  {
		  do{
			  errno = 0;
			  i2cStatus = write(i2cDevFile,i2cInitSequence[0],2);
#if(dbg>=2)
			cout << "i2c deactivate result: " << errno << endl;
#endif
			  nanosleep((const struct timespec[]){{0, 100000000L}}, NULL);

		  }while ((errno != 0));


		  for(uint8_t i = 1; i < i2cInitLength; i++)
		  {
			  i2cStatus = write(i2cDevFile,i2cInitSequence[i],2);
#if(dbg>=2)
			  cout << "i2c init result: " << errno << endl;
#endif

		  }
	  }
	  close(i2cDevFile);
#if(dbg >= 1)
	 cout << "***** EXITING: ProcessingControl::startJack:" << status << endl;
#endif
#endif
	return status;

}


int ProcessingControl::stopJack(void)
{
	int status = 0;

	status = system("killall -9 jackd");

    return status;
}



/*************** Combo Processing Functions ********************/



#define dbg 1
int ProcessingControl::startComboProcessing() // start clients and connect them
{
	int status = 0;
#if(dbg >= 1)
	 cout << "***** ENTERING: ProcessingControl::startComboProcessing" << endl;
#endif

	 // start FlxProcessing
	 system("/home/FlxProcessing &");
#if(dbg >= 1)
	 cout << "***** EXITING: ProcessingControl::startComboProcessing: " <<  endl;
#endif

	return status;

}


#define dbg 0
void ProcessingControl::stopComboProcessing() // stop clients and disconnect them
{
#if(DISABLE_AUDIO_THREAD == 0)
#if(dbg >= 1)
	 cout << "***** ENTERING: ProcessingControl::stopComboProcessing" << endl;
#endif

	system("killall -9 FlxProcessing");  // need more graceful way to shut this down
	usleep(100000);
	this->closeProcessingComboIPC();

#if(dbg >= 1)
	 cout << "***** EXITING: ProcessingControl::stopComboProcessing: " <<  endl;
#endif
#endif
}


/**************** Combo Change Functions*****************/


#define dbg 1
ComboStruct ProcessingControl::getComboStruct()
{
	int status = 0;
#if(dbg >= 1)
	 cout << "******* ENTERING: ProcessingControl::getComboStruct" << endl;
	 cout << "comboName: " << this->name << endl;
#endif
	ComboStruct combo;
	try
	{
		combo.name = this->name;
		//int processIndex = 0;
		//int procParamIndex = 0;
		string processName;
		string procParamName;

		map<string,ProcessIndexing> processMap = this->processIndexMap;
		int bufferIndex = 0;




#if(dbg >= 2)
		 cout << "Getting Processes: " << endl;

#endif
			combo.processCount = this->processCount;

		for(int processIndex = 0; processIndex < this->processCount; processIndex++)
		{

			combo.processSequenceData[processIndex].processName =
					this->processSequenceData[processIndex].processName;
			combo.processSequenceData[processIndex].parameterCount =
					this->processSequenceData[processIndex].parameterCount;
#if(dbg >= 2)
				 cout << "\tprocessName: " << combo.processSequenceData[processIndex].processName << endl;
				 cout << "\tparamCount: " << combo.processSequenceData[processIndex].parameterCount << endl;
#endif

			for(int paramIndex = 0; paramIndex < this->processSequenceData[processIndex].parameterCount; paramIndex++)
			{
				combo.processSequenceData[processIndex].parameterArray[paramIndex].parameterName =
						this->processSequenceData[processIndex].parameterArray[paramIndex].parameterName;
				combo.processSequenceData[processIndex].parameterArray[paramIndex].internalIndexValue =
						this->processSequenceData[processIndex].parameterArray[paramIndex].internalIndexValue;
#if(dbg >= 2)
				cout << "\t\tparameterName: " << combo.processSequenceData[processIndex].parameterArray[paramIndex].parameterName << endl;
				cout << "\t\tvalue: " << combo.processSequenceData[processIndex].parameterArray[paramIndex].internalIndexValue << endl;
#endif

			}

		}

#if(dbg >= 2)
					 cout << "Getting Controls: " << endl;
#endif
					 combo.controlCount = this->controlCount;
		for(int controlIndex = 0; controlIndex < this->controlCount; controlIndex++)
		{

			combo.controlSequenceData[controlIndex].controlName =
					this->controlSequenceData[controlIndex].controlName;
			combo.controlSequenceData[controlIndex].parameterCount =
					this->controlSequenceData[controlIndex].parameterCount;
#if(dbg >= 2)
				 cout << "\tcontrolName: " << combo.controlSequenceData[controlIndex].controlName << endl;
				 cout << "\tparamCount: " << combo.controlSequenceData[controlIndex].parameterCount << endl;
#endif
			for(int paramIndex = 0; paramIndex < this->controlSequenceData[controlIndex].parameterCount; paramIndex++)
			{
				combo.controlSequenceData[controlIndex].parameterArray[paramIndex].parameterName =
						this->controlSequenceData[controlIndex].parameterArray[paramIndex].parameterName;
				combo.controlSequenceData[controlIndex].parameterArray[paramIndex].value =
						this->controlSequenceData[controlIndex].parameterArray[paramIndex].value;
#if(dbg >= 2)
				cout << "\t\tparameterName: " << combo.controlSequenceData[controlIndex].parameterArray[paramIndex].parameterName << endl;
				cout << "\t\tvalue: " << combo.controlSequenceData[controlIndex].parameterArray[paramIndex].value << endl;
#endif

			}

		}
	}
	catch(exception &e)
	{
		cout << "exception in ProcessingControl::getComboStruct: " << e.what() << endl;
		status = -1;
	}

#if(dbg >= 1)
	 cout << "EXIT: ProcessingControl::getComboStruct: " << status << endl;
#endif
	return combo;
}



#define dbg 3
int ProcessingControl::loadComboStruct(ComboStruct comboStruct)
{
	int status = 0;

#if(dbg >= 1)
	 cout << "******* ENTERING: ProcessingControl::loadComboStruct" << endl;
#endif

	try
	{
		this->controlVoltageEnabled = false;
		this->processCount = comboStruct.processCount;
		this->controlCount = comboStruct.controlCount;
		this->processSignalBufferCount = comboStruct.processSignalBufferCount;
		this->processParamControlBufferCount = comboStruct.processParamControlBufferCount;

		this->name= comboStruct.name;
		cout << this->processCount << ":" << this->controlCount;
		cout << ":" << this->processSignalBufferCount << ":" << this->processParamControlBufferCount << endl;
		// set Processes
		int i = 0;
		int j = 0;
		int k = 0;
		for(i = 0; i < this->processCount; i++)
		{
			this->processSequenceData[i] = comboStruct.processSequenceData[i];
#if(dbg >= 3)
			 cout << "this->processSequenceData[" << i << "].processName: " << this->processSequenceData[i].processName << endl;
			 cout << "this->processSequenceData[" << i << "].procType: " << this->processSequenceData[i].processTypeInt << endl;
			 cout << "this->processSequenceData[" << i << "].footswitchNumber: " << this->processSequenceData[i].footswitchNumber << endl;
#endif
#if(dbg >= 2)
			 cout << "this->processSequenceData[" << i << "].processInputCount: " << this->processSequenceData[i].processInputCount << endl;
#endif

#if(dbg >= 2)
			 cout << "this->processSequenceData[" << i << "].processOutputCount: " << this->processSequenceData[i].processOutputCount << endl;
#endif

#if(dbg >= 2)
				 cout << "this->processSequenceData[" << i << "].parameterCount: " << this->processSequenceData[i].parameterCount << endl;
#endif
			for(j = 0; j < comboStruct.processSequenceData[i].parameterCount; j++)
			{

#if(dbg >= 3)
				 cout << "this->processSequenceData[" << i << "].parameter[" << j << "]->";
				 cout << "\tparameterName: "<< this->processSequenceData[i].parameterArray[j].parameterName;
				 cout << "\tinternalIndexValue: "<< this->processSequenceData[i].parameterArray[j].internalIndexValue;
				 cout << "\tparamContBufferIndex: " << this->processSequenceData[i].parameterArray[j].paramContBufferIndex;
				 cout << "\tcontrolConnected: " << this->processSequenceData[i].parameterArray[j].controlConnected << endl;
#endif
			}


			for(; j < 10; j++)
			{
				this->processSequenceData[i].parameterArray[j].internalIndexValue = 0;
				this->processSequenceData[i].parameterArray[j].controlConnected = false;
			}
			this->processSequenceData[i].bufferSize = 256;
			this->processSequenceData[i].inputCouplingMode = 1;
			this->processSequenceData[i].antiAliasingNumber = 1;
			this->processSequenceData[i].waveshaperMode = 0;
		}

		// set Controls

		for(i = 0; i < this->controlCount; i++)
		{
			this->controlSequenceData[i] = comboStruct.controlSequenceData[i];
#if(dbg >= 2)
			 cout << "this->controlSequenceData[" << i << "].name: " << this->controlSequenceData[i].controlName << endl;
			 cout << "this->controlSequenceData[" << i << "].controlTypeInt: " << this->controlSequenceData[i].controlTypeInt << endl;
#endif
			for(j = 0; j < comboStruct.controlSequenceData[i].outputInvConnectionCount; j++)
			{

#if(dbg >= 2)
				 cout << "this->controlSequenceData[" << i << "].outputToParamControlBufferIndex: " << this->controlSequenceData[i].outputToParamControlBufferIndex[j] << endl;
#endif


#if(dbg >= 2)
				 cout << "this->controlSequenceData[" << i << "].outputInvToParamControlBufferIndex: " << this->controlSequenceData[i].outputInvToParamControlBufferIndex[j] << endl;
#endif
			}



#if(dbg >= 2)
				 cout << "this->controlSequenceData[" << i << "].parameterCount: " << this->controlSequenceData[i].parameterCount << endl;
#endif

			int j = 0;
			for(j = 0; j < comboStruct.controlSequenceData[i].parameterCount; j++)
			{

#if(dbg >= 3)

				{
					cout << "this->controlSequenceData[" << i << "].parameter[" << j << "].value: ";
					cout << this->controlSequenceData[i].parameterArray[j].value;
					cout << "\t\tthis->controlSequenceData[" << i << "].parameter[" << j << "].cvEnabled: ";
					cout << this->controlSequenceData[i].parameterArray[j].cvEnabled << endl;
				}
#endif
			}
			for(; j < 10; j++)
			{
				this->controlSequenceData[i].parameterArray[j].value = 0;
				this->controlSequenceData[i].parameterArray[j].cvEnabled = false;
			}
#if(dbg >= 2)
			 cout << "this->controlVoltageEnabled: " << this->controlVoltageEnabled << endl;
#endif
		}

		// set process signal buffering

		for(i = 0; i < this->processSignalBufferCount; i++)
		{
			this->processSignalBufferArray[i] = comboStruct.processSignalBufferArray[i];
		}

		// set parameterControl buffering
#if(dbg >=2)
		 cout << "set parameterControl buffering: " << this->processParamControlBufferCount << endl;
#endif
		for(i = 0; i < comboStruct.processParamControlBufferCount; i++)
		{
			this->processParamControlBufferArray[i] = comboStruct.processParamControlBufferArray[i];

#if(dbg >= 3)
			if(strncmp((char *)this->processParamControlBufferArray[i].destProcessParameterConnector.parentObjectName.c_str(),"empty",5) != 0)
			{
				cout << "destProcessParameterConnector.parentObjectName: " <<
								this->processParamControlBufferArray[i].destProcessParameterConnector.parentObjectName << endl;
				cout << "destProcessParameterConnector.connectorName: " <<
								this->processParamControlBufferArray[i].destProcessParameterConnector.connectorName << endl;
				cout << "destProcessParameterConnector.connectedBufferIndex: " <<
								this->processParamControlBufferArray[i].destProcessParameterConnector.connectedBufferIndex << endl;
				cout << "srcControl.parentObjectName: " << this->processParamControlBufferArray[i].srcControlOutputConnector.parentObjectName << endl;
				cout << "srcControl.connectorName: " << this->processParamControlBufferArray[i].srcControlOutputConnector.connectorName << endl;
				cout << "srcControl.connectedBufferIndex: " <<
								this->processParamControlBufferArray[i].srcControlOutputConnector.connectedBufferIndex << endl;
				cout << "parameterValueIndex: " << this->processParamControlBufferArray[i].parameterValueIndex << endl;
			}
#endif
		}


		/******************* Copy indexing maps ***************************
		 * 	Even though index maps are in the IPC data structure,
		 *  they are still needed to translate the parameter request strings
		 *  into integer indexes in the update<X>Parameter functions
		 */
		this->processIndexMap.clear();
		this->controlIndexMap.clear();

		this->processIndexMap.insert(comboStruct.processIndexMap.begin(),
				comboStruct.processIndexMap.end());

		this->controlIndexMap.insert(comboStruct.controlIndexMap.begin(),
				comboStruct.controlIndexMap.end());

		this->inputSystemBufferIndex[0] = comboStruct.inputSystemBufferIndex[0];
		this->inputSystemBufferIndex[1] = comboStruct.inputSystemBufferIndex[1];
		this->outputSystemBufferIndex[0] = comboStruct.outputSystemBufferIndex[0];
		this->outputSystemBufferIndex[1] = comboStruct.outputSystemBufferIndex[1];
#if(dbg >= 2)
		 cout << "this->inputSystemBufferIndex[0]: " << this->inputSystemBufferIndex[0] << endl;
		 cout << "this->inputSystemBufferIndex[1]: " << this->inputSystemBufferIndex[1] << endl;
		 cout << "this->outputSystemBufferIndex[0]: " << this->outputSystemBufferIndex[0] << endl;
		 cout << "this->outputSystemBufferIndex[1]: " << this->outputSystemBufferIndex[1] << endl;
#endif
	}
	catch(exception &e)
	{
		 cout << "exception in ProcessingControl::loadComboStruct: " << e.what() << endl;
		status = -1;

	}
#if(dbg >= 1)
	 cout << "EXIT: ProcessingControl::loadComboStruct: " << status << endl;
#endif

	return status;
}

#define dbg 1
int ProcessingControl::sendIpcComboStruct(void)
{
	int status = 0;
#if(dbg >= 1)
	 cout << "***** ENTERING: ProcessingControl::sendIpcComboStruct" << endl;
#endif

	try
	{
		this->toComboMemory->controlVoltageEnabled = false;
		this->toComboMemory->processCount = this->processCount;
		this->toComboMemory->controlCount = this->controlCount;
		this->toComboMemory->processSignalBufferCount = this->processSignalBufferCount;
		this->toComboMemory->processParamControlBufferCount = this->processParamControlBufferCount;

		strncpy(this->toComboMemory->name, this->name.c_str(),20);

		// set Processes
		int i = 0;
		int j = 0;
		for(i = 0; i < this->processCount; i++)
		{
			strncpy(this->toComboMemory->processSequenceData[i].processName,
					this->processSequenceData[i].processName.c_str(),20);
			this->toComboMemory->processSequenceData[i].processTypeInt =
					this->processSequenceData[i].processTypeInt;

			this->toComboMemory->processSequenceData[i].footswitchNumber =
					this->processSequenceData[i].footswitchNumber;
			this->toComboMemory->processSequenceData[i].processSequenceIndex =
					this->processSequenceData[i].processSequenceIndex;
#if(dbg >= 2)
			 cout << "this->toComboMemory->processSequenceData[" << i << "].processName: " << this->toComboMemory->processSequenceData[i].processName << endl;
			 cout << "this->toComboMemory->processSequenceData[" << i << "].procType: " << this->toComboMemory->processSequenceData[i].processTypeInt << endl;
			 cout << "this->toComboMemory->processSequenceData[" << i << "].footswitchNumber: " << this->toComboMemory->processSequenceData[i].footswitchNumber << endl;
#endif
			//***************** Get input count and store values **************
			this->toComboMemory->processSequenceData[i].processInputCount =
					this->processSequenceData[i].processInputCount;
#if(dbg >= 2)
			 cout << "this->toComboMemory->processSequenceData[" << i << "].processInputCount: " << this->toComboMemory->processSequenceData[i].processInputCount << endl;
#endif

			for(j = 0; j < this->processSequenceData[i].processInputCount; j++)
			{
				this->toComboMemory->processSequenceData[i].inputConnectedBufferIndexArray[j] =
						this->processSequenceData[i].inputConnectedBufferIndexArray[j];

#if(dbg >= 2)
				 cout << "this->toComboMemory->processSequenceData[" << i << "].input[" << j << "].processSignalBufferIndex: " << this->toComboMemory->processSequenceData[i].inputConnectedBufferIndexArray[j] << endl;
#endif
			}

			//***************** Get output count and store values  **************
			this->toComboMemory->processSequenceData[i].processOutputCount =
					this->processSequenceData[i].processOutputCount;
#if(dbg >= 2)
			 cout << "this->toComboMemory->processSequenceData[" << i << "].processOutputCount: " << this->toComboMemory->processSequenceData[i].processOutputCount << endl;
#endif

			for(j = 0; j < this->processSequenceData[i].processOutputCount; j++)
			{

				this->toComboMemory->processSequenceData[i].outputConnectedBufferIndexArray[j] =
						this->processSequenceData[i].outputConnectedBufferIndexArray[j];
#if(dbg >= 2)
				 cout << "this->toComboMemory->processSequenceData[" << i << "].output[" << j << "].processSignalBufferIndex: " << this->toComboMemory->processSequenceData[i].outputConnectedBufferIndexArray[j] << endl;
#endif
			}

			//***************** Get parameter count and store values **************
			this->toComboMemory->processSequenceData[i].parameterCount =
					this->processSequenceData[i].parameterCount;

			for(j = 0; j < this->processSequenceData[i].parameterCount; j++)
			{
				this->toComboMemory->processSequenceData[i].parameterArray[j].internalIndexValue =
						this->processSequenceData[i].parameterArray[j].internalIndexValue;
				this->toComboMemory->processSequenceData[i].parameterArray[j].paramContBufferIndex =
						this->processSequenceData[i].parameterArray[j].paramContBufferIndex;
				strncpy(this->toComboMemory->processSequenceData[i].parameterArray[j].parameterName,
						this->processSequenceData[i].parameterArray[j].parameterName.c_str(),20);
				this->toComboMemory->processSequenceData[i].parameterArray[j].controlConnected =
						this->processSequenceData[i].parameterArray[j].controlConnected;
#if(dbg >= 2)
				 cout << "this->toComboMemory->processSequenceData[" << i << "].parameter[" << j << "]->";
				 cout << " internalIndexValue: "<< this->toComboMemory->processSequenceData[i].parameterArray[j].internalIndexValue;
				 cout << "\tparamContBufferIndex: " << this->toComboMemory->processSequenceData[i].parameterArray[j].paramContBufferIndex << endl;
#endif
			}

			for(; j < 10; j++)
			{
				this->toComboMemory->processSequenceData[i].parameterArray[j].internalIndexValue = 0;
				this->toComboMemory->processSequenceData[i].parameterArray[j].controlConnected = false;
			}
			this->toComboMemory->processSequenceData[i].bufferSize = 256;//comboStruct.procUtil.bufferSize;
			this->toComboMemory->processSequenceData[i].inputCouplingMode = 1;//comboStruct.procUtil.inputCouplingMode;
			this->toComboMemory->processSequenceData[i].antiAliasingNumber = 1;//comboStruct.procUtil.antiAliasingNumber;
			this->toComboMemory->processSequenceData[i].waveshaperMode = 0;//comboStruct.procUtil.waveshaperMode;
		}

		// set Controls

		for(i = 0; i < this->controlCount; i++)
		{
			strncpy(this->toComboMemory->controlSequenceData[i].controlName,
					this->controlSequenceData[i].controlName.c_str(),20);
			this->toComboMemory->controlSequenceData[i].controlTypeInt =
					this->controlSequenceData[i].controlTypeInt;
#if(dbg >= 2)
			 cout << "this->toComboMemory->controlSequenceData[" << i << "].name: " << this->toComboMemory->controlSequenceData[i].controlName << endl;
			 cout << "this->toComboMemory->controlSequenceData[" << i << "].controlTypeInt: " << this->toComboMemory->controlSequenceData[i].controlTypeInt << endl;
#endif

			this->toComboMemory->controlSequenceData[i].outputConnectionCount =
					this->controlSequenceData[i].outputConnectionCount;

			for(j = 0; j < this->controlSequenceData[i].outputConnectionCount; j++)
			{
				this->toComboMemory->controlSequenceData[i].outputToParamControlBufferIndex[j] =
						this->controlSequenceData[i].outputToParamControlBufferIndex[j];
#if(dbg >= 2)
				 cout << "this->toComboMemory->controlSequenceData[" << i << "].outputToParamControlBufferIndex: " << this->toComboMemory->controlSequenceData[i].outputToParamControlBufferIndex[j] << endl;
#endif
			}

			this->toComboMemory->controlSequenceData[i].outputInvConnectionCount =
					this->controlSequenceData[i].outputInvConnectionCount;

			for(j = 0; j < this->controlSequenceData[i].outputInvConnectionCount; j++)
			{
				this->toComboMemory->controlSequenceData[i].outputInvToParamControlBufferIndex[j] =
						this->controlSequenceData[i].outputInvToParamControlBufferIndex[j];
#if(dbg >= 2)
				 cout << "this->toComboMemory->controlSequenceData[" << i << "].outputInvToParamControlBufferIndex: " << this->toComboMemory->controlSequenceData[i].outputInvToParamControlBufferIndex[j] << endl;
#endif
			}




			this->toComboMemory->controlSequenceData[i].parameterCount =
					this->controlSequenceData[i].parameterCount;
			int j = 0;
			for(j = 0; j < this->controlSequenceData[i].parameterCount; j++)
			{
				strncpy(this->toComboMemory->controlSequenceData[i].parameterArray[j].parameterName,
						this->controlSequenceData[i].parameterArray[j].parameterName.c_str(),20);
				this->toComboMemory->controlSequenceData[i].parameterArray[j].value =
						this->controlSequenceData[i].parameterArray[j].value;
				this->toComboMemory->controlSequenceData[i].parameterArray[j].cvEnabled =
						this->controlSequenceData[i].parameterArray[j].cvEnabled;

#if(dbg >= 2)

				{
					cout << "this->toComboMemory->controlSequenceData[" << i << "].parameter[" << j << "].value: ";
					cout << this->toComboMemory->controlSequenceData[i].parameterArray[j].value;
					cout << "\t\tthis->toComboMemory->controlSequenceData[" << i << "].parameter[" << j << "].cvEnabled: ";
					cout << this->toComboMemory->controlSequenceData[i].parameterArray[j].cvEnabled << endl;
				}
#endif
			}
			for(; j < 10; j++)
			{
				this->toComboMemory->controlSequenceData[i].parameterArray[j].value = 0;
				this->toComboMemory->controlSequenceData[i].parameterArray[j].cvEnabled = false;
			}
#if(dbg >= 2)
			 cout << "this->toComboMemory->controlVoltageEnabled: " << this->toComboMemory->controlVoltageEnabled << endl;
#endif
		}

		// set process signal buffering

		for(i = 0; i < this->processSignalBufferCount; i++)
		{
			strncpy(this->toComboMemory->processSignalBufferArray[i].srcProcessOutputConnector.parentObjectName,
					this->processSignalBufferArray[i].srcProcessOutputConnector.parentObjectName.c_str(),20);
			strncpy(this->toComboMemory->processSignalBufferArray[i].srcProcessOutputConnector.connectorName,
					this->processSignalBufferArray[i].srcProcessOutputConnector.connectorName.c_str(),20);
			this->toComboMemory->processSignalBufferArray[i].srcProcessOutputConnector.connectedBufferIndex =
					this->processSignalBufferArray[i].srcProcessOutputConnector.connectedBufferIndex;
			this->toComboMemory->processSignalBufferArray[i].srcProcessOutputConnector.connectorIndex =
					this->processSignalBufferArray[i].srcProcessOutputConnector.connectorIndex;

			int destProcessInputCount =
					this->processSignalBufferArray[i].destProcessInputConnectorVector.size();

			for(j = 0; j < destProcessInputCount; j++)
			{
				strncpy(this->toComboMemory->processSignalBufferArray[i].destProcessInputConnectorArray[j].parentObjectName,
						this->processSignalBufferArray[i].destProcessInputConnectorVector[j].parentObjectName.c_str(),20);
				strncpy(this->toComboMemory->processSignalBufferArray[i].destProcessInputConnectorArray[j].connectorName,
						this->processSignalBufferArray[i].destProcessInputConnectorVector[j].connectorName.c_str(),20);
				this->toComboMemory->processSignalBufferArray[i].destProcessInputConnectorArray[j].connectedBufferIndex =
						this->processSignalBufferArray[i].destProcessInputConnectorVector[j].connectedBufferIndex;
				this->toComboMemory->processSignalBufferArray[i].destProcessInputConnectorArray[j].connectorIndex =
						this->processSignalBufferArray[i].destProcessInputConnectorVector[j].connectorIndex;
			}
		}

		// set parameterControl buffering
#if(dbg >=2)
		 cout << "set parameterControl buffering: " << this->processParamControlBufferCount << endl;
#endif
		for(i = 0; i < this->processParamControlBufferCount; i++)
		{
			strncpy(this->toComboMemory->processParamControlBufferArray[i].destProcessParameterConnector.parentObjectName,
							this->processParamControlBufferArray[i].destProcessParameterConnector.parentObjectName.c_str(),20);
			strncpy(this->toComboMemory->processParamControlBufferArray[i].destProcessParameterConnector.connectorName,
							this->processParamControlBufferArray[i].destProcessParameterConnector.connectorName.c_str(),20);
			this->toComboMemory->processParamControlBufferArray[i].destProcessParameterConnector.connectedBufferIndex =
					this->processParamControlBufferArray[i].destProcessParameterConnector.connectedBufferIndex;
			strncpy(this->toComboMemory->processParamControlBufferArray[i].srcControlOutputConnector.parentObjectName,
					this->processParamControlBufferArray[i].srcControlOutputConnector.parentObjectName.c_str(),20);
			strncpy(this->toComboMemory->processParamControlBufferArray[i].srcControlOutputConnector.connectorName,
					this->processParamControlBufferArray[i].srcControlOutputConnector.connectorName.c_str(),20);
			this->toComboMemory->processParamControlBufferArray[i].srcControlOutputConnector.connectedBufferIndex =
					this->processParamControlBufferArray[i].srcControlOutputConnector.connectedBufferIndex;
			this->toComboMemory->processParamControlBufferArray[i].parameterValueIndex =
					this->processParamControlBufferArray[i].parameterValueIndex;

#if(dbg >= 2)
			if(strncmp(this->toComboMemory->processParamControlBufferArray[i].destProcessParameterConnector.parentObjectName,"empty",5) != 0)
			{
				cout << "destProcessParameterConnector.parentObjectName: " <<
								this->toComboMemory->processParamControlBufferArray[i].destProcessParameterConnector.parentObjectName << endl;
				cout << "destProcessParameterConnector.connectorName: " <<
								this->toComboMemory->processParamControlBufferArray[i].destProcessParameterConnector.connectorName << endl;
				cout << "destProcessParameterConnector.connectedBufferIndex: " <<
								this->toComboMemory->processParamControlBufferArray[i].destProcessParameterConnector.connectedBufferIndex << endl;
				cout << "srcControl.parentObjectName: " << this->toComboMemory->processParamControlBufferArray[i].srcControlOutputConnector.parentObjectName << endl;
				cout << "srcControl.connectorName: " << this->toComboMemory->processParamControlBufferArray[i].srcControlOutputConnector.connectorName << endl;
				cout << "srcControl.connectedBufferIndex: " <<
								this->toComboMemory->processParamControlBufferArray[i].srcControlOutputConnector.connectedBufferIndex << endl;
				cout << "parameterValueIndex: " << this->toComboMemory->processParamControlBufferArray[i].parameterValueIndex << endl;
			}
#endif
		}

#if(dbg >= 2)
		cout << "set footswitches" << endl;
#endif

		this->toComboMemory->inputSystemBufferIndex[0] = this->inputSystemBufferIndex[0];
#if(dbg >= 2)
		 cout << "this->toComboMemory->inputSystemBufferIndex[0]: " << this->toComboMemory->inputSystemBufferIndex[0] << endl;
#endif


		 this->toComboMemory->inputSystemBufferIndex[1] = this->inputSystemBufferIndex[1];
#if(dbg >= 2)
		 cout << "this->toComboMemory->inputSystemBufferIndex[1]: " << this->toComboMemory->inputSystemBufferIndex[1] << endl;
#endif


		 this->toComboMemory->outputSystemBufferIndex[0] = this->outputSystemBufferIndex[0];
#if(dbg >= 2)
		 cout << "this->toComboMemory->outputSystemBufferIndex[0]: " << this->toComboMemory->outputSystemBufferIndex[0] << endl;
#endif


		 this->toComboMemory->outputSystemBufferIndex[1] = this->outputSystemBufferIndex[1];
#if(dbg >= 2)
		 cout << "this->toComboMemory->outputSystemBufferIndex[1]: " << this->toComboMemory->outputSystemBufferIndex[1] << endl;
#endif
	}
	catch(exception &e)
	{
		 cout << "exception in ProcessingControl::sendIpcComboStruct: " << e.what() << endl;
		status = -1;

	}

/*********************************************************************************************/
	this->toComboMemory->loadCombo = true;
	this->toComboMemory->comboLoaded = false;
	//
	usleep(1000);

	while(this->toComboMemory->comboLoaded == false)
	{
		;
	}
	this->toComboMemory->loadCombo = false;
#if(dbg >= 1)
	 cout << "***** EXITING: ProcessingControl::sendIpcComboStruct: " << status << endl;
#endif


	return status;
}


/***************** Enable AudioOutput functions****************/


void ProcessingControl::enableAudioOutput()
{

#if(dbg >= 1)
	 cout << "******* ENTERING: ProcessingControl::disableAudioOutput" << endl;
#endif

	system("i2cset -f -y 1 0x1a 0x0a 0x07");
#if(dbg >= 1)
	 cout << "******* EXITING: ProcessingControl::disableAudioOutput" << endl;
#endif

}


void ProcessingControl::disableAudioOutput()
{

#if(dbg >= 1)
	 cout << "******* ENTERING: ProcessingControl::disableAudioOutput" << endl;
#endif

	system("i2cset -f -y 1 0x1a 0x0a 0x0F");
#if(dbg >= 1)
	 cout << "******* EXITING: ProcessingControl::disableAudioOutput" << endl;
#endif

}




/******************* Process/Control parameter functions ******************/

#define dbg 0
int ProcessingControl::updateProcessParameter(string parentProcess, string parameter, int parameterValue)
{

#if(dbg >= 1)
	 cout << "******* ENTERING: ProcessingControl::updateProcessParameter" << endl;
	 cout << "parentProcess: " << parentProcess << "\parameter: " << parameter << "\tparameterValue: "  << parameterValue << endl;
#endif
	 int status = 0;

	 try
	 {
		 int procIndex = this->processIndexMap[parentProcess.c_str()].processSequenceIndex;

		 int paramIndex = this->processIndexMap[parentProcess.c_str()].
					paramIndexMap[parameter.c_str()].paramIndex;


		 int connectedBufferIndex = this->processIndexMap[parentProcess.c_str()].
							paramIndexMap[parameter.c_str()].processParamControlBufferIndex;
		#if(dbg >= 2)
			cout << "connectedBufferIndex: " << connectedBufferIndex << endl;
		#endif
		if(status == 0)
		{
			// update shared memory struct
			this->toComboMemory->processParamControlBufferArray[connectedBufferIndex].parameterValueIndex =
					parameterValue;
			// update ProcessingControl internal data for saving to file.
		 	this->processSequenceData[procIndex].parameterArray[paramIndex].internalIndexValue =
		 			parameterValue;
		}
	 }
	 catch(exception &e)
	 {
			cout << "exception in ProcessingControl::updateProcessParameter: " << e.what() << endl;
			status = -1;
	 }


#if(dbg >= 1)
	 cout << "EXIT: ProcessingControl::updateProcessParameter: " << status << endl;
#endif
	 return status;
}


#define dbg 1
int ProcessingControl::updateControlParameter(string parentControl, string parameter, int parameterValue)
{

#if(dbg >= 1)
	 cout << "******* ENTERING: ProcessingControl::updateControlParameter" << endl;
	 cout << "parentControl: " << parentControl << "\tparameter: " << parameter << "\tparameterValue: "  << parameterValue << endl;
#endif
	int status = 0;
		try
		{
			int contIndex = this->controlIndexMap[parentControl.c_str()].controlIndex;
			if(0 > contIndex || 99 < contIndex)
			{
				cout << "contIndex out of bounds: " << contIndex << endl;
				status = -1;
			}

			int paramIndex = this->controlIndexMap[parentControl.c_str()].
							paramIndexMap[parameter.c_str()].contParamIndex;
			if(0 > paramIndex || 99 < paramIndex)
			{
				cout << "paramIndex out of bounds: " << paramIndex << endl;
				status = -1;
			}

	#if(dbg >= 2)
			cout << "contIndex: " << contIndex << "paramIndex: " << paramIndex << endl;
	#endif

			if(status == 0)
			{
				this->toComboMemory->controlSequenceData[contIndex].parameterArray[paramIndex].value =
						parameterValue;
			 	this->controlSequenceData[contIndex].parameterArray[paramIndex].value =
			 			parameterValue;
			}
		}
		catch(exception &e)
		{
			cout << "exception in ProcessingControl::updateControlParameter: " << e.what() << endl;
			status = -1;
		}

#if(dbg >= 1)
	 cout << "******* EXITING: ProcessingControl::updateControlParameter: "  << status << endl;
#endif
	 return status;
}







/*************** Symbol functions****************/

void ProcessingControl::loadSymbols()
{
#if(dbg >= 1)
	 cout << "***** ENTERING: ProcessingControl::loadSymbols" << endl;
#endif

		try
		{

			this->loadComponentSymbolVector();
			this->loadControlSymbolVector();
		}
		catch(exception &e)
		{
			cout << "exception in Processing::loadSymbols: " << e.what() << endl;
		}
#if(dbg >= 1)
	 cout << "***** EXITING: ProcessingControl::loadSymbols" << endl;
#endif

}




vector<string> ProcessingControl::getComponentSymbols()
{
	return this->componentSymbolVector;
}
vector<string> ProcessingControl::getControlSymbols()
{
	return this->controlSymbolVector;


}


#define dbg 0
void ProcessingControl::loadComponentSymbolVector()
{
#if(dbg >= 1)
	 cout << "******* ENTERING: Processing::loadComponentSymbolVector" << endl;
#endif

	try
	{
		this->componentSymbolVector.push_back(delaybSymbol);
		this->componentSymbolVector.push_back(filter3bbSymbol);
		this->componentSymbolVector.push_back((filter3bb2Symbol));
		this->componentSymbolVector.push_back((lohifilterbSymbol));
		this->componentSymbolVector.push_back((mixerbSymbol));
		this->componentSymbolVector.push_back((volumebSymbol));
		this->componentSymbolVector.push_back((waveshaperbSymbol));

	}
	catch(exception &e)
	{
		cout << "exception in Processing::loadComponentSymbolVector: " << e.what() << endl;
	}

#if(dbg >= 1)
	 cout << "******* EXITING: Processing::loadComponentSymbolVector" << endl;
#endif

}


void ProcessingControl::loadControlSymbolVector()
{
#if(dbg >= 1)
	 cout << "******* ENTERING: Processing::loadControlSymbolVector" << endl;
#endif

	try
	{
		this->controlSymbolVector.push_back(normSymbol);
		this->controlSymbolVector.push_back(envGenSymbol);
		this->controlSymbolVector.push_back(lfoSymbol);
	}
	catch(exception &e)
	{
		cout << "exception in Processing::loadControlSymbolVector: " << e.what() << endl;
	}

#if(dbg >= 1)
	 cout << "******* EXITING: Processing::loadControlSymbolVector" << endl;
#endif

}
