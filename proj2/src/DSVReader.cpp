#include "DSVReader.h"
#include <sstream>

struct CDSVReader::SImplementation {
    std::shared_ptr<CDataSource> DDataSource;
    char DDelimiter;
    
    SImplementation(std::shared_ptr<CDataSource> src, char delimiter)
        : DDataSource(src), DDelimiter(delimiter == '"' ? ',' : delimiter) {}
    
    bool End() const {
        return DDataSource->End();
    }
    
    bool ReadRow(std::vector<std::string> &row) {
        row.clear();
        if(DDataSource->End()) {
            return false;
        }
        
        std::string currentField;
        bool inQuotes = false;
        char ch;
        
        while(DDataSource->Get(ch)) {
            if(ch == '"') {
                // Check for escaped quotes
                char nextCh;
                if(DDataSource->Peek(nextCh) && nextCh == '"') {
                    // Add single quote and skip the next one
                    currentField += ch;
                    DDataSource->Get(nextCh);
                }
                else {
                    inQuotes = !inQuotes;
                }
            }
            else if(ch == DDelimiter && !inQuotes) {
                row.push_back(currentField);
                currentField.clear();
            }
            else if(ch == '\n' && !inQuotes) {
                row.push_back(currentField);
                return true;
            }
            else if(ch == '\r') {
                // Handle Windows-style line endings (CR+LF)
                char nextCh;
                if(DDataSource->Peek(nextCh) && nextCh == '\n') {
                    DDataSource->Get(nextCh); // consume the \n
                }
                row.push_back(currentField);
                return true;
            }
            else {
                currentField += ch;
            }
        }
        
        if(!currentField.empty() || !row.empty()) {
            row.push_back(currentField);
            return true;
        }
        
        return false;
    }
};

CDSVReader::CDSVReader(std::shared_ptr<CDataSource> src, char delimiter)
    : DImplementation(std::make_unique<SImplementation>(src, delimiter)) {
}

CDSVReader::~CDSVReader() = default;

bool CDSVReader::End() const {
    return DImplementation->End();
}

bool CDSVReader::ReadRow(std::vector<std::string> &row) {
    return DImplementation->ReadRow(row);
}