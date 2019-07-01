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
        
        struct close;
        
        // representation of states of evaluation which are ready
        // for new input and have possibly generated a response.
        struct open {
            work::space Workspace;
            cosmos::list<ptr<open>> Stack;
            
            open(work::space w, cosmos::list<ptr<open>> s) : Workspace{w}, Stack{s} {}
            open(work::space w) : Workspace{w}, Stack{} {}
            
            virtual ptr<close> read_name(cosmos::name n) const;
            virtual ptr<close> read_number(N n) const;
            virtual ptr<close> read_address(bitcoin::address a) const;
            virtual ptr<close> read_pubkey(bitcoin::pubkey p) const;
            virtual ptr<close> read_secret(bitcoin::secret s) const;
            
            ptr<open> read_function(cosmos::function) const;
            ptr<open> read_construction(constructor) const;
            ptr<open> read_parenthesis() const;
            
            virtual ~open() = 0;
        };
        
        // type generated at the start of an evaluation before 
        // any input has been read. 
        struct interpreter final : public open {
            
            interpreter(work::space w) : open{w} {}

        };
        
        // representation of states of evaluation which necessarily
        // require more input before a response can be generated. 
        struct close {
            response Response;
            cosmos::list<ptr<open>> Stack;
            
            close(response r, cosmos::list<ptr<open>> stack) : Response{r}, Stack{stack} {}
            
            ptr<open> next() const;
            ptr<open> close_structure() const;
            virtual ptr<open> read_operand(op) const = 0;
        };
        
        template <typename A>
        struct atom final : public close {
            ptr<work::atom<A>> Atom;
            
            atom(ptr<work::atom<A>> a, work::space w, list<ptr<open>> stack) : close{response{w, a}, stack}, Atom{a} {}
            
            ptr<open> read_operand(op) const override;
            
            inline static ptr<atom> make(A a, work::space w, list<ptr<open>> stack) {
                return std::make_shared<atom>(new atom{std::make_shared<work::atom<A>>(new work::atom<A>{a}), w, stack});
            }
        };
        
        template <typename A, op o, typename B> 
        struct operation final : public close {
            atom<A> Left;
            atom<B> Right;
            cosmos::operation<A, o, B> Operation;
            
            operation(atom<A> a, atom<B> b) : Left{a}, Right{b}, Operation{} {}
            
            ptr<open> read_operand(op) const override;
        };
        
        template <typename A, op o>
        struct operand final : public open {
            ptr<work::atom<A>> Left;
            
            operand(ptr<work::atom<A>> a, work::space w, list<ptr<open>> stack) : open{w, stack}, Left{a} {}
            
            ptr<close> read_name(name n) const override;
            ptr<close> read_number(N n) const override;
            ptr<close> read_address(bitcoin::address a) const override;
            ptr<close> read_pubkey(bitcoin::pubkey p) const override;
            ptr<close> read_secret(bitcoin::secret s) const override;
            
        };
        
        struct parenthesis final : public open {
            const open* Previous;
            parenthesis(work::space w, const open* p, list<ptr<open>> s) : open{w, s}, Previous{p} {}
            parenthesis(work::space w, const open* p) : open{w}, Previous{p} {}
        };
        
        struct sequence : public open {
            list<ptr<work::item>> Sequence;
            
            sequence(list<ptr<work::item>> s, work::space w, list<ptr<open>> ss)
                : open{w, ss}, Sequence{s} {}
            
            ptr<sequence> next(ptr<work::item> p) const {
                return std::make_shared<sequence>(new sequence{Sequence.prepend(p), Workspace, Stack});
            }
        };
        
        struct list final : public sequence {};
        
        struct function final : public sequence {
            cosmos::function Function;
        };
        
        struct construction : public sequence {
            cosmos::constructor Constructor;
        };
            
        inline ptr<close> open::read_name(cosmos::name n) const {
            return atom<cosmos::name>::make(n, Workspace, Stack);
        }
        
        inline ptr<close> open::read_number(N n) const {
            return atom<N>::make(n, Workspace, Stack);
        }
        
        inline ptr<close> open::read_address(bitcoin::address a) const {
            return atom<bitcoin::address>::make(a, Workspace, Stack);
        }
        
        inline ptr<close> open::read_pubkey(bitcoin::pubkey p) const {
            return atom<bitcoin::pubkey>::make(p, Workspace, Stack);
        }
        
        inline ptr<close> open::read_secret(bitcoin::secret s) const {
            return atom<bitcoin::secret>::make(s, Workspace, Stack);
        }
        
        inline ptr<open> open::read_parenthesis() const {
            return std::make_shared<open>(new parenthesis{Workspace, this, Stack});
        }
        
        open::~open() {};
        
        template <typename A, op o>
        inline ptr<close> operand<A, o>::read_name(name n) const {
            return std::make_shared<close>(new operation<A, o, name>(Left, n));
        }
        
        template <typename A, op o>
        inline ptr<close> operand<A, o>::read_number(N n) const {
            return std::make_shared<close>(new operation<A, o, N>(Left, n));
        }
        
        template <typename A, op o>
        inline ptr<close> operand<A, o>::read_address(bitcoin::address a) const {
            return std::make_shared<close>(new operation<A, o, bitcoin::address>(Left, a));
        }
        
        template <typename A, op o>
        inline ptr<close> operand<A, o>::read_pubkey(bitcoin::pubkey p) const {
            return std::make_shared<close>(new operation<A, o, bitcoin::pubkey>(Left, p));
        }
        
        template <typename A, op o>
        inline ptr<close> operand<A, o>::read_secret(bitcoin::secret s) const {
            return std::make_shared<close>(new operation<A, o, bitcoin::secret>(Left, s));
        }
        
    }
    
    evaluation::response evaluate(const work::space, stringstream& s);
    
}

#endif 
