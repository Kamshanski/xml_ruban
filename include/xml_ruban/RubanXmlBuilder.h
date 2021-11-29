#ifndef XML_RUBAN_RUBANXMLBUILDER_H
#define XML_RUBAN_RUBANXMLBUILDER_H

#include <src/decode/parser/Visitor.h>
#include <include/xml_ruban/RubanXml.h>
#include "MutableDecoderParams.h"
#include "MutableEncoderParams.h"

class RubanXmlBuilder {
private:
    MutableDecoderParams* decoderParams = new MutableDecoderParams();
    MutableEncoderParams* encoderParams = new MutableEncoderParams();

public:
    RubanXmlBuilder* setPrettyPrinting(bool beautify = true);

    RubanXmlBuilder* keepBlankStringValues(bool keepBlankStringValues = false);

    RubanXmlBuilder* shortenEmptyTags(bool shortenEmptyTags = true);

    RubanXmlBuilder* setSpacesAroundAttributeEqualSign(bool setSpaces = false);

    RubanXmlBuilder* setTrim(bool trim = true);

    RubanXml create();

};


#endif //XML_RUBAN_RUBANXMLBUILDER_H
