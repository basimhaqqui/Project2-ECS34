#include "DSVReader.h"
#include "DSVWriter.h"
#include "StringDataSource.h"
#include "StringDataSink.h"
#include <gtest/gtest.h>

TEST(DSVTest, SimpleCSVProcessing) {
    // Create source with employee data
    auto source = std::make_shared<CStringDataSource>("name,age,department\nJohn,34,Sales\n");
    auto output = std::make_shared<CStringDataSink>();
    
    // Initialize CSV reader and writer
    CDSVReader csvReader(source, ',');
    CDSVWriter csvWriter(output, ',');
    
    // Process row by row
    std::vector<std::string> currentRow;
    while (!csvReader.End()) {
        if (csvReader.ReadRow(currentRow)) {
            csvWriter.WriteRow(currentRow);
        }
    }

    // Verify output matches input
    EXPECT_EQ(output->String(), "name,age,department\nJohn,34,Sales\n");
}