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
        uint32_t numUrls = htonl(static_cast<uint32_t>(message.documents.size()));
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
            oss.write(reinterpret_cast<char*>(&numTitleWords), sizeof(numTitleWords));
            // page rank
            uint32_t pageRank = floatToNetwork(doc.pageRank);
            oss.write(reinterpret_cast<char*>(&pageRank), sizeof(pageRank));
            // chei rank
            uint32_t cheiRank = floatToNetwork(doc.cheiRank);
            oss.write(reinterpret_cast<char*>(&cheiRank), sizeof(cheiRank));
            // urls size
            uint32_t size = htonl(static_cast<uint32_t>(doc.outLinks.size()));
            oss.write(reinterpret_cast<char*>(&size), sizeof(size));
            for (const std::string& outLink : doc.outLinks) {
                uint32_t len = htonl(static_cast<uint32_t>(outLink.size()));
                oss.write(reinterpret_cast<char*>(&len), sizeof(len));
                oss.write(outLink.data(), outLink.size());
            }
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
            iss.read(reinterpret_cast<char*>(&numTitleWords), sizeof(numTitleWords));
            numTitleWords = ntohl(numTitleWords);
            // page rank
            uint32_t pageRankInt;
            iss.read(reinterpret_cast<char*>(&pageRankInt), sizeof(pageRankInt));
            float pageRank = networkToFloat(pageRankInt);
            // chei rank
            uint32_t cheiRankInt;
            iss.read(reinterpret_cast<char*>(&cheiRankInt), sizeof(cheiRankInt));
            float cheiRank = networkToFloat(cheiRankInt);

            uint32_t numOutLinks;
            iss.read(reinterpret_cast<char*>(&numOutLinks), sizeof(numOutLinks));
            numOutLinks = ntohl(numOutLinks);
            std::vector<std::string> outLinks;
            outLinks.reserve(numOutLinks);
            for (int i = 0; i < numOutLinks; ++i) {
                uint32_t lenUrl;
                iss.read(reinterpret_cast<char*>(&lenUrl), sizeof(lenUrl));
                lenUrl = ntohl(lenUrl);
                std::string outLink(lenUrl, '\0');
                iss.read(&outLink[0], lenUrl);
                outLinks.push_back(outLink);
            }

            message.documents.push_back(doc_t{url, numWords, numTitleWords, pageRank, cheiRank, outLinks});
        }
    }
    return message;
}

