#include <gtest/gtest.h>

#include "IndexInterface.hpp"

using std::cout, std::endl;

TEST(BasicIndexInterfaceTest, Query) {
    std::string query = "Hi index this is the frontend speaking";

    IndexMessage queryMessage =
        IndexMessage{IndexMessageType::QUERY, query, {}};
    std::string encoded = IndexInterface::Encode(queryMessage);
    IndexMessage decoded = IndexInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, queryMessage.type);
    EXPECT_EQ(decoded.query, queryMessage.query);
}

TEST(BasicIndexInterfaceTest, Documents) {
    doc_t doc1{"a.com", 5, 1231, 2, 0.4, 0.5, "Hi this is an example snippet"};
    doc_t doc2{"b.com", 2, 20, 1, 1.2, 1.3, "This is snippet 2"};
    doc_t doc3{"c.com", 100, 5, 2, 0.0, 0.2, "sdfasdfasdfsafsdfdf"};
    std::vector<doc_t> documents = {doc1, doc2, doc3};

    IndexMessage docMessage =
        IndexMessage{IndexMessageType::DOCUMENTS, "", documents};
    std::string encoded = IndexInterface::Encode(docMessage);
    IndexMessage decoded = IndexInterface::Decode(encoded);

    EXPECT_EQ(decoded.type, docMessage.type);
    EXPECT_EQ(decoded.documents.size(), docMessage.documents.size());
    for (size_t i = 0; i < decoded.documents.size(); ++i) {
        EXPECT_EQ(decoded.documents[i].url, docMessage.documents[i].url);
        EXPECT_EQ(decoded.documents[i].numWords, docMessage.documents[i].numWords);
        EXPECT_EQ(decoded.documents[i].numTitleWords, docMessage.documents[i].numTitleWords);
        EXPECT_EQ(decoded.documents[i].pageRank, docMessage.documents[i].pageRank);
        EXPECT_EQ(decoded.documents[i].cheiRank, docMessage.documents[i].cheiRank);
        EXPECT_EQ(decoded.documents[i].numOutLinks, docMessage.documents[i].numOutLinks);
        EXPECT_EQ(decoded.documents[i].snippet, docMessage.documents[i].snippet);
    }

}TEST(BasicIndexInterfaceTest, EmptyQuery) {
    IndexMessage queryMessage = IndexMessage{IndexMessageType::QUERY, "", {}};
    std::string encoded = IndexInterface::Encode(queryMessage);
    IndexMessage decoded = IndexInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, IndexMessageType::QUERY);
    EXPECT_EQ(decoded.query, "");
    EXPECT_TRUE(decoded.documents.empty());
}

TEST(BasicIndexInterfaceTest, EmptyDocumentsList) {
    IndexMessage docMessage = IndexMessage{IndexMessageType::DOCUMENTS, "", {}};
    std::string encoded = IndexInterface::Encode(docMessage);
    IndexMessage decoded = IndexInterface::Decode(encoded);
    EXPECT_EQ(decoded.type, IndexMessageType::DOCUMENTS);
    EXPECT_TRUE(decoded.documents.empty());
}


TEST(BasicIndexInterfaceTest, FloatingPointPrecision) {
    doc_t doc{"float.com", 5, 2, 4, 0.123456, 0.654321};
    IndexMessage msg = IndexMessage{IndexMessageType::DOCUMENTS, "", {doc}};
    std::string encoded = IndexInterface::Encode(msg);
    IndexMessage decoded = IndexInterface::Decode(encoded);

    ASSERT_EQ(decoded.documents.size(), 1);
    EXPECT_NEAR(decoded.documents[0].pageRank, 0.123456, 1e-5);
    EXPECT_NEAR(decoded.documents[0].cheiRank, 0.654321, 1e-5);
}

TEST(BasicIndexInterfaceTest, RoundTripMixedMessage) {
    doc_t doc{"mixed.com", 42, 10, 2, 0.123, 0.321, "asdfasdfs asdfasdf  dsakkjdlasdfasd fda"};
    IndexMessage original = IndexMessage{IndexMessageType::DOCUMENTS, "", {doc}};
    std::string encoded = IndexInterface::Encode(original);
    IndexMessage decoded = IndexInterface::Decode(encoded);

    EXPECT_EQ(decoded.type, IndexMessageType::DOCUMENTS);
    EXPECT_EQ(decoded.query, "");
    ASSERT_EQ(decoded.documents.size(), 1);
    EXPECT_EQ(decoded.documents[0].url, "mixed.com");
    EXPECT_EQ(decoded.documents[0].numOutLinks, 2);
    EXPECT_EQ(decoded.documents[0].snippet, doc.snippet);
}


