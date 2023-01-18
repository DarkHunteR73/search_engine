#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>
#include <filesystem>

#include "nlohmann/json.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;

class converterJSON {
public:
    converterJSON() = default;

    /**
    * Метод получения содержимого файлов
    * @return Возвращает список с содержимым файлов перечисленных
    * в config.json
    */
    std::vector<std::string> getTextDocuments();

    /**
    * Метод считывает поле max_responses для определения предельного
    * количества ответов на один запрос
    * @return значение поля max_responses. Если оно отсутствует, возвращает 5
    */
    int getResponsesLimit();

    /**
    * Метод получения запросов из файла requests.json
    * @return возвращает список запросов из файла requests.json
    */
    std::vector<std::string> getRequests();

    /**
    * Положить в файл answers.json результаты поисковых запросов
    */
    void putAnswers(std::vector<std::vector<std::pair<int, float>>> answers);

private:
    /**
    * Метод чтения из файлов config.json
    */
    void readConfig();

    json m_config;
};
