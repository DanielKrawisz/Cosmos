#include <cosmos/interpreter.hpp>

namespace cosmos {
        
    interpreter::response interpreter::intpretet(work::space w, statement::expression& s){
        if (s.Atomic != nullptr) return interpret(w, *s.Atomic);
        if (s.Construction != nullptr) return interpret(w, *s.Construction);
        if (s.Construction != nullptr) return interpret(w, *s.Construction);
        return error::format();
    }
    
    interpreter::response interpreter::intpretet(work::space w, statement::set_statement& s){
        response r = interpret(w, s.Predicate);
        if (!r.error()) return r;
        work::space result = r.Result.set(s.Name, *r.Return);
        if (result.valid()) return {result, r.Return};
        return error::unrecognized_name(s.Name);
    }
    
    interpreter::response interpreter::intpretet(work::space w, statement::single& s){
        if (s.Expression != nullptr) return interpret(w, *s.Expression);
        if (s.Set != nullptr) return interpret(w, *s.Set);
        return error::format();
    }
    
    interpreter::response interpreter::interpret(work::space w, statement& s) {
        if (s.Compound != nullptr) return interpret(w, *s.Compound);
        if (s.Single != nullptr) return interpret(w, *s.Single);
        return error::format();
    }
    
}
