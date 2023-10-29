#include "request_queue.h"

RequestQueue::RequestQueue(const SearchServer& search_server)
    : search_server_(search_server)
{
}

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentStatus status) {
    return RequestQueue::AddFindRequest(
    raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
        return document_status == status;
    });
}

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query) {
    return RequestQueue::AddFindRequest(raw_query, DocumentStatus::ACTUAL);
}

int RequestQueue::GetNoResultRequests() const {
    return no_result_requests_;
}

void RequestQueue::EraseOldRequest() {
    if (requests_.front().answer_is_empty) {
        --no_result_requests_;
    }
    requests_.pop_front();
}

void RequestQueue::AddRequest(std::vector<Document> result) {
    bool is_result_empty = true;
    if (!result.empty()) {
        is_result_empty = false;
    }
    if (is_result_empty) {
        ++no_result_requests_;
    }

    requests_.push_back({current_time_, is_result_empty});
}
