#include "IndexInterface.hpp"

uint32_t floatToNetwork(float f) {
    uint32_t i;
    std::memcpy(&i, &f, sizeof(float));
    i = htonl(i);
    return i;
}

float networkToFloat(uint32_t i) {
    i = ntohl(i);
    float f;
    std::memcpy(&f, &i, sizeof(float));
    return f;
}

std::string IndexInterface::Encode(IndexMessage message) {
    if (static_cast<int>(message.type) < 0 ||
        static_cast<int>(message.type) > 1) {
        throw std::runtime_error("Invalid Message Type header");
    }

    std::ostringstream oss;
    oss << MessageHeaders[static_cast<int>(message.type)] << '\0';

    if (message.type == IndexMessageType::QUERY) {
        uint32_t len = htonl(static_cast<uint32_t>(message.query.size()));
        oss.write(reinterpret_cast<char*>(&len), sizeof(len));
        oss.write(message.query.data(), message.query.size());

    } else if (message.type == IndexMessageType::DOCUMENTS) {
        uint32_t numUrls =
            htonl(static_cast<uint32_t>(message.documents.size()));
        oss.write(reinterpret_cast<char*>(&numUrls), sizeof(numUrls));

        for (const doc_t& doc : message.documents) {
            // url
            uint32_t len = htonl(static_cast<uint32_t>(doc.url.size()));
            oss.write(reinterpret_cast<char*>(&len), sizeof(len));
            oss.write(doc.url.data(), doc.url.size());
            // numwords
            uint32_t numWords = htonl(doc.numWords);
            oss.write(reinterpret_cast<char*>(&numWords), sizeof(numWords));
            // num title words
            uint32_t numTitleWords = htonl(doc.numTitleWords);
            oss.write(reinterpret_cast<char*>(&numTitleWords),
                      sizeof(numTitleWords));
            // urls size
            uint32_t numOutLinks = htonl(doc.numOutLinks);
            oss.write(reinterpret_cast<char*>(&numOutLinks),
                      sizeof(numOutLinks));
            // num title match
            uint32_t numTitleMatch = htonl(doc.numTitleMatch);
            oss.write(reinterpret_cast<char*>(&numTitleMatch),
                      sizeof(numTitleMatch));
            // num title match
            uint32_t numBodyMatch = htonl(doc.numBodyMatch);
            oss.write(reinterpret_cast<char*>(&numBodyMatch),
                      sizeof(numBodyMatch));
            // page rank
            uint32_t pageRank = floatToNetwork(doc.pageRank);
            oss.write(reinterpret_cast<char*>(&pageRank), sizeof(pageRank));
            // chei rank
            uint32_t cheiRank = floatToNetwork(doc.cheiRank);
            oss.write(reinterpret_cast<char*>(&cheiRank), sizeof(cheiRank));
            // community
            int community = htonl(doc.community);
            oss.write(reinterpret_cast<char*>(&community), sizeof(community));
            // community
            int communityCount = htonl(doc.communityCount);
            oss.write(reinterpret_cast<char*>(&communityCount),
                      sizeof(communityCount));
            // rank
            uint32_t score = floatToNetwork(doc.rankingScore);
            oss.write(reinterpret_cast<char*>(&score), sizeof(score));

            uint32_t snippetLen =
                htonl(static_cast<uint32_t>(doc.snippet.size()));
            oss.write(reinterpret_cast<char*>(&snippetLen), sizeof(snippetLen));
            oss.write(doc.snippet.data(), doc.snippet.size());

            uint32_t titleLen = htonl(static_cast<uint32_t>(doc.title.size()));
            oss.write(reinterpret_cast<char*>(&titleLen), sizeof(titleLen));
            oss.write(doc.title.data(), doc.title.size());
        }

    } else {
        throw std::runtime_error("Unknown Index Message Type in Encode");
    }

    return oss.str();
}

