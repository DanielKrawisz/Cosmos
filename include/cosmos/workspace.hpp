#ifndef COSMOS_WORKSPACE
#define COSMOS_WORKSPACE

#include "expression.hpp"

namespace cosmos {
        
    // all items in the workspace have a name, and
    // this is the type that we use to represent names.
    struct name final : public expressible {
        string Name;
        
        expression express() const override;
    };
    
    namespace work {
        
        // anything that can appear in the
        // workspace is an item. 
        struct item : public expressible {};
        
        struct operation;
        
        // the workspace. 
        struct space final : public item {
            map<name, ptr<item>> Contents;
            
            bool valid() const {
                return Valid;
            }
            
            space(const space& s) : Contents{s.Contents}, {}
            space() : Contents{}, {};
            
            space& operator=(const space& s) {
                Contents = s.Contents; 
                return *this;
            }
            
        private:
            space set(name, ptr<item>) const;
            
            space(bool b) : Contents{}
            
            friend struct operation;
        };
        
        struct number final : public item {
            expression::number N;
            expression express() const override {
                return N;
            } 
        };
        
        struct hex final : public item {
            expression::hex Hex;
            expression express() const override {
                return Hex;
            }
        };
        
        struct address final : public item {
            expression::address Address;
            expression express() const override {
                return Address;
            }
        };
        
        struct pubkey final : public item {
            expression::pubkey Pubkey;
            expression express() const override {
                return Pubkey;
            }
        };
        
        struct secret final : public item {
            expression::secret Secret;
            expression express() const override {
                return Secret;
            }
        };
        
        struct script final : public item {
            script Script;
            expression express() const override {
                return expression::script{Script};
            }
        };
        
        struct output final : public bitcoin::output::representation, public item {
            expression express() const override {
                return expression::
            }
        };
        
        struct outpoint final : public bitcoin::outpoint::representation, public item {
            expression express() const override;
        };
        
        struct input final : public bitcoin::input::representation, public item {
            expression express() const override;
        };
        
        struct transaction final : public bitcoin::transaction::representation, public item {
            expression express() const override;
        };
        
        struct wallet final : public item {
            expression express() const override;
        };
        
        struct keysource final : public item {
            expression express() const override;
        };
        
    }
    
    ptr<expression> evaluate(work::space)
    
}

#endif 
