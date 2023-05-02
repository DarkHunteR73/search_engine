#include "jsonConverter.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <exception>

namespace fs = std::filesystem;

json jsonConverter::m_config = json();

std::vector<std::string> jsonConverter::getTextDocuments() {
    if (m_config.empty()) {
        try {
            readConfig();
        } catch (std::runtime_error &e) {
            std::cerr << e.what() << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    std::vector<std::string> result;

    if (m_config.contains("files")) {
        auto pathList = m_config["files"].get<std::vector<fs::path>>();
        result.reserve(pathList.size());

        for (auto &i: pathList) {
            std::ifstream file(i);
            if (file.is_open()) {
                std::stringstream ss;
                ss << file.rdbuf();
                result.emplace_back(ss.str());
                file.close();
            } else {
                std::cerr << i << " is not exists." << std::endl;
                result.emplace_back("");
            }
        }
    }

    return result;
}

int jsonConverter::getResponsesLimit() {
    if (m_config.empty()) {
        try {
            readConfig();
        } catch (std::runtime_error &e) {
            std::cerr << e.what() << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    if (m_config["config"].contains("max_responses")) {
        return m_config["config"]["max_responses"].get<int>();
    } else {
        return 5;
    }
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

        for (int k = 3 - count.length(); k > 0; --k) {
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
}

void jsonConverter::readConfig() {
    fs::path path{std::string(PROJECT_ROOT) + "config.json"};

    if (!fs::exists(path)) {
        throw std::runtime_error{"Config file is missing."};
    }

    std::cout << "Reading config file...";

    std::ifstream file(path);
    file >> m_config;
    file.close();

    if (!m_config.contains("config")) {
        std::cout << std::endl;
        throw std::runtime_error{"Config file is empty."};
    } else if (!m_config["config"].contains("version")) {
        std::cout << std::endl;
        throw std::runtime_error{"config.json does not contain version information."};
    } else if (m_config["config"]["version"].get<std::string>() !=
               (std::to_string(ENGINE_VERSION_MAJOR) + '.' +
                std::to_string(ENGINE_VERSION_MINOR))) {
        std::cout << std::endl;
        throw std::runtime_error{"config.json has incorrect file version"};
    }

    std::cout << " OK" << std::endl;

    std::cout << "Starting "
              << (m_config["config"].contains("name") ? m_config["config"]["name"] : "Unnamed engine")
              << " version " << m_config["config"]["version"] << std::endl;
}
