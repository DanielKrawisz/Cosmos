#include <cosmos/cosmos.hpp>
#include <data/encoding/ascii.hpp>
#include <iostream>

namespace cosmos {
    
    bitcoin::address read_address_from_script(const bytes&);
    uint read_uint(const std::string&);
    bitcoin::secret read_wif(const std::string&);
    bitcoin::work::candidate candidate(data::encoding::ascii::string message, uint exponent, uint value);
    
    namespace pow {
            
        class error : std::exception {
            std::string Message;
                
        public:
            error(std::string s) : Message{s} {}
            
            const char* what() const noexcept final override {
                return Message.c_str();
            }
        };
        
        bitcoin::script lock(bitcoin::work::candidate);
        
        bitcoin::transaction main(
            bitcoin::transaction prev,
            bitcoin::outpoint ref, 
            bitcoin::secret key,
            bitcoin::work::candidate candidate)
        {
            if (ref.Reference != prev.hash()) throw error{"invalid reference to previous tx"};
            bitcoin::transaction::representation previous{prev};
            if (!previous.Valid) throw error{"invalid tx"};
            if (previous.Outputs.size() >= ref.Index) throw error{"No such output in previous tx"};
                
            bitcoin::output redeemed{previous.Outputs[ref.Index]};
            
            bitcoin::address address = read_address_from_script(redeemed.ScriptPubKey);
            if (!address.valid()) throw error{"invalid output script"};
            if (address != bitcoin::address{key}) throw error{"cannot redeem address with key"};
            
            bitcoin::vertex vertex{bitcoin::vertex{{bitcoin::vertex::spendable{key, redeemed, ref}}, {bitcoin::output{redeemed.Value, lock(candidate)}}}};
            
            return bitcoin::redeem({}, vertex);
        }

        struct program {
            bitcoin::transaction Previous;
            bitcoin::outpoint Reference;
            bitcoin::secret Key;
            bitcoin::work::candidate Candidate;
            
            program(
                bitcoin::transaction t, 
                bitcoin::outpoint r, 
                bitcoin::secret k, 
                bitcoin::work::candidate c) : Previous{t}, Reference{r}, Key{k}, Candidate{c} {}
        
            program(list<std::string> input) {
                if (input.size() != 6) throw error{"six inputs required"};
                Previous = bitcoin::transaction{input[0]};
                if (!Previous.valid()) throw error{"transaction is not valid"};
                Reference = bitcoin::outpoint{Previous.hash(), read_uint(input[1])};
                Key = read_wif(input[2]);
                Candidate = candidate(input[3], input[4], input[5]);
            }
            
            bitcoin::transaction operator()() {
                return pow::main(Previous, Reference, Key, Candidate);
            };
        };
    
    }

    list<std::string> read_input(int argc, char* argv[]);
    
}
        
int main(int argc, char* argv[]) {
    using namespace cosmos;
    using namespace std;
    
    std::string out;
    
    try {
        out = data::encoding::hex::write(pow::program{read_input(argc, argv)}());
    } catch (cosmos::pow::error e) {
        out = e.what();
    }
    
    cout << out;
    
    return 0;
}

