cmake_minimum_required(VERSION 3.9.0)
set(CMAKE_CXX_STANDARD 17)

project (cosmos)

ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# swap this out for other things when we want to work with other coins. 
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../libbitcoin/libbitcoin-consensus/include")

find_package(Boost 1.62.0 COMPONENTS system regex program_options thread log_setup log REQUIRED)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../../boost/1_62_0/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../../gmp-6.1.2/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../../nlohmann/json/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../data/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../wallet-abstractions/include")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../libbitcoin/secp256k1/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../libbitcoin/libbitcoin-system/include")
#[[include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../libbitcoin/libbitcoin-database/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../libbitcoin/libbitcoin-network/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../libbitcoin/libbitcoin-consensus/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../libbitcoin/libbitcoin-blockchain/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../libbitcoin/libbitcoin-node/include")]]

find_package (Threads)

add_compile_definitions(WITH_CONSENSUS)

add_executable(radix

    ../release/quark/quark.cpp
    ../src/cosmos/expression.cpp
    ../src/cosmos/interpreter.cpp
    
    ../../wallet-abstractions/src/abstractions/wallet/wallet.cpp
    
    ../../data/src/data/map/rb.cpp

)

#target_link_libraries(abstractions ${LIBBITCOIN_LIBRARIES})
    
target_link_libraries(radix ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(radix ${Boost_LIBRARIES})
target_link_libraries(radix libsecp256k1.so)
target_link_libraries(radix libbitcoin-system.so)
#[[target_link_libraries(radix libbitcoin-network.so)
target_link_libraries(radix libbitcoin-database.so)
target_link_libraries(radix libbitcoin-consensus.so)]]
target_compile_options(radix PUBLIC -fext-numeric-literals)
