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
    template<typename T, typename = typename std::enable_if_t<std::is_same<typename std::decay<T>::type, goods_t>::value>>
    bool add(T&& goods)
    {
        std::lock_guard<std::mutex> lock(_locker);

        std::string article = goods.article;
        std::string producer = goods.producer;

        auto it = _storage.insert(std::end(_storage), std::forward<T>(goods));
        if (it == std::end(_storage))
            return false;

        _article_goods.insert(std::make_pair(std::move(article), it));
        _producer_goods.insert(std::make_pair(std::move(producer), it));
        return true;
    }

    bool remove(const std::string& article);
    goods_t get_by_article(const std::string& article) const;
    std::vector<goods_t> get_by_producer(const std::string& producer) const;

private:
    mutable std::mutex _locker;
    storage_t _storage;
    article_goods_t _article_goods;
    producer_goods_t _producer_goods;
};

