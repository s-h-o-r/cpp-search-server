#pragma once

#include <iostream>
#include <iterator>
#include <cstdlib>
#include <utility>
#include <vector>

template <typename Iterator>
class Paginator {
public:
    Paginator(Iterator begin, Iterator end, std::size_t page_size) {
        while (begin != end) {
            std::size_t container_size = distance(begin, end);
            if (page_size > container_size) {
                page_size = container_size;
            }
            auto page_begin = begin;
            std::advance(begin, page_size);
            auto page_end = begin;
            if (page_end > end ) {
                page_end = end;
            }
            pages_.push_back({page_begin, page_end});
        }
    }
    
    auto begin() const {
        return pages_.begin();
    }

    auto end() const {
        return pages_.end();
    }

private:
    std::vector<std::pair<Iterator, Iterator>> pages_;
};

template <typename Container>
auto Paginate(const Container& c, std::size_t page_size) {
    return Paginator(std::begin(c), std::end(c), page_size);
}

template <typename Iterator>
std::ostream& operator<< (std::ostream& out, std::pair<Iterator, Iterator> range) {
    auto range_begin = range.first;
    auto range_end = range.second;
    for (; range_begin != range_end; ++range_begin) {
        out << *range_begin;
    }
    return out;
}
