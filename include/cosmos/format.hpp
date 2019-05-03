#ifndef COSMOS_FORMAT
#define COSMOS_FORMAT

#include "cosmos.hpp"

namespace cosmos {
    
    namespace format {
        
        struct text;
        
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
