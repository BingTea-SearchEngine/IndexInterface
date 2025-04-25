#pragma once

#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>

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
    uint32_t numTitleMatch;
    uint32_t numBodyMatch;
    float pageRank;
    float cheiRank;
    int community;
    int communityCount;
    float rankingScore;
    std::string snippet;
    std::string title;

    friend std::ostream& operator<<(std::ostream& os, const doc_t& doc) {
        os << "{url=\"" << doc.url
           << "\", numWords=" << doc.numWords
           << ", numTitleWords=" << doc.numTitleWords
           << ", numOutLinks=" << doc.numOutLinks
           << ", numTitleMatch=" << doc.numTitleMatch
           << ", numBodyMatch=" << doc.numBodyMatch
           << ", pageRank=" << doc.pageRank
           << ", cheiRank=" << doc.cheiRank
           << ", community=" << doc.community
           << ", communityCount=" << doc.communityCount
           << ", rankingScore=" << doc.rankingScore
           << ", snippet=" << doc.snippet
           << ", title=" << doc.title;
        os << "}";
        return os;
    }

};
bool operator==(const doc_t& lhs, const doc_t& rhs);

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
