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
    // string inputFile = R"(D:\GitHub Repository\Huawei-CodeCraft-2021\preliminary-practice\data\test.txt)";
    // string inputFile = R"(D:\GitHub Repository\Huawei-CodeCraft-2021\preliminary-practice\data\training-1.txt)";
    // string inputFile = R"(D:\GitHub Repository\Huawei-CodeCraft-2021\preliminary-practice\data\training-2.txt)";
    // string inputFile = "/home/shenke/Develop/Repository/Huawei-CodeCraft-2021/preliminary-practice/data/test.txt";
    // string inputFile = "/home/shenke/Develop/Repository/Huawei-CodeCraft-2021/preliminary-practice/data/training-1.txt";
    string inputFile = "/home/shenke/Develop/Repository/Huawei-CodeCraft-2021/preliminary-practice/data/training-2.txt";

    std::freopen(inputFile.c_str(), "r", stdin);
#endif

    InputServer();
    InputVirtualMachine();
    InputRequest();

#ifdef TEST
    fclose(stdin);
#endif
}

void Handler::Handle() {
#ifdef TEST
    auto start = std::chrono::system_clock::now();
#endif

    Init();

    for (uint16_t i = 0u; i < T; ++i) {
        PrintLog("Handling %u day\n", today + 1u);

        // TODO: 扩容
        // Expand(false, 0u);

        // TODO: 迁移
        Migrate();

        // TODO: 处理请求
        HandleRequests();
        ++today;
        // todayCandidateServerIdx = -1;

#ifdef TEST
        for (auto &purchasedServer : purchasedServers) {
            if (purchasedServer.deployedVM.empty()) continue;
            totalEnergyCost += servers[purchasedServer.serverIdx].energyCost;
        }
#endif
    }

    PrintLog("Hardware cost: %lu\n", totalHardwareCost);
    PrintLog("Energy cost: %lu\n", totalEnergyCost);
    PrintLog("Total cost: %lu\n", totalHardwareCost + totalEnergyCost);
    PrintLog("Total migration num: %u\n", totalMigrationNum);

#ifdef TEST
    std::chrono::duration<double, std::milli> duration = std::chrono::system_clock::now() - start;
    PrintLog("Handle elapsed time: %.4fms\n", duration.count());
#endif
}

