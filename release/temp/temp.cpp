// Copyright (c) 2019 Katrina Swales
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include <iostream>
#include <data/encoding/hex.hpp>
#include <data/encoding/invalid.hpp>
#include <abstractions/wallet/keys.hpp>
#include <nlohmann/json.hpp>
#include "HTTPRequest.hpp"

int main(int argc, char* argv[]) {
    data::bytes testArray{0,99,234,23,45,99,128,140};
    data::encoding::hex::string testString(testArray);
    std::cout << "This is a temporary runtime: " << testString << std::endl;

    abstractions::bitcoin::pubkey PubkeyHex("02DE7A990579BCA352633D340F05E7E7158661B0DCA0BA40A5AB5E0328A312216F");
    std::cout << "Address: " << PubkeyHex.address().write() << std::endl;

    auto jsonTest="{\n"
                          "\t\"test\": 1,\n"
                          "\t\"poppy\": \"cute\",\n"
                          "\t\"puppies\": \"semi-cute\"\n"
                          "}"_json;
    std::cout << "Json: " << jsonTest.dump() << std::endl;
    try
    {
        // you can pass http::InternetProtocol::V6 to Request to make an IPv6 request
        http::Request request("http://userfriendly.org/");
        http::Response response = request.send("GET");
        std::cout << std::string(response.body.begin(), response.body.end()) << std::endl; // print the result
    }
    catch (const std::exception& e)
    {
        std::cerr << "Request failed, error: " << e.what() << std::endl;
    }


    return 0;
}