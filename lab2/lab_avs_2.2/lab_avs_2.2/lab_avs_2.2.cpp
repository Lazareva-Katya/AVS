#include <iostream>
#include <mutex>
#include <atomic>
#include <queue>
#include <thread>
#include <vector>

using namespace std;

static const int taskNum = 4 * 1024 * 1024;
static const int producerNum = 1; 
static const int consumerNum = 1; 
static const int queueSize = 4;
atomic<int> sum(0);
atomic<int> counter(0);
atomic<int> sum2(0);
atomic<int> counter2(0);
vector<thread> prods;
vector<thread> consus;
vector<thread> prods2;
vector<thread> consus2;
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
    bool full() {
        if (q2.size() >= queueSize)
            return true;
        else
            return false;
    }

public:
    void push(uint8_t val);
    bool pop(uint8_t& val);
} q2;

void SecondQueue::push(uint8_t val) {
    unique_lock<mutex> lk(m2);

    var_push.wait(lk, [this] {return !full(); });
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

        if (c >= taskNum * producerNum) {
            break;
        }
    }

    sum.fetch_add(s);
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
    cout << "sum1: " << sum << endl;
    cout << "sum2: " << sum2 << endl;
    cout << endl;
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

void first_queue()
{
    for (int i = 0; i < producerNum; i++) {
        prods.push_back(thread(prod));
        prods[i].join();
    }

    for (int i = 0; i < consumerNum; i++) {
        consus.push_back(thread(cons));
        consus[i].join();
    }
}

int main() {
    first_queue();
    for (int i = 0; i < producerNum; i++) {
        prods2.push_back(thread(prod2));
    }

    for (int i = 0; i < consumerNum; i++) {
        consus2.push_back(thread(cons2));
    }

    for (int i = 0; i < producerNum; i++) {
        prods2[i].join();
    }

    for (int i = 0; i < consumerNum; i++) {
        consus2[i].join();
    }
    show();
    return 0;
}
