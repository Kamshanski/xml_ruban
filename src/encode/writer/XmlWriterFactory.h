#ifndef XML_RUBAN_XMLWRITERFACTORY_H
#define XML_RUBAN_XMLWRITERFACTORY_H


#include <sink/Sink.h>
#include <encode/writer/Writer.h>

class XmlWriterFactory {
private:
    EncoderParams* encoderParams;

public:
    explicit XmlWriterFactory(EncoderParams* encoderParams);
    Writer* get(Sink* sink);
    virtual ~XmlWriterFactory();
};


#endif //XML_RUBAN_XMLWRITERFACTORY_H
