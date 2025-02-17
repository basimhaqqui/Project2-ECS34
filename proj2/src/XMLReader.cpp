#include "XMLReader.h"
#include <expat.h>
#include <queue>
#include <cstring>

struct CXMLReader::SImplementation {
    std::shared_ptr<CDataSource> DDataSource;
    XML_Parser DParser;
    std::queue<SXMLEntity> DEntityQueue;
    bool DError;
    
    static void StartElementHandler(void *userData, const XML_Char *name, const XML_Char **attrs) {
        auto Implementation = static_cast<SImplementation*>(userData);
        SXMLEntity Entity;
        Entity.DType = SXMLEntity::EType::StartElement;
        Entity.DNameData = name;
        for(size_t Index = 0; attrs[Index]; Index += 2) {
            Entity.DAttributes.push_back(std::make_pair(attrs[Index], attrs[Index + 1]));
        }
        Implementation->DEntityQueue.push(Entity);
    }
    
    static void EndElementHandler(void *userData, const XML_Char *name) {
        auto Implementation = static_cast<SImplementation*>(userData);
        SXMLEntity Entity;
        Entity.DType = SXMLEntity::EType::EndElement;
        Entity.DNameData = name;
        Implementation->DEntityQueue.push(Entity);
    }
    
    static void CharacterDataHandler(void *userData, const XML_Char *s, int len) {
        auto Implementation = static_cast<SImplementation*>(userData);
        if(len) {
            SXMLEntity Entity;
            Entity.DType = SXMLEntity::EType::CharData;
            Entity.DNameData = std::string(s, len);
            Implementation->DEntityQueue.push(Entity);
        }
    }
    
    SImplementation(std::shared_ptr<CDataSource> src)
        : DDataSource(src), DError(false) {
        DParser = XML_ParserCreate(NULL);
        XML_SetUserData(DParser, this);
        XML_SetElementHandler(DParser, StartElementHandler, EndElementHandler);
        XML_SetCharacterDataHandler(DParser, CharacterDataHandler);
    }
    
    ~SImplementation() {
        XML_ParserFree(DParser);
    }
    
    bool End() const {
        return DDataSource->End() && DEntityQueue.empty();
    }
    
    bool ReadEntity(SXMLEntity &entity, bool skipcdata) {
        while(DEntityQueue.empty() && !DError && !DDataSource->End()) {
            std::vector<char> Buffer;
            if(DDataSource->Read(Buffer, 256)) {
                if(XML_Parse(DParser, Buffer.data(), Buffer.size(), DDataSource->End()) == XML_STATUS_ERROR) {
                    DError = true;
                }
            }
        }
        
        if(DError || (DEntityQueue.empty() && DDataSource->End())) {
            return false;
        }
        
        while(skipcdata && !DEntityQueue.empty() && 
              DEntityQueue.front().DType == SXMLEntity::EType::CharData) {
            DEntityQueue.pop();
        }
        
        if(DEntityQueue.empty()) {
            return false;
        }
        
        entity = DEntityQueue.front();
        DEntityQueue.pop();
        return true;
    }
};

CXMLReader::CXMLReader(std::shared_ptr<CDataSource> src)
    : DImplementation(std::make_unique<SImplementation>(src)) {
}

CXMLReader::~CXMLReader() = default;

bool CXMLReader::End() const {
    return DImplementation->End();
}

bool CXMLReader::ReadEntity(SXMLEntity &entity, bool skipcdata) {
    return DImplementation->ReadEntity(entity, skipcdata);
}