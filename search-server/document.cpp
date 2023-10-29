#include "document.h"

#include <string>

Document::Document() = default;

Document::Document(int id, double relevance, int rating)
    : id(id)
    , relevance(relevance)
    , rating(rating) {
}

std::ostream& operator<<(std::ostream& out, const Document& document_info) {
    using namespace std::string_literals;

    out << "{ document_id = "s << document_info.id << ", relevance = "s << document_info.relevance
    << ", rating = "s << document_info.rating << " }"s;
    return out;
}
