/*
 * @author: shenke
 * @date: 2021/3/12
 * @project: HuaWei_CodeCraft_2021
 * @desp:
 */

#include <bits/stdc++.h>

using std::string;

#define TEST

struct Server {
    string type{};
    uint16_t cpuCore{};
    uint16_t memorySize{};
    uint32_t hardwareCost{};
    uint16_t energyCost{};

    Server() {}

    // Server(string type, uint16_t cpuCore, uint16_t memorySize, uint32_t hardwareCost, uint16_t energyCost) :
    //         type(type), cpuCore(cpuCore), memorySize(memorySize), hardwareCost(hardwareCost), energyCost(energyCost) {}
};

struct VirtualMachine {
    string type{};
    uint16_t cpuCore{};
    uint16_t memorySize{};
    bool nodeNum{}; // 1: 双节点, 0: 单节点

    VirtualMachine() {}

    // VirtualMachine(string type, uint16_t cpuCore, uint16_t memorySize, bool nodeNum) :
    //         type(type), cpuCore(cpuCore), memorySize(memorySize), nodeNum(nodeNum) {}
};

struct Request {
    bool add{}; // 1: add, 0: del
    string type{};
    int id{};

    Request() {}

    // Request(bool add, string type, int id) : add(add), type(type), id(id) {}
};

struct PurchasedServer {
    Server *server{};
    uint16_t remainCpuCoreA{};
    uint16_t remainCpuCoreB{};
    uint16_t remainMemorySizeA{};
    uint16_t remainMemorySizeB{};

    PurchasedServer(Server *server, uint16_t remainCpuCoreA, uint16_t remainCpuCoreB,
                    uint16_t remainMemorySizeA, uint16_t remainMemorySizeB) :
            server(server), remainCpuCoreA(remainCpuCoreA), remainCpuCoreB(remainCpuCoreB),
            remainMemorySizeA(remainMemorySizeA), remainMemorySizeB(remainMemorySizeB) {}
};

struct DeployedVirtualMachine {
    PurchasedServer *purchasedServer{};
    VirtualMachine *vm{};
    bool location{}; // 1: A, 0: B
};

const uint8_t MAX_N = 100;
const uint16_t MAX_M = 1000;
const uint16_t MAX_T = 1000;
const uint8_t MAX_SERVER_TYPE_LENGTH = 20;
const uint32_t MAX_REQUEST_NUM = 100000;

uint8_t N;
uint16_t M;
uint16_t T;

std::unordered_map<string, uint8_t> serverMap;
std::unordered_map<string, uint16_t> virtualMachineMap;
std::unordered_map<int, uint32_t> deployedVMIdMap;  // id -> deployedVMs index

std::vector<Server> servers;
std::vector<VirtualMachine> virtualMachines;
Server *minHardwareCostServer;

std::vector<PurchasedServer> purchasedServers;
std::vector<DeployedVirtualMachine> deployedVMs;
std::vector<string> res;
std::vector<string> ans;

PurchasedServer *PurchaseServer() {
    Server *server;

    // TODO: choose server
    server = minHardwareCostServer;
    auto *purchasedServer = new PurchasedServer(server, server->cpuCore >> 1u, server->cpuCore >> 1u,
                                                server->memorySize >> 1u, server->memorySize >> 1u);
    purchasedServers.push_back(*purchasedServer);
    return purchasedServer;
}

void Expand(uint8_t num) {
    ans.emplace_back("(purchase, " + std::to_string(num) + ")\n");
}

void Migrate(uint8_t num) {
    ans.emplace_back("(migration, " + std::to_string(num) + ")\n");
}

