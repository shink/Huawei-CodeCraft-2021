/*
 * @author: shenke
 * @date: 2021/3/12
 * @project: HuaWei_CodeCraft_2021
 * @desp:
 */

#include "Solver.h"

Solver::Solver() {
#ifdef TEST
    // string inputFile = R"(D:\GitHub Repository\Huawei-CodeCraft-2021\preliminary-practice\data\test.txt)";
    // string inputFile = R"(D:\GitHub Repository\Huawei-CodeCraft-2021\preliminary-practice\data\training-1.txt)";
    // string inputFile = R"(D:\GitHub Repository\Huawei-CodeCraft-2021\preliminary-practice\data\training-2.txt)";
    // string inputFile = "/home/shenke/Develop/Repository/Huawei-CodeCraft-2021/preliminary-practice/data/test.txt";
    string inputFile = "/home/shenke/Develop/Repository/Huawei-CodeCraft-2021/preliminary-practice/data/training-1.txt";
    // string inputFile = "/home/shenke/Develop/Repository/Huawei-CodeCraft-2021/preliminary-practice/data/training-2.txt";

    // string outputFile = R"(D:\GitHub Repository\Huawei-CodeCraft-2021\preliminary-practice\output.txt)";
    string outputFile = "/home/shenke/Develop/Repository/Huawei-CodeCraft-2021/preliminary-practice/output.txt";

    std::freopen(inputFile.c_str(), "r", stdin);
    std::freopen(outputFile.c_str(), "w", stdout);
#endif
}

void Solver::Input() {
    InputServer();
    InputVirtualMachine();
    InputRequest();
}

void Solver::Init() {}

void Solver::Solve() {
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

        // TODO: 扩容
        if (purchasedServerNumToday > 0) {
            Expand(1);
            ans.emplace_back("(" + servers[maxHardwareCostServerId].type + ", " + std::to_string(purchasedServerNumToday) + ")\n");
        } else {
            Expand(0);
        }

        // TODO: 迁移
        Migrate(0);

        for (auto &s : res) {
            ans.emplace_back(s);
        }
        res.clear();

#ifdef TEST
        for (auto &purchasedServer : purchasedServers) {
            Server &server = servers[purchasedServer.serverId];
            if (purchasedServer.remainCpuCoreA != purchasedServer.remainCpuCoreB
                || purchasedServer.remainCpuCoreA != server.cpuCore >> 1u
                || purchasedServer.remainMemorySizeA != purchasedServer.remainMemorySizeB
                || purchasedServer.remainMemorySizeA != server.memorySize >> 1u) {

                cost += server.energyCost;
            }
        }
#endif
    }

#ifdef TEST
    std::chrono::duration<double, std::milli> duration = std::chrono::system_clock::now() - start;
    printf("Solve use: %.3fms\n", duration.count());
    printf("Cost: %ld\n", cost);
    printf("---------------\n");
#endif
}

void Solver::Output() {
    for (auto &s : ans) {
        std::cout << s;
    }
}

void Solver::InputServer() {
    scanf("%du", &N);
    string typeStr, cpuCoreStr, memorySizeStr, hardwareCostStr, energyCostStr;
    uint16_t cpuCore;
    uint16_t memorySize;
    uint32_t hardwareCost, maxHardwareCost = 0u;
    uint16_t energyCost;
    servers.resize(N);

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

        servers[i] = *server;
        serverMap[servers[i].type] = i;
        if (hardwareCost > maxHardwareCost) {
            maxHardwareCost = hardwareCost;
            maxHardwareCostServerId = i;
        }
    }

#ifdef TEST
    printf("Server type num: %d\n", N);
#endif
}

void Solver::InputVirtualMachine() {
    scanf("%du", &M);
    string typeStr, cpuCoreStr, memorySizeStr, nodeNumStr;
    uint16_t cpuCore;
    uint16_t memorySize;
    virtualMachines.resize(M);

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
        virtualMachines[i] = *vm;
        virtualMachineMap[virtualMachines[i].type] = i;
    }

#ifdef TEST
    printf("VM type num: %d\n", M);
#endif
}

