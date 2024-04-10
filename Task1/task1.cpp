#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <iostream>

int main() {
    std::string url = "https://api.mainnet-beta.solana.com";

    // Prepare the JSON payload for the RPC method call
    nlohmann::json jsonPayload = {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getBalance"},
        {"params", {"GPNmU6ShynWA6zoDLbb5uN1gbPqFgp1eB2pDSCnZaeo6"}}
    };

    nlohmann::json jsonPayload2 = {
        {"jsonrpc", "2.0"},
        {"id", 2},
        {"method", "getAccountInfo"},
        {"params", {
            "GPNmU6ShynWA6zoDLbb5uN1gbPqFgp1eB2pDSCnZaeo6",
            {{"encoding", "jsonParsed"}}
        }}
    };

    // Perform the GET request
    auto response = cpr::Post(cpr::Url{url}, cpr::Body{jsonPayload.dump()}, cpr::Header{{"Content-Type", "application/json"}});
    auto response2 = cpr::Post(cpr::Url{url}, cpr::Body{jsonPayload2.dump()}, cpr::Header{{"Content-Type", "application/json"}});
    // Parse the JSON response
    auto jsonResponse = nlohmann::json::parse(response.text);
    auto jsonResponse2 = nlohmann::json::parse(response2.text);

    // Extract and display the balance
    std::cout << "Balance: " << jsonResponse["result"]["value"].get<int>() << std::endl;
    std::cout << "Response: " << jsonResponse2.dump(4) << std::endl;

    return 0;
}
