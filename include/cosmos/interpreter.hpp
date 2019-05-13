// Copyright (c) 2018-2019 Daniel Krawisz
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef COSMOS_INTERPRETER
#define COSMOS_INTERPRETER

#include "workspace.hpp"

namespace cosmos {
    
    namespace evaluation {
        
        struct error {
            std::string Message;
            
            static error unrecognized_name(name);
            static error format();
            
            error() : Message{} {}
            error(string m) : Message{m} {};
            
            bool operator==(const error e) const {
                return Message == e.Message;
            }
            
            bool operator!=(const error e) const {
                return Message != e.Message;
            }
            
        };
        
        struct response {
            
            // The new space that results from this operation. 
            work::space Result; 
            
            // The response which is returned to the user.
            ptr<work::item> Return; 
            
            evaluation::error Error;
            
            bool valid() const {
                return Result.valid();
            }
            
            bool error() const {
                return Error != evaluation::error{};
            };
            
            response(response& a) : Result{a.Result}, Return{a.Return}, Error{a.Error} {}
            
            response& operator=(response&& a) {
                Result = a.Result;
                Return = a.Return;
                Error = a.Error;
                a.Return = nullptr;
                return *this;
            }
            
            response() : Result{}, Return{nullptr}, Error{} {}
            
            response(const work::space s) : Result{s}, Return{nullptr}, Error{} {};
            response(const work::space s, ptr<work::item> i) : Result{s}, Return{i}, Error{} {}
            response(struct error e) : Result{}, Return{nullptr}, Error{e} {}
            response(const response& a) : Result{a.Result}, Return{a.Return}, Error(a.Error) {}
            response(response&& a) : Result{a.Result}, Return{a.Return}, Error{a.Error} {
                a.Return = nullptr;
            }

        };
        
        struct intermediate;
        
        struct open {
            response Response;
            list<ptr<intermediate>> Stack;
            
            open(response r, list<ptr<intermediate>> s) : Response{r}, Stack{s} {}
            open(response r) : Response{r}, Stack{} {}
            
            virtual ptr<intermediate> read_name(name) const = 0;
            virtual ptr<intermediate> read_number(N) const = 0;
            virtual ptr<intermediate> read_address(bitcoin::address) const = 0;
            virtual ptr<intermediate> read_pubkey(bitcoin::pubkey) const = 0;
            virtual ptr<intermediate> read_secret(bitcoin::secret) const = 0;
            virtual ptr<open> read_function(cosmos::function) const = 0;
            virtual ptr<open> read_construction(constructor) const = 0;
            virtual ptr<open> read_parenthesis() const = 0;
            virtual uint precedence() const = 0;
        };
        
        struct intermediate {
            const work::space Workspace;
            list<ptr<intermediate>> Stack;
            
            intermediate(work::space w, list<ptr<intermediate>> ss) : Workspace{w}, Stack{ss} {}
            
            virtual ptr<open> read_operand(operand) const = 0;
            ptr<open> read_comma() const;
            ptr<open> close() const;
        };
        
        template <typename A>
        struct atom final : public intermediate {
            A Atom;
            
            atom(A a, work::space w, list<ptr<intermediate>> ss) : intermediate{w, ss}, Atom{a} {}
            
            ptr<open> read_operand(operand) const override;
        };
        
        template <typename A>
        struct operation final : public open {
            atom<A> Left;
            operand Op;
            
            ptr<intermediate> read_name(name) const override;
            ptr<intermediate> read_number(N) const override;
            ptr<intermediate> read_address(bitcoin::address) const override;
            ptr<intermediate> read_pubkey(bitcoin::pubkey) const override;
            ptr<intermediate> read_secret(bitcoin::secret) const override;
            ptr<open> read_function(cosmos::function) const override;
            ptr<open> read_construction(constructor) const override;
            ptr<open> read_parenthesis() const override;
            uint precedence() const override;
            
        };
        
        struct sequence : public open {
            list<ptr<work::item>> Sequence;
            
            sequence(list<ptr<work::item>> s, evaluation::response r, list<ptr<intermediate>> ss)
                : open{r, ss}, Sequence{s} {}
                
            ptr<intermediate> read_name(name) const override;
            ptr<intermediate> read_number(N) const override;
            ptr<intermediate> read_address(bitcoin::address) const override;
            ptr<intermediate> read_pubkey(bitcoin::pubkey) const override;
            ptr<intermediate> read_secret(bitcoin::secret) const override;
            ptr<open> read_function(cosmos::function) const override;
            ptr<open> read_construction(constructor) const override;
            ptr<open> read_parenthesis() const override;
            uint precedence() const override;
            
        };
        
        struct function final : public sequence {
            cosmos::function Function;
        };
        
        struct construction : public sequence {
            cosmos::constructor Constructor;
        };
        
        struct interpreter final : public open {
            
            interpreter(work::space w) : open{evaluation::response{w}} {}
            
            uint precedence() const override {
                return 10000;
            };

            ptr<intermediate> read_name(cosmos::name n) const override {
                return std::make_shared<intermediate>(new atom<cosmos::name>{n, Response.Result, Stack});
            }
            
            ptr<intermediate> read_number(N n) const override {
                return std::make_shared<intermediate>(new atom<N>{n, Response.Result, Stack});
            }
            
            ptr<intermediate> read_address(bitcoin::address a) const override {
                return std::make_shared<intermediate>(new atom<bitcoin::address>{a, Response.Result, Stack});
            }
            
            ptr<intermediate> read_pubkey(bitcoin::pubkey p) const override {
                return std::make_shared<intermediate>(new atom<bitcoin::pubkey>{p, Response.Result, Stack});
            }
            
            ptr<intermediate> read_secret(bitcoin::secret s) const override {
                return std::make_shared<intermediate>(new atom<bitcoin::secret>{s, Response.Result, Stack});
            }
            
            ptr<open> read_function(cosmos::function) const override;
            ptr<open> read_construction(constructor) const override;
            ptr<open> read_parenthesis() const override;

        };
        
    }
    
    evaluation::response evaluate(const work::space, stringstream& s);
    
}

#endif 