IndexMessage IndexInterface::Decode(const std::string& encoded) {
    std::istringstream iss(encoded);
    std::string header;
    std::getline(iss, header, '\0');

    IndexMessageType messageType;
    if (header == "QUERY") {
        messageType = IndexMessageType::QUERY;
    } else if (header == "DOCUMENTS") {
        messageType = IndexMessageType::DOCUMENTS;
    } else {
        throw std::runtime_error("Invalid MessageType header" + header);
    }

    IndexMessage message;
    message.type = messageType;

    if (message.type == IndexMessageType::QUERY) {
        uint32_t lenN;
        iss.read(reinterpret_cast<char*>(&lenN), sizeof(lenN));
        uint32_t len = ntohl(lenN);

        std::string f(len, '\0');
        iss.read(&f[0], len);
        message.query = f;
    } else if (message.type == IndexMessageType::DOCUMENTS) {
        uint32_t numDocuments;
        iss.read(reinterpret_cast<char*>(&numDocuments), sizeof(numDocuments));
        numDocuments = ntohl(numDocuments);

        message.documents.reserve(numDocuments);
        for (int i = 0; i < numDocuments; ++i) {
            // url
            uint32_t lenUrl;
            iss.read(reinterpret_cast<char*>(&lenUrl), sizeof(lenUrl));
            lenUrl = ntohl(lenUrl);
            std::string url(lenUrl, '\0');
            iss.read(&url[0], lenUrl);
            // num words
            uint32_t numWords;
            iss.read(reinterpret_cast<char*>(&numWords), sizeof(numWords));
            numWords = ntohl(numWords);
            // num title words
            uint32_t numTitleWords;
            iss.read(reinterpret_cast<char*>(&numTitleWords),
                     sizeof(numTitleWords));
            numTitleWords = ntohl(numTitleWords);
            // num out links
            uint32_t numOutLinks;
            iss.read(reinterpret_cast<char*>(&numOutLinks),
                     sizeof(numOutLinks));
            numOutLinks = ntohl(numOutLinks);
            // num title match
            uint32_t numTitleMatch;
            iss.read(reinterpret_cast<char*>(&numTitleMatch),
                     sizeof(numTitleMatch));
            numTitleMatch = ntohl(numTitleMatch);
            // num body match
            uint32_t numBodyMatch;
            iss.read(reinterpret_cast<char*>(&numBodyMatch),
                     sizeof(numBodyMatch));
            numBodyMatch = ntohl(numBodyMatch);
            // page rank
            uint32_t pageRankInt;
            iss.read(reinterpret_cast<char*>(&pageRankInt),
                     sizeof(pageRankInt));
            float pageRank = networkToFloat(pageRankInt);
            // chei rank
            uint32_t cheiRankInt;
            iss.read(reinterpret_cast<char*>(&cheiRankInt),
                     sizeof(cheiRankInt));
            float cheiRank = networkToFloat(cheiRankInt);
            // community
            int community;
            iss.read(reinterpret_cast<char*>(&community), sizeof(community));
            community = ntohl(community);
            // community count
            int communityCount;
            iss.read(reinterpret_cast<char*>(&communityCount),
                     sizeof(communityCount));
            communityCount = ntohl(communityCount);
            //
            uint32_t rankInt;
            iss.read(reinterpret_cast<char*>(&rankInt), sizeof(rankInt));
            float rank = networkToFloat(rankInt);

            uint32_t snippetLen;
            iss.read(reinterpret_cast<char*>(&snippetLen), sizeof(snippetLen));
            snippetLen = ntohl(snippetLen);
            std::string snippet(snippetLen, '\0');
            iss.read(&snippet[0], snippetLen);

            uint32_t titleLen;
            iss.read(reinterpret_cast<char*>(&titleLen), sizeof(titleLen));
            titleLen = ntohl(titleLen);
            std::string title(titleLen, '\0');
            iss.read(&title[0], titleLen);

            message.documents.push_back(
                doc_t{url, numWords, numTitleWords, numOutLinks, numTitleMatch,
                      numBodyMatch, pageRank, cheiRank, community,
                      communityCount, rank, snippet, title});
        }
    }
    return message;
}

bool operator==(const doc_t& lhs, const doc_t& rhs) {
    return lhs.url == rhs.url && lhs.numWords == rhs.numWords &&
           lhs.numTitleWords == rhs.numTitleWords &&
           lhs.numOutLinks == rhs.numOutLinks && lhs.pageRank == rhs.pageRank &&
           lhs.cheiRank == rhs.cheiRank && lhs.community == rhs.community &&
           lhs.communityCount == rhs.communityCount &&
           lhs.snippet == rhs.snippet && lhs.title == rhs.title;
}
