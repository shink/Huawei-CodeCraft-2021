/*
 * @author: shenke
 * @date: 2021/3/12
 * @project: HuaWei_CodeCraft_2021
 * @desp:
 */

#include "Handler.h"

Handler::Handler() {}

void Handler::Input() {
#ifdef TEST
    // string inputFile = "D:\\GitHub Repository\\Huawei-CodeCraft-2021\\preliminary-practice\\data\\test.txt";
    // string inputFile = "D:\\GitHub Repository\\Huawei-CodeCraft-2021\\preliminary-practice\\data\\training-1.txt";
    // string inputFile = "D:\\GitHub Repository\\Huawei-CodeCraft-2021\\preliminary-practice\\data\\training-2.txt";
    // string inputFile = "/home/shenke/Develop/Repository/Huawei-CodeCraft-2021/preliminary-practice/data/test.txt";
    string inputFile = "/home/shenke/Develop/Repository/Huawei-CodeCraft-2021/preliminary-practice/data/training-1.txt";
    // string inputFile = "/home/shenke/Develop/Repository/Huawei-CodeCraft-2021/preliminary-practice/data/training-2.txt";

    std::freopen(inputFile.c_str(), "r", stdin);
#endif

    InputServer();
    InputVirtualMachine();
    InputRequest();

#ifdef TEST
    fclose(stdin);
#endif
}

void Handler::Solve() {
    Init();

    for (uint16_t i = 0u; i < T; ++i) {
        // TODO: 迁移
        Migrate(i);

        // TODO: 扩容
        Expand(i);

        // TODO: 处理请求
        HandleRequests(i);

        // TODO: 聚拢结果
        GatherAnswer();

#ifdef TEST
        for (auto &purchasedServer : purchasedServers) {
            Server &server = servers[purchasedServer.serverIdx];
            if (purchasedServer.remainCpuCoreA != purchasedServer.remainCpuCoreB
                || purchasedServer.remainCpuCoreA != server.cpuCore >> 1u
                || purchasedServer.remainMemorySizeA != purchasedServer.remainMemorySizeB
                || purchasedServer.remainMemorySizeA != server.memorySize >> 1u) {

                totalEnergyCost += server.energyCost;
            }
        }
#endif
    }

    PrintLog("Hardware cost: %lu\n", totalHardwareCost);
    PrintLog("Energy cost: %lu\n", totalEnergyCost);
    PrintLog("Total cost: %lu\n", totalHardwareCost + totalEnergyCost);
    PrintLog("Total migration num: %u\n", totalMigrationNum);
}

void Handler::Output() {
#ifdef TEST
    // string outputFile = "D:\\GitHub Repository\\Huawei-CodeCraft-2021\\preliminary-practice\\output.txt";
    string outputFile = "/home/shenke/Develop/Repository/Huawei-CodeCraft-2021/preliminary-practice/output.txt";
    std::freopen(outputFile.c_str(), "w", stdout);
#endif

    for (string &s : ans) {
        std::cout << s;
    }

#ifdef TEST
    fclose(stdout);
#endif
}

void Handler::InputServer() {
    scanf("%u", &N);
    string modelStr, cpuCoreStr, memorySizeStr, hardwareCostStr, energyCostStr;
    uint16_t cpuCore;
    uint16_t memorySize;
    uint32_t hardwareCost;
    uint16_t energyCost;
    servers.resize(N);
    uint32_t cpuCoreSum = 0u, memorySizeSum = 0u;

    for (uint8_t i = 0u; i < N; ++i) {
        std::cin >> modelStr >> cpuCoreStr >> memorySizeStr >> hardwareCostStr >> energyCostStr;
        auto *server = new Server();

        server->model = modelStr.substr(1, modelStr.size() - 2);

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

        server->scale = CalculateScale(memorySize, cpuCore);
        servers[i] = *server;
        serverMap[servers[i].model] = i;

        cpuCoreSum += cpuCore;
        memorySizeSum += memorySize;
    }

    serverMemoryCpuScale = CalculateScale(memorySizeSum, cpuCoreSum);

    PrintLog("Server model num: %u\n", N);
    PrintLog("Cpu core sum: %u, average: %.2lf\n", cpuCoreSum, (double_t) cpuCoreSum / (double_t) N);
    PrintLog("Memory size sum: %u, average: %.2lf\n", memorySizeSum, (double_t) memorySizeSum / (double_t) N);
    PrintLog("Memory size and cpu core scale: %.2lf\n", serverMemoryCpuScale);
}