void Handler::Output() {
#ifdef TEST
    // string outputFile = R"(D:\GitHub Repository\Huawei-CodeCraft-2021\preliminary-practice\output.txt)";
    string outputFile = "/home/shenke/Develop/Repository/Huawei-CodeCraft-2021/preliminary-practice/output.txt";
    std::freopen(outputFile.c_str(), "w", stdout);
#endif

    for (const Result &res : result) {
        const std::vector<ExtendResult> &extendResult = res.extendResult;
        // 扩容结果
        std::cout << "(purchase, " << extendResult.size() << ")\n";
        for (const ExtendResult &extendRes : extendResult) {
            std::cout << "(" << servers[extendRes.serverIdx].model << ", " << extendRes.purchaseNum << ")\n";
        }

        // 迁移结果
        const std::vector<MigrateResult> &migrateResult = res.migrateResult;
        std::cout << "(migration, " << migrateResult.size() << ")\n";
        for (const MigrateResult &migrateRes : migrateResult) {
            if (migrateRes.location == ALL) {
                std::cout << "(" << migrateRes.vmId << ", " << migrateRes.purchasedServerId << ")\n";
            } else if (migrateRes.location == NODE_A) {
                std::cout << "(" << migrateRes.vmId << ", " << migrateRes.purchasedServerId << ", A)\n";
            } else if (migrateRes.location == NODE_B) {
                std::cout << "(" << migrateRes.vmId << ", " << migrateRes.purchasedServerId << ", B)\n";
            }
        }

        // 部署结果
        const std::vector<RequestResult> &requestResult = res.requestResult;
        for (const RequestResult &requestRes : requestResult) {
            if (requestRes.location == ALL) {
                std::cout << "(" << requestRes.purchasedServerId << ")\n";
            } else if (requestRes.location == NODE_A) {
                std::cout << "(" << requestRes.purchasedServerId << ", A)\n";
            } else if (requestRes.location == NODE_B) {
                std::cout << "(" << requestRes.purchasedServerId << ", B)\n";
            }
        }
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

        cpuCoreSum += cpuCore;
        memorySizeSum += memorySize;
    }

    PrintLog("Server model num: %u\n", N);
    PrintLog("Cpu core sum: %u, average: %.2lf\n", cpuCoreSum, (double_t) cpuCoreSum / (double_t) N);
    PrintLog("Memory size sum: %u, average: %.2lf\n", memorySizeSum, (double_t) memorySizeSum / (double_t) N);
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
    addRequestCounts.resize(T);
    result.resize(T);
    VirtualMachine *vm;

    uint16_t maxCpuCore = 0u, maxMemorySize = 0u;
    uint32_t totalCpuCore = 0u, totalMemorySize = 0u;
    uint32_t maxTotalCpuCore = 0u, maxTotalMemorySize = 0u;

    for (uint16_t i = 0u; i < T; ++i) {
        uint32_t R;
        scanf("%u", &R);
        string operationStr, modelStr, idStr;
        result[i].requestResult.resize(R);
        std::vector<Request> request(R);
        uint32_t addRequestCount = 0u;

        for (uint32_t j = 0u; j < R; ++j) {
            Request req;

            std::cin >> operationStr;
            req.add = (operationStr[1] == 'a');
            if (req.add) {
                std::cin >> modelStr;
                req.vmIdx = virtualMachineMap[modelStr.substr(0, modelStr.size() - 1)];
                req.addIdx = addRequestCount++;
            }

            std::cin >> idStr;
            int id = 0;
            for (int k = 0; k < idStr.size() - 1; ++k) {
                id = id * 10 + (idStr[k] - 48);
            }
            req.vmId = id;

            if (req.add) {
                uint16_t vmIdx = req.vmIdx;
                idVirtualMachineMap[id] = vmIdx;
                vm = &virtualMachines[vmIdx];
                totalCpuCore += vm->cpuCore;
                totalMemorySize += vm->memorySize;
                maxCpuCore = std::max(maxCpuCore, vm->cpuCore);
                maxMemorySize = std::max(maxMemorySize, vm->memorySize);
                totalCpuCore += vm->cpuCore;
                totalMemorySize += vm->memorySize;
                maxTotalCpuCore = std::max(maxTotalCpuCore, totalCpuCore);
                maxTotalMemorySize = std::max(maxTotalMemorySize, totalMemorySize);
            } else {
                vm = &virtualMachines[idVirtualMachineMap[id]];
                totalCpuCore -= vm->cpuCore;
                totalMemorySize -= vm->memorySize;
            }

            request[j] = req;
        }

        requests[i] = request;
        addRequestCounts[i] = addRequestCount;
    }

    demand = {maxCpuCore, maxMemorySize, maxTotalCpuCore, maxTotalMemorySize};

    PrintLog("Request day num: %u\n", T);
    PrintLog("Demand: %s\n", demand.ToString().c_str());
}

void Handler::Init() {
    // 将 servers 按照 mem + cpu 的和进行升序排列
    std::sort(servers.begin(), servers.end(), [](const Server &a, const Server &b) {
        return a.scale < b.scale;
    });

    while (candidateServers.empty()) {
        // 选择候选服务器
        for (uint8_t i = 0u; i < N; ++i) {
            double_t demandScale = CalculateScale(demand.maxTotalMemorySize, demand.maxTotalCpuCore);
            if (servers[i].cpuCore >= demand.maxCpuCore && servers[i].memorySize >= demand.maxMemorySize &&
                demandScale - candidateServerInterval < servers[i].scale && demandScale + candidateServerInterval > servers[i].scale) {
                candidateServers.emplace_back(i);
            }
        }
        candidateServerInterval += 0.01;
    }
    todayCandidateServerIdx = candidateServers[candidateServers.size() - 1];
    PrintLog("Candidate servers size: %zu\n", candidateServers.size());

    // 将 requests 按照 cpu + mem 大小进行降序排列
    for (uint16_t i = 0u; i < T; ++i) {
        std::vector<Request> &request = requests[i];
        uint32_t j = 0u, R = request.size();
        uint32_t left, right;
        while (j < R) {
            left = j;
            while (left < R && !request[left].add) ++left;
            right = left;
            while (right < R && request[right].add) ++right;
            std::sort(request.begin() + left, request.begin() + right, [this](const Request &a, const Request &b) {
                VirtualMachine &vm1 = virtualMachines[a.vmIdx];
                VirtualMachine &vm2 = virtualMachines[b.vmIdx];
                return vm1.cpuCore + vm1.memorySize > vm2.cpuCore + vm2.memorySize;
            });
            j = right;
        }
    }
}

