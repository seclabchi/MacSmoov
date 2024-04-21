//
//  channel_map.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 4/13/24.
//

#ifndef channel_map_hpp
#define channel_map_hpp

#include <stdio.h>
#include <string>
#include <cstdint>
#include <vector>

namespace fmsmoov {

typedef struct {
    uint16_t in_chan;
    uint16_t this_chan;
    std::string name;
} CHANNEL_MAP_ELEMENT;

class ChannelMap {
public:
    std::vector<fmsmoov::CHANNEL_MAP_ELEMENT> the_map;
};

}

#endif /* channel_map_hpp */
