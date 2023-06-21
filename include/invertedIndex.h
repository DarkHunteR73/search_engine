#ifndef INVERTED_INDEX_H
#define INVERTED_INDEX_H

#include "threadPool.h"

#include <map>
#include <string>
#include <vector>

struct entry {
    size_t doc_id, count;

    bool operator==(const entry& other) const;
};

class invertedIndex {
public:
    invertedIndex() = default;

    /**
    * Обновить или заполнить базу документов, по которой будем совершать
    поиск*
    @param input_docs содержимое документов
    */
    void updateDocumentBase(const std::vector<std::string> &input_docs, threadPool *tp = nullptr);

    /**
    * Метод определяет количество вхождений слова word в загруженной базе
    документов
    * @param word слово, частоту вхождений которого необходимо определить
    * @return возвращает подготовленный список с частотой слов
    */
    std::vector<entry> getWordCount(const std::string& word);

    size_t size();

private:
    std::vector<std::string> docs;
    std::map<std::string, std::vector<entry>> freq_dictionary;
};
#endif //INVERTED_INDEX_H
