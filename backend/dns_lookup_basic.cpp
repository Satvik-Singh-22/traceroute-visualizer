#include <iostream>
#include <netdb.h>
#include <arpa/inet.h>
#include <vector>
#include <string>

using namespace std;

struct Record { 
    string type;
    string value;
};

vector<Record> dns_lookup_basic(const string& host) {
    vector<Record> results;
    addrinfo hints{}, *res, *p;
    hints.ai_family = AF_UNSPEC;  // Both IPv4 + IPv6

    if (getaddrinfo(host.c_str(), NULL, &hints, &res) != 0) 
        return results;

    char ipstr[INET6_ADDRSTRLEN];

    for (p = res; p != NULL; p = p->ai_next) {
        void* addr;
        string type;

        if (p->ai_family == AF_INET) {
            sockaddr_in* ipv4 = (sockaddr_in*)p->ai_addr;
            addr = &(ipv4->sin_addr);
            type = "A";
        } else {
            sockaddr_in6* ipv6 = (sockaddr_in6*)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            type = "AAAA";
        }

        inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
        results.push_back({type, string(ipstr)});
    }

    freeaddrinfo(res);
    return results;
}
