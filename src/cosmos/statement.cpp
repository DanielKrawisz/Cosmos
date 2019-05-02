#include <cosmos/statement.hpp>

namespace cosmos {
    
    bool statement::expression::valid() const {
        return (Atomic != nullptr && Construction == nullptr && Operation == nullptr && Atomic->valid())
            || (Construction != nullptr && Atomic == nullptr && Operation == nullptr && Construction->valid())
            || (Operation != nullptr && Atomic == nullptr && Construction == nullptr && Operation->valid());
    }
        
    bool statement::single::valid() const {
        return (Expression == nullptr && Set != nullptr)
            || (Expression != nullptr && Set == nullptr);
    };
        
    bool statement::valid() const {
        return (Compound != nullptr && Single == nullptr && Compound->valid())
            || (Compound == nullptr && Single != nullptr && Single->valid());
    }
    
}

