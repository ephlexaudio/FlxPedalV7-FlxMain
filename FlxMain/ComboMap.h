/*
 * ComboMap.h
 *
 *  Created on: Oct 28, 2018
 *      Author: buildrooteclipse
 */

#ifndef COMBOMAP_H_
#define COMBOMAP_H_

#include "ComboDataInt.h"
#include "FileSystemInt.h"
	class ComboMap : public ComboDataInt
	{
	private:
		map<string, ComboDataInt> comboDataMap;
		vector<string> comboNameVector;
		FileSystemInt fsInt; // used only by PedalUi, so only saving will be done



	public:
		ComboMap ();
		~ComboMap ();

		int loadComboMapAndList(void);
		std::vector<string> getComboNameList(void);
		int addNewComboObjectToMapAndList(string comboName);
		int addNewComboObjectToMap(string comboName);
		int loadCombosFromFileSystemToComboMap();
		int deleteComboObjectFromMapAndList(string comboName);
		ComboDataInt getComboObject(string comboName);
		bool isInComboMap(string comboName);
		void eraseFromMap(string comboName);
		int saveCombo(ComboStruct comboData);

	};


#endif /* COMBOMAP_H_ */
