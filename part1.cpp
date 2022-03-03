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

using namespace std;
#define rep(i, a, b) for(int i = a; i < (b); ++i)
#define all(x) begin(x), end(x)
#define sz(x) (int)(x).size()
typedef long long ll;
typedef long double ld;
typedef pair<int, int> pii;
typedef vector<int> vi;

const int n = 10; // I wouldn't recommend cranking this over 100. The code works just fine regardless, but it'll just take a long time because
// the probability of sending people who have already been sent / not sending the leader is high, so it just takes a while to complete. 

int main() {
    
    int activeGuest = -1;
    mutex guestLock;
    condition_variable cv;
    int partyOver = 0, cupcakeOnPlate = 1;

    auto guestThread = [&](int tnum) {
        {
            string greeting = "Guest " + to_string(tnum) + ", to the Minotaur: Happy Birthday, friend!\n";
            cout << greeting;
        }
        int cupcakesEaten = 0;
        while(!partyOver){
            {
                unique_lock<mutex> lk(guestLock);
                cv.wait(lk, [&]{return activeGuest == tnum || partyOver;});
            }
            if(partyOver) break;
            cout << "Guest " << tnum << " is entering the labyrinth!" << endl;
            // sleep(1); // simulate traveling the labyrinth

            if(tnum == 0){
                // we're the leader!
                if(!cupcakeOnPlate) {
                    cout << "Guest " << tnum << " politely asks for a cupcake, but does not eat it." << endl;
                    cupcakeOnPlate = 1;
                    cupcakesEaten++;
                }
                if(cupcakesEaten == n-1){
                    cout << "Guest " << tnum << " says, to the Minotaur: We've all reached the labyrinth!\nThe Minotaur responds: Party's over!" << endl;
                    partyOver = 1;
                }
            }else{
                if(cupcakeOnPlate && !cupcakesEaten){
                    cout << "Guest " << tnum << " eats a cupcake, and thanks the chef!" << endl;
                    cupcakesEaten++;
                    cupcakeOnPlate = 0;
                }
            }

            {
                lock_guard<mutex> lk(guestLock);
                activeGuest = -1;
                cout << "Guest " << tnum << " exits the labyrinth." << endl;
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
    sleep(1);

    // While the party is over, the minotaur picks a random guest and sends them through the labyrinth
    while (!partyOver) {
        {
            lock_guard<mutex> lk(guestLock);
            activeGuest = rand()%n;
            cout << "The Minotaur picks guest " << activeGuest << " to enter the labyrinth..." << endl;
        }
        cv.notify_all();
        {
            unique_lock<mutex> lk(guestLock);
            cv.wait(lk, [&]{return activeGuest == -1;});
        }
    }
    
    // Everyone goes home! Thanks, Minotaur, and Happy Birthday!
    for(int i = 0; i < n; i++) threads[i]->join();
    cout.flush();

    return 0;
}
