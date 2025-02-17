#include "XMLWriter.h"
#include <stack>
#include <string>

struct CXMLWriter::SImplementation {
    std::shared_ptr<CDataSink> DDataSink;
    std::stack<std::string> DElementStack;
    size_t DIndentLevel;
    
    SImplementation(std::shared_ptr<CDataSink> sink)
        : DDataSink(sink), DIndentLevel(0) {
    }
    
    bool WriteIndent() {
        for(size_t i = 0; i < DIndentLevel; ++i) {
            if(!DDataSink->Put(' ')) {
                return false;
            }
            if(!DDataSink->Put(' ')) {
                return false;
            }
        }
        return true;
    }
    
    bool WriteString(const std::string &str) {
        for(char ch : str) {
            if(!DDataSink->Put(ch)) {
                return false;
            }
        }
        return true;
    }
    
    bool WriteEscaped(const std::string &str) {
        for(char ch : str) {
            switch(ch) {
                case '<':
                    if(!WriteString("&lt;")) return false;
                    break;
                case '>':
                    if(!WriteString("&gt;")) return false;
                    break;
                case '&':
                    if(!WriteString("&amp;")) return false;
                    break;
                case '\'':
                    if(!WriteString("&apos;")) return false;
                    break;
                case '"':
                    if(!WriteString("&quot;")) return false;
                    break;
                default:
                    if(!DDataSink->Put(ch)) return false;
            }
        }
        return true;
    }
    
    bool Flush() {
        while(!DElementStack.empty()) {
            DIndentLevel--;
            if(!WriteIndent()) {
                return false;
            }
            if(!WriteString("</")) {
                return false;
            }
            if(!WriteString(DElementStack.top())) {
                return false;
            }
            if(!WriteString(">\n")) {
                return false;
            }
            DElementStack.pop();
        }
        return true;
    }
    
    bool WriteEntity(const SXMLEntity &entity) {
        switch(entity.DType) {
            case SXMLEntity::EType::StartElement:
                if(!WriteIndent()) {
                    return false;
                }
                if(!WriteString("<")) {
                    return false;
                }
                if(!WriteString(entity.DNameData)) {
                    return false;
                }
                for(const auto &attr : entity.DAttributes) {
                    if(!WriteString(" ")) {
                        return false;
                    }
                    if(!WriteString(attr.first)) {
                        return false;
                    }
                    if(!WriteString("=\"")) {
                        return false;
                    }
                    if(!WriteEscaped(attr.second)) {
                        return false;
                    }
                    if(!WriteString("\"")) {
                        return false;
                    }
                }
                if(!WriteString(">\n")) {
                    return false;
                }
                DElementStack.push(entity.DNameData);
                DIndentLevel++;
                break;
                
            case SXMLEntity::EType::EndElement:
                DIndentLevel--;
                if(!WriteIndent()) {
                    return false;
                }
                if(!WriteString("</")) {
                    return false;
                }
                if(!WriteString(entity.DNameData)) {
                    return false;
                }
                if(!WriteString(">\n")) {
                    return false;
                }
                if(!DElementStack.empty()) {
                    DElementStack.pop();
                }
                break;
                
            case SXMLEntity::EType::CharData:
                if(!WriteIndent()) {
                    return false;
                }
                if(!WriteEscaped(entity.DNameData)) {
                    return false;
                }
                if(!WriteString("\n")) {
                    return false;
                }
                break;
                
            case SXMLEntity::EType::CompleteElement:
                if(!WriteIndent()) {
                    return false;
                }
                if(!WriteString("<")) {
                    return false;
                }
                if(!WriteString(entity.DNameData)) {
                    return false;
                }
                for(const auto &attr : entity.DAttributes) {
                    if(!WriteString(" ")) {
                        return false;
                    }
                    if(!WriteString(attr.first)) {
                        return false;
                    }
                    if(!WriteString("=\"")) {
                        return false;
                    }
                    if(!WriteEscaped(attr.second)) {
                        return false;
                    }
                    if(!WriteString("\"")) {
                        return false;
                    }
                }
                if(!WriteString("/>\n")) {
                    return false;
                }
                break;
        }
        return true;
    }
};

CXMLWriter::CXMLWriter(std::shared_ptr<CDataSink> sink)
    : DImplementation(std::make_unique<SImplementation>(sink)) {
}

CXMLWriter::~CXMLWriter() = default;

bool CXMLWriter::Flush() {
    return DImplementation->Flush();
}

bool CXMLWriter::WriteEntity(const SXMLEntity &entity) {
    return DImplementation->WriteEntity(entity);
}