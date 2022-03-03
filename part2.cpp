#include <iostream>
#include <algorithm>
#include <thread>
#include <vector>
#include <mutex> 
#include <filesystem>
#include <numeric>
#include <time.h>
#include <string>
#include <condition_variable>
#include <fstream>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <iostream>
#include <cstdlib>
#include <deque>
#include <chrono>

using namespace std;
#define rep(i, a, b) for(int i = a; i < (b); ++i)
#define all(x) begin(x), end(x)
#define sz(x) (int)(x).size()
typedef long long ll;
typedef long double ld;
typedef pair<int, int> pii;
typedef vector<int> vi;

const int n = 10;
const int party_length_seconds = 20;

int main() {
    
    int activeGuest = -1;
    mutex guestLock;
    condition_variable cv;
    int partyOver = 0;

    deque<int> guestQueue;
    mutex queueLock;

    auto guestThread = [&](int tnum) {
        {
            string greeting = "Guest " + to_string(tnum) + ", to the Minotaur: Happy Birthday, friend!\n";
            cout << greeting;
        }
        int cupcakesEaten = 0;
        while(!partyOver){
            
            // Go about our business, but there's a 1/10 chance per second that we decide
            // to get in line for the showroom
            while(!partyOver) {
                if(rand()%15 == 0) break;
                // Different per person so there's not just waves of people all enqueueing at once,
                // these will separate over time
                std::this_thread::sleep_for(std::chrono::milliseconds(500 + 31 * tnum));
            }
            if(partyOver) break;

            queueLock.lock();
            if(guestQueue.empty() && activeGuest == -1) {
                // we can go into the showroom right away!
                activeGuest = tnum;
                queueLock.unlock();
            } else {
                {
                    string msg = "Guest " + to_string(tnum) + " gets in line.\n";
                    cout << msg;
                    cout.flush();
                }
                guestQueue.push_back(tnum);
                queueLock.unlock();
                unique_lock<mutex> lk(guestLock);
                cv.wait(lk, [&]{return activeGuest == tnum || partyOver;});
                if(partyOver) break;
            }

            cout << "Guest " << tnum << " is entering the showroom!" << endl;
            sleep(1); // simulate admiring the vase
            if(partyOver) break;

            {
                lock_guard<mutex> lk(guestLock);
                queueLock.lock();
                stringstream msg;
                msg << "Guest " << tnum << " exits the showroom";

                if(partyOver){
                    msg << "." << endl;
                } else {
                    if(guestQueue.empty()) {
                        activeGuest = -1;
                        msg << " to an empty line. " << endl;
                    } else {
                        activeGuest = guestQueue.front(); guestQueue.pop_front();
                        msg << " and sends Guest " << activeGuest << " in." << endl;
                    }
                }
                cout << msg.str();
                queueLock.unlock();
            }
            cv.notify_all();
        }
        // If we're here, this means the party's over. Time to go home!
        {
            string greeting = "Guest " + to_string(tnum) + ", to the Minotaur: Goodbye! See you next year!\n";
            cout << greeting;
        }
    };

    vector<thread *> threads;
    for(int i = 0; i < n; i++) threads.push_back(new thread(guestThread, i));
    cout.flush();

    // Party lasts some time.
    sleep(party_length_seconds);
    cout << "Minotaur says: 'Party's over, everyone!'" << endl;
    {
        lock_guard<mutex> lk(guestLock);
        partyOver = true;
    }
    cv.notify_all();
    
    // Everyone goes home! Thanks, Minotaur, and Happy Birthday!
    for(int i = 0; i < n; i++) threads[i]->join();
    cout.flush();

    return 0;
}