void Handler::Expand(const bool &retry, const uint16_t &retryCount) {
}

void Handler::Migrate() {
    uint32_t threshold = deployedVMNum / 200u;
    uint32_t migrateNum = 0u;
    bool ended = (migrateNum >= threshold);
    if (ended) return;

    // 将 purchasedServers 中开机的服务器按照剩余容量升序排列
    std::sort(purchasedServers.begin(), purchasedServers.end(), [](const PurchasedServer &a, const PurchasedServer &b) {
        return a.remainCpuCoreA + a.remainCpuCoreB + a.remainMemorySizeA + a.remainMemorySizeB <
               b.remainCpuCoreA + b.remainCpuCoreB + b.remainMemorySizeA + b.remainMemorySizeB;
    });
    // 排序后需要重新映射 purchasedServer id -> purchasedServers index
    ReMapPurchasedServerIdx();

    // originPurchasedServer -> destPurchasedServer
    for (int i = (int) purchasedServers.size() - 1; i >= 0 && !ended; --i) {
        PurchasedServer &originPurchasedServer = purchasedServers[i];
        std::unordered_set<uint32_t> &deployedVM = originPurchasedServer.deployedVM;
        std::vector<uint32_t> tmp;
        for (auto &deployedVMIdx : deployedVM) tmp.emplace_back(deployedVMIdx);
        SortDeployedVM(tmp);

        for (const uint32_t &deployedVMIdx : tmp) {
            if (ended) break;

            int left = 0, right = i, mid;
            while (left < right) {
                mid = (left + right) / 2;
                PurchasedServer &destPurchasedServer = purchasedServers[mid];
                if (Migrate(originPurchasedServer, destPurchasedServer, deployedVMIdx) != NONE) {
                    if (++migrateNum >= threshold) ended = true;
                    break;
                } else {
                    left = mid + 1;
                }
            }
        }
    }

#ifdef TEST
    totalMigrationNum += migrateNum;
#endif
}

void Handler::HandleRequests() {
    std::vector<Request> &request = requests[today];
    uint16_t purchasedServerNum = 0u;

    for (const Request &req : request) {
        if (req.add) {
            purchasedServerNum += AddVirtualMachine(req);
        } else {
            DeleteVirtualMachine(req.vmId);
        }
    }

    if (purchasedServerNum > 0u) {
        result[today].extendResult.emplace_back(todayCandidateServerIdx, purchasedServerNum);
    }
}

void Handler::Purchase(const uint8_t &serverIdx) {
    const Server &server = servers[serverIdx];
    uint16_t halfCpuCore = server.cpuCore >> 1u;
    uint16_t halfMemorySize = server.memorySize >> 1u;

    auto *purchasedServer = new PurchasedServer();
    purchasedServer->id = purchasedServers.size();
    purchasedServerIdxMap[purchasedServer->id] = purchasedServers.size();
    purchasedServer->serverIdx = serverIdx;
    purchasedServer->remainCpuCoreA = halfCpuCore;
    purchasedServer->remainCpuCoreB = halfCpuCore;
    purchasedServer->remainMemorySizeA = halfMemorySize;
    purchasedServer->remainMemorySizeB = halfMemorySize;
    purchasedServers.emplace_back(*purchasedServer);

#ifdef TEST
    totalHardwareCost += server.hardwareCost;
#endif
}

