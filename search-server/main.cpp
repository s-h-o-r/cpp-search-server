#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    int id;
    double relevance;
};

class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document) {
        const vector<string> content = SplitIntoWordsNoStop(document);
        double term_frequency = 1. / content.size();
        for (const string& word : content) {
            word_to_documents_freqs_[word][document_id] += term_frequency;
        }
        ++document_count_;
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        const Query query_words = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_words);

        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                 return lhs.relevance > rhs.relevance;
             });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

private:

    struct Query {
        map<string, double> plus_words_with_frequency;
        set<string> minus_words;
    };

    int document_count_ = 0;

    map<string, map<int, double>> word_to_documents_freqs_;

    set<string> stop_words_;

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    double CalculateIDF(const string& word) const {
            return log(static_cast<double>(document_count_) / word_to_documents_freqs_.at(word).size());
    }

    Query ParseQuery(const string& text) const {
        Query query_words;
        for (const string& word : SplitIntoWordsNoStop(text)) {
            if (word[0] == '-') {
                query_words.minus_words.insert(word.substr(1));
            } else {
                if (word_to_documents_freqs_.count(word) > 0) {
                    query_words.plus_words_with_frequency.insert({word, CalculateIDF(word)});
                }
            }
        }
        return query_words;
    }

    vector<Document> FindAllDocuments(const Query& query_words) const {
        vector<Document> matched_documents;
        map<int, double> documents_with_tf_idf;
        
        for (const auto& [plus_word, idf] : query_words.plus_words_with_frequency) {
            if (word_to_documents_freqs_.count(plus_word) > 0) {
                for (const auto& [id, tf] : word_to_documents_freqs_.at(plus_word)) {
                        documents_with_tf_idf[id] += tf * idf;
                }
            }
        }
        for (const auto& minus_word : query_words.minus_words) {
            if (word_to_documents_freqs_.count(minus_word)) {
                for (const auto& [id, tf] : word_to_documents_freqs_.at(minus_word)) {
                    documents_with_tf_idf.erase(id);
                }
            }
        }

        for (const auto& [id, relevance] : documents_with_tf_idf) {
            matched_documents.push_back({id, relevance});
        }

        return matched_documents;
    }
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
             << "relevance = "s << relevance << " }"s << endl;
    }
}