/*
 *
 *      (\(\
 *     (='.')           cefix --
 *    o(_")")           a multipurpose library on top of OpenSceneGraph
 *  
 *
 *                      Copyright 2001-2011, stephanmaximilianhuber.com 
 *                      Stephan Maximilian Huber. 
 *
 *                      All rights reserved.
 *
 */

#ifndef CEFIX_FILE_UTILS_HEADER
#define CEFIX_FILE_UTILS_HEADER

#include <string>
#include <vector>
#include <cefix/Export.h>
#include <cefix/Date.h>


namespace cefix {

	/** shows a system dialog to select a file for opening
	 @param result hold the selected file 
	 @param path path to open in the dialog 
	 @param filetypes comma-separated list of allowed filetypes, e.g. "xml,html,htm" (mac only)
	 @result true if the user chose a file
	*/
	bool CEFIX_EXPORT showOpenFileDialog(std::string& result, const std::string& path = "", const std::string& filetypes="");
	
	/** shows a system dialog to select a file for saving
	 @param result hold the selected file 
	 @param path path to open in the dialog 
	 @result true if the user chose a file
	*/
	bool CEFIX_EXPORT showSaveFileDialog(std::string& result, const std::string& suggestedFileName, const std::string& path = "");
	
	/** read a file into a string */
	void CEFIX_EXPORT readFileIntoString(const std::string& filename, std::string& result);	
	
	std::string CEFIX_EXPORT readFileIntoString(const std::string& filename);
	
	/** reads a file into a std::vector */
	void  CEFIX_EXPORT readFileIntoVector(const std::string& filename, std::vector<std::string>& result);
	
	std::vector<std::string> CEFIX_EXPORT readFileIntoVector(const std::string& filename);
	
	/** tries to open a file via the finder (Mac only) */
	bool CEFIX_EXPORT openFileExternally(const std::string& filename);
	/** tries to reveal a file via the finder (Mac only) */
	bool CEFIX_EXPORT revealFileExternally(const std::string& filename);
	
	bool getModifiedTimeStamp(const std::string& filename, time_t& t);
	inline bool getModifiedTimeStamp(const std::string& filename, Date& date)
	{
		time_t t;
		bool result = getModifiedTimeStamp(filename, t);
		if (result)
			date = Date::getFromUnixTimeStamp(t);
		return result;
	}
	bool setModifiedTimeStamp(const std::string& filename, time_t t);
	inline bool setModifiedTimeStamp(const std::string& filename, const Date& date)
	{
		return setModifiedTimeStamp(filename, date.getAsUnixTimeStamp());
	}
}


#endif