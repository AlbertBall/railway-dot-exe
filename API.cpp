/*! **************************************************************************
 * @file API.cpp
 * @author K. Zarebski
 * @date 2021-10-03
 * @brief File class and method definitions for the ROS API unit
 ****************************************************************************/
#pragma hdrstop
#include "API.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
API::API(const AnsiString& file_name)
{
   file_path_ = file_name;
   // Print something if program is opened
   TIniFile* ini_file_ = new TIniFile(file_path_);
   ini_file_->WriteBool("session", "running", true);
}
void API::add_metadata_str(const AnsiString& label, AnsiString* data)
{
    metadata_str_[label] = data;
}
void API::add_metadata_bool(const AnsiString& label, bool* data)
{
    metadata_bool_[label] = data;
}
void API::add_metadata_int(const AnsiString& label, int* data)
{
    metadata_int_[label] = data;
}
void API::write_string(const AnsiString& label, const AnsiString& data)
{
	metadata_str_[label] = new AnsiString(data);
}
void API::write_int(const AnsiString& label, const int& data)
{
	metadata_int_[label] = new int(data);
}
void API::write_bool(const AnsiString& label, const bool& data)
{
	metadata_bool_[label] = new bool(data);
}
void API::find_metadata_file()
{
	try {
		AnsiString search_file_;

        // If the current railway is unknown do nothing
		if(metadata_str_.find("railway") == metadata_str_.end())
		{
			return;
		}

		// identify the root directory for the current project, it could be in
        // RailOS itself or be from a separate package folder
		if(
			metadata_str_.find("railway_file") != metadata_str_.end() &&
			*metadata_str_["railway_file"] != ""
		)
		{
		   search_file_ = *metadata_str_["railway_file"];
		}
		else if(metadata_str_.find("session_file") != metadata_str_.end() &&
			*metadata_str_["session_file"] != ""
		)
		{
		   search_file_ = *metadata_str_["session_file"];
		}
		else
		{
			// Failed to find so do nothing
            return;
		}

		const AnsiString railways_dir_ = ExtractFilePath(search_file_);
		AnsiString root_dir_ = TDirectory::GetParent(TDirectory::GetParent(railways_dir_));

		const AnsiString file_name_label_ = StringReplace(*metadata_str_["railway"], " ", "_", TReplaceFlags() <<rfReplaceAll);
		const AnsiString toml_file_ = root_dir_ + "\\" + "Metadata" + "\\" + (*metadata_str_["railway"]) + ".toml";

		// if no file found do nothing
		if(!FileExists(toml_file_)) return;

		write_string("metadata_file", toml_file_);
	}

    catch(Exception &e)
	{
		//no action for API errors
		return;
	}
}
void API::dump()
{
	try
	{
		if(file_path_.IsEmpty()) return;  // Do not interrupt ROS if no file specified
		TIniFile* ini_file_ = new TIniFile(file_path_);
		if(!ini_file_) return;  // Do not interrupt ROS if failure
		std::map<AnsiString, AnsiString*>::iterator it_str = metadata_str_.begin();
		while(it_str != metadata_str_.end())
		{
			const AnsiString data_ = (it_str->second) ? *(it_str->second) : AnsiString("");
			ini_file_->WriteString("session", it_str->first, data_);
			it_str++;
		}
		std::map<AnsiString, bool*>::iterator it_bool = metadata_bool_.begin();
		while(it_bool != metadata_bool_.end())
		{
			const bool data_ = (it_bool->second) ? *(it_bool->second) : false;
			ini_file_->WriteBool("session", it_bool->first, data_);
            it_bool++;
		}
        std::map<AnsiString, int*>::iterator it_int = metadata_int_.begin();
		while(it_int != metadata_int_.end())
		{
            const int data_ = (it_int->second) ? *(it_int->second) : -1;
			ini_file_->WriteInteger("session", it_int->first, data_);
			it_int++;
		}
		if(ini_file_) delete ini_file_;
	}
	catch(Exception &e)
	{
		//no action for API errors
		return;
	}
}

void API::reset_all()
{
	try
	{

		std::map<AnsiString, AnsiString*>::iterator it_str = metadata_str_.begin();
		while(it_str != metadata_str_.end())
		{
            (*it_str->second) = AnsiString();
			it_str++;
		}
	}
	catch(Exception &e)
	{
		//no action for API errors
		return;
	}
}


API::~API()
{
    TIniFile* ini_file_ = new TIniFile(file_path_);
    if(!ini_file_) return;  // Do not interrupt ROS if failure
    // Clear session
    ini_file_->EraseSection("session");
    ini_file_->WriteBool("session", "running", false);
    if(ini_file_) delete ini_file_;
}
