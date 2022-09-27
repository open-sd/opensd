////////////////////////////////////////////////////////////////////////////////////////////////////
//  OpenSD
//  An open-source userspace driver for Valve's Steam Deck hardware
//
//  Copyright 2022 seek
//  https://gitlab.com/open-sd/opensd
//  Licensed under the GNU GPLv3+
//
//  This program is free software: you can redistribute it and/or modify it under the terms of the 
//  GNU General Public License as published by the Free Software Foundation, either version 3 of 
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along with this program. 
//  If not, see <https://www.gnu.org/licenses/>.             
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "ini.hpp"
#include "log.hpp"
// C++
#include <sstream>
#include <fstream>
#include <algorithm>


int Ini::IniFile::LoadFile( std::filesystem::path filePath )
{
    std::string             line;
    std::ifstream           file;
    Section                 t_sec;
    unsigned int            line_count = 0;
    unsigned int            section_count = 0;
    unsigned int            key_count = 0;
    unsigned int            value_count = 0;
    
    
    namespace fs = std::filesystem;
    mData.clear();
    
    if (!fs::exists(filePath))
    {
        gLog.Write( Log::ERROR, "Ini::IniFile::LoadFile(): File '" + filePath.string() + "' not found." );
        return Err::FILE_NOT_FOUND;
    }
    
    file.open( filePath );
    if (!file.is_open())
    {
        int e = errno;
        gLog.Write( Log::ERROR, "Ini::IniFile::LoadFile(): Failed to open '" + filePath.string() + "': " + Err::GetErrnoString(e) );
        return Err::CANNOT_OPEN;
    }
    
    // Create the first/default section.
    // This section is special since it has no block and only
    // holds comments before the first block, if any are present.
    t_sec.name = "NONE";
    t_sec.keys.clear();
    mData.push_back( t_sec );

    // Read file line-by-line
    while (std::getline( file, line ))
    {
        std::stringstream           ss(line);   // copy line into stream
        std::vector<std::string>    t_vec;
        std::string                 t_str;
        bool                        ignore = false;
        
       
        // parse line by whitespace into a vector
        while (ss >> t_str)
            t_vec.push_back(t_str);

        // Check for blank line
        if (!t_vec.size())
        {
            // Push a comment key to the back of the last section
            Key             t_key;
            t_key.comment   = true;
            mData.back().keys.push_back(t_key);
        }
        else
        {
            // Make sure first string is not empty
            if (t_vec.front().size())
            {
                // Check for section change first
                // Make sure there are at least 3 characters in the first string
                // Section names must be enclose in square brackets:  [SectionName]
                if (t_vec.front().starts_with( '[' ))
                {
                    if (( t_vec.front().size() > 2) && (!t_vec.front().ends_with( ']') ))
                    {
                        // Error in section name
                        // Things could get pretty messed up if we ignore this, so we
                        // need to abort.
                        gLog.Write( Log::DEBUG, "Ini::IniFile::LoadFile(): Error on line " + std::to_string(line_count) + 
                                    ": Unclosed section name.  Aborting." );
                        return Err::INVALID_FORMAT;
                    }
                    else
                    {
                        // Section name is properly enclosed, but we still need to
                        // make sure the string inside is alphanumeric.
                        std::string     test_str = t_vec.front().substr( 1, t_vec.front().size() - 2 );
                        
                        if (test_str == "NONE")
                        {
                            gLog.Write( Log::DEBUG, "Ini::IniFile::LoadFile(): Error on line " + std::to_string(line_count) +
                                        ": Section name 'NONE' is reserved. " );
                            return Err::INVALID_FORMAT;
                        }
                        
                        for (auto& c : test_str)
                        {
                            if (!std::isalnum(c))
                            {
                                gLog.Write( Log::DEBUG, "Ini::IniFile::LoadFile(): Error on line " + std::to_string(line_count) + 
                                            ": Section name contains invalid characters.  Aborting." );
                                return Err::INVALID_FORMAT;
                            }
                        }
                        // No problems found, so add a new section name
                        t_sec.name = test_str;
                        t_sec.keys.clear();
                        mData.push_back( t_sec );
                        ++section_count;
                    }
                }
                else
                {
                    // Check for comments
                    // Comments lines will being with # as the first non-whitespace character
                    // Check if line is a comment
                    if (t_vec.front().at(0) == '#')
                    {
                        // Push whole line as the key name, but flag it as a comment
                        Key             t_key;
                        t_key.name      = line;
                        t_key.comment   = true;
                        mData.back().keys.push_back( t_key );
                    }
                    else
                    {
                        // Check for keyed lines
                        // Key lines must have at least 3 words.  They must be formatted like this:
                        //   i.e.:  KeyName = SomeValue
                        // Multivalue keys are the same with extra space-delimited values:
                        //   i.e.:  KeyName = SomeValue 1 2 3 lastValue
                        if (t_vec.size() > 2)
                        {
                            // Verify second word is '='
                            if (t_vec.at(1) != "=")
                            {
                                gLog.Write( Log::DEBUG, "Ini::IniFile::LoadFile(): Error on line " + std::to_string(line_count) + 
                                            ": Expected key assignment, but missing '='.  Ignoring line." );
                            }
                            else
                            {
                                if (t_vec.front().empty())
                                {
                                    gLog.Write( Log::DEBUG, "Ini::IniFile::LoadFile(): Error on line "  + std::to_string(line_count) + 
                                                ": Missing or invalid key name.  Ignoring line." );
                                }
                                else
                                {
                                    for (auto& c : t_vec.front())
                                    {
                                        if (!std::isalnum( c ))
                                        {
                                            gLog.Write( Log::DEBUG, "Ini::IniFile::LoadFile(): Error on line "  + std::to_string(line_count) + 
                                                        ": Key name contains invalid characters.  Ignoring line." );
                                            ignore = true;
                                        }
                                    }
                                    
                                    if (!ignore)
                                    {
                                        Key             t_key;
                                        t_key.name      = t_vec.front();
                                        t_key.comment   = false;
                                        // Copy all the values to the temp key
                                        for (unsigned int i = 2; i < t_vec.size(); ++i)
                                        {
                                            t_key.values.push_back( t_vec.at(i) );
                                            ++value_count;
                                        }
                                        // add the temp key to the current section
                                        mData.back().keys.push_back( t_key );
                                        ++key_count;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        ++line_count;
    }
    
    gLog.Write( Log::DEBUG, "Ini::IniFile::LoadFile(): Parsed " + std::to_string(line_count) + " lines, " +
                std::to_string(section_count) + " sections, " + std::to_string(key_count) + " keys and " +
                std::to_string(value_count) + " values (total). " );
    
    return Err::OK;
}



int Ini::IniFile::SaveFile( std::filesystem::path filePath )
{
    std::ofstream       file;


    namespace fs = std::filesystem;
    
    if (mData.empty())
    {
        gLog.Write( Log::DEBUG, "Ini::IniFile::SaveFile(): Nothing to save." );
        return Err::EMPTY;
    }
    
    // Make sure the directory exists
    if (!filePath.parent_path().empty())
        if (!fs::exists( filePath.parent_path() ))
        {
            gLog.Write( Log::DEBUG, "Ini::IniFile::SaveFile(): Creating directory '" + filePath.parent_path().string() + "'..." );
            // Try to create it if it doesn't
            if (!fs::create_directory( filePath.parent_path() ))
            {
                int e = errno;
                gLog.Write( Log::DEBUG, "Ini::IniFile::SaveFile(): Failed to create directory '" + filePath.parent_path().string() + "': " + Err::GetErrnoString(e) );
                return Err::CANNOT_CREATE;
            }
        }
    
    if (fs::exists(filePath))
        gLog.Write( Log::DEBUG, "Ini::IniFile::SaveFile(): File '" + filePath.string() + "' exists.  File will be overwritten." );
        
    file.open( filePath, std::ios::out | std::ios::trunc );
    if (!file.is_open())
    {
        int e = errno;
        gLog.Write( Log::DEBUG, "Ini::IniFile::SaveFile(): Failed to open '" + filePath.string() + "' for writing: " + Err::GetErrnoString(e) );
        return Err::CANNOT_OPEN;
    }
    
    // File is open, write ini data to it
    for (auto& s : mData)
    {
        // Write section name
        if (s.name != "NONE")
        {
            file << '[' << s.name << ']' << std::endl;
            if (file.fail())
            {
                int e = errno;
                gLog.Write( Log::DEBUG, "Ini::IniFile::SaveFile(): Failed to write to '" + filePath.string() + 
                            "': " + Err::GetErrnoString(e) );
                return Err::WRITE_FAILED;
            }
                
        }
            
        // loop through keys in section
        for (auto& k : s.keys)
        {
            // Handle comments
            if (k.comment)
            {
                // Make sure comments start with # if its not a blank line
                if (!k.name.empty())
                    if (!k.name.starts_with('#'))
                        k.name = "# " + k.name;
                
                file << k.name << std::endl;
                if (file.fail())
                {
                    int e = errno;
                    gLog.Write( Log::DEBUG, "Ini::IniFile::SaveFile(): Failed to write to '" + filePath.string() + 
                                "': " + Err::GetErrnoString(e) );
                    return Err::WRITE_FAILED;
                }
            }
            else
            {
                // Ignore keys without values
                if (k.values.size())
                {
                    // Handle keys
                    std::string     str = k.name + " =";
                    for (auto v : k.values)
                        str += " " + v;
                    
                    file << str << std::endl;
                    if (file.fail())
                    {
                        int e = errno;
                        gLog.Write( Log::DEBUG, "Ini::IniFile::SaveFile(): Failed to write to '" + filePath.string() + 
                                    "': " + Err::GetErrnoString(e) );
                        return Err::WRITE_FAILED;
                    }
                }
            }
        }
        // Add an extra line at the end of a section so it doesnt get
        // too mushed together.
        file << std::endl;
        if (file.fail())
        {
            int e = errno;
            gLog.Write( Log::DEBUG, "Ini::IniFile::SaveFile(): Failed to write to '" + filePath.string() + 
                        "': " + Err::GetErrnoString(e) );
            return Err::WRITE_FAILED;
        }
    }
    
    return Err::OK;
}



std::vector<std::string> Ini::IniFile::GetSectionList()
{
    std::vector<std::string>    sv;
    
    for (auto& v : mData)
        if (v.name != "NONE")
            sv.push_back( v.name );
    
    return sv;
}



std::vector<std::string> Ini::IniFile::GetKeyList( std::string section )
{
    std::vector<std::string>    sv;
    
    for (auto& s : mData)
        if (s.name == section)
            for (auto& k : s.keys)
                if (!k.comment)
                    sv.push_back( k.name );
    
    return sv;
}



std::vector<std::string> Ini::IniFile::GetVal( std::string section, std::string key )
{
    if (section == "NONE")
    {
        gLog.Write( Log::ERROR, "Ini::IniFile::SetVal(): Failed to set value: Use of reserved section name." );
        return {};
    }
    
    for (auto& s : section)
    {
        if (!isalnum(s))
        {
            gLog.Write( Log::DEBUG, "Ini::IniFile::GetVal(): Failed to get value: Invalid section name." );
            return {};
        }
    }

    for (auto& s : key)
    {
        if (!isalnum(s))
        {
            gLog.Write( Log::DEBUG, "Ini::IniFile::GetVal(): Failed to get value: Invalid key name." );
            return {};
        }
    }

    for (auto& s : mData)
        if (s.name == section)
            for (auto& k : s.keys)
                if (!k.comment)
                    if (k.name == key)
                        return k.values;
    
    // Return empty vector if not found
    gLog.Write( Log::DEBUG, "Ini::IniFile::GetVal(): Failed to get value: Not found." );
    return {};
}



int Ini::IniFile::SetVal( std::string section, std::string key, std::vector<std::string> vals )
{
    if (section == "NONE")
    {
        gLog.Write( Log::ERROR, "Ini::IniFile::SetVal(): Failed to set value: Use of reserved section name." );
        return Err::INVALID_PARAMETER;
    }
    
    for (auto& s : section)
    {
        if (!isalnum(s))
        {
            gLog.Write( Log::DEBUG, "Ini::IniFile::SetVal(): Failed to set value: Invalid section name." );
            return Err::INVALID_PARAMETER;
        }
    }

    for (auto& s : key)
    {
        if (!isalnum(s))
        {
            gLog.Write( Log::DEBUG, "Ini::IniFile::SetVal(): Failed to set value: Invalid key name." );
            return Err::INVALID_PARAMETER;
        }
    }
            
    // find section or creat it if it doesn't exist
    for (auto& s : mData)
    {
        if (s.name == section)
        {
            for (auto& k : s.keys)
            {
                if (!k.comment)
                    if (k.name == key)
                    {
                        // Key found, so update values
                        k.values.clear();
                        k.values = vals;
                        return Err::OK;
                    }
            }
            
            // Key name not found, so create a new key
            Key         new_key;
            new_key.name    = key;
            new_key.comment = false;
            new_key.values = vals;
            
            // Add new key to end of section
            s.keys.push_back( new_key );
            return Err::OK;
        }
    }

    // Section (and key) do not exist, so create them
    Key         new_key;
    new_key.name    = key;
    new_key.comment = false;
    new_key.values  = vals;
    
    Section     new_sec;
    new_sec.name    = section;
    new_sec.keys.push_back( new_key );
    
    // Add new section to the end of the section list
    mData.push_back( new_sec );
    
    return Err::OK;
}



Ini::IniFile::IniFile()
{
    // Nothing to do
}



Ini::IniFile::~IniFile()
{
    mData.clear();
}




//////////////////////////////////////////////////////////
//  ValVec Helper class
//////////////////////////////////////////////////////////
unsigned int Ini::ValVec::Count()
{
    return mData.size();
}



std::string Ini::ValVec::String( unsigned int index )
{
    if (index >= mData.size())
        return "";
        
    return mData.at(index);
}



int Ini::ValVec::Int( unsigned int index )
{
    int             i = 0;
    
    if (index >= mData.size())
        return 0;
    
    try { i = std::stoi( mData.at(index) ); } catch (...)
    {
        return 0;
    }
    
    return i;
}



double Ini::ValVec::Double( unsigned int index )
{
    double          d = 0;
    
    if (index >= mData.size())
        return 0;
    
    try { d = std::stod( mData.at(index) ); } catch (...)
    {
        return 0;
    }
    
    return d;
}
