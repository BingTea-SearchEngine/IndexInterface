#pragma once

#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

enum class IndexMessageType {
    QUERY = 0,
    DOCUMENTS = 1,
};

const std::string MessageHeaders[] = {
    "QUERY",
    "DOCUMENTS",
};

struct doc_t {
    std::string url;
    uint32_t numWords;
    uint32_t numTitleWords;
    uint32_t numOutLinks;
    float pageRank;
    float cheiRank;

    friend std::ostream& operator<<(std::ostream& os, const doc_t& doc) {
        os << "{url=\"" << doc.url
           << "\", numWords=" << doc.numWords
           << ", numTitleWords=" << doc.numTitleWords
           << ", numOutLinks=" << doc.numOutLinks
           << ", pageRank=" << doc.pageRank
           << ", cheiRank=" << doc.cheiRank
           << ", outLinks=[";
        os << "]}";
        return os;
    }

};

struct IndexMessage {
    IndexMessageType type;
    std::string query;
    std::vector<doc_t> documents;

    friend std::ostream& operator<<(std::ostream& os, const IndexMessage& m) {
        os << "IndexMessage { type=" << MessageHeaders[static_cast<int>(m.type)];
        os << ", query=\"" << m.query << "\"";
        os << ", documents=[";
        for (size_t i = 0; i < m.documents.size(); ++i) {
            os << m.documents[i];
            if (i != m.documents.size() - 1) {
                os << ", ";
            }
        }
        os << "] }";
        return os;
    }
};

class IndexInterface {
public:
    static std::string Encode(IndexMessage message);

    static IndexMessage Decode(const std::string& encoded);
};
