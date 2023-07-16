#ifndef CONVERTER_JSON_H
#define CONVERTER_JSON_H

#include <string>
#include <vector>

#include "nlohmann/json.hpp"
#include "searchServer.h"

using json = nlohmann::json;

struct relativeIndex;

class jsonConverter {
public:
    jsonConverter() = default;

    /**
    * Метод получения содержимого файлов
    * @return Возвращает список с содержимым файлов перечисленных
    * в config.json
    */
    static std::vector<std::string> getTextDocuments();

    /**
    * Метод считывает поле max_responses для определения предельного
    * количества ответов на один запрос
    * @return значение поля max_responses. Если оно отсутствует, возвращает 5
    */
    static int getResponsesLimit();

    /**
    * Метод изменяет значение max_responses
    * @param значение
    */
    static void setResponsesLimit(int);

    /**
     * Метод считывает имя движка из файла config.json
     */
    static std::string getTitle();

    /**
    * Метод получения запросов из файла requests.json
    * @return возвращает список запросов из файла requests.json
    */
    static std::vector<std::string> getRequests();

    /**
    * Положить в файл answers.json результаты поисковых запросов
    */
    static void putAnswers(relativeIndexArray_t answers);

    /**
    * Инициализация кэша из config.json
    */
    static void initCache();
};
#endif //CONVERTER_JSON_H
