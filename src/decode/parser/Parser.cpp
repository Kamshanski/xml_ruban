#include <src/rules/Syntax.h>
#include "src/decode/parser/Parser.h"
#include "src/exceptions/ParserInternalExceptions.h"
#include "src/exceptions/CharacterMisexpectationExceptions.h"

void Parser::setOpenTagReadStarted(char beginning) {
    state = START_READ_TAG;
    state = valueChar(beginning);
}

bool Parser::isUsed() const {
    return _isUsed;
}

void Parser::requireNotUsed() const {
    if (isUsed()) {
        throw IllegalStateException("This object cannot be used twice. Create a new one");
    }
}

XmlTag* Parser::parse() {
    requireNotUsed();

    _isUsed = true;

    processSource();

    return requireTagNotNull(this->tag);
}

int Parser::processSource() {
    i = from;

    while (source->hasNext()) {
        char ch = source->nextChar();

        processChar(ch);

        if (state == TAG_CLOSED) {
            return i;
        }

        ++i;
    }

    return i;
}

void Parser::processChar(char ch) {
    int ii = i;
// may add & and ; for advanced string read
    if (ch == TAG_OPEN_BRACE) state = openTagSign();
    else if (ch == TAG_CLOSE_BRACE) state = closeTagSign();
    else if (is_whitespace(ch)) state = whitespace(ch);
    else if (ch == ATTR_EQ) state = equalitySign();
    else if (ch == ATTR_QUOTE) state = quoteSign();
    else if (ch == TAG_END_MARKER) state = endMarker();
    else state = valueChar(ch);

}

