To compile and run part1:

g++ -std=c++17 -O3 -pthread ./part1.cpp && ./a.out

To compile and run part2:

g++ -std=c++17 -O3 -pthread ./part2.cpp && ./a.out


Part 1:
This uses the algorithm discussed in class. The guests choose a leader "guest 0", and every other guest is the follower. 
If the leader enters the maze and there is no cupcake, he increments a counter and puts a cupcake back on the plate. 
If a non-leader enters a maze and sees a cupcake AND has yet to eat a cupcake, ever, they eat a cupcake.
If the leader's counter ever hits n-1, this means every guest has entered the maze and they can notify accordingly.

Part 2:
The queue is the best approach in this scenario. In every other case, it is possible for a guest to be waiting for
infinite time without ever having a chance to see the vase, whereas some other guests might be able to view it multiple
times in that same window. This is called "starvation". Approach 3 solves this issue by ensuring that a guest will
always be able to view the vase (assuming that other guests leave the showroom after finite time, which they will).