void Handler::InputVirtualMachine() {
    scanf("%u", &M);
    string modelStr, cpuCoreStr, memorySizeStr, nodeNumStr;
    uint16_t cpuCore;
    uint16_t memorySize;
    virtualMachines.resize(M);

    for (uint16_t i = 0u; i < M; ++i) {
        std::cin >> modelStr >> cpuCoreStr >> memorySizeStr >> nodeNumStr;
        auto *vm = new VirtualMachine();

        vm->model = modelStr.substr(1, modelStr.size() - 2);

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

        vm->nodeType = (nodeNumStr[0] == '1');
        virtualMachines[i] = *vm;
        virtualMachineMap[virtualMachines[i].model] = i;
    }

    PrintLog("VM model num: %u\n", M);
}

void Handler::InputRequest() {
    scanf("%u", &T);
    requests.resize(T);
    VirtualMachine *vm;
    int cpuCoreDemand, memorySizeDemand;
    int totalCpuCoreDemand = 0, totalMemorySizeDemand = 0;

    for (uint16_t i = 0u; i < T; ++i) {
        uint32_t R;
        scanf("%u", &R);
        string operationStr, modelStr, idStr;
        std::vector<Request> request(R);
        int cpuCoreDemand = 0, memorySizeDemand = 0;

        for (uint32_t j = 0u; j < R; ++j) {
            Request req;

            std::cin >> operationStr;
            req.add = (operationStr[1] == 'a');
            if (req.add) {
                std::cin >> modelStr;
                req.model = modelStr.substr(0, modelStr.size() - 1);
            }

            std::cin >> idStr;
            int id = 0;
            for (int k = 0; k < idStr.size() - 1; ++k) {
                id = id * 10 + (idStr[k] - 48);
            }
            req.id = id;

            if (req.add) {
                uint16_t vmIdx = virtualMachineMap[req.model];
                idVirtualMachineMap[id] = vmIdx;
                vm = &virtualMachines[vmIdx];
                cpuCoreDemand += vm->cpuCore;
                memorySizeDemand += vm->memorySize;
                totalCpuCoreDemand += vm->cpuCore;
                totalMemorySizeDemand += vm->memorySize;
            } else {
                vm = &virtualMachines[idVirtualMachineMap[id]];
                cpuCoreDemand -= vm->cpuCore;
                memorySizeDemand -= vm->memorySize;
                totalCpuCoreDemand -= vm->cpuCore;
                totalMemorySizeDemand -= vm->memorySize;
            }

            request[j] = req;
        }

        requests[i] = request;
    }

    requestMemoryCpuScale = CalculateScale(totalMemorySizeDemand, totalCpuCoreDemand);

    PrintLog("Request day num: %u\n", T);
    PrintLog("Request memory cpu scale: %.4lf\n", requestMemoryCpuScale);
}

void Handler::Init() {
    // 将 servers 按照 mem / cpu 的比值进行升序排列
    SortServer(0u, N - 1, [](const Server &server1, const Server &server2) {
        return server1.scale > server2.scale;
    });

    serverIdx = 0u;
    while (servers[serverIdx].scale < requestMemoryCpuScale) ++serverIdx;
    ++serverIdx;
    PrintLog("Purchased server info: %s\n", servers[serverIdx].ToString().c_str());

}

void Handler::Expand(uint16_t day) {

}

