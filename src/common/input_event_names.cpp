#include "input_event_names.hpp"
#include "log.hpp"


int EvName::GetCode( uint16_t evType, std::string codeName )
{
    switch (evType)
    {
        case EV_KEY:
            // Make sure key name exists in map
            if (!KEY_MAP.count( codeName ))
            {
                gLog.Write( Log::DEBUG, "EvName::GetCode(): Invalid or unknown KEY name specified." );
                return -1;
            }
            
            // Return with mapped event code
            return KEY_MAP.at( codeName );
        break;
        
        case EV_ABS:
            // Make sure absolute axis name exists in map
            if (!ABS_MAP.count( codeName ))
            {
                gLog.Write( Log::DEBUG, "EvName::GetCode(): Invalid or unknown ABS name specified." );
                return -1;
            }
            
            // Return with mapped event code
            return ABS_MAP.at( codeName );
        break;
        
        case EV_REL:
            // Make sure relative axis name exists in map
            if (!REL_MAP.count( codeName ))
                return -1;
            
            // Return with mapped event code
            return REL_MAP.at( codeName );
        break;
        
        default:
            gLog.Write( Log::DEBUG, "EvName::GetCode(): Invalid type specified." );
            return -1;
        break;
    }
}
