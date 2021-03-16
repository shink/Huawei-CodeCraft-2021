/*
 * @author: shenke
 * @date: 2021/3/12
 * @project: HuaWei_CodeCraft_2021
 * @desp:
 */

#include <bits/stdc++.h>

#include <utility>

using std::string;

#define TEST

struct Server {
    string type{};
    uint16_t cpuCore{};
    uint16_t memorySize{};
    uint32_t hardwareCost{};
    uint16_t energyCost{};

    Server() = default;

    Server(string type, uint16_t cpuCore, uint16_t memorySize, uint32_t hardwareCost, uint16_t energyCost) :
            type(std::move(type)), cpuCore(cpuCore), memorySize(memorySize), hardwareCost(hardwareCost),
            energyCost(energyCost) {}
};

struct VirtualMachine {
    string type{};
    uint16_t cpuCore{};
    uint16_t memorySize{};
    bool nodeNum{}; // 1: 双节点, 0: 单节点

    VirtualMachine() = default;

    VirtualMachine(string type, uint16_t cpuCore, uint16_t memorySize, bool nodeNum) :
            type(std::move(type)), cpuCore(cpuCore), memorySize(memorySize), nodeNum(nodeNum) {}
};

struct Request {
    bool add{}; // 1: add, 0: del
    string type{};
    int id{};

    Request() = default;

    Request(bool add, string type, int id) : add(add), type(std::move(type)), id(id) {}
};

struct PurchasedServer {
    uint8_t serverId;
    uint16_t remainCpuCoreA{};
    uint16_t remainCpuCoreB{};
    uint16_t remainMemorySizeA{};
    uint16_t remainMemorySizeB{};

    PurchasedServer(uint8_t serverId, uint16_t remainCpuCoreA, uint16_t remainCpuCoreB,
                    uint16_t remainMemorySizeA, uint16_t remainMemorySizeB) :
            serverId(serverId), remainCpuCoreA(remainCpuCoreA), remainCpuCoreB(remainCpuCoreB),
            remainMemorySizeA(remainMemorySizeA), remainMemorySizeB(remainMemorySizeB) {}
};

struct DeployedVirtualMachine {
    uint16_t purchasedServerId{};
    uint16_t vmId{};
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
std::vector<std::vector<Request>> requests;
uint8_t minHardwareCostServerId = 0u;

std::vector<PurchasedServer> purchasedServers;
std::vector<DeployedVirtualMachine> deployedVMs;
std::vector<string> res;
std::vector<string> ans;

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
            minHardwareCostServerId = i;
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
        std::vector<Request> request;

        for (uint32_t j = 0; j < R; ++j) {
            Request req;

            std::cin >> operationStr;
            req.add = (operationStr[1] == 'a');
            if (req.add) {
                std::cin >> typeStr;
                req.type = typeStr.substr(0, typeStr.size() - 1);
            }

            std::cin >> idStr;
            id = 0;
            for (int k = 0; k < idStr.size() - 1; ++k) {
                id = id * 10 + (idStr[k] - 48);
            }
            req.id = id;

            request.push_back(req);
        }

        requests.push_back(request);
    }

#ifdef TEST
    printf("Request day num: %d\n", T);
#endif
}

void Expand(uint8_t num) {
    ans.push_back("(purchase, " + std::to_string(num) + ")\n");
}

void Migrate(uint8_t num) {
    ans.push_back("(migration, " + std::to_string(num) + ")\n");
}

uint16_t PurchaseServer() {
    // TODO: choose server
    Server &server = servers[minHardwareCostServerId];
    auto *purchasedServer = new PurchasedServer(minHardwareCostServerId, server.cpuCore >> 1u, server.cpuCore >> 1u,
                                                server.memorySize >> 1u, server.memorySize >> 1u);
    purchasedServers.push_back(*purchasedServer);
    return purchasedServers.size() - 1;
}

bool CheckCapacity(uint16_t day) {
#ifdef TEST
    printf("Checking capacity\n");
#endif

    return false;
}

