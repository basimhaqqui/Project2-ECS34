#include <gtest/gtest.h>
#include "DSVReader.h"
#include "DSVWriter.h"
#include "StringDataSource.h"
#include "StringDataSink.h"

TEST(DSVWriter, EmptyRow) {
    auto Sink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(Sink, ',');
    std::vector<std::string> row;
    
    EXPECT_TRUE(Writer.WriteRow(row));
    EXPECT_EQ(Sink->String(), "\n");
}

TEST(DSVWriter, SimpleRow) {
    auto Sink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(Sink, ',');
    std::vector<std::string> row = {"hello", "world"};
    
    EXPECT_TRUE(Writer.WriteRow(row));
    EXPECT_EQ(Sink->String(), "hello,world\n");
}

TEST(DSVWriter, QuotedFields) {
    auto Sink = std::make_shared<CStringDataSink>();
    CDSVWriter Writer(Sink, ',');
    std::vector<std::string> row = {"hello,there", "\"quoted\"", "normal"};
    
    EXPECT_TRUE(Writer.WriteRow(row));
    EXPECT_EQ(Sink->String(), "\"hello,there\",\"\"\"quoted\"\"\",normal\n");
}

TEST(DSVReader, EmptySource) {
    auto Source = std::make_shared<CStringDataSource>("");
    CDSVReader Reader(Source, ',');
    std::vector<std::string> row;
    
    EXPECT_TRUE(Reader.End());
    EXPECT_FALSE(Reader.ReadRow(row));
    EXPECT_TRUE(row.empty());
}

TEST(DSVReader, SimpleRow) {
    auto Source = std::make_shared<CStringDataSource>("hello,world\n");
    CDSVReader Reader(Source, ',');
    std::vector<std::string> row;
    
    EXPECT_FALSE(Reader.End());
    EXPECT_TRUE(Reader.ReadRow(row));
    ASSERT_EQ(row.size(), 2);
    EXPECT_EQ(row[0], "hello");
    EXPECT_EQ(row[1], "world");
    EXPECT_TRUE(Reader.End());
}

TEST(DSVReader, QuotedFields) {
    auto Source = std::make_shared<CStringDataSource>("\"hello,world\",\"\"\"quoted\"\"\"\n");
    CDSVReader Reader(Source, ',');
    std::vector<std::string> row;
    
    EXPECT_FALSE(Reader.End());
    EXPECT_TRUE(Reader.ReadRow(row));
    ASSERT_EQ(row.size(), 2);
    EXPECT_EQ(row[0], "hello,world");
    EXPECT_EQ(row[1], "\"quoted\"");
    EXPECT_TRUE(Reader.End());
}

TEST(DSVReader, MultipleRows) {
    auto Source = std::make_shared<CStringDataSource>("a,b,c\n1,2,3\n");
    CDSVReader Reader(Source, ',');
    std::vector<std::string> row;
    
    EXPECT_FALSE(Reader.End());
    EXPECT_TRUE(Reader.ReadRow(row));
    ASSERT_EQ(row.size(), 3);
    EXPECT_EQ(row[0], "a");
    EXPECT_EQ(row[1], "b");
    EXPECT_EQ(row[2], "c");
    
    EXPECT_TRUE(Reader.ReadRow(row));
    ASSERT_EQ(row.size(), 3);
    EXPECT_EQ(row[0], "1");
    EXPECT_EQ(row[1], "2");
    EXPECT_EQ(row[2], "3");
    
    EXPECT_TRUE(Reader.End());
}