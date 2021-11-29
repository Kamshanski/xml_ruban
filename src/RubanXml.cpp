#include "include/xml_ruban/RubanXml.h"
#include <iostream>
#include <sink/FileSink.h>
#include <decode/parser/TagBuilder.h>


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



XmlTag* RubanXml::xmlTreeFromString(std::string xml) {
    auto source = StringSource(std::move(xml));
    return xmlTreeFromSource(&source);
}

XmlTag* RubanXml::xmlTreeFromFile(const std::string& path) {
    auto source = FileSource(path);
    return xmlTreeFromSource(&source);
}

XmlTag* RubanXml::xmlTreeFromSource(Source* source) {
    try {
        TagBuilder builder = TagBuilder();

        Parser* parser = xmlParserFactory->get(source, &builder);
        parser->parse();

        XmlTag* tag = builder.getRoot();
        delete parser;
        return tag;
    } catch (std::exception& ex) {
        source->close();
        throw ex;
    }
}
void RubanXml::parseFromSource(Source* source, Visitor* visitor) {
    try {
        Parser* parser = xmlParserFactory->get(source, visitor);
        parser->parse();
        delete parser;
    } catch (std::exception& ex) {
        source->close();
        throw ex;
    }
}


RubanXml::~RubanXml() {
    delete xmlWriterFactory;
    delete xmlParserFactory;
}
