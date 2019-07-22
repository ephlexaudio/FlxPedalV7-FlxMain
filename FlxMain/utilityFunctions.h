/*
 * utilityFunctions.h
 *
 *  Created on: Mar 20, 2016
 *      Author: mike
 */

#ifndef UTILITYFUNCTIONS_H_
#define UTILITYFUNCTIONS_H_

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <json/json.h>
#include <linux/types.h>
#include <algorithm>

#include "structs.h"

	void startTimer(void);
	int stopTimer(const char *description);

	void pabort(const char *s);
	void clearBuffer(char *buffer, int length);
	string getCompactedJSONData(Json::Value data);
	string getStringListString(vector<string> stringVector);
	bool validateString(string dataString);
	string removeReturnRelatedCharacters(string dirtyString);
	ProcessControlChange parseValueChangeRequest(string changeRequestString);
	int validateJsonBuffer(char *jsonBuffer);



#endif /* UTILITYFUNCTIONS_H_ */