uint16_t AddVirtualMachine(uint16_t day, int id, const string &type) {
    uint16_t vmId = virtualMachineMap[type];
    VirtualMachine &vm = virtualMachines[vmId];
    uint16_t purchasedServerId;
    PurchasedServer *purchasedServer;
    auto *deployedVM = new DeployedVirtualMachine();
    bool deployed = false;

    if (vm.nodeNum) {
        for (uint32_t i = 0u; i < purchasedServers.size(); ++i) {
            purchasedServerId = i;
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
            purchasedServerId = PurchaseServer();
            purchasedServer = &(purchasedServers[purchasedServerId]);
            purchasedServer->remainCpuCoreA -= vm.cpuCore >> 1u;
            purchasedServer->remainCpuCoreB -= vm.cpuCore >> 1u;
            purchasedServer->remainMemorySizeA -= vm.memorySize >> 1u;
            purchasedServer->remainMemorySizeB -= vm.memorySize >> 1u;
            res.push_back("(" + std::to_string(purchasedServers.size() - 1) + ")\n");
        }
    } else {
        for (uint32_t i = 0u; i < purchasedServers.size(); ++i) {
            purchasedServerId = i;
            purchasedServer = &(purchasedServers[i]);
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
            purchasedServerId = PurchaseServer();
            purchasedServer = &(purchasedServers[purchasedServerId]);
            purchasedServer->remainCpuCoreA -= vm.cpuCore;
            purchasedServer->remainMemorySizeA -= vm.memorySize;
            res.push_back("(" + std::to_string(purchasedServers.size() - 1) + ", " + "A)\n");
            deployedVM->location = true;
        }
    }

    deployedVM->purchasedServerId = purchasedServerId;
    deployedVM->vmId = vmId;
    deployedVMs.push_back(*deployedVM);
    deployedVMIdMap[id] = deployedVMs.size() - 1;

    return deployed ? 0 : 1;
}

void DeleteVirtualMachine(int id) {
    DeployedVirtualMachine &deployedVM = deployedVMs[deployedVMIdMap[id]];
    PurchasedServer &purchasedServer = purchasedServers[deployedVM.purchasedServerId];
    VirtualMachine &vm = virtualMachines[deployedVM.vmId];

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

void Init() {

}

void Solve() {
#ifdef TEST
    auto start = std::chrono::system_clock::now();
#endif

    for (uint16_t i = 0u; i < T; ++i) {
#ifdef TEST
        if (i % 100 == 0) printf("Solving %d day\n", i + 1);
#endif

        std::vector<Request> &request = requests[i];
        uint16_t purchasedServerNumToday = 0;

        // TODO: 综合分析当天所有请求

        for (auto &req : request) {
            if (req.add) {
                purchasedServerNumToday += AddVirtualMachine(i, req.id, req.type);
            } else {
                DeleteVirtualMachine(req.id);
            }
        }

        if (purchasedServerNumToday > 0) {
            Expand(1);
            ans.push_back("(" + servers[minHardwareCostServerId].type + ", " + std::to_string(purchasedServerNumToday) + ")\n");
        } else {
            Expand(0);
        }

        for (auto &s : res) {
            ans.push_back(s);
        }
        res.clear();
    }

#ifdef TEST
    std::chrono::duration<double, std::milli> duration = std::chrono::system_clock::now() - start;
    printf("Solve use: %.3fms\n", duration.count());
#endif
}

void Output() {
    for (auto &s : ans) {
        std::cout << s;
    }
}

int main(int argc, char *argv[]) {
#ifdef TEST
    auto start = std::chrono::system_clock::now();

    string inputFile = R"(D:\GitHub Repository\Huawei-CodeCraft-2021\preliminary-practice\data\test.txt)";
    // string inputFile = R"(D:\GitHub Repository\Huawei-CodeCraft-2021\preliminary-practice\data\training-1.txt)";
    // string inputFile = R"(D:\GitHub Repository\Huawei-CodeCraft-2021\preliminary-practice\data\training-2.txt)";
    // string inputFile = "/home/shenke/Develop/Repository/Huawei-CodeCraft-2021/preliminary-practice/data/test.txt";
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
    Init();
    Solve();

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