uint8_t AddVirtualMachine(uint16_t day, int id, const string &type) {
    VirtualMachine &vm = virtualMachines[virtualMachineMap[type]];
    PurchasedServer *purchasedServer;
    auto *deployedVM = new DeployedVirtualMachine();
    bool deployed = false;

    if (vm.nodeNum) {
        for (int i = 0; i < purchasedServers.size(); ++i) {
            purchasedServer = &(purchasedServers[i]);
            if (purchasedServer->remainCpuCoreA >= vm.cpuCore >> 1u
                && purchasedServer->remainMemorySizeA >= vm.memorySize >> 1u
                && purchasedServer->remainCpuCoreB >= vm.cpuCore >> 1u
                && purchasedServer->remainMemorySizeB >= vm.memorySize >> 1u) {

                purchasedServer->remainCpuCoreA -= vm.cpuCore >> 1u;
                purchasedServer->remainCpuCoreB -= vm.cpuCore >> 1u;
                purchasedServer->remainMemorySizeA -= vm.memorySize >> 1u;
                purchasedServer->remainMemorySizeB -= vm.memorySize >> 1u;
                res.push_back("(" + std::to_string(i) + ")\n");
                deployed = true;
                break;
            }
        }

        if (!deployed) {
            purchasedServer = PurchaseServer();
            purchasedServer->remainCpuCoreA -= vm.cpuCore >> 1u;
            purchasedServer->remainCpuCoreB -= vm.cpuCore >> 1u;
            purchasedServer->remainMemorySizeA -= vm.memorySize >> 1u;
            purchasedServer->remainMemorySizeB -= vm.memorySize >> 1u;
            res.push_back("(" + std::to_string(purchasedServers.size() - 1) + ")\n");
        }
    } else {
        for (int i = 0; i < purchasedServers.size(); ++i) {
            purchasedServer = &purchasedServers[i];
            if (purchasedServer->remainCpuCoreA >= vm.cpuCore &&
                purchasedServer->remainMemorySizeA >= vm.memorySize) {

                purchasedServer->remainCpuCoreA -= vm.cpuCore;
                purchasedServer->remainMemorySizeA -= vm.memorySize;
                res.push_back("(" + std::to_string(i) + ", " + "A)\n");
                deployed = true;
                deployedVM->location = true;
                break;
            }
            if (purchasedServer->remainCpuCoreB >= vm.cpuCore &&
                purchasedServer->remainMemorySizeB >= vm.memorySize) {

                purchasedServer->remainCpuCoreB -= vm.cpuCore;
                purchasedServer->remainMemorySizeB -= vm.memorySize;
                res.push_back("(" + std::to_string(i) + ", " + "B)\n");
                deployed = true;
                deployedVM->location = false;
                break;
            }
        }

        if (!deployed) {
            purchasedServer = PurchaseServer();
            purchasedServer->remainCpuCoreA -= vm.cpuCore;
            purchasedServer->remainMemorySizeA -= vm.memorySize;
            res.push_back("(" + std::to_string(purchasedServers.size() - 1) + ", " + "A)\n");
            deployedVM->location = true;
        }
    }

    deployedVM->purchasedServer = purchasedServer;
    deployedVM->vm = &vm;
    deployedVMs.push_back(*deployedVM);
    deployedVMIdMap[id] = deployedVMs.size() - 1;

    std::cout << "--------\n";
    std::cout << id << std::endl;
    std::cout << deployedVM->purchasedServer->remainCpuCoreA << std::endl;
    std::cout << deployedVM->purchasedServer->remainCpuCoreB << std::endl;
    std::cout << deployedVM->purchasedServer->remainMemorySizeA << std::endl;
    std::cout << deployedVM->purchasedServer->remainMemorySizeB << std::endl;

    return deployed ? 0 : 1;
}

void DeleteVirtualMachine(int id) {
    DeployedVirtualMachine &deployedVM = deployedVMs[deployedVMIdMap[id]];
    PurchasedServer &purchasedServer = *(deployedVM.purchasedServer);
    VirtualMachine &vm = *(deployedVM.vm);

    if (vm.nodeNum) {
        purchasedServer.remainCpuCoreA += vm.cpuCore >> 1u;
        purchasedServer.remainCpuCoreB += vm.cpuCore >> 1u;
        purchasedServer.remainMemorySizeA += vm.memorySize >> 1u;
        purchasedServer.remainMemorySizeB += vm.memorySize >> 1u;
    } else {
        if (deployedVM.location) {
            purchasedServer.remainCpuCoreA += vm.cpuCore;
            purchasedServer.remainMemorySizeA += vm.memorySize;
        } else {
            purchasedServer.remainCpuCoreB += vm.cpuCore;
            purchasedServer.remainMemorySizeB += vm.memorySize;
        }
    }

    deployedVMIdMap.erase(id);
}

void InputServer() {
    scanf("%du", &N);
    string typeStr, cpuCoreStr, memorySizeStr, hardwareCostStr, energyCostStr;
    uint16_t cpuCore;
    uint16_t memorySize;
    uint32_t hardwareCost, minHardwareCost = 500000;
    uint16_t energyCost;

    for (uint8_t i = 0u; i < N; ++i) {
        std::cin >> typeStr >> cpuCoreStr >> memorySizeStr >> hardwareCostStr >> energyCostStr;
        auto *server = new Server();

        server->type = typeStr.substr(1, typeStr.size() - 2);

        cpuCore = 0u;
        for (int j = 0; j < cpuCoreStr.size() - 1; ++j) {
            cpuCore = cpuCore * 10 + (cpuCoreStr[j] - 48);
        }
        server->cpuCore = cpuCore;

        memorySize = 0u;
        for (int j = 0; j < memorySizeStr.size() - 1; ++j) {
            memorySize = memorySize * 10 + (memorySizeStr[j] - 48);
        }
        server->memorySize = memorySize;

        hardwareCost = 0u;
        for (int j = 0; j < hardwareCostStr.size() - 1; ++j) {
            hardwareCost = hardwareCost * 10 + (hardwareCostStr[j] - 48);
        }
        server->hardwareCost = hardwareCost;

        energyCost = 0u;
        for (int j = 0; j < energyCostStr.size() - 1; ++j) {
            energyCost = energyCost * 10 + (energyCostStr[j] - 48);
        }
        server->energyCost = energyCost;

        servers.push_back(*server);
        serverMap[servers[i].type] = i;
        if (hardwareCost < minHardwareCost) {
            minHardwareCost = hardwareCost;
            minHardwareCostServer = &servers[i];
        }
    }

#ifdef TEST
    printf("Server type num: %d\n", N);
#endif
}

