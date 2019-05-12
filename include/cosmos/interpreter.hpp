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
            
            error Error;
            
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
            
        private: 
            response() : Result{}, Return{nullptr}, Error{} {}
            
            response(const work::space s) : Result{s}, Return{nullptr}, Error{} {};
            response(const work::space s, ptr<work::item> i) : Result{s}, Return{i}, Error{} {}
            response(struct error e) : Result{}, Return{nullptr}, Error{e} {}
            response(const response& a) : Result{a.Result}, Return{a.Return}, Error(a.Error) {}
            response(response&& a) : Result{a.Result}, Return{a.Return}, Error{a.Error} {
                a.Return = nullptr;
            }
            
            friend struct interpreter;
        };
        
        struct intermediate;
        
        struct open {
            virtual ptr<intermediate> read_name(name) const = 0;
            virtual ptr<intermediate> read_number(N) const = 0;
            virtual ptr<intermediate> read_address(bitcoin::address) const = 0;
            virtual ptr<intermediate> read_pubkey(bitcoin::pubkey) const = 0;
            virtual ptr<intermediate> read_secret(bitcoin::secret) const = 0;
            virtual ptr<open> read_function(function) const = 0;
            virtual ptr<open> read_construction(constructor) const = 0;
            virtual ptr<open> read_parenthesis() const = 0;
            virtual const response response() const = 0;
        };
        
        struct intermediate {
            virtual ptr<open> read_operand(operand) const = 0;
            virtual ptr<open> read_comma() const = 0;
            virtual ptr<open> read_separator() const = 0;
            virtual ptr<open> close() const = 0;
        };
        
        struct name final : public intermediate {
            const cosmos::name Name;
            const work::space Workspace;
            
            name(cosmos::name n, work::space w) : Name{n}, Workspace{w} {}
            
            ptr<open> read_operand(operand) const override;
            ptr<open> read_comma() const override;
            ptr<open> read_separator() const override;
            ptr<open> close() const override;
            
        };
        
        struct number final : public intermediate {
            const N Number;
            const work::space Workspace;
            
            number(N n, work::space w) : Number{n}, Workspace{w} {}
            
            ptr<open> read_operand(operand) const override;
            ptr<open> read_comma() const override;
            ptr<open> read_separator() const override;
            ptr<open> close() const override;
            
        };
        
        struct address final : public intermediate {
            const bitcoin::address Address;
            const work::space Workspace;
            
            address(bitcoin::address a, work::space w) : Address{a}, Workspace{w} {}
            
            ptr<open> read_operand(operand) const override;
            ptr<open> read_comma() const override;
            ptr<open> read_separator() const override;
            ptr<open> close() const override;
            
        };
        
        struct pubkey final : public intermediate {
            bitcoin::pubkey Pubkey;
            work::space Workspace;
            
            pubkey(bitcoin::pubkey p, work::space w) : Pubkey{p}, Workspace{w} {}
            
            ptr<open> read_operand(operand) const override;
            ptr<open> read_comma() const override;
            ptr<open> read_separator() const override;
            ptr<open> close() const override;
            
        };
        
        struct secret final : public intermediate {
            const bitcoin::secret Secret;
            const work::space Workspace;
            
            secret(bitcoin::secret s, work::space w) : Secret{s}, Workspace{w} {}
            
            ptr<open> read_operand(operand) const override;
            ptr<open> read_comma() const override;
            ptr<open> read_separator() const override;
            ptr<open> close() const override;
            
        };
        
        struct interpreter final : public open {
        
            evaluation::response Response;
            
            interpreter(work::space w) : Response{w}, Stack{} {}
            
            const evaluation::response response() const override {
                return Response;
            }

            virtual ptr<intermediate> read_name(cosmos::name n) const override {
                return std::make_shared<intermediate>(new name{n, Response.Result});
            }
            
            virtual ptr<intermediate> read_number(N n) const override {
                return std::make_shared<intermediate>(new number{n, Response.Result});
            }
            
            virtual ptr<intermediate> read_address(bitcoin::address a) const override {
                return std::make_shared<intermediate>(new address{a, Response.Result});
            }
            
            virtual ptr<intermediate> read_pubkey(bitcoin::pubkey p) const override {
                return std::make_shared<intermediate>(new pubkey{p, Response.Result});
            }
            
            virtual ptr<intermediate> read_secret(bitcoin::secret s) const override {
                return std::make_shared<intermediate>(new secret{s, Response.Result});
            }
            
            virtual ptr<open> read_function(function) const override;
            virtual ptr<open> read_construction(constructor) const override;
            virtual ptr<open> read_parenthesis() const override;
        
        private:
            list<ptr<intermediate>> Stack;
        };
        
    }
    
    evaluation::response evaluate(const work::space, const expression *);
    
}

#endif 
