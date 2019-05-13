// Copyright (c) 2019 Daniel Krawisz
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef COSMOS_EVALUATION_OPERATORS
#define COSMOS_EVALUATION_OPERATORS

#include <cosmos/cosmos.hpp>
#include <cosmos/name.hpp>

namespace cosmos {
    
    template <typename x, op o, typename y> struct operation {
        operation() {
            throw exception::invalid_operation{};
        }
        
        unconstructable operator()(x a, y b) {
            throw exception::invalid_operation{};
        }
    };
    
    template <> struct operation<N, plus, N> {
        N operator()(N n, N m) {
            return n + m;
        }
    };
    
    template <> struct operation<N, times, N> {
        N operator()(N n, N m) {
            return n * m;
        }
    };

    template <> struct operation<bitcoin::secret, plus, bitcoin::secret> {
        bitcoin::secret operator()(bitcoin::secret a, bitcoin::secret b) {
            return a + b;
        }
    };

    template <> struct operation<bitcoin::secret, times, bitcoin::secret> {
        bitcoin::secret operator()(bitcoin::secret a, bitcoin::secret b) {
            return a * b;
        }
    };
    
    template <> struct operation<bitcoin::pubkey, plus, bitcoin::pubkey> {
        bitcoin::pubkey operator()(bitcoin::pubkey a, bitcoin::pubkey b) {
            return a + b;
        }
    };
    
    template <> struct operation<bitcoin::pubkey, times, bitcoin::secret> {
        bitcoin::pubkey operator()(bitcoin::pubkey a, bitcoin::secret b) {
            return a * b;
        }
    };
    
    template <> struct operation<bitcoin::script, concat, bitcoin::script> {
        bitcoin::script operator()(bitcoin::script a, bitcoin::script b);
    };
    
    template <typename x> struct operation<name, set, x> {
        x operator()(name n, x val); // What to do here? 
    };
};

#endif 
