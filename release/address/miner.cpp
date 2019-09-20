#include "miner.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

namespace cosmos::bitcoin {
    using json = nlohmann::json;
    
    chan<miner::state> c{};
        
    miner::running* worker{};
    
    void miner::running::program::operator()(state s) {
        c.put(miner::run(s, User));
    }
        
    void on_ctrl_c(int) {
        if (worker != nullptr) c.put(worker->stop());
        else c.put(miner::state{"main loop not started"});
        c.close();
        delete worker;
    }

    /*data::program::output miner::operator()(int argc, char* argv[]) {
        list<std::string> inputs = data::program::list_input_parser<list<std::string>>{}(argc, argv));
        // TODO open file here. 
        worker = running::run(state::restore());
        signal(SIGINT, on_ctrl_c);
        state end;
        c.get(end);
    }*/
    
    std::vector<json> save_addresses(data::ordered_list<miner::address> l) {
        std::vector<json> j{l.size()};
        for (int i = 0; i < j.size(); i++) {
            j[i]["key"] = l.first().Secret.write();
            j[i]["address"] = l.first().Address.write();
            l = l.rest();
        }
        return j;
    }
    
    /*data::ordered_list<miner::address> restore_addresses(json& j) {
        data::ordered_list<address> l;
        for (json::iterator it = j.begin(); it != j.end(); ++it) {
            address a{(*it)["key"]};
            if (a.valid()) l = l.insert(a);
        }
        
        return l;
    }*/
    
    void miner::state::save(std::ostream& out) const {
        json j;
        j["increment"] = std::to_string(Increment);
        j["max_size"] = Addresses.MaxSize;
        j["keys"] = save_addresses(Addresses.List);
        out << j;
    };
    
    /*miner::state miner::state::restore(std::istream& disk) {
        json z;
        disk >> z;
        std::string wif;
        std::cout << "provide entropy for next key: "; 
        std::cin >> wif;
        bitcoin::secret next{wif};
        return state{std::stoul(z["increment"]), addresses{std::stdoul(z["max_size"]), restore_adresses(z["keys"])}, next};
    }*/
    
} 

int main(int argc, char* argv[]) {
    return data::program::environment::main{data::program::catch_all<cosmos::bitcoin::miner>{}}(argc, argv);
}
