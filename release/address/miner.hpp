#ifndef COSMOS_RELEASE_MINER
#define COSMOS_RELEASE_MINER

#include <cosmos/cosmos.hpp>
#include <data/tools/ordered_list.hpp>
#include <thread>
#include <csignal>

namespace cosmos {
    // an address miner
    namespace miner {
        
        // way of organizing addresses by proof-of-work
        struct address {
            bitcoin::secret Secret;
            bitcoin::pubkey Pubkey;
            bitcoin::address Address;
            
            bool operator<=(const address& a) const {
                return Address.Digest >= a.Address.Digest;
            }
            
            address(bitcoin::secret s) : Secret{s}, Pubkey{s.to_public()}, Address{Pubkey.address()} {}
            
            bool read(address& a, istream& i);
            void write(ostream& o);
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
            
            bool read(addresses& a, istream& i);
            void write(ostream& o);
        };
        
        // current state of the program. 
        struct state {
            uint32 Rounds;
            uint32 Increment;
            addresses Addresses;
            bitcoin::secret Next;
            std::string Error;
            
            state() {}
            state(const std::string& e) : Error{e} {}
            state(uint32 r, uint32 i, addresses a, bitcoin::secret n) : Rounds{r}, Increment{i}, Addresses{a}, Next{n} {}
            
            void round() {
                Addresses = Addresses.update(Next);
                Next.Secret.Value += Increment;
            }
        
            // read in program state from user input.
            static state create(list<std::string> cmd, std::istream& in);
        };
        
        enum command {
            none = 0,
            stop = 1
        };
        
        // work until a command is received to stop. 
        state work(state s, data::channel<command> user) {
            while (true) {
                for (uint32 i = 0; i < s.Rounds; i++) s.round();
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
        
        state run(state s, data::channel<command> user) {
            try {
                return work(s, user);
            } catch (std::exception& e) {
                return state{e.what()};
            } catch (...) {
                return state{"unknown error"};    
            }
        }
        
        class running {
            struct program {
                data::channel<command> User;
                void operator()(state s);
            };
        
            std::thread Program;
            data::channel<command> User;
            
            running(state s, data::channel<command> user) : Program{program{}, s, User} {}
        public:
            static running* run(state s) {
                return new running{s, data::channel<command>{}};
            }
            
            state stop() {
                User.put(command::stop);
                Program.join();
            }
        };
        
        void on_ctrl_c(int);

        data::program::output run(int argc, char* argv[]);
        
    };
} 

#endif