void Handler::Migrate(uint16_t day) {
    uint32_t threshold = deployedVMNum / 200u;
    uint32_t migrateNum = 0u;
    int n = purchasedServers.size();
    bool ended = (migrateNum >= threshold);
    PurchasedServer *purchasedServer1, *purchasedServer2;
    DeployedVirtualMachine *deployedVM;
    VirtualMachine *vm;

    // 根据 cpu + mem 的大小降序排列
    // SortDeployedVM();

    // 迁移双节点虚拟机
    // for (int i = 0; i < n - 1 && !ended; ++i) {
    //     purchasedServer1 = &purchasedServers[i];
    //     if (!purchasedServer1->type) continue;
    //     if (CheckCapacity(*purchasedServer1) != ALL) continue;
    //
    //     for (int j = i + 1; j < n && !ended; ++j) {
    //         purchasedServer2 = &purchasedServers[j];
    //         if (!purchasedServer2->type) continue;
    //
    //         uint16_t idx = 0u;
    //         uint32_t deployedVMIdx;
    //         while (idx < purchasedServer2->deployedVMA.size() && !ended) {
    //             deployedVMIdx = purchasedServer2->deployedVMA[idx];
    //             deployedVM = &deployedVMs[deployedVMIdx];
    //             vm = &virtualMachines[deployedVM->vmIdx];
    //             if (CheckCapacity(*purchasedServer1, *vm) != ALL) {
    //                 ++idx;
    //                 continue;
    //             }
    //
    //             // 迁移
    //             migrationResult.emplace_back("(" + std::to_string(deployedVM->vmId) + ", " + std::to_string(i) + ")\n");
    //
    //             uint16_t halfCpuCore = vm->cpuCore >> 1u;
    //             uint16_t halfMemorySize = vm->memorySize >> 1u;
    //             purchasedServer1->remainCpuCoreA -= halfCpuCore;
    //             purchasedServer1->remainCpuCoreB -= halfCpuCore;
    //             purchasedServer1->remainMemorySizeA -= halfMemorySize;
    //             purchasedServer1->remainMemorySizeB -= halfMemorySize;
    //             purchasedServer1->deployedVMA.push_back(deployedVMIdx);
    //
    //             purchasedServer2->remainCpuCoreA += halfCpuCore;
    //             purchasedServer2->remainCpuCoreB += halfCpuCore;
    //             purchasedServer2->remainMemorySizeA += halfMemorySize;
    //             purchasedServer2->remainMemorySizeB += halfMemorySize;
    //             purchasedServer2->deployedVMA.erase(purchasedServer2->deployedVMA.begin() + idx);
    //
    //             if (++migrateNum >= threshold) ended = true;
    //         }
    //     }
    // }

    // TODO: 迁移单节点虚拟机

    migrationResult.emplace_back("(migration, " + std::to_string(migrateNum) + ")\n");

#ifdef TEST
    totalMigrationNum += migrateNum;
#endif
}

void Handler::HandleRequests(uint16_t day) {
    std::vector<Request> request = requests[day];
    uint16_t purchasedServerNum = 0u;
    for (const Request &req : request) {
        if (req.add) {
            purchasedServerNum += AddVirtualMachine(day, req.id, req.model);
        } else {
            DeleteVirtualMachine(req.id);
        }
    }

    if (purchasedServerNum == 0u) {
        extendResult.emplace_back("(purchase, 0)\n");
    } else {
        extendResult.emplace_back("(purchase, 1)\n");
        extendResult.emplace_back("(" + servers[serverIdx].model + ", " + std::to_string(purchasedServerNum) + ")\n");
    }
}

void Handler::GatherAnswer() {
    for (string &s : extendResult) {
        ans.push_back(s);
    }
    extendResult.clear();

    uint32_t n = migrationResult.size();
    ans.emplace_back(migrationResult[n - 1]);
    for (uint32_t i = 0u; i < n - 1; ++i) {
        ans.push_back(migrationResult[i]);
    }
    migrationResult.clear();

    for (string &s : requestResult) {
        ans.push_back(s);
    }
    requestResult.clear();
}

void Handler::SortServer(uint8_t left, uint8_t right, bool (*compare)(const Server &, const Server &)) {
    uint8_t n = right - left;
    for (uint8_t i = 0u; i < n; ++i) {
        for (uint8_t j = left; j < right - i; ++j) {
            if (compare(servers[j], servers[j + 1])) {
                // serverMap[servers[j].model] = j + 1;
                // serverMap[servers[j + 1].model] = j;
                std::swap(servers[j], servers[j + 1]);
            }
        }
    }
}

void Handler::Purchase(uint16_t startDay, uint16_t endDay) {
    // TODO
    Server &server = servers[serverIdx];
    uint16_t halfCpuCore = server.cpuCore >> 1u;
    uint16_t halfMemorySize = server.memorySize >> 1u;
    auto *purchasedServer = new PurchasedServer();
    purchasedServer->serverIdx = serverIdx;
    purchasedServer->remainCpuCoreA = halfCpuCore;
    purchasedServer->remainCpuCoreB = halfCpuCore;
    purchasedServer->remainMemorySizeA = halfMemorySize;
    purchasedServer->remainMemorySizeB = halfMemorySize;
    purchasedServer->remainScaleA = server.scale;
    purchasedServer->remainScaleB = server.scale;
    purchasedServers.emplace_back(*purchasedServer);

#ifdef TEST
    totalHardwareCost += server.hardwareCost;
#endif
}

