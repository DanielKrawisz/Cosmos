// Copyright (c) 2018-2019 Daniel Krawisz
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef COSMOS_EVALUATION_INTERPRETER
#define COSMOS_EVALUATION_INTERPRETER

#include <cosmos/workspace.hpp>
#include "operators.hpp"

namespace cosmos {
    // namespace for evaluating user commands. 
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
        
        // representation of states of evaluation which are ready
        // for new input and have possibly generated a response.
        struct open {
            response Response;
            list<ptr<intermediate>> Stack;
            
            open(response r, list<ptr<intermediate>> s) : Response{r}, Stack{s} {}
            open(response r) : Response{r}, Stack{} {}
            
            virtual ptr<intermediate> read_name(cosmos::name n) const;
            virtual ptr<intermediate> read_number(N n) const;
            virtual ptr<intermediate> read_address(bitcoin::address a) const;
            virtual ptr<intermediate> read_pubkey(bitcoin::pubkey p) const;
            virtual ptr<intermediate> read_secret(bitcoin::secret s) const;
            
            virtual ptr<open> read_function(cosmos::function) const = 0;
            virtual ptr<open> read_construction(constructor) const = 0;
            virtual ptr<open> read_parenthesis() const = 0;
            virtual uint precedence() const = 0;
        };
        
        // type generated at the start of an evaluation before 
        // any input has been read. 
        struct interpreter final : public open {
            
            interpreter(work::space w) : open{evaluation::response{w}} {}
            
            uint precedence() const override {
                return 10000;
            };
            
            ptr<open> read_function(cosmos::function) const override;
            ptr<open> read_construction(constructor) const override;
            ptr<open> read_parenthesis() const override;

        };
        
        // representation of states of evaluation which necessarily
        // require more input before a response can be generated. 
        struct intermediate {
            const work::space Workspace;
            list<ptr<intermediate>> Stack;
            
            intermediate(work::space w, list<ptr<intermediate>> ss) : Workspace{w}, Stack{ss} {}
            
            virtual ptr<open> read_operand(op) const = 0;
            virtual ptr<open> read_comma() const = 0;
            ptr<open> close() const;
        };
        
        template <typename A>
        struct atom final : public intermediate {
            A Atom;
            
            atom(A a, work::space w, list<ptr<intermediate>> ss) : intermediate{w, ss}, Atom{a} {}
            
            ptr<open> read_operand(op) const override;
            ptr<open> read_comma() const override;
        };
            
        inline ptr<intermediate> open::read_name(cosmos::name n) const {
            return std::make_shared<intermediate>(new atom<cosmos::name>{n, Response.Result, Stack});
        }
        
        inline ptr<intermediate> open::read_number(N n) const {
            return std::make_shared<intermediate>(new atom<N>{n, Response.Result, Stack});
        }
        
        inline ptr<intermediate> open::read_address(bitcoin::address a) const {
            return std::make_shared<intermediate>(new atom<bitcoin::address>{a, Response.Result, Stack});
        }
        
        inline ptr<intermediate> open::read_pubkey(bitcoin::pubkey p) const {
            return std::make_shared<intermediate>(new atom<bitcoin::pubkey>{p, Response.Result, Stack});
        }
        
        inline ptr<intermediate> open::read_secret(bitcoin::secret s) const {
            return std::make_shared<intermediate>(new atom<bitcoin::secret>{s, Response.Result, Stack});
        }
        
        template <typename A, op o, typename B> 
        struct operation final : public intermediate {
            atom<A> Left;
            atom<B> Right;
            cosmos::operation<A, o, B> Operation;
            
            operation(atom<A> a, atom<B> b) : Left{a}, Right{b}, Operation{} {}
            
            ptr<open> read_operand(op) const override;
            ptr<open> read_comma() const override;
        };
        
        template <typename A, op o>
        struct operand final : public open {
            atom<A> Left;
            
            ptr<intermediate> read_name(name n) const override {
                return std::make_shared<intermediate>(new operation<A, o, name>(Left, n));
            }
            
            ptr<intermediate> read_number(N n) const override {
                return std::make_shared<intermediate>(new operation<A, o, N>(Left, n));
            }
            
            ptr<intermediate> read_address(bitcoin::address a) const override {
                return std::make_shared<intermediate>(new operation<A, o, bitcoin::address>(Left, a));
            }
            
            ptr<intermediate> read_pubkey(bitcoin::pubkey p) const override {
                return std::make_shared<intermediate>(new operation<A, o, bitcoin::pubkey>(Left, p));
            }
            
            ptr<intermediate> read_secret(bitcoin::secret s) const override {
                return std::make_shared<intermediate>(new operation<A, o, bitcoin::secret>(Left, s));
            }
            
            ptr<open> read_function(cosmos::function) const override;
            ptr<open> read_construction(constructor) const override;
            ptr<open> read_parenthesis() const override;
            uint precedence() const override;
            
        };
        
        template <typename A>
        inline ptr<open> atom<A>::read_operand(op o) const {
            return std::make_shared<open>(new operand{Atom, o});
        }
        
        struct sequence : public open {
            list<ptr<work::item>> Sequence;
            
            sequence(list<ptr<work::item>> s, evaluation::response r, list<ptr<intermediate>> ss)
                : open{r, ss}, Sequence{s} {}
            
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
        
    }
    
    evaluation::response evaluate(const work::space, stringstream& s);
    
}

#endif 