void Solver::InputRequest() {
    scanf("%du", &T);
    requests.resize(T);

    for (uint16_t i = 0u; i < T; ++i) {
        uint32_t R;
        scanf("%du", &R);
        string operationStr, typeStr, idStr;
        std::vector<Request> request(R);

        for (uint32_t j = 0u; j < R; ++j) {
            Request req;

            std::cin >> operationStr;
            req.add = (operationStr[1] == 'a');
            if (req.add) {
                std::cin >> typeStr;
                req.type = typeStr.substr(0, typeStr.size() - 1);
            }

            std::cin >> idStr;
            int id = 0;
            for (int k = 0; k < idStr.size() - 1; ++k) {
                id = id * 10 + (idStr[k] - 48);
            }
            req.id = id;

            request[j] = req;
        }

        requests[i] = request;
    }

#ifdef TEST
    printf("Request day num: %d\n", T);
#endif
}

uint16_t Solver::AddVirtualMachine(uint16_t day, int id, const string &type) {
    uint16_t vmId = virtualMachineMap[type];
    VirtualMachine &vm = virtualMachines[vmId];
    uint32_t purchasedServerId;
    PurchasedServer *purchasedServer;
    auto *deployedVM = new DeployedVirtualMachine();
    bool deployed = false;

    if (vm.nodeNum) {
        for (uint32_t i = 0u; i < purchasedServers.size(); ++i) {
            purchasedServerId = i;
            purchasedServer = &(purchasedServers[i]);
            if (purchasedServer->remainCpuCoreA >= vm.cpuCore >> 1u
                && purchasedServer->remainCpuCoreB >= vm.cpuCore >> 1u
                && purchasedServer->remainMemorySizeA >= vm.memorySize >> 1u
                && purchasedServer->remainMemorySizeB >= vm.memorySize >> 1u) {

                purchasedServer->remainCpuCoreA -= vm.cpuCore >> 1u;
                purchasedServer->remainCpuCoreB -= vm.cpuCore >> 1u;
                purchasedServer->remainMemorySizeA -= vm.memorySize >> 1u;
                purchasedServer->remainMemorySizeB -= vm.memorySize >> 1u;
                res.emplace_back("(" + std::to_string(i) + ")\n");
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
            res.emplace_back("(" + std::to_string(purchasedServers.size() - 1) + ")\n");
        }
    } else {
        for (uint32_t i = 0u; i < purchasedServers.size(); ++i) {
            purchasedServerId = i;
            purchasedServer = &(purchasedServers[i]);
            if (purchasedServer->remainCpuCoreA >= vm.cpuCore &&
                purchasedServer->remainMemorySizeA >= vm.memorySize) {

                purchasedServer->remainCpuCoreA -= vm.cpuCore;
                purchasedServer->remainMemorySizeA -= vm.memorySize;
                res.emplace_back("(" + std::to_string(i) + ", " + "A)\n");
                deployed = true;
                deployedVM->location = true;
                break;
            }
            if (purchasedServer->remainCpuCoreB >= vm.cpuCore &&
                purchasedServer->remainMemorySizeB >= vm.memorySize) {

                purchasedServer->remainCpuCoreB -= vm.cpuCore;
                purchasedServer->remainMemorySizeB -= vm.memorySize;
                res.emplace_back("(" + std::to_string(i) + ", " + "B)\n");
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
            res.emplace_back("(" + std::to_string(purchasedServers.size() - 1) + ", " + "A)\n");
            deployedVM->location = true;
        }
    }

    deployedVM->purchasedServerId = purchasedServerId;
    deployedVM->vmId = vmId;
    deployedVMs.emplace_back(*deployedVM);
    deployedVMIdMap[id] = deployedVMs.size() - 1;

    return deployed ? 0 : 1;
}

void Solver::DeleteVirtualMachine(int id) {
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

bool Solver::CheckCapacity(uint16_t day) {
#ifdef TEST
    printf("Checking capacity\n");
#endif

    return false;
}

uint16_t Solver::PurchaseServer() {
    // TODO: choose server
    Server &server = servers[maxHardwareCostServerId];

#ifdef TEST
    cost += server.hardwareCost;
#endif

    auto *purchasedServer = new PurchasedServer(maxHardwareCostServerId, server.cpuCore >> 1u, server.cpuCore >> 1u,
                                                server.memorySize >> 1u, server.memorySize >> 1u);
    purchasedServers.emplace_back(*purchasedServer);
    return purchasedServers.size() - 1;
}

void Solver::Expand(uint8_t num) {
    ans.emplace_back("(purchase, " + std::to_string(num) + ")\n");
}

void Solver::Migrate(uint8_t num) {
    ans.emplace_back("(migration, " + std::to_string(num) + ")\n");
}

Solver::~Solver() {
#ifdef TEST
    fclose(stdin);
    fclose(stdout);
#endif
}
