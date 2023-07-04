#include "jsonConverter.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <mutex>
#include <sstream>
#include <exception>

namespace fs = std::filesystem;

json configCache = json();
std::once_flag configCheckedFlag;

std::vector<std::string> jsonConverter::getTextDocuments() {
    if (configCache.empty()) {
        try {
            std::call_once(configCheckedFlag, initCache);
        } catch (std::runtime_error &e) {
            std::cerr << e.what() << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    std::cout << "Receiving text documents:" << std::endl;

    std::vector<std::string> result;

    if (configCache.contains("files")) {
        auto pathList = configCache["files"].get<std::vector<fs::path>>();
        result.reserve(pathList.size());

        for (auto &i: pathList) {
            std::ifstream file(i);
            if (file.is_open()) {
                std::cout << i.string();
                std::stringstream ss;
                ss << file.rdbuf();
                result.emplace_back(ss.str());
                file.close();
                std::cout << " OK." << std::endl;
            } else {
                std::cerr << i.string() << " is not exists." << std::endl;
                result.emplace_back("");
            }
        }
    }

    return result;
}

int jsonConverter::getResponsesLimit() {
    if (configCache.empty()) {
        try {
            std::call_once(configCheckedFlag, initCache);
        } catch (std::runtime_error &e) {
            std::cerr << e.what() << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    if (configCache["config"].contains("max_responses")) {
        return configCache["config"]["max_responses"].get<int>();
    } else {
        return 5;
    }
}

std::string jsonConverter::getTitle() {
    if (configCache.empty()) {
        try {
            std::call_once(configCheckedFlag, initCache);
        } catch (std::runtime_error &e) {
            std::cerr << e.what() << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    return configCache["config"].contains("name") && !configCache["config"]["name"].empty()
        ? configCache["config"]["name"].get<std::string>()
        : std::string("Unnamed engine");
}

std::vector<std::string> jsonConverter::getRequests() {
    json j;
    fs::path path(std::string(PROJECT_ROOT) + "requests.json");
    std::ifstream file(path);

    if (file.is_open()) {
        file >> j;
        file.close();
    } else {
        std::cerr << "requests.json is missing." << std::endl;
    }

    if (j.contains("requests")) {
        return j["requests"].get<std::vector<std::string>>();
    } else {
        return {};
    }
}

void jsonConverter::putAnswers(relativeIndexArray_t answers) {
    json j;
    j["answers"];

    for (size_t i = 0; i < answers.size(); ++i) {
        std::string count = std::to_string(i + 1);

        for (int k = count.length(); k < 3; ++k) {
            count = '0' + count;
        }
        count = "request" + count;

        json::reference request = j["answers"][count];
        request["result"] = !answers[i].empty();

        if (answers[i].size() > 1) {
            json::reference relevance = request["relevance"];

            for (auto &it: answers[i]) {
                json entry;
                entry["docid"] = it.doc_id;
                entry["rank"] = it.rank;
                relevance.emplace_back(entry);
            }
        } else if (answers[i].size() == 1) {
            request["docid"] = answers[i][0].doc_id;
            request["rank"] = answers[i][0].rank;
        }
    }

    fs::path path(std::string(PROJECT_ROOT) + "answers.json");
    if (fs::exists(path)) {
        fs::remove(path);
    }

    std::ofstream file(path);
    file << std::setw(4) << j << std::endl;
    file.close();

    std::cout << "Results are saved to answers.json" << std::endl;
}

void jsonConverter::initCache() {
    fs::path path{std::string(PROJECT_ROOT) + "config.json"};

    if (!fs::exists(path)) {
        throw std::runtime_error{"Config file is missing."};
    }

    std::cout << "Reading config file...";

    std::ifstream file(path);
    file >> configCache;
    file.close();

    if (!configCache.contains("config")) {
        std::cout << std::endl;
        throw std::runtime_error{"Config file is empty."};
    } else if (!configCache["config"].contains("version")) {
        std::cout << std::endl;
        throw std::runtime_error{"config.json does not contain version information."};
    } else if (configCache["config"]["version"].get<std::string>() !=
               (std::to_string(ENGINE_VERSION_MAJOR) + '.' +
                std::to_string(ENGINE_VERSION_MINOR))) {
        std::cout << std::endl;
        throw std::runtime_error{"config.json has incorrect file version"};
    }

    std::cout << " OK" << std::endl;

    std::cout << "Starting "
              << jsonConverter::getTitle()
              << " version " << configCache["config"]["version"].get<std::string>() << std::endl;
}
