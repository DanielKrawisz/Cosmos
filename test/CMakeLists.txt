cmake_minimum_required(VERSION 3.1...3.14)



# Back compatibility for VERSION range

if(${CMAKE_VERSION} VERSION_LESS 3.12)

    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})

endif()





ADD_EXECUTABLE(testCosmos  testLib.cpp )

target_include_directories(testCosmos PUBLIC .)

target_link_libraries(testCosmos wallet-abstractions ${CRYPTOPP_LIBRARIES} ${Boost_LIBRARIES} ${LIB_BITCOIN_LIBRARIES} data gmock_main)

get_target_property(OUT testCosmos LINK_LIBRARIES)

message(STATUS ${OUT})
