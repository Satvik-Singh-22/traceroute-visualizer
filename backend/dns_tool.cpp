#include <iostream>
#include <vector>
#include <string>
#include "dns_lookup_basic.cpp"
#include "dns_lookup_full.cpp"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: dns_tool <hostname>" << endl;
        return 1;
    }

    string host = argv[1];
    vector<DNSRecord> all;

    // A + AAAA
    for (auto r : dns_lookup_basic(host))
        all.push_back({r.type, r.value});

    // CNAME
    for (auto r : dns_lookup_full(host, ns_t_cname))
        all.push_back(r);

    // MX
    for (auto r : dns_lookup_full(host, ns_t_mx))
        all.push_back(r);

    // NS
    for (auto r : dns_lookup_full(host, ns_t_ns))
        all.push_back(r);

    // TXT
    for (auto r : dns_lookup_full(host, ns_t_txt))
        all.push_back(r);

    // Print JSON
    cout << "[" << endl;
    for (size_t i = 0; i < all.size(); i++) {
        cout << "  {\"type\": \"" << all[i].type
             << "\", \"value\": \"" << all[i].value << "\"}";
        if (i + 1 < all.size())
            cout << ",";
        cout << endl;
    }
    cout << "]" << endl;

    return 0;
}
