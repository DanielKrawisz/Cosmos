// Copyright (c) 2019 Daniel Krawisz
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef COSMOS_FORMAT
#define COSMOS_FORMAT

#include "cosmos.hpp"

namespace cosmos {
    
    namespace format {
        
        struct text;
        struct hex;
        
        // This will be template-specialized for each type
        // that can be formatted as text. 
        template <typename format, typename type> struct write {
            write() = delete;
            void operator()(const type& t, stringstream& ss) const;
        };
        
        // This will be template-specialized for each type
        // that can be formatted as text. 
        template <typename format, typename type> struct read {
            read() = delete;
            void operator()(type& t, stringstream& ss) const;
        };
        
        template <> struct write<text, string> {
            void operator()(const string& t, stringstream& ss) const;
        };
        
        template <> struct read<text, string> {
            void operator()(string& t, stringstream& ss) const;
        };
        
        template <> struct write<hex, bytes> {
            void operator()(const bytes& t, stringstream& ss) const;
        };
        
        template <> struct read<hex, bytes> {
            void operator()(bytes& t, stringstream& ss) const;
        };
        
    };
    
    template <typename X>
    inline void write_text(X x, stringstream& ss) {
        format::write<format::text, X>(x, ss);
    }
    
    template <typename X>
    inline X read_text(stringstream& ss) {
        return format::read<format::text, X>(ss);
    }
    
    template <typename X>
    inline string text(X x) {
        stringstream ss;
        write_text(x, ss);
        return ss.str();
    }
    
};

#endif 
