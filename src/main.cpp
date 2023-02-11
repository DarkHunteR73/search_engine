#include <iostream>

#include "jsonConverter.h"
#include "invertedIndex.h"
#include "searchServer.h"

int main() {
    jsonConverter converter;

    invertedIndex idx;
    idx.updateDocumentBase(converter.getTextDocuments());
    searchServer engine(idx);
    converter.putAnswers(engine.search(converter.getRequests()));
    return EXIT_SUCCESS;
}
