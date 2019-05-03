#include <cosmos/expression.hpp>
#include <data/fold.hpp>

namespace cosmos {
    
    bool expression::valid(expression::parameters Parameters) {
        return data::fold([](bool b, ptr<expression> p)->bool{
                return b && p != nullptr && p->valid();
            }, true, Parameters);
    }
    
    expression::compound::~compound() {}

}
