// Copyright (c) 2018-2019 Daniel Krawisz
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef COSMOS_WORKSPACE
#define COSMOS_WORKSPACE

#include "expression.hpp"
#include "name.hpp"

namespace cosmos {
    
    namespace work {
        
        struct operation;
    
        // anything that can appear in the
        // workspace is an item. 
        struct item {
            virtual ptr<expression> express() const = 0;
        };
        
        // the workspace. 
        struct space final : public item {
            bool Valid;
            map<name, ptr<item>> Contents;
            
            bool valid() const {
                if (Valid == false) return false;
                return Contents.valid();
            }
            
            space(const space& s) : Valid{s.Valid}, Contents{s.Contents} {}
            space() : Valid{true}, Contents{} {};
            
            space& operator=(const space& s) {
                Contents = s.Contents; 
                return *this;
            }
            
            ptr<expression> express() const override;
            
        private:
            space set(name, ptr<item>) const;
            
            space(bool b) : Valid{b}, Contents{} {}
            
            friend struct operation;
        };
        
        template <typename X>
        struct atom final : public item {
            X Atom;
            
            atom(X a) : Atom{a} {}
            
            ptr<expression> express() const override;
        };
        
        struct output final : public bitcoin::output::representation, public item {
            ptr<expression> express() const override;
        };
        
        struct outpoint final : public bitcoin::outpoint::representation, public item {
            ptr<expression> express() const override;
        };
        
        struct input final : public bitcoin::input, public item {
            ptr<expression> express() const override;
        };
        
        struct transaction final : public bitcoin::transaction::representation, public item {
            ptr<expression> express() const override;
        };
        
        struct wallet final : public item {
            ptr<expression> express() const override;
        };
        
        struct keysource final : public item {
            ptr<expression> express() const override;
        };
        
    }
    
}

#endif 
