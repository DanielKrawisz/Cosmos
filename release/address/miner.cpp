#include "miner.hpp"

namespace cosmos {
        
    chan<miner::state> c{};
        
    miner::running* worker{};
    
    void miner::running::program::operator()(state s) {
        c.put(run(s, User));
    }
        
    void on_ctrl_c(int) {
        if (worker != nullptr) c.put(worker->stop());
        else c.put(miner::state{"main loop not started"});
        c.close();
        delete worker;
    }

    data::program::output miner::operator()(int argc, char* argv[]) {
        worker = running::run(state::create(data::program::list_input_parser<list<std::string>>{}(argc, argv)));
        signal(SIGINT, on_ctrl_c);
        state end;
        c.get(end);
    }
    
} 

int main(int argc, char* argv[]) {
    return data::program::environment::main{data::program::catch_all<cosmos::miner>{}}(argc, argv);
}
