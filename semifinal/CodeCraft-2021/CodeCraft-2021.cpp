/*
 * @author: shenke
 * @date: 2021/3/30
 * @project: HuaWei_CodeCraft_2021
 * @desp:
 */

#include "Handler.h"

int main(int argc, char *argv[]) {

    Handler handler;
    handler.Init();
    handler.Input();
    handler.Handle();
    handler.Release();

    exit(0);
}
