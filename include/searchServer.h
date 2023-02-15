#ifndef SEARCH_SERVER_H
#define SEARCH_SERVER_H

#include <string>
#include <vector>
#include "invertedIndex.h"

struct relativeIndex {
    size_t doc_id;
    float rank;

    bool operator==(const relativeIndex& other) const;
};

class searchServer {
public:
    /**
    * @param idx в конструктор класса передаётся ссылка на класс
    InvertedIndex,
    * чтобы SearchServer мог узнать частоту слов встречаемых в
    запросе
    */
    searchServer(invertedIndex& idx);

    /**
    * Метод обработки поисковых запросов
    * @param queries_input поисковые запросы взятые из файла
    requests.json
    * @return возвращает отсортированный список релевантных ответов для
    заданных запросов
    */

    std::vector<std::vector<relativeIndex>> search(const std::vector<std::string>& queries_input);
private:
    invertedIndex m_index;
};
#endif //SEARCH_SERVER_H
