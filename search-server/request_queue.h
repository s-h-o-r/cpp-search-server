#pragma once

#include "document.h"
#include "search_server.h"
#include <deque>
#include <string>
#include <vector>

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);
    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);
    std::vector<Document> AddFindRequest(const std::string& raw_query);
    int GetNoResultRequests() const;

private:
    struct QueryResult {
        const int adding_time;
        bool answer_is_empty = true;
    };
    
    const SearchServer& search_server_;
    std::deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    int no_result_requests_ = 0;
    int current_time_ = 0;

    void EraseOldRequest();
    void AddRequest(std::vector<Document> result);
};

template <typename DocumentPredicate>
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
    ++current_time_;
    std::vector<Document> result = search_server_.FindTopDocuments(raw_query, document_predicate);
    while (!requests_.empty() && min_in_day_ <= current_time_ - requests_.front().adding_time) {
        RequestQueue::EraseOldRequest();
    }

    RequestQueue::AddRequest(result);
    return result;
}


