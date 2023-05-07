#include <iostream>

#include "jsonConverter.h"
#include "invertedIndex.h"
#include "searchServer.h"

int main() {
    invertedIndex idx;
    idx.updateDocumentBase(jsonConverter::getTextDocuments());
    searchServer engine(idx);
    jsonConverter::putAnswers(engine.search(jsonConverter::getRequests()));
    return EXIT_SUCCESS;
}
