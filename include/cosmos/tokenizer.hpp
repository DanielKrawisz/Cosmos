#ifndef COSMOS_TOKENIZER
#define COSMOS_TOKENIEZR

#include <regex>

#include "cosmos.hpp"

namespace cosmos {
        
    struct tokenizer {
        enum token_type {
            none = 0, 
            eof = 1, 
            whitespace = 2, 
            separator = 3,
            comma = 5,
            open_brace = 6, 
            close_brace = 7, 
            open_paren = 8, 
            close_paren = 9, 
            set = 10, 
            plus = 11, 
            times = 12, 
            concat = 13, 
            hex = 14, 
            name = 15, 
            address = 16, 
            wif = 17
        };
        
        using iterator = string::const_iterator;
        
        struct token {
            token_type Token;
            iterator Begin;
            iterator End;
            
        private:
            token(token_type t, iterator b, iterator e) : Token{t}, Begin{b}, End{e} {}
            
            friend struct tokenizer;
        };
        
        list<token> operator()(string& s){
            return data::list::reverse(tokenize(s.begin(), s.end()));
        }
        
    private:
        list<token> tokenize(iterator i, iterator e) {
            token next = read_next(i, e);
            // TODO error case. 
            if (next.Token == eof) return {next};
            return tokenize(next.End, e) + next;
        }
        
        token read_next(iterator i, iterator e) {
            if (i == e) return {token{eof, i, e}};
            // TODO: find all the other tokens here. 
        }
    };
    
}

#endif 

