#include <cosmos/cosmos.hpp>
#include <data/encoding/ascii.hpp>
#include <abstractions/script/pow.hpp>
#include <abstractions/script/pay_to_address.hpp>
#include <abstractions/pattern/pay_to_address.hpp>
#include <abstractions/crypto/address.hpp>
#include <iostream>

namespace cosmos::bitcoin {
    
    namespace pow {
            
        class error : std::exception {
            std::string Message;
                
        public:
            error(std::string s) : Message{s} {}
            
            const char* what() const noexcept final override {
                return Message.c_str();
            }
        };
        
        // patterns recognized by this wallet (only one for now) 
        abstractions::pattern::pay_to_address<secret, pubkey, address, transaction> p2pkh{};
        
        uint read_uint_dec(const std::string&);
        
        byte read_byte_dec(const std::string&);
        
        satoshi read_satoshi_amount(const std::string&);
        
        abstractions::work::uint24 read_uint24_dec(const std::string&);
        
        inline const secret read_wif(const std::string& s) {
            secret k{s};
            if (!k.valid()) throw error{"invalid wif"};
        }
        
        using ascii = data::encoding::ascii::string;
        
        inline const ascii read_ascii(const std::string& s) {
            data::encoding::ascii::string z{&s};
            if (!z.valid()) throw error{"Cannot read ascii string"};
            return z;
        }
        
        inline const address read_address(const std::string& s) {
            address a{s};
            if (!a.valid()) throw error{"invalid address"};
            return a;
        }
    
        inline const address read_address_from_script(const bytes& b) {
            struct abstractions::script::pay_to_address p{b};
            if (!p.valid()) throw error{"invalid pay-to-address script"};
            return p.Address;
        }
        
        inline const work::target read_target(
            const std::string& exponent,
            const std::string& value) {
            return work::target{read_byte_dec(exponent), read_uint24_dec(value)};
        }
        
        inline const script pow_lock(work::message m, work::target t) {
            return abstractions::script::lock_by_pow(m, t)->compile();
        }
        
        inline const output pow_lock_output(
            satoshi s, 
            work::message m, 
            work::target t) {
            return output{s, pow_lock(m, t)};
        }
        
        inline const output pay_to_address_output(satoshi s, address a) {
            return output{s, abstractions::script::pay_to(a)->compile()};
        }   
        
        const transaction main(
            const list<spendable> outputs, 
            const ascii data, 
            const satoshi spend, 
            const work::target target, 
            const address change, 
            const satoshi fee);
        /*{
            list<spendable> to_be_redeemed{};
            satoshi redeemed_value = 0;
            for (spendable o : outputs) {
                if (!o.valid()) throw error{"invalid reference to previous tx"};
                address address = read_address_from_script(o.Spendable.Output.ScriptPubKey);
                if (!address.valid()) throw error{"invalid output script"};
                if (address != address{o.Spendable.Key}) throw error{"cannot redeem address with key"};
                to_be_redeemed = to_be_redeemed + o.Spendable;
                redeemed_value += o.Spendable.Output.Value;
            }
            
            return redeem({p2pkh},
                vertex{to_be_redeemed, {
                    abstractions::bitcoin::op_return{bytes(data)},
                    pow_lock_output(spend, abstractions::work::reference(hash(data)), target), 
                    pay_to_address_output(redeemed_value - fee, change)}});
        }*/

        class program {
            list<spendable> Previous;
            ascii Data;
            satoshi Spend;
            work::target Target;
            address Change;
            satoshi Fee; 
            
            program(
                transaction tx, 
                outpoint r, 
                secret k, 
                ascii d, 
                satoshi s, 
                work::target t, 
                address c, 
                satoshi f) : Previous{{tx, r, k}}, Data{d}, Target{t}, Change{c}, Fee{f} {}
        
        public:
            // Transform intput into constructed types.
            static const program make(const list<std::string> input) {
                if (input.size() != 9) throw error{"nine inputs required"};
                transaction p{input[0]};
                if (!p.valid()) throw error{"transaction is not valid"};
                return program{p, outpoint{p.id(), read_uint_dec(input[1])}, 
                    read_wif(input[2]), read_ascii(input[3]), read_satoshi_amount(input[4]), 
                    read_target(input[5], input[6]), read_address(input[7]), read_satoshi_amount(input[8])};
            }
            
            bool valid() const {
                return data::fold([](bool p, spendable x)->bool{return p && x.valid();}, 0, Previous);
            }
            
            const bitcoin::transaction operator()() const {
                const bitcoin::transaction tx{pow::main(Previous, Data, Spend, Target, Change, Fee)};
                if (!tx.valid()) throw error{"invalid tx was produced"};
                auto p = Previous;
                auto i = bitcoin::transaction::representation{tx}.Inputs;
                uint n = 0;
                while(!p.empty()) {
                    bitcoin::output o = p.first().Spendable.Output;
                    if (!bitcoin::machine{tx, n, o.Value}.run(o.ScriptPubKey, i.first().ScriptSignature))
                        throw error{"redemption script not valid"};
                    p = p.rest();
                    i = i.rest();
                    n++;
                }
                
                // TODO test whether pow_lock object can be reconstructed from the script. 
                // TODO test redeem tx (optional; only for small pow targets).
                return tx;
            };
        };
    
    }

    const list<std::string> read_input(int argc, char* argv[]) noexcept {
        list<std::string> l{};
        for (int i = 0; i < argc; i++) l = l + std::string(argv[i]);
        return l;
    }
    
}

namespace cosmos {

    string run(const list<std::string> input) noexcept {
        try {
            return data::encoding::hex::write(bitcoin::pow::program::make(input)());
        } catch (std::exception& e) {
            return e.what();
        } catch (...) {
            return "unknown error.";
        }
    }
    
}
        
int main(int argc, char* argv[]) {
    std::cout << cosmos::run(cosmos::read_input(argc, argv));
    return 0;
}