void InputVirtualMachine() {
    scanf("%du", &M);
    string typeStr, cpuCoreStr, memorySizeStr, nodeNumStr;
    uint16_t cpuCore;
    uint16_t memorySize;

    for (uint16_t i = 0u; i < M; ++i) {
        std::cin >> typeStr >> cpuCoreStr >> memorySizeStr >> nodeNumStr;
        auto *vm = new VirtualMachine();

        vm->type = typeStr.substr(1, typeStr.size() - 2);

        cpuCore = 0u;
        for (int j = 0; j < cpuCoreStr.size() - 1; ++j) {
            cpuCore = cpuCore * 10 + (cpuCoreStr[j] - 48);
        }
        vm->cpuCore = cpuCore;

        memorySize = 0u;
        for (int j = 0; j < memorySizeStr.size() - 1; ++j) {
            memorySize = memorySize * 10 + (memorySizeStr[j] - 48);
        }
        vm->memorySize = memorySize;

        vm->nodeNum = (nodeNumStr[0] == '1');
        virtualMachines.push_back(*vm);
        virtualMachineMap[virtualMachines[i].type] = i;
    }

#ifdef TEST
    printf("VM type num: %d\n", M);
#endif
}

void InputRequest() {
    scanf("%du", &T);

    for (uint16_t i = 0; i < T; ++i) {
        uint16_t R;
        scanf("%du", &R);
        string operationStr, typeStr, idStr;
        bool add;
        string type;
        int id;
        uint8_t purchasedServerNum = 0;

        for (uint32_t j = 0; j < R; ++j) {
            std::cin >> operationStr;
            add = (operationStr[1] == 'a');
            if (add) {
                std::cin >> typeStr;
                type = typeStr.substr(0, typeStr.size() - 1);
            }

            std::cin >> idStr;
            id = 0;
            for (int k = 0; k < idStr.size() - 1; ++k) {
                id = id * 10 + (idStr[k] - 48);
            }

            // TODO: boom shit
            if (add) {
                purchasedServerNum += AddVirtualMachine(i, id, type);
            } else {
                DeleteVirtualMachine(id);
            }
        }

        // TODO: purchase
        if (purchasedServerNum > 0) {
            Expand(1);
            ans.push_back("(" + minHardwareCostServer->type + ", " + std::to_string(purchasedServerNum) + ")\n");
        } else {
            Expand(0);
        }

        // TODO: Migrate
        Migrate(0);

        for (auto &s : res) {
            ans.push_back(s);
        }

        res.clear();
        purchasedServerNum = 0;
    }

#ifdef TEST
    printf("Request day num: %d\n", T);
#endif
}

void Output() {
    for (const string &s : ans) {
        std::cout << s;
    }
}

int main(int argc, char *argv[]) {
#ifdef TEST
    auto start = std::chrono::system_clock::now();

    // string inputFile = R"(D:\GitHub Repository\Huawei-CodeCraft-2021\preliminary-practice\data\test.txt)";
    // string inputFile = R"(D:\GitHub Repository\Huawei-CodeCraft-2021\preliminary-practice\data\training-1.txt)";
    // string inputFile = R"(D:\GitHub Repository\Huawei-CodeCraft-2021\preliminary-practice\data\training-2.txt)";
    string inputFile = "/home/shenke/Develop/Repository/Huawei-CodeCraft-2021/preliminary-practice/data/test.txt";
    // string inputFile = "/home/shenke/Develop/Repository/Huawei-CodeCraft-2021/preliminary-practice/data/training-1.txt";
    // string inputFile = "/home/shenke/Develop/Repository/Huawei-CodeCraft-2021/preliminary-practice/data/training-2.txt";

    std::freopen(inputFile.c_str(), "r", stdin);

    // if (argc > 1) {
    //     printf("input filePath: %s", argv[1]);
    //     std::freopen(argv[1], "r", stdin);
    //
    //     if (argc > 2) {
    //         printf("output filePath: %s", argv[2]);
    //         std::freopen(argv[2], "r", stdout);
    //     }
    // }
#endif

    // TODO: read standard input
    InputServer();
    InputVirtualMachine();
    InputRequest();

    // TODO: process
    // Init();

    // TODO: write standard output
    Output();

    // TODO: fflush(stdout);


#ifdef TEST
    fclose(stdin);
    // fclose(stdout);

    std::chrono::duration<double, std::milli> duration = std::chrono::system_clock::now() - start;
    printf("Total use: %.3fms\n", duration.count());
#endif

    exit(0);
}
