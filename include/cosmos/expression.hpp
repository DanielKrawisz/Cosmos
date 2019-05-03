#ifndef COSMOS_EXPRESSION
#define COSMOS_EXPRESSION

#include "cosmos.hpp"

namespace cosmos {
    
    struct expression {
        
        using parameters = list<ptr<expression>>;
        static bool valid(parameters);
        static void write_text(const parameters&, string inter);
    
        struct compound;
        
        template <typename X>
        struct constructor;
        
        template <function fn>
        struct application;
        
        template <operand op>
        struct operation;
        
        template <typename X>
        struct atomic;
        
        virtual bool valid() const = 0;
        virtual void write_text(stringstream& ss) const = 0; 
        
    };
    
    struct expressible {
        virtual ptr<expression> express() const = 0;
    };
    
    bool valid(expression const* e) {
        if (e == nullptr) return false;
        return e->valid();
    }
        
    struct expression::compound : public expression {
        parameters Parameters;
        
        bool valid() const override {
            return expression::valid(Parameters);
        }
        
        compound(list<ptr<expression>> arg) : Type{t}, Arguments{arg} {}
        
        virtual ~compound() = 0;
    };

}

#endif
