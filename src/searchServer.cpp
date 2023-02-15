#include "searchServer.h"
#include "jsonConverter.h"

#include <limits>
#include <thread>
#include <mutex>

searchServer::searchServer(invertedIndex &idx) : m_index(idx) {}

std::vector<std::vector<relativeIndex>> searchServer::search(const std::vector<std::string> &queries_input) {
    jsonConverter converter;
    std::vector<std::vector<relativeIndex>> result(queries_input.size());

    std::vector<std::thread> threadVec(queries_input.size());
    std::mutex mtx;
    std::vector<std::vector<int>> sumsForEachQuery(queries_input.size(),
                                                   std::vector<int>(m_index.size(), 0));
    int maxAbsoluteRelevance = 0;

    for (size_t i = 0; i < threadVec.size(); ++i) {
        threadVec[i] = std::thread([this, &mtx, &maxAbsoluteRelevance]
                                           (const std::string &queryText,
                                            std::vector<int> &sumsForEachFile) {
            std::stringstream ss(queryText);
            std::string tmp;

            while (!ss.eof()) {
                ss >> tmp;

                const auto wordCount = m_index.getWordCount(tmp);

                if (!wordCount.empty()) {
                    for (int j = 0; j < wordCount.size(); ++j) {
                        sumsForEachFile[wordCount[j].doc_id] += wordCount[j].count;
                    }
                }
            }

            std::vector<int>::iterator absoluteRelevance;
            absoluteRelevance = std::max_element(sumsForEachFile.begin(), sumsForEachFile.end());

            const std::lock_guard<std::mutex> lock(mtx);
            if (*absoluteRelevance > maxAbsoluteRelevance) {
                maxAbsoluteRelevance = *absoluteRelevance;
            }
        }, queries_input[i], std::ref(sumsForEachQuery[i]));
    }

    for (auto &it: threadVec) {
        it.join();
    }

    int responsesLimit = converter.getResponsesLimit();
    for (int i = 0; i < threadVec.size(); ++i) {
        threadVec[i] = std::thread([responsesLimit, maxAbsoluteRelevance]
                                           (const std::vector<int> &sums, std::vector<relativeIndex> &result) {
            std::vector<relativeIndex> tmp;
            for (size_t j = 0; j < sums.size(); ++j) {
                if (sums[j] != 0) {
                    tmp.push_back({j, static_cast<float>(sums[j]) / maxAbsoluteRelevance});
                }
            }

            std::sort(tmp.begin(), tmp.end(), [](relativeIndex a, relativeIndex b) {
                return a.rank > b.rank ||
                       (std::abs(a.rank - b.rank) <= std::numeric_limits<float>::epsilon() && a.doc_id < b.doc_id);
            });

            if (tmp.size() <= responsesLimit) {
                result = std::move(tmp);
            } else {
                for (int j = 0; j < responsesLimit; ++j) {
                    result.push_back(tmp[j]);
                }
            }
        }, std::ref(sumsForEachQuery[i]), std::ref(result[i]));
    }

    for (auto &it: threadVec) {
        it.join();
    }

    return result;
}

bool relativeIndex::operator==(const relativeIndex &other) const {
    return doc_id == other.doc_id &&
           std::abs(rank - other.rank) <= std::numeric_limits<float>::epsilon();
}