ParserState Parser::quoteSign() {
    char ch = ATTR_QUOTE;
    switch (state) {
        case NONE: throw IllegalXmlTagStart(i, ch);
        case START_READ_TAG:
        case READ_TAG_NAME:
        case START_READ_CLOSE_TAG_NAME:
        case READ_CLOSE_TAG_NAME: throw IllegalCharInTagNameException(i, ch);
        case INSIDE_TAG: throw IllegalCharInOpenTagException(i, ch);
        case READ_ATTR_NAME: throw IllegalCharInAttrNameException(i, ch);
        case WAIT_FOR_ATTR_EQ_SIGN: throw EqualSignMisexpectationException(i, ch);
        case WAIT_FOR_ATTR_OPEN_QUOTE: return READ_ATTR_VALUE;
        case READ_ATTR_VALUE: {
            readAttributeValue();
            return WAIT_FOR_ONE_WHITESPACE_THEN_INSIDE_TAG;
        }
        case WAIT_FOR_ONE_WHITESPACE_THEN_INSIDE_TAG: throw WhitespaceMisexpectationException(i, ch);
        case READ_TAG_VALUE: return valueChar(ch);
        case OPEN_TAG_CLOSE_MARKER: throw IllegalCharAfterTagCloseMarkerException(i, ch);
        case TAG_CLOSED: throw IllegalCharAfterTagEndException(i, ch);
        case WAIT_FOR_ONE_CLOSE_BRACE_THEN_TAG_CLOSE: throw CloseBraceMisexpectationException(i, ch);
        default: throw UnknownSateException(i, state);
    }
}
ParserState Parser::equalitySign() {
    char ch = ATTR_EQ;
    switch (state) {
        case NONE: throw IllegalXmlTagStart(i, ch);
        case START_READ_TAG:
        case READ_TAG_NAME:
        case START_READ_CLOSE_TAG_NAME:
        case READ_CLOSE_TAG_NAME: throw IllegalCharInTagNameException(i, ch);
        case INSIDE_TAG: throw IllegalCharInOpenTagException(i, ch);
        case READ_ATTR_NAME: {
            readAttributeName();
            return WAIT_FOR_ATTR_OPEN_QUOTE;
        }
        case WAIT_FOR_ATTR_EQ_SIGN: return WAIT_FOR_ATTR_OPEN_QUOTE;
        case WAIT_FOR_ATTR_OPEN_QUOTE: throw QuoteSignMisexpectationException(i, ch);
        case READ_ATTR_VALUE:
        case READ_TAG_VALUE: {
            return valueChar(ch);
        }
        case WAIT_FOR_ONE_WHITESPACE_THEN_INSIDE_TAG: throw WhitespaceMisexpectationException(i, ch);
        case OPEN_TAG_CLOSE_MARKER: throw IllegalCharAfterTagCloseMarkerException(i, ch);
        case TAG_CLOSED: throw IllegalCharAfterTagEndException(i, ch);
        case WAIT_FOR_ONE_CLOSE_BRACE_THEN_TAG_CLOSE: throw CloseBraceMisexpectationException(i, ch);
        default: throw UnknownSateException(i, state);
    }
}
ParserState Parser::whitespace(char ch) {
    switch (state) {
        case NONE: return SAME_STATE();
        case START_READ_TAG: throw IllegalCharInTagNameException(i, to_escape_sequence(true));
        case READ_TAG_NAME: {
            createTag();
            return INSIDE_TAG;
        }
        case INSIDE_TAG: return SAME_STATE();
        case READ_ATTR_NAME: {
            if (attrName == nullptr) {
                readAttributeName();
            }
            return WAIT_FOR_ATTR_EQ_SIGN;
        }
        case WAIT_FOR_ATTR_EQ_SIGN: return SAME_STATE();
        case WAIT_FOR_ATTR_OPEN_QUOTE: return SAME_STATE();
        case READ_ATTR_VALUE:return valueChar(ch);
        case WAIT_FOR_ONE_WHITESPACE_THEN_INSIDE_TAG: return INSIDE_TAG;
        case READ_TAG_VALUE: return valueChar(ch);
        case OPEN_TAG_CLOSE_MARKER:
        case START_READ_CLOSE_TAG_NAME: throw IllegalCharAfterTagCloseMarkerException(i, ch);
        case TAG_CLOSED: return SAME_STATE();
        case READ_CLOSE_TAG_NAME: return WAIT_FOR_ONE_CLOSE_BRACE_THEN_TAG_CLOSE;
        case WAIT_FOR_ONE_CLOSE_BRACE_THEN_TAG_CLOSE: return SAME_STATE();
        default: throw UnknownSateException(i, state);
    }
}
ParserState Parser::endMarker() {
    char ch = TAG_END_MARKER;
    switch (state) {
        case NONE: throw IllegalXmlTagStart(i, ch);
        case START_READ_TAG: {
            if (isOpened) {
                return START_READ_CLOSE_TAG_NAME;
            } else {
                throw IllegalCharInTagNameException(i, ch);
            }
        }
        case READ_TAG_NAME: {
            createTag();
            return OPEN_TAG_CLOSE_MARKER;
        }
        case INSIDE_TAG: return OPEN_TAG_CLOSE_MARKER;
        case READ_ATTR_NAME: throw IllegalCharInAttrNameException(i, ch);
        case WAIT_FOR_ATTR_EQ_SIGN: throw EqualSignMisexpectationException(i, ch);
        case WAIT_FOR_ATTR_OPEN_QUOTE: throw QuoteSignMisexpectationException(i, ch);
        case READ_ATTR_VALUE: return valueChar(ch);
        case WAIT_FOR_ONE_WHITESPACE_THEN_INSIDE_TAG: return OPEN_TAG_CLOSE_MARKER;
        case READ_TAG_VALUE: return valueChar(ch);
        case OPEN_TAG_CLOSE_MARKER: throw IllegalCharAfterTagCloseMarkerException(i, ch);
        case TAG_CLOSED: throw IllegalCharAfterTagEndException(i, ch);
        case START_READ_CLOSE_TAG_NAME:
        case READ_CLOSE_TAG_NAME: throw IllegalCharInTagNameException(i, ch);
        case WAIT_FOR_ONE_CLOSE_BRACE_THEN_TAG_CLOSE: throw CloseBraceMisexpectationException(i, ch);
        default: throw UnknownSateException(i, state);
    }
}
ParserState Parser::closeTagSign() {
    char ch = TAG_CLOSE_BRACE;
    switch (state) {
        case NONE: throw IllegalXmlTagStart(i, ch);
        case START_READ_TAG:
        case START_READ_CLOSE_TAG_NAME : throw IllegalCharInTagNameException(i ,ch);
        case READ_TAG_NAME: {
            createTag();
            return READ_TAG_VALUE;
        }
        case INSIDE_TAG: {
            markTagIsOpened();
            return READ_TAG_VALUE;
        }
        case READ_ATTR_NAME: throw IllegalCharInAttrNameException(i, ch);
        case WAIT_FOR_ATTR_EQ_SIGN: throw EqualSignMisexpectationException(i, ch);
        case WAIT_FOR_ATTR_OPEN_QUOTE: throw QuoteSignMisexpectationException(i, ch);
        case READ_ATTR_VALUE: throw IllegalCharInAttrValueException(i, ch);
        case WAIT_FOR_ONE_WHITESPACE_THEN_INSIDE_TAG: {
            markTagIsOpened();
            return READ_TAG_VALUE;
        }
        case READ_TAG_VALUE: throw IllegalCharInTagTextValueException(i, ch);
        case OPEN_TAG_CLOSE_MARKER: {
            if (listener != nullptr) {
                std::string openTagName = requireTagNameNotNullOrBlank(tagName);
                listener->onTagClosed(openTagName);
            }
            return TAG_CLOSED;
        }
        case TAG_CLOSED: throw IllegalCharAfterTagEndException(i, ch);
        case READ_CLOSE_TAG_NAME:
        case WAIT_FOR_ONE_CLOSE_BRACE_THEN_TAG_CLOSE: {
            closeTag();
            return TAG_CLOSED;
        }
        default: throw UnknownSateException(i, state);
    }
}
ParserState Parser::openTagSign() {
    char ch = TAG_OPEN_BRACE;
    switch (state) {
        case NONE: return START_READ_TAG;
        case START_READ_TAG:
        case START_READ_CLOSE_TAG_NAME :
        case READ_TAG_NAME:
        case READ_CLOSE_TAG_NAME: throw IllegalCharInTagNameException(i ,ch);
        case INSIDE_TAG: throw IllegalCharInOpenTagException(i, ch);
        case READ_ATTR_NAME: throw IllegalCharInAttrNameException(i, ch);
        case WAIT_FOR_ATTR_EQ_SIGN: throw EqualSignMisexpectationException(i, ch);
        case WAIT_FOR_ATTR_OPEN_QUOTE: throw QuoteSignMisexpectationException(i, ch);
        case READ_ATTR_VALUE: throw IllegalCharInAttrValueException(i, ch);
        case WAIT_FOR_ONE_WHITESPACE_THEN_INSIDE_TAG: throw WhitespaceMisexpectationException(i, ch);
        case READ_TAG_VALUE: {
            readTextValue();
            return START_READ_TAG; // START_READ_TAG + isOpened=true дают чтение нового внутреннего тэга
        }
        case OPEN_TAG_CLOSE_MARKER: throw IllegalCharAfterTagCloseMarkerException(i, ch);
        case TAG_CLOSED: throw IllegalCharAfterTagEndException(i, ch);
        case WAIT_FOR_ONE_CLOSE_BRACE_THEN_TAG_CLOSE: throw CloseBraceMisexpectationException(i, ch);
        default: throw UnknownSateException(i, state);
    }
}
ParserState Parser::valueChar(char ch) {
    switch (state) {
        case NONE: throw IllegalXmlTagStart(i, ch);
        case START_READ_TAG: {
            if (isOpened) {
                readTagValue(ch);
                return READ_TAG_VALUE;
            } else {
                append(ch);
                return READ_TAG_NAME;
            }
        }
        case READ_TAG_NAME: {
            append(ch);
            return SAME_STATE();
        }
        case INSIDE_TAG: {
            append(ch);
            return READ_ATTR_NAME;
        }
        case READ_ATTR_NAME: {
            append(ch);
            return SAME_STATE();
        }
        case WAIT_FOR_ATTR_EQ_SIGN: throw EqualSignMisexpectationException(i, ch);
        case WAIT_FOR_ATTR_OPEN_QUOTE: throw QuoteSignMisexpectationException(i, ch);
        case READ_ATTR_VALUE: {
            append(ch);
            return SAME_STATE();
        }
        case WAIT_FOR_ONE_WHITESPACE_THEN_INSIDE_TAG: throw WhitespaceMisexpectationException(i, ch);
        case READ_TAG_VALUE: {
            append(ch);
            return SAME_STATE();
        }
        case OPEN_TAG_CLOSE_MARKER: throw IllegalCharAfterTagCloseMarkerException(i, ch);
        case TAG_CLOSED: throw IllegalCharAfterTagEndException(i, ch);
        case START_READ_CLOSE_TAG_NAME : {
            append(ch);
            return READ_CLOSE_TAG_NAME;
        }
        case READ_CLOSE_TAG_NAME: {
            append(ch);
            return SAME_STATE();
        }
        case WAIT_FOR_ONE_CLOSE_BRACE_THEN_TAG_CLOSE: throw CloseBraceMisexpectationException(i, ch);
        default: throw UnknownSateException(i, state);
    }
}

