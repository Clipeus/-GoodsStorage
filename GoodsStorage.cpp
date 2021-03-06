// GoodsStorage.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "goods_storage.h"

std::mutex _cout_locker;
std::condition_variable _condition;
std::atomic<bool> _stop = false;
goods_storage_t _goods_storage;
std::random_device _rnd;
std::uniform_int_distribution<> _rnd_dis(500, 5000);

void writer(const std::string& producer)
{
    std::mutex mutex;

    while (!_stop.load())
    {
        for (int i = 1; i <= 5; i++)
        {
            _goods_storage.add(goods_t{"A-" + std::to_string(i), producer, "Goods " + std::to_string(i), i + 10});

            std::lock_guard<std::mutex> lock(_cout_locker);
            std::cout << '#' << std::this_thread::get_id() << ' ' << "Added ..." << std::endl;
        }

        std::unique_lock<std::mutex> lock(mutex);
        _condition.wait_for(lock, std::chrono::milliseconds(_rnd_dis(_rnd)), [] { return _stop.load(); });

        for (int i = 1; i <= 5; i++)
        {
            _goods_storage.remove("A-" + std::to_string(i));

            std::lock_guard<std::mutex> lock(_cout_locker);
            std::cout << '#' << std::this_thread::get_id() << ' ' << "Removed ..." << std::endl;
        }

        _condition.wait_for(lock, std::chrono::milliseconds(_rnd_dis(_rnd)), [] { return _stop.load(); });
    }
}

std::ostream& operator << (std::ostream& os, const goods_t& goods)
{
    return os << '#' << std::this_thread::get_id() << ' ' << "{" << goods.article << ", " << goods.producer << ", " << goods.name << ", " << goods.price << "}";
}

void readerA()
{
    std::mutex mutex;

    while (!_stop.load())
    {
        for (int i = 1; i <= 5; i++)
        {
            goods_t goods = _goods_storage.get_by_article("A-" + std::to_string(i));
            
            std::lock_guard<std::mutex> lock(_cout_locker);
            std::cout << "Article" << std::endl;
            std::cout << goods << std::endl;
        }

        std::unique_lock<std::mutex> lock(mutex);
        _condition.wait_for(lock, std::chrono::milliseconds(_rnd_dis(_rnd)), []{ return _stop.load();});
    }
}

void readerP()
{
    std::mutex mutex;

    while (!_stop.load())
    {
        for (int i = 1; i <= 5; i++)
        {
            auto v_goods = _goods_storage.get_by_producer("Producer " + std::to_string(i));

            std::lock_guard<std::mutex> lock(_cout_locker);
            std::cout << "Producer" << std::endl;
            std::copy(v_goods.cbegin(), v_goods.cend(), std::ostream_iterator<goods_t>{std::cout, "\n"});
        }

        std::unique_lock<std::mutex> lock(mutex);
        _condition.wait_for(lock, std::chrono::milliseconds(_rnd_dis(_rnd)), [] { return _stop.load(); });
    }
}

int main()
{
    std::cout << "Hello World!\n"; 

   /* goods_t g{"A2", "producer", "Goods2", 10};
    _goods_storage.add(g);
    _goods_storage.add(goods_t{"A1", "producer", "Goods1", 10});

    goods_t goods1 = _goods_storage.get_by_article("A1");
    goods_t goods2 = _goods_storage.get_by_article("A2");*/
    //_goods_storage.add("test");

    std::thread tw1(writer, "Producer 1");
    std::thread tw2(writer, "Producer 2");
    std::thread tw3(writer, "Producer 3");
    std::thread tw4(writer, "Producer 4");
    std::thread tw5(writer, "Producer 5");

    std::thread tra1(readerA);
    std::thread tra2(readerA);
    std::thread tra3(readerA);

    std::thread trp1(readerP);
    std::thread trp2(readerP);
    std::thread trp3(readerP);

    std::cout << "Press any key to exit ...!\n";
    _getch();

    _stop.store(true);
    _condition.notify_all();

    tw1.join();
    tw2.join();
    tw3.join();
    tw4.join();
    tw5.join();

    tra1.join();
    tra2.join();
    tra3.join();

    trp1.join();
    trp2.join();
    trp3.join();

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
