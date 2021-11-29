#ifndef XML_RUBAN_CHECKEXTENSIONS_H
#define XML_RUBAN_CHECKEXTENSIONS_H

#endif //XML_RUBAN_CHECKEXTENSIONS_H

void checkNotNull(void* value, std::string& msg = "Expected non null value") {
    if (value == nullptr) {
        throw std::invalid_argument(msg);
    }
}