void Parser::readTagValue(char ch) {
    Parser innerParser = Parser(source, params, listener, i+1); // +1 as '<' is already consumed
    innerParser.setOpenTagReadStarted(ch);

    i = innerParser.processSource();

    XmlTag* outerTag = requireTagNotNull(this->tag);
    XmlTag* innerTag = requireTagNotNull(innerParser.tag, true);

    outerTag->addValue(innerTag);
}

void Parser::readTextValue() {
    checkTagIsOpened();

    if (buf.isNotEmpty()) {
        if (buf.isNotBlank() || params->isKeepBlankStringValues()) {
            std::string textValue = buf.extract();
            trimIfNeed(textValue);

            XmlTag* tag = requireTagNotNull(this->tag);

            tag->addValue(textValue);

            if (listener != nullptr) {
                listener->onTagTextValue(textValue);
            }
        }

        buf.clear();
    }
}

void Parser::readAttributeValue() {
    std::string value = buf.extract();
    value = requireAttrValueNotNull(&value);

    std::string name = requireAttrNameNotNullOrBlank(attrName);
    XmlTag* tag = requireTagNotNull(this->tag);

    if (listener != nullptr) {
        listener->onAttribute(name, value);
    }
//
//    tag->addAttribute(name, value);

    attrName = nullptr;
}

