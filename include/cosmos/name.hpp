// Copyright (c) 2019 Daniel Krawisz
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef COSMOS_NAME
#define COSMOS_NAME

#include "format.hpp"

namespace cosmos {
    
    struct name : string {};
    
    namespace format {
        
        // This will be template-specialized for each type
        // that can be formatted as text. 
        template <> struct write<text, name> {
            void operator()(const name& n, stringstream& ss) const {
                ss << "$";
                write<text, string>{}(static_cast<string&>(n), ss);
            }
        };
        
    };
    
};

#endif 

