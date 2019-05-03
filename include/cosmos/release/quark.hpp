
#include <cosmos/interpreter.hpp>
#include <data/io/main.hpp>
#include <data/fold.hpp>

namespace cosmos {
    
    // version quark reads the input to the program
    // as instructions about an empty wallet. 
    namespace quark {
    
        string concatinate(list<string> x) {
            return (string)(data::reduce([](string s, string v)->string{return s + v;}, x));
        } 
    
        data::program::output main(list<string> x) {
            return {text(evaluate({}, concatinate(x)))};
        }
    }
    
}

int main(int argc, char*[] argv) {
    return data::program::main(cosmos::quark::main, data::program::list_input_parser)(argc, argv);
}
