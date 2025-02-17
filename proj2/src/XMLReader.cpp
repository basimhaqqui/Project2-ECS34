#include "XMLReader.h"
#include <expat.h>
#include <deque>
#include <memory>
#include <string>
#include <array>

class CXMLReader::Implementation {
private:
    std::shared_ptr<CDataSource> sourceStream;
    XML_Parser xmlParser;
    std::deque<SXMLEntity> entityBuffer;
    std::string textAccumulator;
    bool isFinished;
    
    // Buffer size for reading chunks of data
    static constexpr size_t BUFFER_SIZE = 8192;

    static void HandleStartElement(void* context, const XML_Char* name, const XML_Char** attributes) {
        auto* impl = static_cast<Implementation*>(context);
        impl->ProcessPendingText();
        
        SXMLEntity newEntity;
        newEntity.DType = SXMLEntity::EType::StartElement;
        newEntity.DNameData = name;
        
        // Process attributes if present
        for (int i = 0; attributes && attributes[i]; i += 2) {
            const char* attrName = attributes[i];
            const char* attrValue = attributes[i + 1];
            if (attrValue) {
                newEntity.DAttributes.emplace_back(attrName, attrValue);
            }
        }
        
        impl->entityBuffer.push_back(newEntity);
    }
    
    static void HandleEndElement(void* context, const XML_Char* name) {
        auto* impl = static_cast<Implementation*>(context);
        impl->ProcessPendingText();
        
        SXMLEntity newEntity;
        newEntity.DType = SXMLEntity::EType::EndElement;
        newEntity.DNameData = name;
        impl->entityBuffer.push_back(newEntity);
    }
    
    static void HandleCharacterData(void* context, const XML_Char* text, int length) {
        if (length > 0) {
            auto* impl = static_cast<Implementation*>(context);
            impl->textAccumulator.append(text, length);
        }
    }
    
    void ProcessPendingText() {
        if (!textAccumulator.empty()) {
            SXMLEntity textEntity;
            textEntity.DType = SXMLEntity::EType::CharData;
            textEntity.DNameData = textAccumulator;
            entityBuffer.push_back(textEntity);
            textAccumulator.clear();
        }
    }
    
    bool ParseNextChunk() {
        std::array<char, BUFFER_SIZE> buffer;
        size_t bytesRead = 0;
        
        // Read data from source
        while (bytesRead < BUFFER_SIZE && !sourceStream->End()) {
            char currentChar;
            if (sourceStream->Get(currentChar)) {
                buffer[bytesRead++] = currentChar;
            }
        }
        
        if (bytesRead == 0) {
            isFinished = true;
            return XML_Parse(xmlParser, nullptr, 0, 1) != XML_STATUS_ERROR;
        }
        
        return XML_Parse(xmlParser, buffer.data(), bytesRead, 0) != XML_STATUS_ERROR;
    }

public:
    explicit Implementation(std::shared_ptr<CDataSource> source)
        : sourceStream(std::move(source))
        , isFinished(false) {
        
        xmlParser = XML_ParserCreate(nullptr);
        XML_SetUserData(xmlParser, this);
        XML_SetElementHandler(xmlParser, HandleStartElement, HandleEndElement);
        XML_SetCharacterDataHandler(xmlParser, HandleCharacterData);
    }
    
    ~Implementation() {
        XML_ParserFree(xmlParser);
    }
    
    bool ReadNextEntity(SXMLEntity& entity, bool skipCharData) {
        while (entityBuffer.empty() && !isFinished) {
            if (!ParseNextChunk()) {
                return false;
            }
        }
        
        if (entityBuffer.empty()) {
            return false;
        }
        
        entity = entityBuffer.front();
        entityBuffer.pop_front();
        
        if (skipCharData && entity.DType == SXMLEntity::EType::CharData) {
            return ReadNextEntity(entity, skipCharData);
        }
        
        return true;
    }
    
    bool HasReachedEnd() const {
        return isFinished && entityBuffer.empty();
    }
};

CXMLReader::CXMLReader(std::shared_ptr<CDataSource> source)
    : DImplementation(std::make_unique<Implementation>(std::move(source))) {}

CXMLReader::~CXMLReader() = default;

bool CXMLReader::ReadEntity(SXMLEntity& entity, bool skipCharData) {
    return DImplementation->ReadNextEntity(entity, skipCharData);
}

bool CXMLReader::End() const {
    return DImplementation->HasReachedEnd();
}