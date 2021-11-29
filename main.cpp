#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-raw-string-literal"
#include "include/xml_ruban/RubanXml.h"
#include <iostream>
#include <include/xml_ruban/RubanXmlBuilder.h>

class Visua : public Visitor {
    void onAttribute(std::string attrName, std::string attrValue) override {
        std::cout << "Found attribute: " << attrName << "=" << attrValue << std::endl;
    }
    void onTagOpen(std::string tagName) override {
        std::cout << "Tag opened: " << tagName << std::endl;
    }
    void onTagClosed(std::string tagName) override {
        std::cout << "Tag closed: " << tagName << std::endl;
    }
    void onTagTextValue(std::string tagTextValue) override {
        std::cout << "Tag text value: " << tagTextValue << std::endl;
    }
};

void testString() {
    std::cout << "\n\nTest String\n\n";

    std::string xml = "<0 param=\"value\">\n"
                      "     <1>\n"
                      "         Some Text\n"
                      "     </1>\n"
                      "     <2 param2=\"something\" param3=\"nothing\">\n"
                      "         Pre-Text <Inline>Inlined text</Inline> Inner-text <Inline attr = \"inlined text 2\"/> Post-text.\n"
                      "     </2>\n"
                      "     <3/>\n"
                      "     <4 at=\"1\"  apa  =  \"popo\"/>\n"
                      "     <5   at=\"2\"  apa  = \"uuuu\"     />\n"
                      "     <6   at=\"2\"   ></6>\n"
                      "     <7>    </7>\n"
                      "</0>";

    RubanXmlBuilder* builder = (new RubanXmlBuilder())
            ->shortenEmptyTags(true)
            ->keepBlankStringValues(false)
            ->setPrettyPrinting(true)
            ->setTrim(true);
    RubanXml rx = builder->create();

    auto visitor = new Visua();
    auto tag = rx.xmlTreeFromString(xml, visitor);
    std::cout << rx.xmlTagToString(tag) << std::endl;

    delete builder;
    delete tag;
    delete visitor;
}

void testFile(std::string sourcePath, std::string sinkPath) {
    std::cout << "\n\nTest File\n\n";

    RubanXmlBuilder* builder = (new RubanXmlBuilder())
            ->shortenEmptyTags(true)
            ->keepBlankStringValues(false)
            ->setPrettyPrinting(true)
            ->setTrim(true);
    RubanXml rx = builder->create();

    auto visitor = new Visua();
    auto tag = rx.xmlTreeFromFile(sourcePath, visitor);

    rx.xmlTagToFile(tag, std::move(sinkPath));

    delete builder;
    delete tag;
    delete visitor;
}

void testCustomTree() {
    std::cout << "\n\nTest Custom Tree\n\n";

    RubanXmlBuilder* builder = (new RubanXmlBuilder())
            ->shortenEmptyTags(true)
            ->keepBlankStringValues(false)
            ->setPrettyPrinting(true)
            ->setTrim(true);
    RubanXml rx = builder->create();

    auto rootTag = new XmlTag("Tag1");
    rootTag->addAttribute("attr1", "val1");
    rootTag->addAttribute("attr2", "val2");

    rootTag->addValue("Text Before");

    auto innerTag1 = new XmlTag("attrTag");
    innerTag1->addAttribute("attrTag2", "valTag2");
    rootTag->addValue(innerTag1);

    auto innerTag2 = new XmlTag("Integer");
    innerTag2->addValue("1234");
    rootTag->addValue(innerTag2);

    auto innerTag3 = new XmlTag("EmptyTag");
    rootTag->addValue(innerTag3);

    auto innerTag4 = new XmlTag("Boolean");
    innerTag4->addValue("tRuE");
    rootTag->addValue(innerTag4);

    rootTag->addValue("Text After");

    rootTag->getValues();


    auto visitor = new Visua();
    std::cout << rx.xmlTagToString(rootTag) << std::endl;

    XmlTag* intTag = rootTag->firstOrNullTag("Integer");
    if (intTag != nullptr) {
        std::cout << "Integer tag = " << intTag->getValues()->single()->asXmlPrimitive()->asInt() << std::endl;
    } else {
        std::cout << "Integer tag was not found" << std::endl;
    }

    XmlTag* boolTag = rootTag->firstOrNullTag("Boolean");
    if (boolTag != nullptr) {
        std::cout << "Boolean tag = " << std::boolalpha << boolTag->getValues()->single()->asXmlPrimitive()->asBoolean() << std::endl;
    } else {
        std::cout << "Boolean tag was not found" << std::endl;
    }


    delete builder;
    delete rootTag;
    delete visitor;
}

int main() {
//    std::ofstream myfile;
//    myfile.open (u8"example.txt");
//    myfile << u8"Writing фыаролуа this to a file.\n";
//    myfile.close();



    // parse in visitor
    // make source closeable +
    // handle exceptions for file close +


    testString();

    testFile("C:\\Users\\Dawan\\Desktop\\source.xml", "C:\\Users\\Dawan\\Desktop\\sink.xml");

    testCustomTree();

    return 0;
}


#pragma clang diagnostic pop