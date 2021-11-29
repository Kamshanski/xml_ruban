#include "include/xml_ruban/RubanXml.h"
#include <iostream>
#include <sink/FileSink.h>


RubanXml::RubanXml(XmlWriterFactory* xmlWriterFactory, XmlParserFactory* xmlParserFactory)
        : xmlWriterFactory(xmlWriterFactory), xmlParserFactory(xmlParserFactory) {}

RubanXml::RubanXml() {
    this->xmlWriterFactory = new XmlWriterFactory(new EncoderParams());
    this->xmlParserFactory = new XmlParserFactory(new DecoderParams());
}

std::string RubanXml::xmlTagToString(XmlTag* tag) {
    StringSink sink = StringSink();
    xmlTagToSink(tag, &sink);
    sink.close();
    return sink.toString();
}

void RubanXml::xmlTagToFile(XmlTag* tag, std::string path) {
    FileSink sink = FileSink(path);
    xmlTagToSink(tag, &sink);
    sink.close();
}

Sink* RubanXml::xmlTagToSink(XmlTag* tag, Sink* sink) {
    try {
        Writer* writer = xmlWriterFactory->get(sink);
        writer->write(tag);
        delete writer;
        return sink;
    } catch (std::exception& ex) {
        sink->close();
        throw ex;
    }
}



XmlTag* RubanXml::xmlTreeFromString(std::string xml, Visitor* listener) {
    auto source = StringSource(std::move(xml));
    return xmlTreeFromSource(&source, listener);
}

XmlTag* RubanXml::xmlTreeFromFile(const std::string& path, Visitor* listener) {
    auto source = FileSource(path);
    return xmlTreeFromSource(&source, listener);
}

XmlTag* RubanXml::xmlTreeFromSource(Source* source, Visitor* listener) {
    try {
        Parser* parser = xmlParserFactory->get(source, listener);
        XmlTag* tag = parser->parse();
        delete parser;
        return tag;
    } catch (std::exception& ex) {
        source->close();
        throw ex;
    }
}


RubanXml::~RubanXml() {
    delete xmlWriterFactory;
    delete xmlParserFactory;
}
