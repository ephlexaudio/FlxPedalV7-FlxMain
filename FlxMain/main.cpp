/*
 * main.cpp

 *
 *  Created on: Dec 24, 2015
 *      Author: mike
 */

#define hostUiDbg 2
#define pedalUiDbg 2
#define processingDbg 0
#define enableFlxProcessingAndIPC 1

#include <string>
#include <cstring>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <stdint.h>
#include <unistd.h>
#include <sched.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <linux/i2c-dev.h>
#include <gnu/stubs.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>



#include "utilityFunctions.h"
#include "ComboDataInt.h"
#include "PedalUiInt.h"
#include "HostUiInt.h"
#include "ComboMap.h"
#include "FileSystemInt.h"
#include "GPIOClass.h"
#include "ProcessingControl.h"
#include "PedalUtilityData.h"
#include "config.h"


using namespace std;



bool debugOutput = true; // set by running FlxMain with option "-d"



static void signal_handler(int sig)
{

	 std::cout << "signal received: " << sig <<", FlxMain exiting ..." << std::endl;


	system("killall -9 jackd");
	signal(sig, SIG_DFL);
	kill(getpid(),sig);

}


#define dbg 0
int main(int argc, char *argv[])
{
	cpu_set_t main_ui_set;
	CPU_ZERO(&main_ui_set);
	CPU_SET(1,&main_ui_set);
	sched_setaffinity(getpid(),sizeof(main_ui_set),&main_ui_set);
	cout << "FlxMain PID: " << getpid() << endl;
	 cout << "************************************************************************************************" << endl;
	 cout << "*****************************************   OFX START   ****************************************" << endl;
	 cout << "************************************************************************************************" << endl;


	PedalUtilityData pedalUtilityData;
	HostUiInt hostUi;
	ProcessingControl procCont;


	ComboMap comboMap;
	FileSystemInt fsInt;

	bool hostGuiActive = false;

	int exit = 0;
	errno = 0;
	string pinString;
	bool usbConnected = false;
	string comboName;
	string pedalUiRequest;
	string pedalUiRequestCommand;
	string pedalUiRequestData;
	string hostUiRequest;
	string hostUiRequestCommand;
	string hostUiRequestData;
	hostUiRequest.resize(FILE_SIZE);
	hostUiRequestData.resize(FILE_SIZE);
	string FlxMainStatus;
	Json::Value FlxParamJsonData;
	Json::Reader FlxParamJsonReader;
	Json::Reader hostUiJsonReader;
	Json::Value hostUiJsonData;
	std::string serverRequest;
	std::string serverRequestCommand;
	std::string serverRequestDataString;
	Json::Value serverRequestDataJson;
	Json::Reader serverRequestReader;
	bool loadSuccess = false;

	/*****************START: Start up and init of pedal *******************/
	signal(SIGHUP, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGILL, signal_handler);
	signal(SIGTRAP, signal_handler);
	signal(SIGABRT, signal_handler);
	signal(SIGFPE, signal_handler);
	signal(SIGBUS, signal_handler);
	signal(SIGKILL, signal_handler);
	signal(SIGSEGV, signal_handler);
	signal(SIGPIPE, signal_handler);
	signal(SIGALRM, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGSTKFLT, signal_handler);
	signal(SIGSTOP, signal_handler);
	signal(SIGXCPU, signal_handler);
	signal(SIGXFSZ, signal_handler);
	signal(SIGVTALRM, signal_handler);
	signal(SIGPROF, signal_handler);


	errno = 0;


	pedalUtilityData.readUtilityDataFromFile();


	comboMap.loadComboMapAndList();
	comboMap.setProcessUtilityData(pedalUtilityData.getProcessUtility());

	procCont.setJackUtility(pedalUtilityData.getJackUtility());

	if(procCont.startJack() == -1)
	{
		cout << "startJack failed"  << endl;
		signal_handler(SIGQUIT);
	}

	sleep(1); // give JACK more time to start up
	procCont.disableAudioOutput();
	FlxMainStatus.clear();


	procCont.loadSymbols();
#if(enableFlxProcessingAndIPC)
	procCont.startComboProcessing();
	usleep(10);
	pedalUtilityData.sendIpcProcessUtility();
#endif


	/***************** Start up FlxPedalUi process and connect ***************/


	popen("/home/FlxPedalUi -d | tee /uiLog.txt &", "w");

	PedalUiInt pedalUi;

	pedalUi.openIPCFiles();
	pedalUi.sendPortOpen(false);
	pedalUi.sendHostGuiActive(false);

	hostUi.openPort();
	//*************************** ENTER MAIN PROCESSING LOOP *****************************************

	while(exit == 0)
	{

		//if(hostUi.isUsbCableConnected() == true && usbConnected == false)
		if(hostUi.isFlxEditorRunning() == true && hostGuiActive == false)
		{
			//if(hostUi.openPort() >= 0)
			{
				//pedalUi.sendPortOpen(true);
				pedalUi.sendHostGuiActive(true);
				//usbConnected = true;
				hostGuiActive = true;
			}
			/*else
			{
				//pedalUi.sendPortOpen(false);
				pedalUi.sendHostGuiActive(false);
				hostGuiActive = false;
			}*/

		}
		//else if(hostUi.isUsbCableConnected() == false && usbConnected == true)
		else if(hostUi.isFlxEditorRunning() == false && hostGuiActive == true)
		{
			//if(hostUi.closePort() >= 0)
			{
				//usbConnected = false;
				//pedalUi.sendPortOpen(false);
				pedalUi.sendHostGuiActive(false);
				hostGuiActive = false;
			}
			/*else
			{
				//pedalUi.sendPortOpen(true);
				pedalUi.sendHostGuiActive(true);
				hostGuiActive = true;
			}*/
		}


		if(hostGuiActive == true)
		{
			hostUiRequest = hostUi.getUserRequest();
			//***************************** Process requests from Host PC *********************************************
			if(hostUiRequest.size() > 5)  // data is validated/filtered in UsbInt
			{

#if(hostUiDbg >= 1)
				 cout << "HOST UI REQUEST: " << hostUiRequest.substr(0,100) << endl;
				 cout << "hostUiRequest size: " << hostUiRequest.size() << endl;
#endif

				hostUiRequest = sanitizeString(hostUiRequest);
				{
					unsigned int colonPosition = hostUiRequest.find(":");
					if(0 < colonPosition && colonPosition < hostUiRequest.length())
					{
						hostUiRequestCommand = hostUiRequest.substr(0,colonPosition);
						hostUiRequestData = hostUiRequest.substr(colonPosition+1);

#if(hostUiDbg >= 2)
						cout << "hostUiRequestCommand: " << hostUiRequestCommand << endl;
						cout << "hostUiRequestData: " << hostUiRequestData.substr(0,100) << endl;

#endif
					}
					else hostUiRequestCommand = hostUiRequest;

					if(hostUiRequestCommand.compare("getControlTypes") == 0)
					{
						hostUi.sendControlTypeData(procCont.getControlSymbols());
					}
					else if(hostUiRequestCommand.compare("getComponents") == 0)
					{
						hostUi.sendComponentData(procCont.getComponentSymbols());
					}
					else if(hostUiRequestCommand.compare("listCombos") == 0)
					{
						vector<string> comboList = comboMap.getComboNameList();
						hostUi.sendComboList(getStringListString(comboList));
#if(hostUiDbg >= 2)
						 cout << "sent listCombos" << endl;
#endif
					}
					else if(hostUiRequestCommand.compare("getCombo") == 0)
					{
						if(hostUiRequestData.empty() == false && validateString(hostUiRequestData) == true)
						{
							string tempComboName = hostUiRequestData;
							procCont.disableAudioOutput();
							if(comboMap.isInComboMap(tempComboName))
							{
								comboName = tempComboName;

#if(hostUiDbg >= 2)
								cout << "hostUiRequestData: " << hostUiRequestData << endl;
								cout << "sending combo data to host PC."  << endl;
#endif
								hostUi.sendComboToHost(hostUiRequestData);
#if(hostUiDbg >= 1)
								cout << "combo data sent to host PC."  << endl;
#endif
								//ComboStruct comboStruct = comboMap.getComboObject(comboName).getComboStruct();
								procCont.loadComboStruct(comboMap.getComboObject(comboName).getComboStruct());
								if(procCont.sendIpcComboStruct() == 0) loadSuccess = true;
								else loadSuccess = false;
								if(loadSuccess == true)
								{
									FlxMainStatus = "combo running";
								}
								else
								{
									 cout << "could not get combo" << endl;
									FlxMainStatus = "load failed";
								}
								usleep(10000);
							}
							else
							{
								 cout << "could not get combo" << endl;
								FlxMainStatus = "load failed";
							}
							procCont.enableAudioOutput();
						}
						else
						{
							 cout << "empty or invalid combo name given." << endl;
						}
					}
					else if(hostUiRequestCommand.compare("changeValue") == 0)
					{
						if(hostUiRequestData.empty() == false && validateString(hostUiRequestData) == true)
						{
							try
							{
								ProcessControlChange change = parseValueChangeRequest(hostUiRequestData);
								if(change.procContName.find("control") != string::npos)
								{
									procCont.updateControlParameter(change.procContName, change.parameter, change.parameterValueIndex);
								}
								else
								{
									procCont.updateProcessParameter(change.procContName, change.parameter, change.parameterValueIndex);
								}
							}
							catch(exception &e)
							{
								cout << "error parsing changeValue request: " << e.what() << endl;
							}
							hostUi.sendResponse("ValueChanged","dummyString");
						}
						else
						{
							 cout << "empty or invalid change data given." << endl;
						}
					}
					else if(hostUiRequestCommand.compare("saveCombo") == 0)
					{
						vector<string> comboList;
						if(hostUiRequestData.empty() == false && validateString(hostUiRequestData) == true)
						{
							bool validComboName;

							procCont.disableAudioOutput();
							usleep(10000);

							/***********************************************************************
							 * getComboFromHost saves new combo data file DIRECTLY to file system via
							 * inclusion of FileSystemInt class into HostUiInt class.
							 ***********************************************************************/
							string tempComboName = hostUi.getComboFromHost(hostUiRequestData);

							if(tempComboName.empty() == false)
							{
								if(comboMap.isInComboMap(tempComboName) == false) // combo was added
								{
									comboName = tempComboName;
									 cout << "new combo added" << endl;
									 cout << "new combo data to be saved: " << endl;
									 cout << comboName << endl;
									comboMap.addNewComboObjectToMapAndList(comboName);

									validComboName = true;
								}
								else // existing combo: update by erasing map element and adding newer struct to map.
								{
									comboName = tempComboName;
									comboMap.eraseFromMap(comboName);
									comboMap.addNewComboObjectToMapAndList(comboName);
									validComboName = true;
									usleep(500000);
								}

							}
							else
							{
								cout << "combo name was empty" << endl;
								validComboName = false;
							}

							if(validComboName == true)
							{

								hostUi.sendComboList(getStringListString(
										comboMap.getComboNameList()));
								//****************** Load saved combo *****************

								ComboStruct comboStruct = comboMap.getComboObject(comboName).getComboStruct();
								if(procCont.loadComboStruct(comboStruct) == 0)
								{
									 cout << "combo running: " << comboName << endl;
									FlxMainStatus = "combo running";
								}
								else
								{
									 cout << "could not load combo" << endl;
									FlxMainStatus = "load failed";
								}
							}
							else
							{
								cout << "invalid combo name. Combo was not saved." << endl;
								hostUi.sendComboList("invalid combo name. Combo was not saved.");

							}
							usleep(10000);
							procCont.enableAudioOutput();
						}
						else
						{
							 cout << "no valid data given to save." << endl;
							hostUi.sendComboList("no valid data given to save.");
						}
					}
					else if(hostUiRequestCommand.compare("deleteCombo") == 0)
					{
						vector<string> comboList;
						if(hostUiRequestData.empty() == false)
						{
							if(fsInt.deleteComboFromFileSystem(hostUiRequestData) == 0)
							{
								comboList = fsInt.getComboListFromFileSystem();

								comboMap.deleteComboObjectFromMapAndList(hostUiRequestData); // update comboMap

								 cout << "combo map list size: " << comboList.size() << endl;

								string comboNameArray;

								hostUi.sendComboList(getStringListString(comboList));
								FlxMainStatus = "combo deleted";
							}
							else
							{
								 cout << "failed to delete combo." << endl;
							}
						}
						else
						{
							 cout << "no combo name given to delete." << endl;
						}
					}
					else if(hostUiRequestCommand.compare("getCurrentStatus") == 0)
					{
						hostUi.sendCurrentStatus(FlxMainStatus);
	#if(hostUiDbg >= 1)
						 cout << "sent current status to HOST: " << FlxMainStatus << endl;
	#endif
					}

					else
					{
						 cout << "sent nothing to host" << endl;
					}
				}
				hostUiRequest.clear();
				hostUiRequestCommand.clear();
				hostUiRequestData.clear();
			}

			usleep(5000);
		}


		//************* Process requests from PCB UI Interface  *************
		// this needs to be running even when USB is connected to be able to
		// use the Utility menu

		if((pedalUiRequest = pedalUi.getUserRequest()).size() > 5)
		{

#if(pedalUiDbg >= 3)
			 cout << "PEDAL UI REQUEST: " << pedalUiRequest << endl;
			 cout << "pedalUiRequest size: " << pedalUiRequest.size() << endl;
#endif

			// validate request (make sure it isn't corrupted)

			if(validateString(pedalUiRequest) == true)
			{
				size_t colonPosition = pedalUiRequest.find(":");
				if(0 < colonPosition && colonPosition < pedalUiRequest.length())
				{
					pedalUiRequestCommand = pedalUiRequest.substr(0,colonPosition);
					pedalUiRequestData = pedalUiRequest.substr(colonPosition+1);
				}
				else pedalUiRequestCommand = pedalUiRequest;
#if(pedalUiDbg >= 2)
				cout << "pedalUiRequestCommand: " << pedalUiRequestCommand;
				cout << "\tpedalUiRequestData: " << pedalUiRequestData << endl;
#endif
				if(pedalUiRequestCommand.compare("listCombos") == 0)
				{
					vector<string> comboList = comboMap.getComboNameList();
					string comboListString = getStringListString(comboList);
					pedalUi.sendComboList(comboListString);

#if(pedalUiDbg >= 2)
					 cout << comboListString << endl;
					 cout << "sent listCombos" << endl;
#endif
				}
				else if(pedalUiRequestCommand.compare("getCombo") == 0)
				{
					if(pedalUiRequestData.size() > 1)
					{
						comboName = pedalUiRequestData;
						pedalUi.sendComboPedalUiData(comboMap.getComboObject(comboName).getPedalUiJson());
						procCont.disableAudioOutput();  // mutes the popping noises during the combo change
						if(comboMap.isInComboMap(comboName))
						{

							comboName = pedalUiRequestData;

							usleep(5000); // let Processing::audioCallback finish and get disabled before
										// changing combos


							procCont.loadComboStruct(comboMap.getComboObject(comboName).getComboStruct());
#if(enableFlxProcessingAndIPC)
							if(procCont.sendIpcComboStruct() == 0) loadSuccess = true;
							else loadSuccess = false;
							usleep(1000);

							if(loadSuccess == true)
							{
#if(pedalUiDbg >= 1)
								 cout << "sent pedalUiJson for: " << comboName << endl;
#endif
								FlxMainStatus = "combo running";
							}
							else
							{
								FlxMainStatus = "load failed";
							}
#endif
						}
						else
						{
							 cout << "comboName not found." << endl;
							FlxMainStatus = "load failed";
						}
						procCont.enableAudioOutput();
					}
					else
					{
						 cout << "invalid pedalUiRequestData: " << pedalUiRequestData << endl;
					}
				}
				else if(pedalUiRequestCommand.compare("changeValue") == 0)
				{
					try
					{
						int size = pedalUiRequestData.size();
						if(size > 0)
						{
							ProcessControlChange change = parseValueChangeRequest(pedalUiRequestData);
#if(enableFlxProcessingAndIPC)
							procCont.updateControlParameter(change.procContName, change.parameter, change.parameterValueIndex);
#endif
						}
					}
					catch(exception &e)
					{
						cout << "error parsing changeValue request: " << e.what() << endl;
					}
				}
				else if(pedalUiRequestCommand.compare("saveCombo") == 0)
				{
					ComboStruct comboData = procCont.getComboStruct();
					cout << "saveCombo: " << comboData.name << endl;
					comboMap.saveCombo(comboData);
					usleep(200000);
					FlxMainStatus = "combo saved";
				}
				else if(pedalUiRequestCommand.compare("getUtilities") == 0)
				{
					pedalUtilityData.readUtilityDataFromFile();
					pedalUi.sendFlxUtilPedalUiData(pedalUtilityData.getUtilityDataForPedalUi());
				}
				else if(pedalUiRequestCommand.compare("changeUtilValue") == 0)
				{
					string utilityParameterName;
					string utilityParameterValue;
					int  equalIndex;

					if((equalIndex = pedalUiRequestData.find("=")) > 0)
					{
						utilityParameterName = pedalUiRequestData.substr(0,equalIndex);
						utilityParameterValue = pedalUiRequestData.substr(equalIndex+1);
						int utilType = 0;
						if((utilType = pedalUtilityData.updateUtilityValue(utilityParameterName, utilityParameterValue)) == -1)
						{
							cout << "invalid utility value." << endl;
						}
						else
						{
							try
							{		//TODO: Use the Observer Pattern here
								switch(utilType)
								{
									case 0:
										comboMap.setProcessUtilityData(pedalUtilityData.getProcessUtility());
										break;
									case 1:
										procCont.setJackUtility(pedalUtilityData.getJackUtility());
										break;
									case 2:
#if(enableFlxProcessingAndIPC)
										pedalUtilityData.sendIpcNoiseGateUtility();
#endif
										break;
									case 3:
#if(enableFlxProcessingAndIPC)
										pedalUtilityData.sendIpcEnvTriggerUtility();
#endif
										break;
									default:;
								}
							}
							catch(exception &e)
							{
								cout << "exception process updating utility value: " << e.what() << endl;
							}
						}
					}
					else
					{
						cout << "invalid changeUtilValue request." << endl;
					}
					/***************************************************/
				}
				else if(pedalUiRequestCommand.compare("saveUtilities") == 0)
				{
					pedalUtilityData.writeUtilityDataToFile();
				}
				else if(pedalUiRequestCommand.compare("powerOff") == 0)
				{
					// exit main loop, shutdown IPC, processing, and connectors, and then poweroff.
					exit = 1;
				}
				else
				{
					 cout << "sent nothing to pedal" << endl;
				}
			}
			else
			{
				 cout << "pedalUiRequest is invalid or corrupted." << endl;
			}

			pedalUiRequest.clear();
			pedalUiRequestCommand.clear();
			pedalUiRequestData.clear();
		}
	}
	pedalUi.closeIPCFiles();
	procCont.disableAudioOutput();
	procCont.stopComboProcessing();
	hostUi.closePort();
	procCont.stopJack();
	system("poweroff");
	while(1);
	return 0;
}
