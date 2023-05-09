#include "threadPool.h"
#include "searchServer.h"
#include "jsonConverter.h"

#include <limits>

searchServer::searchServer(invertedIndex &idx) : m_index(idx) {}

relativeIndexArray_t searchServer::search(const std::vector<std::string> &queries_input) {
    relativeIndexArray_t result(queries_input.size());

    std::vector<std::vector<int>> sumsForEachQuery(queries_input.size(),
                                                   std::vector<int>(m_index.size(), 0));
    std::atomic<int> maxAbsoluteRelevance = 1;

    {
        threadPool tp(std::min(queries_input.size(), static_cast<size_t>(std::thread::hardware_concurrency() - 1)));

        for (size_t i = 0; i < queries_input.size(); ++i) {
            tp.addTask([this, &maxAbsoluteRelevance]
                               (const std::string &queryText,
                                std::vector<int> &sumsForEachFile) {
                std::stringstream ss(queryText);
                std::string tmp;

                while (!ss.eof()) {
                    ss >> tmp;

                    const auto wordCount = m_index.getWordCount(tmp);

                    if (!wordCount.empty()) {
                        for (auto& j : wordCount) {
                            sumsForEachFile[j.doc_id] += j.count;
                        }
                    }
                }

                std::vector<int>::iterator absoluteRelevance;
                absoluteRelevance = std::max_element(sumsForEachFile.begin(), sumsForEachFile.end());

                if (*absoluteRelevance > maxAbsoluteRelevance) {
                    maxAbsoluteRelevance = *absoluteRelevance;
                }
            }, std::ref(queries_input[i]), std::ref(sumsForEachQuery[i]));
        }

        tp.wait();

        size_t responsesLimit;

#ifdef TEST_MODE
        responsesLimit = 5;
#else
        responsesLimit = jsonConverter::getResponsesLimit();
#endif
        for (size_t i = 0; i < queries_input.size(); ++i) {
            tp.addTask([responsesLimit, &maxAbsoluteRelevance]
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
                    for (size_t j = 0; j < responsesLimit; ++j) {
                        result.push_back(tmp[j]);
                    }
                }
            }, std::ref(sumsForEachQuery[i]), std::ref(result[i]));
        }
    }

    return result;
}

bool relativeIndex::operator==(const relativeIndex &other) const {
    return doc_id == other.doc_id &&
           std::abs(rank - other.rank) <= std::numeric_limits<float>::epsilon();
}
