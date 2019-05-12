// Copyright (c) 2018-2019 Daniel Krawisz
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef COSMOS_EXPRESSION
#define COSMOS_EXPRESSION

#include "cosmos.hpp"
#include "token.hpp"
#include "format.hpp"

namespace cosmos {
    
    struct expression {
        
        using parameters = list<ptr<expression>>;
        static bool valid(const parameters& p) {
            for (ptr<expression> e : p) if (e == nullptr) return false;
            return true;
        }
        
        template <typename t>
        static void write_sequence(const parameters&, stringstream& ss);
        
        inline static void write_list(const parameters& p, stringstream& ss) {
            token::write<token::open_brace>{}(ss);
            write_sequence<token::comma>(p, ss);
            token::write<token::close_brace>{}(ss);
        }
    
        struct compound;
        
        struct list;
        
        template <typename X>
        struct constructor;
        
        template <function fn>
        struct application;
        
        template <operand op>
        struct operation;
        
        template <typename X>
        struct atomic;
        
        virtual ~expression() = 0;
        
    };
    
    struct expression::compound : public expression {
        parameters Parameters;
        
        bool valid() const {
            return expression::valid(Parameters);
        }
        
        compound(parameters arg) : Parameters{arg} {}
        
        virtual ~compound() = 0;
    };
    
    struct expression::list final : public compound {};
    
    namespace format {
        template <> struct write<text, expression::list> {
            void operator()(const expression::list& t, stringstream& ss) const {
                expression::write_list(t.Parameters, ss);
            }
        };
    }
    
    template <operand op>
    struct expression::operation final : public expression::compound {};
    
    namespace format {
        template <operand op> struct write<text, expression::operation<op>> {
            void operator()(const expression::operation<op>& o, stringstream& ss) const {
                expression::write_sequence<token::operand<op>::token>(o, ss);
            }
        };
    }
    
    template <typename X>
    struct expression::atomic final : public expression {
        X Atom;
    };
    
    namespace format {
        template <typename X> struct write<text, expression::atomic<X>> {
            void operator()(const expression::atomic<X>& x, stringstream& ss) const {
                format::write<format::text, X>(x.Atom, ss);
            }
        };
    }

}

#endif
