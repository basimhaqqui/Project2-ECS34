#include "DSVWriter.h"
#include <string>

struct CDSVWriter::SImplementation {
    std::shared_ptr<CDataSink> DDataSink;
    char DDelimiter;
    bool DQuoteAll;
    
    SImplementation(std::shared_ptr<CDataSink> sink, char delimiter, bool quoteall)
        : DDataSink(sink), DDelimiter(delimiter == '"' ? ',' : delimiter), DQuoteAll(quoteall) {}
    
    bool WriteRow(const std::vector<std::string> &row) {
        if(row.empty()) {
            return DDataSink->Put('\n');
        }
        
        for(size_t i = 0; i < row.size(); ++i) {
            const std::string &field = row[i];
            bool needsQuotes = DQuoteAll || 
                             field.find(DDelimiter) != std::string::npos ||
                             field.find('"') != std::string::npos ||
                             field.find('\n') != std::string::npos;
            
            if(needsQuotes) {
                if(!DDataSink->Put('"')) {
                    return false;
                }
                
                for(char ch : field) {
                    if(ch == '"') {
                        // Escape quotes with another quote
                        if(!DDataSink->Put('"') || !DDataSink->Put('"')) {
                            return false;
                        }
                    }
                    else {
                        if(!DDataSink->Put(ch)) {
                            return false;
                        }
                    }
                }
                
                if(!DDataSink->Put('"')) {
                    return false;
                }
            }
            else {
                for(char ch : field) {
                    if(!DDataSink->Put(ch)) {
                        return false;
                    }
                }
            }
            
            if(i < row.size() - 1) {
                if(!DDataSink->Put(DDelimiter)) {
                    return false;
                }
            }
        }
        
        return DDataSink->Put('\n');
    }
};

CDSVWriter::CDSVWriter(std::shared_ptr<CDataSink> sink, char delimiter, bool quoteall)
    : DImplementation(std::make_unique<SImplementation>(sink, delimiter, quoteall)) {
}

CDSVWriter::~CDSVWriter() = default;

bool CDSVWriter::WriteRow(const std::vector<std::string> &row) {
    return DImplementation->WriteRow(row);
}