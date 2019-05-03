#ifndef COSMOS_PARSER
#define COSMOS_PARSER

#include "cosmos.hpp"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_lexeme.hpp>

namespace cosmos {
        
    namespace qi = boost::spirit::qi;
        
    struct parser {
        
        static const auto separator = qi::lexeme[";"];
        static const auto comma = qi::lexeme[","];
        static const auto open_brace = qi::lexeme["{"];
        static const auto close_brace = qi::lexeme["}"];
        static const auto open_paren = qi::lexeme["("];
        static const auto close_paren = qi::lexeme[")"];
        static const auto set = qi::lexeme["="];
        static const auto plus = qi::lexeme["+"];
        static const auto times = qi::lexeme["*"];
        static const auto concat = qi::lexeme["<>"];
        
    };
    
}

#endif 

