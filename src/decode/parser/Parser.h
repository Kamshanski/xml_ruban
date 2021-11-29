#ifndef XML_RUBAN_PARSER_H
#define XML_RUBAN_PARSER_H


#include <src/source/Source.h>
#include <src/decode/DecoderParams.h>
#include <src/_utils/StringBuilder.h>
#include <src/entities/XmlTag.h>
#include "Visitor.h"
#include "ParserState.h"

class Parser {
private:
    Source* source;
    DecoderParams* params;
    Visitor* listener = nullptr;

    ParserState state = ParserState::NONE;
    int i = 0;
    int from = 0;
    StringBuilder buf;

    XmlTag* tag = nullptr;
    bool isOpened = false;

    std::string* tagName = nullptr;
    std::string* attrName = nullptr;

    bool _isUsed = false;
////////////////////////////////////////////////////////
    void setOpenTagReadStarted(char beginning);
    Parser(Source* source, DecoderParams* params, Visitor* listener, int i);
public:
    Parser(Source* source, DecoderParams* params, Visitor* listener);

    bool isUsed() const;
    void requireNotUsed() const;

    XmlTag* parse();
private:

    int processSource();
    void processChar(char ch);

    ParserState quoteSign();
    ParserState equalitySign();
    ParserState whitespace(char ch);
    ParserState endMarker();
    ParserState closeTagSign();
    ParserState openTagSign();
    ParserState valueChar(char ch);

    void readTagValue(char ch);
    void readTextValue();
    void readAttributeValue();
    void readAttributeName();
    void createTag();
    void closeTag();
    void markTagIsOpened();

    std::string& trimIfNeed(std::string& text);

    void append(const std::string& s);
    void append(char c);

    ParserState SAME_STATE();

    XmlTag* requireTagNotNull(XmlTag* tag, bool inner = false);
    std::string requireTagNameNotNullOrBlank(std::string* tagName);
    std::string requireAttrValueNotNull(std::string* attrValue);
    std::string requireAttrNameNotNullOrBlank(std::string* attrName);

    void checkTagIsOpened();

public:
    virtual ~Parser();
};


#endif //XML_RUBAN_PARSER_H
