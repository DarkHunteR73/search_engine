#include <iostream>

#include "converterJSON.h"

int main() {
    converterJSON converter;

    // putAnswers test
    std::vector<std::vector<std::pair<int, float>>> vec{{{0,0.989},{1,0.897},{2,0.750},{3,0.670},{4,0.561}},{{0,0.769}},{}};
    converter.putAnswers(vec);
    //
    return EXIT_SUCCESS;
}
