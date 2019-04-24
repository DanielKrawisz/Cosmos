#ifndef COSMOS_WORKSPACE
#define COSMOS_WORKSPACE

#include "cosmos.hpp"

namespace cosmos {
    
    struct operation;
    
    struct expression {
        
        // The operation corresponding to this expression, 
        // if it exists. 
        ptr<operation> Operation;  
        
        expression() : Operation{nullptr} {};
        
        struct atom;
        
        struct data;
        
        struct number;
        
        struct address;
        
        struct secret;
        
        struct name;
        
        template<typename left> struct plus;
        
        template<typename left> struct times;
        
        struct concatinate {};
        
        struct set;
        
        struct function_argument;
        
        struct function_application {
            function Function;
            function_argument argument();
        };
            
        number operator()(N);
        address operator()(bitcoin::address);
        secret operator()(bitcoin::secret);
        data operator()(bytes); 
        name operator()(string);
        function_application operator()(function);
        
    };
    
    struct expression::atom : public expression {};
    
    struct expression::data : public atom {
        expression::concatinate concatinate() const;
    };
    
    struct expression::number final : public atom {
        plus<number> plus() const;
        times<number> times() const;
    };
    
    struct expression::address final : public atom {};
    
    struct expression::secret final : public atom {};
    
    struct expression::name final : public atom {
        expression::set set() const;
    };
    
    struct expression::set final : public expression {};
    
    struct expression::function_argument final : public expression {};
    
}

#endif 
