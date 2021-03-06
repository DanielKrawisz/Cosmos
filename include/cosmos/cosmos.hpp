// Copyright (c) 2019 Daniel Krawisz
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef COSMOS_COSMOS
#define COSMOS_COSMOS

#include <string>
#include <vector>
    
#include <boost/filesystem.hpp>

#include <data/io/main.hpp>
#include <data/io/unimplemented.hpp>

#include <data/function.hpp>
#include <data/encoding/hex.hpp>

#include <data/data.hpp>

#include <abstractions/wallet/machine.hpp>
#include <abstractions/transaction.hpp>
#include <abstractions/wallet/wallet.hpp>
#include <abstractions/wallet/spendable.hpp>
#include <abstractions/work/work.hpp>

namespace cosmos {
    
    using namespace data::exported;
    
    using byte = uint8_t;
    using uint32 = uint32_t;
    
    using string = const std::string;
    
    template <typename X>
    using vector = std::vector<X>;
    
    using bytes = vector<byte>;
    
    using ostream = std::ostream;
    using istream = std::istream;
    
    using stringstream = std::stringstream;
    
    template <typename X>
    using ptr = std::shared_ptr<X>;
    
    using unconstructable = data::unconstructable;
    
    // Bitcoin primitives. 
    namespace bitcoin {
        using satoshi = uint32_t;
        using index = uint32_t;
        using address = abstractions::bitcoin::address;
        using pubkey = abstractions::bitcoin::pubkey;
        using secret = abstractions::bitcoin::secret;
        using txid = abstractions::bitcoin::txid;
        using digest = data::sha256::digest;
        using script = abstractions::bitcoin::script;
        
        using outpoint = abstractions::bitcoin::outpoint;
        using input = abstractions::bitcoin::input;
        using output = abstractions::bitcoin::output;
        using transaction = abstractions::bitcoin::transaction;
        
        using machine = abstractions::bitcoin::machine;
        using wallet = abstractions::bitcoin::wallet;
        
        namespace work {
            using target = abstractions::work::target;
            using message = abstractions::work::message;
            using candidate = abstractions::work::candidate;
        }
        
        using pattern = abstractions::bitcoin::pattern;
        using spendable = abstractions::bitcoin::spendable;
    }
        
    // functions understood by this machine. 
    enum function {
        identity = 0,
        SHA256 = 1, 
        SHA512 = 2, 
        concatinate = 3, 
        address = 3, 
        public_key = 4, 
        update = 5,
        spend = 6,
        next_address = 7, 
        evaluate_script = 8
    };
    
    enum op {
        equal = 0,
        plus = 1, 
        times = 2,
        less = 3, 
        greater = 4, 
        less_equal = 5,
        greater_equal = 6, 
        concat = 7, 
        set = 8
    };
    
    enum constructor {
        outpoint = 1, 
        input = 2, 
        output = 3, 
        transaction = 4, 
        wallet = 6
    };
    
    namespace file {
        using path = boost::filesystem::path;
    }
    
    namespace program {
        using output = data::program::output;
    }
    
    namespace exception {
        using unimplemented = data::method::unimplemented;
        
        struct invalid_operation final : std::exception {
            const char* what() const noexcept final override {
                return "invalid operation";
            }
        };
    }
    
};

#endif 
