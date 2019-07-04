#include <cosmos/cosmos.hpp>
#include <data/encoding/ascii.hpp>
#include <abstractions/script/pow.hpp>
#include <abstractions/script/pay_to_address.hpp>
#include <abstractions/pattern/pay_to_address.hpp>
#include <iostream>

namespace cosmos {
    
    namespace pow {
            
        class error : std::exception {
            std::string Message;
                
        public:
            error(std::string s) : Message{s} {}
            
            const char* what() const noexcept final override {
                return Message.c_str();
            }
        };
        
        abstractions::bitcoin::pay_to_address p2pkh{};
        
        uint read_uint(const std::string&);
        
        byte read_byte(const std::string&);
        
        abstractions::work::uint24 read_uint24(const std::string&);
        
        inline bitcoin::secret read_wif(const std::string& s) {
            bitcoin::secret k{s};
            if (!k.valid()) throw error{"invalid wif"};
        }
        
        using ascii = data::encoding::ascii::string;
        
        inline ascii read_ascii(const std::string& s) {
            data::encoding::ascii::string z{s};
            if (!z.valid()) throw error{"Cannot read ascii string"};
            return z;
        }
    
        bitcoin::address read_address_from_script(const bytes& b) {
            struct abstractions::script::pay_to_address p{b};
            if (!p.valid()) throw error{"invalid pay-to-address script"};
            return p.Address;
        }
        
        inline bitcoin::work::candidate candidate(
            const ascii& message, 
            bitcoin::work::target target) {
            return bitcoin::work::candidate{abstractions::work::reference(bytes(message)), target};
        }
        
        inline bitcoin::work::target read_target(
            const std::string& exponent,
            const std::string& value) {
            return bitcoin::work::target{read_byte(exponent), read_uint24(value)};
        }
        
        inline bitcoin::script lock(bitcoin::work::candidate c) {
            return abstractions::script::lock_by_pow(c)->compile();
        }
        
        bitcoin::transaction main(
            bitcoin::transaction prev,
            bitcoin::outpoint ref, 
            bitcoin::secret key,
            ascii message, 
            bitcoin::work::target target)
        {
            if (ref.Reference != prev.id()) throw error{"invalid reference to previous tx"};
            bitcoin::transaction::representation previous{prev};
            if (!previous.valid()) throw error{"invalid tx"};
            if (previous.Outputs.size() >= ref.Index) throw error{"No such output in previous tx"};
                
            bitcoin::output redeemed{previous.Outputs[ref.Index]};
            
            bitcoin::address address = read_address_from_script(redeemed.ScriptPubKey);
            if (!address.valid()) throw error{"invalid output script"};
            if (address != bitcoin::address{key}) throw error{"cannot redeem address with key"};
            
            bitcoin::vertex vertex{bitcoin::vertex{{bitcoin::vertex::spendable{key, redeemed, ref}}, {bitcoin::output{redeemed.Value, lock(candidate(message, target))}}}};
            
            return bitcoin::redeem({p2pkh}, vertex);
        }

        struct program {
            bitcoin::transaction Previous;
            bitcoin::outpoint Reference;
            bitcoin::secret Key;
            ascii Message;
            bitcoin::work::target Target;
            
            program(
                bitcoin::transaction t, 
                bitcoin::outpoint r, 
                bitcoin::secret k, 
                ascii m, 
                bitcoin::work::target t) : Previous{t}, Reference{r}, Key{k}, Message{m}, Target{t} {}
        
            static program make(list<std::string> input) {
                if (input.size() != 6) throw error{"six inputs required"};
                bitcoin::transaction p{input[0]};
                if (!p.valid()) throw error{"transaction is not valid"};
                return program{p, bitcoin::outpoint{p.id(), read_uint(input[1])}, read_wif(input[2]), read_ascii(input[3]), 
                read_target(input[4], input[5])};
            }
            
            bool valid() const {
                return Previous.valid() && Key.valid();
            }
            
            bitcoin::transaction operator()() {
                return pow::main(Previous, Reference, Key, Message, Target);
            };
        };
    
    }

    list<std::string> read_input(int argc, char* argv[]) {
        list<std::string> l{};
        for (int i = 0; i < argc; i++) l = l + std::string(argv[i]);
        return l;
    }
    
}
        
int main(int argc, char* argv[]) {
    using namespace cosmos;
    using namespace std;
    
    std::string out;
    
    try {
        out = data::encoding::hex::write(pow::program::make(read_input(argc, argv))());
    } catch (std::exception& e) {
        out = e.what();
    }
    
    cout << out;
    
    return 0;
}