void Handler::Migrate(const VirtualMachine &vm, DeployedVirtualMachine &deployedVM, uint16_t destPurchasedServerIdx, bool destLocation) {
    uint16_t cpuCore = vm.cpuCore;
    uint16_t memorySize = vm.memorySize;

    if (vm.nodeType) {

    } else {

    }

    deployedVM.purchasedServerIdx = destPurchasedServerIdx;
    deployedVM.location = destLocation;

}

void Handler::SortDeployedVM() {
    VirtualMachine *vm1, *vm2;
    for (PurchasedServer &purchasedServer : purchasedServers) {
        std::vector<uint32_t> &deployedVMA = purchasedServer.deployedVMA;
        int n = deployedVMA.size();
        for (int i = 0; i < n - 1; ++i) {
            for (int j = 0; j < n - i - 1; ++j) {
                vm1 = &virtualMachines[deployedVMs[deployedVMA[j]].vmIdx];
                vm2 = &virtualMachines[deployedVMs[deployedVMA[j + 1]].vmIdx];
                if (vm1->cpuCore + vm1->memorySize < vm2->cpuCore + vm2->memorySize) {
                    std::swap(deployedVMA[j], deployedVMA[j + 1]);
                }
            }
        }

        std::vector<uint32_t> &deployedVMB = purchasedServer.deployedVMB;
        n = deployedVMB.size();
        for (int i = 0; i < n - 1; ++i) {
            for (int j = 0; j < n - i - 1; ++j) {
                vm1 = &virtualMachines[deployedVMs[deployedVMB[j]].vmIdx];
                vm2 = &virtualMachines[deployedVMs[deployedVMB[j + 1]].vmIdx];
                if (vm1->cpuCore + vm1->memorySize < vm2->cpuCore + vm2->memorySize) {
                    std::swap(deployedVMB[j], deployedVMB[j + 1]);
                }
            }
        }
    }
}

uint16_t Handler::AddVirtualMachine(uint16_t day, int id, const string &model) {
    uint16_t n = purchasedServers.size();
    uint16_t vmIdx = idVirtualMachineMap[id];
    VirtualMachine &vm = virtualMachines[vmIdx];
    PurchasedServer *purchasedServer;

    if (vm.nodeType) {
        // 双节点
        for (uint16_t i = 0u; i < n; ++i) {
            purchasedServer = &(purchasedServers[i]);
            if (!purchasedServer->type) continue;

            if (CheckCapacity(*purchasedServer, vm) == ALL) {
                DeployVirtualMachine(i, vmIdx, id, ALL);
                return 0u;
            }
        }
    } else {
        // 单节点
        for (uint16_t i = 0u; i < n; ++i) {
            purchasedServer = &(purchasedServers[i]);
            if (purchasedServer->type) continue;

            LOCATION location = CheckCapacity(*purchasedServer, vm);
            if (location != NONE) {
                DeployVirtualMachine(i, vmIdx, id, location);
                return 0u;
            }
        }
    }

    Purchase(day, T);
    DeployVirtualMachine(n, vmIdx, id, ALL);
    purchasedServers[n].type = vm.nodeType;
    return 1u;
}