LOCATION Handler::Migrate(PurchasedServer &originPurchasedServer, PurchasedServer &destPurchasedServer, const uint32_t &deployedVMIdx) {
    DeployedVirtualMachine &deployedVM = deployedVMs[deployedVMIdx];
    VirtualMachine &vm = virtualMachines[deployedVM.vmIdx];
    int vmId = deployedVM.vmId;
    uint16_t destPurchasedServerId = destPurchasedServer.id;

    LOCATION location = CheckCapacity(destPurchasedServer, vm);
    if (location == NONE) return NONE;

    if (vm.nodeType && location == ALL) {
        // 双节点
        uint16_t halfCpuCore = vm.cpuCore >> 1u;
        uint16_t halfMemorySize = vm.memorySize >> 1u;
        originPurchasedServer.remainCpuCoreA += halfCpuCore;
        originPurchasedServer.remainCpuCoreB += halfCpuCore;
        originPurchasedServer.remainMemorySizeA += halfMemorySize;
        originPurchasedServer.remainMemorySizeB += halfMemorySize;
        originPurchasedServer.deployedVM.erase(deployedVMIdx);
        destPurchasedServer.remainCpuCoreA -= halfCpuCore;
        destPurchasedServer.remainCpuCoreB -= halfCpuCore;
        destPurchasedServer.remainMemorySizeA -= halfMemorySize;
        destPurchasedServer.remainMemorySizeB -= halfMemorySize;
        destPurchasedServer.deployedVM.emplace(deployedVMIdx);
        deployedVM.purchasedServerId = destPurchasedServerId;
        result[today].migrateResult.emplace_back(vmId, destPurchasedServerId, ALL);
        return ALL;
    }

    // 单节点
    uint16_t cpuCore = vm.cpuCore;
    uint16_t memorySize = vm.memorySize;
    bool flag = destPurchasedServer.remainCpuCoreA + destPurchasedServer.remainMemorySizeA >
                destPurchasedServer.remainCpuCoreB + destPurchasedServer.remainMemorySizeB;
    if (location == NODE_A || (location == ALL && flag)) {
        if (deployedVM.location) {
            // A -> A
            originPurchasedServer.remainCpuCoreA += cpuCore;
            originPurchasedServer.remainMemorySizeA += memorySize;
        } else {
            // B -> A
            originPurchasedServer.remainCpuCoreB += cpuCore;
            originPurchasedServer.remainMemorySizeB += memorySize;
        }
        destPurchasedServer.remainCpuCoreA -= cpuCore;
        destPurchasedServer.remainMemorySizeA -= memorySize;
        destPurchasedServer.deployedVM.emplace(deployedVMIdx);
        originPurchasedServer.deployedVM.erase(deployedVMIdx);
        deployedVM.purchasedServerId = destPurchasedServerId;
        deployedVM.location = true;
        result[today].migrateResult.emplace_back(vmId, destPurchasedServerId, NODE_A);
        return NODE_A;
    } else if (location == NODE_B || (location == ALL && !flag)) {
        if (deployedVM.location) {
            // A -> B
            originPurchasedServer.remainCpuCoreA += cpuCore;
            originPurchasedServer.remainMemorySizeA += memorySize;
        } else {
            // B -> B
            originPurchasedServer.remainCpuCoreB += cpuCore;
            originPurchasedServer.remainMemorySizeB += memorySize;
        }
        destPurchasedServer.remainCpuCoreB -= cpuCore;
        destPurchasedServer.remainMemorySizeB -= memorySize;
        destPurchasedServer.deployedVM.emplace(deployedVMIdx);
        originPurchasedServer.deployedVM.erase(deployedVMIdx);
        deployedVM.purchasedServerId = destPurchasedServerId;
        deployedVM.location = false;
        result[today].migrateResult.emplace_back(vmId, destPurchasedServerId, NODE_B);
        return NODE_B;
    }

    return NONE;
}

void Handler::ReMapPurchasedServerIdx() {
    uint16_t n = purchasedServers.size();
    for (uint16_t i = 0u; i < n; ++i) purchasedServerIdxMap[purchasedServers[i].id] = i;
}

void Handler::SortDeployedVM(std::vector<uint32_t> &deployedVM) {
    std::sort(deployedVM.begin(), deployedVM.end(), [this](const uint32_t &a, const uint32_t &b) {
        const DeployedVirtualMachine &deployedVM1 = deployedVMs[a];
        const DeployedVirtualMachine &deployedVM2 = deployedVMs[b];
        const VirtualMachine &vm1 = virtualMachines[deployedVM1.vmIdx];
        const VirtualMachine &vm2 = virtualMachines[deployedVM2.vmIdx];

        if (!deployedVM1.location && deployedVM2.location) return true;
        else if (deployedVM1.location && !deployedVM2.location) return false;
        else {
            return vm1.cpuCore + vm1.memorySize > vm2.cpuCore + vm2.memorySize;
        }
    });
}

