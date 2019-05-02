#ifndef COSMOS_INTERPRETER
#define COSMOS_INTERPRETER

#include "statement.hpp"

namespace cosmos {
    
    struct interpreter {
        
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
                return Error != interpreter::error{};
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
            response(response&& a) : Result{a.Result}, Return{a.Return}, Error{a.Error} {
                a.Return = nullptr;
            }
            
            response(const work::space s) : Result{s}, Return{nullptr}, Error{} {};
            response(const work::space s, ptr<work::item> i) : Result{s}, Return{i}, Error{} {}
            response(struct error e) : Result{}, Return{nullptr}, Error{e} {}
            
            friend struct interpreter;
        };
        
        static response intpretet(work::space w, statement::expression& s);
        
        static response intpretet(work::space w, statement::set_statement& s);
        
        static response intpretet(work::space w, statement::single& s);
        
        static response interpret(work::space w, statement::compound& s);
        
        static response interpret(work::space w, statement& s);
            
    };
    
}

#endif 