void Parser::readAttributeName() {
    attrName = new std::string();
    *attrName = buf.extract();

    trimIfNeed(*attrName);
}

void Parser::createTag() {

    std::string tagName = buf.extract();
    tagName = requireTagNameNotNullOrBlank(&tagName);
    this->tagName = new std::string();
    *this->tagName = tagName;

    markTagIsOpened();

    if (listener != nullptr) {
        listener->onTagOpen(tagName);
    }

    tag = new XmlTag(tagName);
}

void Parser::closeTag() {
    std::string closeTagName = buf.extract();
    closeTagName = requireTagNameNotNullOrBlank(&closeTagName);
    std::string openTagName = requireTagNameNotNullOrBlank(tagName);

    if (closeTagName != openTagName) {
        throw TagNamesAreDifferentException(i, state, openTagName, closeTagName);
    }

    if (listener != nullptr) {
        listener->onTagClosed(closeTagName);
    }
}

void Parser::markTagIsOpened() {
    isOpened = true;
}

std::string& Parser::trimIfNeed(std::string& text) {
    if (params->isTrim()) {
        return trim(text);
    }
    return text;
}

void Parser::append(const std::string& s) {
    buf.append(s);
}

void Parser::append(char c) {
    buf.append(to_string(c));
}

ParserState Parser::SAME_STATE() {
    return state;
}


XmlTag* Parser::requireTagNotNull(XmlTag* tag, bool inner) {
    if (tag == nullptr) {
        if (inner)
            throw TagIsNullException(i, state);
        else
            throw InnerTagIsNullException(i, state);
    }
    return tag;
}

std::string Parser::requireTagNameNotNullOrBlank(std::string* tagName) {
    if (tagName == nullptr) {
        throw TagNameIsNullException(i, state);
    }
    if (is_blank(*tagName)) {
        throw TagNameIsBlankException(i, state);
    }
    return *tagName;
}

std::string Parser::requireAttrValueNotNull(std::string* attrValue) {
    if (attrValue == nullptr) {
        throw AttrValueIsNullException(i, state);
    }
    return *attrValue;
}

std::string Parser::requireAttrNameNotNullOrBlank(std::string* attrName) {
    if (attrName == nullptr) {
        throw AttrNameIsNullException(i, state);
    }
    if (is_blank(*attrName)) {
        throw AttrNameIsBlankException(i, state);
    }
    return *attrName;
}

void Parser::checkTagIsOpened() {
    if (!isOpened) {
        throw TagIsNotOpenedException(i, state);
    }
}

Parser::Parser(Source* source, DecoderParams* params, Visitor* listener, int from) :
        source(source), params(params), listener(listener), from(from) {
    if (from < 0) {
        std::stringstream ss;
        ss << "From cant be less than 0: " << from;
        throw std::out_of_range(ss.str());
    }
}
Parser::Parser(Source* source, DecoderParams* params, Visitor* listener) :
        Parser(source, params, listener, 0) {

}
Parser::~Parser() {
    delete tagName;
    delete attrName;
}