uint16_t Handler::AddVirtualMachine(const Request &req) {
    uint16_t n = purchasedServers.size();
    for (uint16_t i = 0u; i < n; ++i) {
        if (DeployVirtualMachine(i, req)) return 0u;
    }

    uint16_t purchaseNum = 1u;
    Purchase(todayCandidateServerIdx);
    while (!DeployVirtualMachine(n, req)) {
        Purchase(todayCandidateServerIdx);
        ++n;
        ++purchaseNum;
        PrintLog("trying");
    }

    return purchaseNum;
}

bool Handler::DeployVirtualMachine(const uint16_t &purchasedServerIdx, const Request &req) {
    PurchasedServer &purchasedServer = purchasedServers[purchasedServerIdx];
    uint16_t purchasedServerId = purchasedServer.id;
    uint16_t vmIdx = req.vmIdx;
    int vmId = req.vmId;
    uint32_t addIdx = req.addIdx;
    VirtualMachine &vm = virtualMachines[vmIdx];

    LOCATION location = CheckCapacity(purchasedServer, vm);
    if (location == NONE) return false;

    auto *deployedVM = new DeployedVirtualMachine();
    auto *requestResult = new RequestResult(purchasedServerId);
    uint32_t n = deployedVMs.size();
    uint16_t halfCpuCore = vm.cpuCore >> 1u;
    uint16_t halfMemorySize = vm.memorySize >> 1u;

    if (vm.nodeType) {
        purchasedServer.remainCpuCoreA -= halfCpuCore;
        purchasedServer.remainCpuCoreB -= halfCpuCore;
        purchasedServer.remainMemorySizeA -= halfMemorySize;
        purchasedServer.remainMemorySizeB -= halfMemorySize;
        purchasedServer.deployedVM.emplace(n);
        requestResult->location = ALL;
    } else {
        bool flag = purchasedServer.remainCpuCoreA + purchasedServer.remainMemorySizeA > purchasedServer.remainCpuCoreB + purchasedServer.remainMemorySizeB;
        if (location == NODE_A || (location == ALL && flag)) {
            purchasedServer.remainCpuCoreA -= vm.cpuCore;
            purchasedServer.remainMemorySizeA -= vm.memorySize;
            purchasedServer.deployedVM.emplace(n);
            deployedVM->location = true;
            requestResult->location = NODE_A;
        } else if (location == NODE_B || (location == ALL && !flag)) {
            purchasedServer.remainCpuCoreB -= vm.cpuCore;
            purchasedServer.remainMemorySizeB -= vm.memorySize;
            purchasedServer.deployedVM.emplace(n);
            deployedVM->location = false;
            requestResult->location = NODE_B;
        }
    }

    result[today].requestResult[addIdx] = *requestResult;
    deployedVM->purchasedServerId = purchasedServer.id;
    deployedVM->vmIdx = vmIdx;
    deployedVM->vmId = vmId;
    deployedVMs.emplace_back(*deployedVM);
    idDeployedVMMap[vmId] = n;
    ++deployedVMNum;
    return true;
}

void Handler::DeleteVirtualMachine(int id) {
    uint32_t deployedVMIdx = idDeployedVMMap[id];
    DeployedVirtualMachine &deployedVM = deployedVMs[deployedVMIdx];
    PurchasedServer &purchasedServer = purchasedServers[purchasedServerIdxMap[deployedVM.purchasedServerId]];
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
    purchasedServer.deployedVM.erase(deployedVMIdx);
}

LOCATION Handler::CheckCapacity(const PurchasedServer &purchasedServer, const VirtualMachine &vm) {
    uint16_t cpuCore = vm.cpuCore;
    uint16_t memorySize = vm.memorySize;
    uint16_t halfCpuCore = cpuCore >> 1u;
    uint16_t halfMemorySize = memorySize >> 1u;

    if (vm.nodeType) {
        // 双节点
        if (purchasedServer.remainCpuCoreA >= halfCpuCore && purchasedServer.remainCpuCoreB >= halfCpuCore &&
            purchasedServer.remainMemorySizeA >= halfMemorySize &&
            purchasedServer.remainMemorySizeB >= halfMemorySize) {
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
