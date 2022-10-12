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
#include "../common/log.hpp"
#include "../common/prog_args.hpp"
#include "daemon.hpp"
#include "config.hpp"
#include <vector>
#include <string>
#include <iostream>



const std::string   INFO_BLOCK      =
{
    "OpenSD daemon\n"
    "Copyright 2022 seek\n"
    "Version " + Config::VERSION_STR + "\n"
    "https://gitlab.com/open-sd/opensd\n"
    "Licensed under the GNU GPLv3+\n"
};

const std::string   LICENSE_BLOCK =
{
    "    This program comes with ABSOLUTELY NO WARRANTY.\n"
    "    This is free software, and you are welcome to redistribute it under certain\n"
    "    conditions.  See https://gitlab.com/open-sd/opensd for license.\n"
};

const std::string   HELP_BLOCK =
{
    "  Usage:\n"
    "\n"
    "    -h    --help             Show this help message.\n"
    "    -v    --version          Output version number and exit.\n"
    "    -l    --log-level        Set minumum logging level.  Default: 'warn'\n"
    "                             Valid options are:\n"
    "                                 verbose, debug, info, warn, error\n"
};



void ShowInfoLine()
{
    std::cout << INFO_BLOCK << std::endl;
    std::cout << LICENSE_BLOCK << std::endl;
}



void ShowHelp()
{
    std::cout << HELP_BLOCK << std::endl;
}



void ShowVersion()
{
    std::cout << Config::VERSION_STR << std::endl;
}



int main( int argc, char **argv )
{
    std::vector<std::string>    arg_list( argv, argv + argc );
    Daemon                      opensdd;
    ProgArgs                    args( arg_list );
    
    
    // Set initial Logging level
    gLog.SetFilterLevel( Log::WARN );





    // Handle command line arguments
    
    // Version
    if (args.HasOpt( "v", "version" ))
    {
        ShowVersion();
        return 0;
    }
    
    ShowInfoLine();
    
    // CLI help
    if (args.HasOpt( "h", "help" ))
    {
        ShowHelp();
        return 0;
    }
    
    // Logging level
    if (args.HasOpt( "l", "log-level" ))
    {
        std::vector<std::string>    levels = { "", "verb", "verbose", "debug", "info", "warn", "warning", "error" }; 
        std::string                 result;
        unsigned int                i = 0;
        
        result = args.GetOptParam( "l", "log-level" );
        for (auto& s : levels)
        {
            if (s == result)
                break;
            ++i;
        }
        
        switch (i)
        {
            case 1: // verb
            case 2: // verbose
                gLog.SetFilterLevel( Log::VERB );
            break;
            
            case 3: // sebug
                gLog.SetFilterLevel( Log::DEBUG);
            break;
            
            case 4: // info
                gLog.SetFilterLevel( Log::INFO );
            break;
            
            case 5: // warn
            case 6: // warning
                gLog.SetFilterLevel( Log::WARN );
            break;
            
            case 7: // error
                gLog.SetFilterLevel( Log::ERROR );
            break;
            
            case 0: // nada
            default:  // Unknown
                std::cout << "Invalid option.  Run again with --help for usage.\n";
                return -1;
            break;
        }
    }
   
    // Exit if there were argument parsing errors
    if (args.GetErrorCount())
    {
        std::cout << "Invalid option.  Run again with --help for usage.\n";
        return -1;
    }
    
    
    // Run daemon and exit
    return opensdd.Run();
}
