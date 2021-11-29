
#include <decode/parser/XmlParserFactory.h>

#include "decode/parser/XmlParserFactory.h"


XmlParserFactory::XmlParserFactory(DecoderParams* decoderParams) : decoderParams(decoderParams) {
    if (this->decoderParams == nullptr) {
        throw std::invalid_argument("Decoder Params cannot be null");
    }
}

Parser* XmlParserFactory::get(Source* source, Visitor* listener) {
    if (source == nullptr) {
        throw std::invalid_argument("Source cannot be null");
    }
    return new Parser(source, decoderParams, listener);
}
XmlParserFactory::~XmlParserFactory() {
    delete decoderParams;
}
