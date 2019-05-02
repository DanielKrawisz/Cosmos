#ifndef COSMOS_STATEMENT
#define COSMOS_STATEMENT

#include "workspace.hpp"

namespace cosmos {
    
    struct statement final : public writable {
        struct expression final : public writable {
            struct atomic final : public writable {
                ptr<name> Name;
                ptr<expression::number> Number;
                ptr<expression::hex> Hex;
                ptr<expression::address> Address;
                ptr<expression::secret> Secret;
                
                bool valid() {}
                
                const formats& write() const override;
            };
            
            struct construction final : public writable {
                const formats& write() const override;
            };
            
            struct operation final : public writable {
                const formats& write() const override;
            };
            
            ptr<atomic> Atomic;
            ptr<construction> Construction;
            ptr<operation> Operation;
            
            bool valid() const;
            
            const formats& write() const override;

        };
        
        struct single;
        
        struct set_statement final : public writable {
            name Name;
            single Predicate;
            
            bool valid() const {
                return Name.valid() && Predicate.valid();
            }
            
            const formats& write() const override;
        };
        
        struct single final : public writable {
            ptr<expression> Expression;
            ptr<set_statement> Set;
            
            bool valid() const;
            
            const formats& write() const override;
        
        };
        
        struct compound final : public writable {
            list<single> Statements;
            bool valid() const;
            const formats& write() const override;
        };
        
        ptr<compound> Compound;
        ptr<single> Single;
        
        bool valid() const;
        
        const formats& write() const override;
        
    };

}

#endif
