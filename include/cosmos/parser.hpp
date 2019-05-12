#ifndef COSMOS_PARSER
#define COSMOS_PARSER

#include "cosmos.hpp"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_lexeme.hpp>

namespace cosmos {
    
    namespace parse {
        namespace qi = boost::spirit::qi;
        
        const auto separator = qi::lexeme[";"];
        const auto comma = qi::lexeme[","];
        const auto open_brace = qi::lexeme["{"];
        const auto close_brace = qi::lexeme["}"];
        const auto open_paren = qi::lexeme["("];
        const auto close_paren = qi::lexeme[")"];
        const auto set = qi::lexeme["="];
        const auto plus = qi::lexeme["+"];
        const auto times = qi::lexeme["*"];
        const auto concat = qi::lexeme["<>"];
        
    };
    
}

#endif 

