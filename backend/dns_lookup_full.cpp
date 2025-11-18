#include <iostream>
#include <vector>
#include <string>
#include <arpa/inet.h>
#include <resolv.h>
#include <arpa/nameser.h>

using namespace std;

struct DNSRecord {
    string type;
    string value;
};

// Parse DNS answer section
vector<DNSRecord> parse_dns(u_char* answer, int len) {
    vector<DNSRecord> results;

    ns_msg msg;
    if (ns_initparse(answer, len, &msg) < 0)
        return results;

    int count = ns_msg_count(msg, ns_s_an);

    for (int i = 0; i < count; i++) {
        ns_rr rr;
        ns_parserr(&msg, ns_s_an, i, &rr);

        string type;
        string value;

        switch (ns_rr_type(rr)) {
            case ns_t_a: {
                char ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, ns_rr_rdata(rr), ip, sizeof(ip));
                type = "A";
                value = ip;
                break;
            }
            case ns_t_aaaa: {
                char ip6[INET6_ADDRSTRLEN];
                inet_ntop(AF_INET6, ns_rr_rdata(rr), ip6, sizeof(ip6));
                type = "AAAA";
                value = ip6;
                break;
            }
            case ns_t_cname: {
                char cname[1024];
                ns_name_uncompress(ns_msg_base(msg), 
                                   ns_msg_end(msg),
                                   ns_rr_rdata(rr), 
                                   cname, 
                                   sizeof(cname));
                type = "CNAME";
                value = cname;
                break;
            }
            case ns_t_ns: {
                char nsd[1024];
                ns_name_uncompress(ns_msg_base(msg), ns_msg_end(msg),
                                   ns_rr_rdata(rr), nsd, sizeof(nsd));
                type = "NS";
                value = nsd;
                break;
            }
            case ns_t_mx: {
                const u_char* rdata = ns_rr_rdata(rr);
                int preference = (rdata[0] << 8) | rdata[1];
                char exch[1024];
                ns_name_uncompress(ns_msg_base(msg), ns_msg_end(msg),
                                   rdata + 2, exch, sizeof(exch));
                type = "MX";
                value = exch;
                break;
            }
            case ns_t_txt: {
                int txt_len = ns_rr_rdata(rr)[0];
                value = string((char*)ns_rr_rdata(rr) + 1, txt_len);
                type = "TXT";
                break;
            }
        }

        if (!value.empty())
            results.push_back({type, value});
    }

    return results;
}

vector<DNSRecord> dns_lookup_full(const string& host, int dns_type) {
    u_char answer[4096];
    int len = res_query(host.c_str(), ns_c_in, dns_type, answer, sizeof(answer));
    if (len < 0)
        return {};

    return parse_dns(answer, len);
}
