#include <iostream>
#include <mutex>
#include <atomic>
#include <queue>
#include <thread>
#include <vector>

using namespace std;

const int taskNum = 4 * 1024 * 1024;
const int producerNum = 1; 
const int consumerNum = 2; 
const int queueSize = 1;
atomic<int> sum(0);
atomic<int> counter(0);
atomic<int> sum2(0);
atomic<int> counter2(0);
vector<thread> prods_t;
vector<thread> cons_t;
vector<thread> prods_t2;
vector<thread> cons_t2;
uint8_t val;
uint8_t val2;

class DynamicQueue {
private:
    queue<uint8_t> q;
    mutex m;

public:
    void push(uint8_t val);
    bool pop(uint8_t& val);
} q;

void DynamicQueue::push(uint8_t val) {
    lock_guard<mutex> lk(m);
    q.push(val);
}

bool DynamicQueue::pop(uint8_t& val) {
    m.lock();

    if (q.empty()) {
        m.unlock();
        this_thread::sleep_for(chrono::milliseconds(1));
        m.lock();
        if (q.empty())
        {
            m.unlock();
            return false;
        }
    }

    val = q.front();
    q.pop();
    m.unlock();
    return true;

}

class SecondQueue {
private:
    queue<uint8_t> q2;
    mutex m2;
    condition_variable var_push;
    condition_variable var_pop;
    bool full();
public:
    void push(uint8_t val);
    bool pop(uint8_t& val);
} q2;

bool SecondQueue::full() {

    return q2.size() >= queueSize;
}

void SecondQueue::push(uint8_t val) {
    unique_lock<mutex> lk(m2);

    var_push.wait(lk, [&] {return !full(); });
    q2.push(val);

    var_pop.notify_all();
}

bool SecondQueue::pop(uint8_t& val) {
    unique_lock<mutex> lk(m2);
    if (q2.empty()) {
        var_pop.wait_for(lk, chrono::milliseconds(1));
        if (q2.empty())
            return false;
    }
    val = q2.front();
    q2.pop();
    var_push.notify_all();
    return true;
}

void cons() {
    int s = 0;
    val = 0;
    int c = 0;

    while (q.pop(val)) {
        c = counter;
        s += val;
        counter++;

        if (c >= taskNum * producerNum)
            break;
        
    }
    sum.fetch_add(s);
}

void cons2() {
    int s = 0;
    val2 = 0;
    int c = 0;

    while (q2.pop(val2)) {
        c = counter2;
        s += val2;
        counter2++;

        if (c >= taskNum * producerNum) {
            break;
        }
    }
    sum2.fetch_add(s);
}


void prod() {
    for (int i = 0; i < taskNum; i++) {
        q.push(1);
    }
}

void prod2() {
    for (int i = 0; i < taskNum; i++) {
        q2.push(1);
    }
}

void show()
{
    cout << "sum1: " << sum << "\n";
    cout << "sum2: " << sum2 << "\n";
    cout << "\n";
}

int main() {
    chrono::high_resolution_clock::time_point start1;
    chrono::high_resolution_clock::time_point end1;
    chrono::high_resolution_clock::time_point start2;
    chrono::high_resolution_clock::time_point end2;
    chrono::duration<float, milli> t1;
    chrono::duration<float, milli> t2;
    start1 = chrono::high_resolution_clock::now();
    for (int i = 0; i < producerNum; i++) {
        prods_t.push_back(thread(prod));
    }

    for (int i = 0; i < consumerNum; i++) {
        cons_t.push_back(thread(cons));
    }

    for (int i = 0; i < producerNum; i++) {
        prods_t[i].join();
    }

    for (int i = 0; i < consumerNum; i++) {
        cons_t[i].join();
    }
    end1 = chrono::high_resolution_clock::now();
    t1 = end1 - start1;
    start2 = chrono::high_resolution_clock::now();
    for (int i = 0; i < producerNum; i++) {
        prods_t2.push_back(thread(prod2));
    }

    for (int i = 0; i < consumerNum; i++) {
        cons_t2.push_back(thread(cons2));
    }

    for (int i = 0; i < producerNum; i++) {
        prods_t2[i].join();
    }

    for (int i = 0; i < consumerNum; i++) {
        cons_t2[i].join();
    }
    end2 = chrono::high_resolution_clock::now();
    t2 = end2 - start2;
    show();
    cout << "time1: " << t1.count() << " ms" << "\n";
    cout << "time2: " << t2.count() << " ms" << "\n";
    return 0;
}
