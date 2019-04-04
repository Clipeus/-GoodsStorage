#include "pch.h"
#include "goods_storage.h"

goods_storage_t::goods_storage_t()
{
}

goods_storage_t::~goods_storage_t()
{
}

bool goods_storage_t::add(const goods_t& goods)
{
    std::lock_guard<std::mutex> lock(_locker);

    auto it = _storage.insert(std::end(_storage), goods);
    if (it == std::end(_storage))
        return false;

    _article_goods.insert(std::make_pair(goods.article, it));
    _producer_goods.insert(std::make_pair(goods.producer, it));

    return true;
}

bool goods_storage_t::add(goods_t&& goods)
{
    std::lock_guard<std::mutex> lock(_locker);

    auto it = _storage.emplace(std::end(_storage), goods);
    if (it == std::end(_storage))
        return false;

    _article_goods.emplace(std::make_pair(goods.article, it));
    _producer_goods.emplace(std::make_pair(goods.producer, it));

    return true;
}

bool goods_storage_t::remove(const std::string& article)
{
    std::lock_guard<std::mutex> lock(_locker);

    auto it = _article_goods.find(article);
    if (it == std::end(_article_goods))
        return false;

    _producer_goods.erase((it->second)->producer);
    _storage.erase(it->second);
    _article_goods.erase(it);

    return true;
}

goods_t goods_storage_t::get_by_article(const std::string& article) const
{
    std::lock_guard<std::mutex> lock(_locker);

    const auto it = _article_goods.find(article);
    if (it == std::cend(_article_goods))
        return goods_t{};

    return *(it->second);
}

std::vector<goods_t> goods_storage_t::get_by_producer(const std::string& producer) const
{
    std::lock_guard<std::mutex> lock(_locker);

    std::vector<goods_t> result;
    const auto it_range = _producer_goods.equal_range(producer);
    if (it_range.first != std::cend(_producer_goods))
    {
        std::transform(it_range.first, it_range.second, std::back_inserter(result), [](auto it) {
            return *(it.second);
        });
    }

    return result;
}
