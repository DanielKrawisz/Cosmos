#ifndef COSMOS_EXPRESSION
#define COSMOS_EXPRESSION

#include "cosmos.hpp"
#include <data/fold.hpp>

namespace cosmos {
    
    struct expression : public writable {
    
        // constructible objects in the workspace. 
        enum constructible {
            invalid = 0, 
            outpoint = 1, 
            input = 2, 
            output = 3, 
            transaction = 4, 
            key_generator = 5, 
            wallet = 6, 
        };
        
        struct construction final : public writable {
            constructible Type;
            list<ptr<expression>> Arguments;
            
            bool valid() {
                return Type != 0 &&
                    data::fold([](bool b, ptr<expression> p)->bool{
                            return b && p != nullptr;
                        }, true, Arguments);
            }
            
            const formats& write() const override;

            construction(constructible t, list<ptr<expression>> arg) : Type{t}, Arguments{arg} {}
        };
        
        struct number;
        struct address;
        struct secret;
        struct hex;
        
    };
    
    struct expression::number final : public expression {
        N number;
        const formats& write() const override;
    };
    
    struct expression::address final : public bitcoin::address, public expression {
        const formats& write() const override;
    };
    
    struct expression::secret final : public bitcoin::secret, public expression {
        const formats& write() const override;
    };
    
    struct expression::hex final : public bytes, public expression {
        const formats& write() const override;
    };
    
    struct expressible {
        virtual expression express() const = 0;
    };

}

#endif
