#include <iostream>
#include <vector>
#include <string>
#include <arpa/nameser.h>

#include "dns_lookup_full.hpp"

int main(int argc, char* argv[]) {
    // Expect exactly one argument: the hostname to resolve
    if (argc != 2) {
        std::cerr << "Usage: dns_tool <hostname>\n";
        return 1;
    }

    std::string host = argv[1];
    std::vector<DNSRecord> all;

    // Perform detailed DNS queries for various record types
    for (auto r : dns_lookup_full(host, ns_t_cname)) all.push_back(r);  // Canonical name
    for (auto r : dns_lookup_full(host, ns_t_mx))    all.push_back(r);  // Mail exchangers
    for (auto r : dns_lookup_full(host, ns_t_ns))    all.push_back(r);  // Name servers
    for (auto r : dns_lookup_full(host, ns_t_txt))   all.push_back(r);  // TXT / metadata

    // Output everything as a simple JSON array
    std::cout << "[\n";
    for (size_t i = 0; i < all.size(); i++) {
        std::cout << "  {\"type\": \"" << all[i].type
                  << "\", \"value\": \"" << all[i].value << "\"}";
        if (i + 1 < all.size()) std::cout << ",";
        std::cout << "\n";
    }
    std::cout << "]\n";

    return 0;
}
