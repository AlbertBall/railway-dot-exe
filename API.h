/*! **************************************************************************
 * @file API.h
 * @author K. Zarebski
 * @date 2021-10-03
 * @brief File containing classes and methods for broadcasting ROS status data
 *
 * The classes and methods within this file are used to access variable values
 * from within the ROS Interface, this information is then dumped so it can be
 * accessed externally.
 ****************************************************************************/

#ifndef APIH
#define APIH
//---------------------------------------------------------------------------

#include "System.hpp"
#include "System.IniFiles.hpp"
#include "System.SysUtils.hpp"
#include "System.AnsiStrings.hpp"
#include "System.IOUtils.hpp"

#include <map>
#include <vector>
#include <algorithm>
#include <windows.h>            //needed for 64 bit compilation

/*! **************************************************************************
 * @class API
 * @brief a class which handles the fetching of information from the ROS
 * interface via pointers to variables of interest
 *
 * The API class dumps variables of interest from ROS interface instance to an
 * INI file so the metadata can be read by an external program
 ****************************************************************************/
class API
{
     private:
		AnsiString file_path_;
		std::vector<AnsiString> reset_excludes_;
        std::map<AnsiString, AnsiString*> metadata_str_;
        std::map<AnsiString, bool*> metadata_bool_;
		std::map<AnsiString, int*> metadata_int_;
        std::map<AnsiString, AnsiString> train_statuses_;
     public:
        /*! ******************************************************************
        * @brief construct an API object using creating the given INI file
        *
        * @param file_name path of INI file to save collected data to
        *********************************************************************/
		API(const AnsiString& file_name);

        /*! ******************************************************************
		* @brief reset all variables
		*
        * Does not reset variables within the 'reset_excludes_' member
		*
		* sets all data to be empty
		*
		*********************************************************************/
		void reset_all();

		/*! ******************************************************************
		* @brief tries to find a metadata file for the currently session
		*
		* This assumes that the TOML file has been named after the loaded
		* loaded railway file.
		*
		*********************************************************************/
		void find_metadata_file();

		/*! ******************************************************************
		* @brief add pointer to string variable to monitor value
		*
		* Adds the specified string variable to be tracked, storing a pointer
		* to access the value within a mapping.
		*
		* @param label key to save recorded information under
		* @param data pointer to string variable to track
		*********************************************************************/
		void add_metadata_str(const AnsiString& label, AnsiString* data);

        /*! ******************************************************************
        * @brief add pointer to boolean variable to monitor value
        *
        * Adds the specified boolean variable to be tracked, storing a pointer
        * to access the value within a mapping.
        *
        * @param label key to save recorded information under
        * @param data pointer to boolean variable to track
        *********************************************************************/
        void add_metadata_bool(const AnsiString& label, bool* data);

		/*! ******************************************************************
		* @brief add pointer to integer variable to monitor value
		*
		* Adds the specified integer variable to be tracked, storing a pointer
		* to access the value within a mapping.
		*
		* @param label key to save recorded information under
		* @param data pointer to integer variable to track
		*********************************************************************/
		void add_metadata_int(const AnsiString& label, int* data);

        /*! ******************************************************************
		* @brief write a string directly to the metadata
		*
		* Rather than tracking a string variable using a pointer write it
		* directly, this is useful in cases where the value is only a temporary
        * and so is not kept within the global scope.
		*
		* @param label key to save recorded information under
		* @param string data to write
		*********************************************************************/
		void write_string(const AnsiString& label, const AnsiString& data);

        /*! ******************************************************************
		* @brief write an integer directly to the metadata
		*
		* Rather than tracking a string variable using a pointer write it
		* directly, this is useful in cases where the value is only a temporary
        * and so is not kept within the global scope.
		*
		* @param label key to save recorded information under
		* @param integer data to write
		*********************************************************************/
		void write_int(const AnsiString& label, const int& data);

        /*! ******************************************************************
		* @brief write a boolean directly to the metadata
		*
		* Rather than tracking a string variable using a pointer write it
		* directly, this is useful in cases where the value is only a temporary
        * and so is not kept within the global scope.
		*
		* @param label key to save recorded information under
		* @param bool data to write
		*********************************************************************/
		void write_bool(const AnsiString& label, const bool& data);

        /*! ******************************************************************
        * @brief save currently recorded status data to INI file
        *
        * Iterates through all metadata mappings saving the results to the
        * INI file specified during definition.
        *
        *********************************************************************/
        void dump();

        /*! ******************************************************************
        * @brief destructor which clears INI file when program is terminated
        *
        * When ROS is closed the destructor additionally clears all keys
        * within the INI file except the 'running' status which is set to 0
        *
        *********************************************************************/
        ~API();
};

#endif