void Handler::DeployVirtualMachine(uint16_t purchasedServerIdx, uint16_t vmIdx, int vmId, const LOCATION &location) {
    PurchasedServer &purchasedServer = purchasedServers[purchasedServerIdx];
    const VirtualMachine &vm = virtualMachines[vmIdx];
    auto *deployedVM = new DeployedVirtualMachine();
    uint32_t n = deployedVMs.size();
    uint16_t halfCpuCore = vm.cpuCore >> 1u;
    uint16_t halfMemorySize = vm.memorySize >> 1u;

    if (vm.nodeType) {
        purchasedServer.remainCpuCoreA -= halfCpuCore;
        purchasedServer.remainCpuCoreB -= halfCpuCore;
        purchasedServer.remainMemorySizeA -= halfMemorySize;
        purchasedServer.remainMemorySizeB -= halfMemorySize;
        purchasedServer.deployedVMA.push_back(n);
        requestResult.emplace_back("(" + std::to_string(purchasedServerIdx) + ")\n");
    } else {
        // TODO: 分析服务器左右节点平衡
        if (location == ALL || location == NODE_A) {
            purchasedServer.remainCpuCoreA -= vm.cpuCore;
            purchasedServer.remainMemorySizeA -= vm.memorySize;
            purchasedServer.deployedVMA.push_back(n);
            deployedVM->location = true;
            requestResult.emplace_back("(" + std::to_string(purchasedServerIdx) + ", " + "A)\n");
        } else if (location == NODE_B) {
            purchasedServer.remainCpuCoreB -= vm.cpuCore;
            purchasedServer.remainMemorySizeB -= vm.memorySize;
            purchasedServer.deployedVMB.push_back(n);
            deployedVM->location = false;
            requestResult.emplace_back("(" + std::to_string(purchasedServerIdx) + ", " + "B)\n");
        }
    }

    deployedVM->purchasedServerIdx = purchasedServerIdx;
    deployedVM->vmIdx = vmIdx;
    deployedVM->vmId = vmId;
    deployedVMs.emplace_back(*deployedVM);
    idDeployedVMMap[vmId] = n;
    ++deployedVMNum;
}

void Handler::DeleteVirtualMachine(int id) {
    uint32_t deployedVMIdx = idDeployedVMMap[id];
    DeployedVirtualMachine &deployedVM = deployedVMs[deployedVMIdx];
    PurchasedServer &purchasedServer = purchasedServers[deployedVM.purchasedServerIdx];
    VirtualMachine &vm = virtualMachines[deployedVM.vmIdx];
    uint16_t cpuCore = vm.cpuCore;
    uint16_t memorySize = vm.memorySize;

    if (vm.nodeType) {
        purchasedServer.remainCpuCoreA += cpuCore >> 1u;
        purchasedServer.remainCpuCoreB += cpuCore >> 1u;
        purchasedServer.remainMemorySizeA += memorySize >> 1u;
        purchasedServer.remainMemorySizeB += memorySize >> 1u;
    } else {
        if (deployedVM.location) {
            purchasedServer.remainCpuCoreA += cpuCore;
            purchasedServer.remainMemorySizeA += memorySize;
        } else {
            purchasedServer.remainCpuCoreB += cpuCore;
            purchasedServer.remainMemorySizeB += memorySize;
        }
    }

    --deployedVMNum;
    idDeployedVMMap.erase(id);
    idVirtualMachineMap.erase(id);
}

inline LOCATION Handler::CheckCapacity(const PurchasedServer &purchasedServer) {
    bool a = purchasedServer.remainCpuCoreA > 0u && purchasedServer.remainMemorySizeA > 0u;
    bool b = purchasedServer.remainCpuCoreB > 0u && purchasedServer.remainMemorySizeB > 0u;
    if (a && b) return ALL;
    else if (a) return NODE_A;
    else if (b) return NODE_B;
    else return NONE;
}

inline LOCATION Handler::CheckCapacity(const PurchasedServer &purchasedServer, const VirtualMachine &vm) {
    uint16_t cpuCore = vm.cpuCore;
    uint16_t memorySize = vm.memorySize;
    uint16_t halfCpuCore = cpuCore >> 1u;
    uint16_t halfMemorySize = memorySize >> 1u;

    if (vm.nodeType) {
        // 双节点
        if (purchasedServer.remainCpuCoreA >= halfCpuCore && purchasedServer.remainCpuCoreB >= halfCpuCore &&
            purchasedServer.remainMemorySizeA >= halfMemorySize && purchasedServer.remainMemorySizeB >= halfMemorySize) {
            return ALL;
        } else {
            return NONE;
        }
    }

    // 单节点
    bool a = purchasedServer.remainCpuCoreA >= cpuCore && purchasedServer.remainMemorySizeA >= memorySize;
    bool b = purchasedServer.remainCpuCoreB >= cpuCore && purchasedServer.remainMemorySizeB >= memorySize;
    if (a && b) return ALL;
    else if (a) return NODE_A;
    else if (b) return NODE_B;
    else return NONE;
}

inline double_t Handler::CalculateScale(double_t memorySize, double_t cpuCore) {
    return memorySize / cpuCore;
}
