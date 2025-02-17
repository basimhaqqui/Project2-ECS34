#include "XMLReader.h"
#include <expat.h>
#include <queue>
#include <memory>
#include <vector>

struct CXMLReader::SImplementation {
    std::shared_ptr<CDataSource> DataSource;
    XML_Parser Parser;
    std::queue<SXMLEntity> EntityQueue;
    bool EndOfData;
    std::string CharDataBuffer;

    static void StartElementHandler(void *userData, const char *name, const char **atts) {
        auto *impl = static_cast<SImplementation *>(userData);
        impl->FlushCharData();
        SXMLEntity entity;
        entity.DType = SXMLEntity::EType::StartElement;
        entity.DNameData = name;

        for (int i = 0; atts && atts[i]; i += 2) {
            if (atts[i + 1]) {
                entity.DAttributes.emplace_back(atts[i], atts[i + 1]);
            }
        }

        impl->EntityQueue.push(entity);
    }

    static void EndElementHandler(void *userData, const char *name) {
        auto *impl = static_cast<SImplementation *>(userData);
        impl->FlushCharData();
        SXMLEntity entity;
        entity.DType = SXMLEntity::EType::EndElement;
        entity.DNameData = name;
        impl->EntityQueue.push(entity);
    }

    static void CharDataHandler(void *userData, const char *s, int len) {
        auto *impl = static_cast<SImplementation *>(userData);
        if (s && len > 0) {
            impl->CharDataBuffer.append(s, len);
        }
    }

    SImplementation(std::shared_ptr<CDataSource> src) 
        : DataSource(std::move(src))
        , EndOfData(false) {
        Parser = XML_ParserCreate(nullptr);
        XML_SetUserData(Parser, this);
        XML_SetElementHandler(Parser, StartElementHandler, EndElementHandler);
        XML_SetCharacterDataHandler(Parser, CharDataHandler);
    }

    ~SImplementation() {
        XML_ParserFree(Parser);
    }

    void FlushCharData() {
        if (!CharDataBuffer.empty()) {
            SXMLEntity entity;
            entity.DType = SXMLEntity::EType::CharData;
            entity.DNameData = CharDataBuffer;
            EntityQueue.push(entity);
            CharDataBuffer.clear();
        }
    }

    bool ReadEntity(SXMLEntity &entity, bool skipcdata) {
        const size_t READ_SIZE = 4096;
        char buffer[READ_SIZE];
        
        while (EntityQueue.empty() && !EndOfData) {
            size_t bytesRead = 0;
            
            // Read data byte by byte until buffer is full or source is exhausted
            while (bytesRead < READ_SIZE && !DataSource->End()) {
                char ch;
                if (DataSource->Get(ch)) {
                    buffer[bytesRead++] = ch;
                }
            }

            if (bytesRead == 0) {
                EndOfData = true;
                XML_Parse(Parser, nullptr, 0, 1);
                break;
            }

            if (XML_Parse(Parser, buffer, bytesRead, 0) == XML_STATUS_ERROR) {
                return false;
            }
        }

        if (!EntityQueue.empty()) {
            entity = EntityQueue.front();
            EntityQueue.pop();

            if (skipcdata && entity.DType == SXMLEntity::EType::CharData) {
                return ReadEntity(entity, skipcdata);
            }
            return true;
        }

        return false;
    }
};

CXMLReader::CXMLReader(std::shared_ptr<CDataSource> src)
    : DImplementation(std::make_unique<SImplementation>(std::move(src))) {}

CXMLReader::~CXMLReader() = default;

bool CXMLReader::End() const {
    return DImplementation->EndOfData && DImplementation->EntityQueue.empty();
}

bool CXMLReader::ReadEntity(SXMLEntity &entity, bool skipcdata) {
    return DImplementation->ReadEntity(entity, skipcdata);
}