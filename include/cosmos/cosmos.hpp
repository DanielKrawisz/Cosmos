#ifndef COSMOS_COSMOS
#define COSMOS_COSMOS

#include <string>
#include <vector>
    
#include <boost/filesystem.hpp>

#include <data/io/main.hpp>
#include <data/io/unimplemented.hpp>

#include <data/list/linked_list.hpp>
#include <data/map/rb.hpp>

#include <data/crypto/sha256.hpp>

#include <data/math/number/gmp/N.hpp>

#include <abstractions/wallet/address.hpp>
#include <abstractions/wallet/keys.hpp>
#include <abstractions/wallet/input.hpp>
#include <abstractions/wallet/output.hpp>
#include <abstractions/wallet/transaction.hpp>

namespace cosmos {
    
    using byte = uint8_t;
    
    using string = const std::string;
    
    template <typename X>
    using vector = std::vector<X>;
    
    using bytes = vector<byte>;
    
    using ostream = std::ostream;
    using istream = std::istream;
    
    using stringstream = std::stringstream;
    
    template <typename X>
    using ptr = std::shared_ptr<X>;
    
    template <typename X>
    using list = data::linked_list<X>;
    
    template <typename X, typename Y>
    using map = data::rb_map<X, Y>;
    
    using N = data::math::number::gmp::N;
    
    // Bitcoin primitives. 
    namespace bitcoin {
        using satoshi = uint32_t;
        using index = uint32_t;
        using address = abstractions::bitcoin::address;
        using pubkey = abstractions::bitcoin::pubkey;
        using secret = abstractions::bitcoin::secret;
        using txid = abstractions::bitcoin::txid;
        using digest = data::sha256::digest;
        using script = bytes;
        
        using outpoint = abstractions::bitcoin::outpoint;
        using input = abstractions::bitcoin::input;
        using output = abstractions::bitcoin::output;
        using transaction = abstractions::bitcoin::transaction;
    }
    
    struct writable {
        
        struct formats {
            virtual void text(stringstream&) const = 0;
            virtual void json(stringstream&) const = 0;
        };
        
        virtual const formats& write() const = 0;
    };
        
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
