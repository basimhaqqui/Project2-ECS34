#include <gtest/gtest.h>
#include "XMLReader.h"
#include "XMLWriter.h"
#include "StringDataSource.h"
#include "StringDataSink.h"

TEST(XMLWriter, EmptyElement) {
    auto Sink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(Sink);
    SXMLEntity Entity;
    Entity.DType = SXMLEntity::EType::CompleteElement;
    Entity.DNameData = "test";
    
    EXPECT_TRUE(Writer.WriteEntity(Entity));
    EXPECT_EQ(Sink->String(), "<test/>");
}

TEST(XMLWriter, ElementWithAttributes) {
    auto Sink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(Sink);
    SXMLEntity Entity;
    Entity.DType = SXMLEntity::EType::StartElement;
    Entity.DNameData = "test";
    Entity.DAttributes.push_back(std::make_pair("attr1", "value1"));
    Entity.DAttributes.push_back(std::make_pair("attr2", "value2"));
    
    EXPECT_TRUE(Writer.WriteEntity(Entity));
    
    Entity.DType = SXMLEntity::EType::EndElement;
    EXPECT_TRUE(Writer.WriteEntity(Entity));
    
    EXPECT_EQ(Sink->String(), "<test attr1=\"value1\" attr2=\"value2\"></test>\n");
}

TEST(XMLWriter, CharacterData) {
    auto Sink = std::make_shared<CStringDataSink>();
    CXMLWriter Writer(Sink);
    SXMLEntity Entity;
    
    Entity.DType = SXMLEntity::EType::StartElement;
    Entity.DNameData = "test";
    EXPECT_TRUE(Writer.WriteEntity(Entity));
    
    Entity.DType = SXMLEntity::EType::CharData;
    Entity.DNameData = "Hello & Goodbye";
    EXPECT_TRUE(Writer.WriteEntity(Entity));
    
    Entity.DType = SXMLEntity::EType::EndElement;
    Entity.DNameData = "test";
    EXPECT_TRUE(Writer.WriteEntity(Entity));
    
    EXPECT_EQ(Sink->String(), "<test>  Hello &amp; Goodbye</test>");
}

TEST(XMLReader, EmptyElement) {
    auto Source = std::make_shared<CStringDataSource>("<test/>");
    CXMLReader Reader(Source);
    SXMLEntity Entity;
    
    EXPECT_FALSE(Reader.End());
    EXPECT_TRUE(Reader.ReadEntity(Entity));
    EXPECT_EQ(Entity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(Entity.DNameData, "test");
    
    EXPECT_TRUE(Reader.ReadEntity(Entity));
    EXPECT_EQ(Entity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(Entity.DNameData, "test");
    
    EXPECT_TRUE(Reader.End());
}

TEST(XMLReader, ElementWithAttributes) {
    auto Source = std::make_shared<CStringDataSource>("<test attr1=\"value1\" attr2=\"value2\"></test>");
    CXMLReader Reader(Source);
    SXMLEntity Entity;
    
    EXPECT_FALSE(Reader.End());
    EXPECT_TRUE(Reader.ReadEntity(Entity));
    EXPECT_EQ(Entity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(Entity.DNameData, "test");
    ASSERT_EQ(Entity.DAttributes.size(), 2);
    EXPECT_EQ(Entity.DAttributes[0].first, "attr1");
    EXPECT_EQ(Entity.DAttributes[0].second, "value1");
    EXPECT_EQ(Entity.DAttributes[1].first, "attr2");
    EXPECT_EQ(Entity.DAttributes[1].second, "value2");
    
    EXPECT_TRUE(Reader.ReadEntity(Entity));
    EXPECT_EQ(Entity.DType, SXMLEntity::EType::EndElement);
    EXPECT_EQ(Entity.DNameData, "test");
    
    EXPECT_TRUE(Reader.End());
}

TEST(XMLReader, CharacterData) {
    auto Source = std::make_shared<CStringDataSource>("<test>Hello &amp; Goodbye</test>");
    CXMLReader Reader(Source);
    SXMLEntity Entity;
    
    EXPECT_FALSE(Reader.End());
    EXPECT_TRUE(Reader.ReadEntity(Entity));
    EXPECT_EQ(Entity.DType, SXMLEntity::EType::StartElement);}