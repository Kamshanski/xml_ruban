#ifndef XML_RUBAN_RUBANXML_H
#define XML_RUBAN_RUBANXML_H

#include <src/encode/writer/XmlWriterFactory.h>
#include <src/decode/parser/XmlParserFactory.h>
#include <src/sink/StringSink.h>
#include <src/source/StringSource.h>
#include <src/source/FileSource.h>
#include "src/entities/XmlTag.h"
#include "src/entities/XmlPrimitive.h"
#include "src/entities/XmlValues.h"
#include "src/entities/XmlAttributes.h"
#include "src/entities/XmlElement.h"
#include "src/decode/parser/Visitor.h"

class RubanXml {
    XmlWriterFactory* xmlWriterFactory;
    XmlParserFactory* xmlParserFactory;
public:
    RubanXml(XmlWriterFactory* xmlWriterFactory, XmlParserFactory* xmlParserFactory);
    RubanXml();

    std::string xmlTagToString(XmlTag* tag);
    void xmlTagToFile(XmlTag* tag, std::string path);
    Sink* xmlTagToSink(XmlTag* tag, Sink* sink);
    XmlTag* xmlTreeFromSource(Source* tag, Visitor* listener = nullptr);
    XmlTag* xmlTreeFromString(std::string xml, Visitor* listener = nullptr);
    XmlTag* xmlTreeFromFile(const std::string& path, Visitor* listener = nullptr);

    virtual ~RubanXml();
};

#endif //XML_RUBAN_RUBANXML_H
