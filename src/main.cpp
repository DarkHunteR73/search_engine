#include <iostream>

#include "jsonConverter.h"
#include "invertedIndex.h"
#include "searchServer.h"
#include "threadPool.h"

int main() {
    threadPool tp(std::thread::hardware_concurrency() - 1);

    invertedIndex idx;

    idx.updateDocumentBase(jsonConverter::getTextDocuments(), &tp);
    searchServer engine(idx);
    jsonConverter::putAnswers(engine.search(jsonConverter::getRequests(), &tp));

#ifdef WIN32
    std::cin.get();
#endif

    return EXIT_SUCCESS;
}
