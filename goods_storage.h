#pragma once

#include "goods.h"

class goods_storage_t
{
    using storage_t = std::list<goods_t>;
    using article_goods_t = std::unordered_map<std::string, storage_t::iterator>;
    using producer_goods_t = std::multimap<std::string, storage_t::iterator>;

public:
    goods_storage_t();
    virtual ~goods_storage_t();

public:
    bool add(const goods_t& goods);
    bool add(goods_t&& goods);
    bool remove(const std::string& article);
    goods_t get_by_article(const std::string& article) const;
    std::vector<goods_t> get_by_producer(const std::string& producer) const;

private:
    mutable std::mutex _locker;
    storage_t _storage;
    article_goods_t _article_goods;
    producer_goods_t _producer_goods;
};

