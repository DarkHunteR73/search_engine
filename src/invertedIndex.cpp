#include "invertedIndex.h"
#include "threadPool.h"

#include <mutex>
#include <sstream>
#include <algorithm>

void invertedIndex::updateDocumentBase(std::vector<std::string> input_docs, threadPool *tp) {
    if (docs != input_docs) {
        docs = input_docs;
        freq_dictionary.clear();

        std::mutex mtx;

        bool hasExternTp = false;
        if (tp == nullptr) {
            tp = new threadPool(std::thread::hardware_concurrency() - 1);
            hasExternTp = true;
        }

        for (size_t i = 0; i < input_docs.size(); ++i) {
            tp->addTask([this, &mtx](const std::string &text, const size_t docIndex) {
                std::stringstream ss(text);
                std::string tmp;

                while (!ss.eof()) {
                    ss >> tmp;
                    {
                        const std::lock_guard<std::mutex> lock(mtx);

                        if (freq_dictionary.contains(tmp)) {
                            bool match = false;
                            for (auto &it: freq_dictionary[tmp]) {
                                if (it.doc_id == docIndex) {
                                    ++it.count;
                                    match = true;
                                    break;
                                }
                            }

                            if (!match) {
                                freq_dictionary[tmp].emplace_back(entry{docIndex, 1});
                            }
                        } else {
                            freq_dictionary[tmp] = std::vector<entry>{{docIndex, 1}};
                        }
                    }
                }
            }, docs[i], i);
        }

        tp->wait();

        if (hasExternTp == true) {
            delete tp;
        }

        for (auto& it : freq_dictionary) {
            std::sort(it.second.begin(), it.second.end(), [](entry a, entry b) {
                return a.doc_id < b.doc_id;
            });
        }
    }
}

std::vector<entry> invertedIndex::getWordCount(const std::string &word) {
    return freq_dictionary.contains(word) ? freq_dictionary[word] : std::vector<entry>();
}

size_t invertedIndex::size() {
    return docs.size();
}

bool entry::operator==(const entry &other) const {
    return (doc_id == other.doc_id && count == other.count);
}
