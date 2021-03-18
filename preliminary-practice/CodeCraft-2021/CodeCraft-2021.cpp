/*
 * @author: shenke
 * @date: 2021/3/12
 * @project: HuaWei_CodeCraft_2021
 * @desp:
 */

#include "Solver.h"


int main(int argc, char *argv[]) {
#ifdef TEST
    auto start = std::chrono::system_clock::now();
#endif

    Solver solver;
    // TODO: read standard input
    solver.Input();

    // TODO: process
    solver.Init();
    solver.Solve();

    // TODO: write standard output
    solver.Output();

#ifdef TEST
    std::chrono::duration<double, std::milli> duration = std::chrono::system_clock::now() - start;
    printf("Total use: %.3fms\n", duration.count());
#endif

    // TODO: fflush(stdout);
    solver.~Solver();

    exit(0);
}
