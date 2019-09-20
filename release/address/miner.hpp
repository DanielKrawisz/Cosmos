#ifndef COSMOS_RELEASE_MINER
#define COSMOS_RELEASE_MINER

#include <cosmos/cosmos.hpp>
#include <data/tools/ordered_list.hpp>
#include <thread>
#include <csignal>

namespace cosmos::bitcoin {
    // an address miner
    struct miner {
        
        // way of organizing addresses by proof-of-work
        struct address {
            secret Secret;
            pubkey Pubkey;
            bitcoin::address Address;
            
            bool valid() {
                return Secret.valid() && Pubkey.valid() && Address.valid();
            }
            
            bool operator<=(const address& a) const {
                return Address.Digest >= a.Address.Digest;
            }
            
            address(secret s) : Secret{s}, Pubkey{s.to_public()}, Address{Pubkey.address()} {}
            address(std::string& wif) : address(secret{wif}) {}
        };
        
        // ordered list of addresses. 
        struct addresses {
            uint32 MaxSize;
            data::ordered_list<address> List;
            
            addresses update(address next) const {
                if (next <= List.first()) return {MaxSize, List.rest().insert(next)};
                return *this;
            }
            
            address min() const {
                return List.last();
            }
            
            address max() const {
                return List.first();
            }
        };
        
        // current state of the program. 
        struct state {
            uint32 Increment;
            addresses Addresses;
            secret Next;
            std::string Error;
            
            state() {}
            state(const std::string& e) : Error{e} {}
            state(uint32 i, addresses a, secret n) : Increment{i}, Addresses{a}, Next{n} {}
            
            void round(); /*{
                Addresses = Addresses.update(Next);
                Next.Secret.Value += Increment;
            }*/
        
            // read in program state from user input.
            static state restore(std::istream& disk);
            void save(std::ostream& out) const;
        };
        
        enum command {
            none = 0,
            stop = 1
        };
        
        // work until a command is received to stop. 
        static state work(state s, data::channel<command> user) {
            const uint32 rounds = 10000;
            while (true) {
                for (uint32 i = 0; i < rounds; i++) s.round();
                command c;
                if (user.get(c, false))
                    switch (c) {
                        default: 
                        case command::none:
                            break;
                        case command::stop:
                            return s;
                    }
            }
        }
        
        static state run(state s, data::channel<command> user) {
            try {
                return work(s, user);
            } catch (std::exception& e) {
                return state{std::string{e.what()}};
            } catch (...) {
                return state{std::string{"unknown error"}};    
            }
        }
        
        class running {
            struct program {
                data::channel<command> User;
                void operator()(state s);
            };
        
            std::thread Program;
            data::channel<command> User;
            
            running(state s, data::channel<command> user) : Program{program{user}, s} {}
        public:
            static running* run(state s) {
                return new running{s, data::channel<command>{}};
            }
            
            state stop() {
                User.put(command::stop);
                Program.join();
                return {}; // TODO put what ought to be here. 
            }
        };

        data::program::output operator()(int argc, char* argv[]);
        
    };
} 

#endif
