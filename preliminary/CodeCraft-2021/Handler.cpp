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
    // string inputFile = R"(D:\GitHub Repository\Huawei-CodeCraft-2021\preliminary\data\test.txt)";
    string inputFile = R"(D:\GitHub Repository\Huawei-CodeCraft-2021\preliminary\data\training-1.txt)";
    // string inputFile = R"(D:\GitHub Repository\Huawei-CodeCraft-2021\preliminary\data\training-2.txt)";
    // string inputFile = "/home/shenke/Develop/Repository/Huawei-CodeCraft-2021/preliminary/data/test.txt";
    // string inputFile = "/home/shenke/Develop/Repository/Huawei-CodeCraft-2021/preliminary/data/training-1.txt";
    // string inputFile = "/home/shenke/Develop/Repository/Huawei-CodeCraft-2021/preliminary/data/training-2.txt";

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

        Migrate();
        HandleRequests();
        IncreasePurchaseNumber();
        ++today;

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
    string outputFile = R"(D:\GitHub Repository\Huawei-CodeCraft-2021\preliminary\output.txt)";
    // string outputFile = "/home/shenke/Develop/Repository/Huawei-CodeCraft-2021/preliminary/output.txt";
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
    string numStr, modelStr, cpuCoreStr, memorySizeStr, hardwareCostStr, energyCostStr;
    uint16_t cpuCore;
    uint16_t memorySize;
    uint32_t hardwareCost;
    uint16_t energyCost;

    std::cin >> numStr;
    N = (uint8_t) std::stoi(numStr);
    servers.resize(N);

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

        servers[i] = *server;
    }

    PrintLog("Server model num: %u\n", N);
}

void Handler::InputVirtualMachine() {
    string numStr, modelStr, cpuCoreStr, memorySizeStr, nodeNumStr;
    uint16_t cpuCore;
    uint16_t memorySize;

    std::cin >> numStr;
    M = (uint16_t) std::stoi(numStr);
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
    string numStr;
    std::cin >> numStr;
    T = (uint16_t) std::stoi(numStr);
    requests.resize(T);
    addRequestCounts.resize(T);
    result.resize(T);

    uint16_t vmIdx;
    for (uint16_t i = 0u; i < T; ++i) {
        uint32_t R;
        string operationStr, modelStr, idStr;
        std::cin >> numStr;
        R = (uint16_t) std::stoi(numStr);
        result[i].requestResult.resize(R);
        std::vector<Request> request(R);
        uint32_t addRequestCount = 0u;

        for (uint32_t j = 0u; j < R; ++j) {
            Request req;

            std::cin >> operationStr;
            req.add = (operationStr[1] == 'a');
            if (req.add) {
                std::cin >> modelStr;
                vmIdx = virtualMachineMap[modelStr.substr(0, modelStr.size() - 1)];
            }

            std::cin >> idStr;
            int id = 0;
            for (int k = 0; k < idStr.size() - 1; ++k) {
                id = id * 10 + (idStr[k] - 48);
            }
            req.vmId = id;

            if (req.add) {
                req.addIdx = addRequestCount++;
                req.vmIdx = vmIdx;
                idVirtualMachineMap[id] = vmIdx;
            } else {
                req.vmIdx = idVirtualMachineMap[id];
            }

            request[j] = req;
        }

        requests[i] = request;
        addRequestCounts[i] = addRequestCount;
    }

    PrintLog("Request day num: %u\n", T);
}

void Handler::Init() {
    // 将 servers 按照 energyCost 的和进行升序排列
    std::sort(servers.begin(), servers.end(), [](const Server &a, const Server &b) {
        return a.energyCost < b.energyCost;
    });

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

void Handler::Migrate() {
    uint32_t threshold = deployedVMNum / 200u;
    uint32_t migrateNum = 0u;
    bool ended = (migrateNum >= threshold);
    if (ended) return;

    // 将 purchasedServers 按照剩余容量升序排列
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
        std::vector<uint32_t> tmp(deployedVM.begin(), deployedVM.end());
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
    for (const Request &req : request) {
        if (req.add)
            AddVirtualMachine(req);
        else
            DeleteVirtualMachine(req.vmId);
    }
}

void Handler::IncreasePurchaseNumber() {
    std::vector<ExtendResult> &extendResult = result[today].extendResult;
    for (ExtendResult &res : extendResult) {
        totalPurchasedServerNum += res.purchaseNum;
    }
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
    }

    if (location == NODE_B || (location == ALL && !flag)) {
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
        else return vm1.cpuCore + vm1.memorySize > vm2.cpuCore + vm2.memorySize;
    });
}

void Handler::AddVirtualMachine(const Request &req) {
    uint16_t n = purchasedServers.size();
    const VirtualMachine &vm = virtualMachines[req.vmIdx];
    if (vm.nodeType) {
        // 双节点
        std::vector<double_t> value;
        value.resize(n, 0);
        for (uint16_t i = 0u; i < n; ++i) {
            value[i] = CalculatePurchasedServerValue(purchasedServers[i], vm, ALL);
        }

        double_t minValue = 1e10;
        bool exist = false;
        uint16_t idx = 0u;
        // 找到不为 0 的所有参数中的最小的那个
        for (uint16_t i = 0u; i < n; ++i) {
            if (value[i] > 0 && value[i] < minValue) {
                minValue = value[i];
                idx = i;
                exist = true;
            }
        }
        if (exist) {  // 说明存在可以放置的服务器
            DeployVirtualMachine(idx, req, ALL);
        } else {
            uint16_t purchasedServerId = Purchase(vm);
            DeployVirtualMachine(purchasedServerId, req, ALL);
        }

    } else {
        // 单节点
        std::vector<double_t> value;
        value.resize(n << 1u, 0);
        for (uint16_t i = 0u; i < n; ++i) {
            value[(i << 1u)] = CalculatePurchasedServerValue(purchasedServers[i], vm, NODE_A);
            value[(i << 1u) + 1u] = CalculatePurchasedServerValue(purchasedServers[i], vm, NODE_B);
        }

        double_t minValue = 1e10;
        bool exist = false;
        uint16_t idx = 0u;
        // 找到不为 0 的所有参数中的最小的那个
        for (uint16_t i = 0u; i < (uint16_t) value.size(); ++i) {
            if (value[i] > 0 && value[i] < minValue) {
                minValue = value[i];
                idx = i;
                exist = true;
            }
        }
        if (exist) {  // 说明存在可以放置的服务器
            DeployVirtualMachine(idx >> 1u, req, (idx & 1u) == 0u ? NODE_A : NODE_B);
        } else {
            uint16_t purchasedServerId = Purchase(vm);
            DeployVirtualMachine(purchasedServerId, req, NODE_A);
        }
    }
}

uint16_t Handler::Purchase(const VirtualMachine &vm) {
    uint8_t serverIdx;
    // servers 已经根据 energyCost 从小到大排过序，找到刚好合适的 server 就退出
    for (uint8_t i = 0u; i < N; ++i) {
        if (CheckCapacity(servers[i], vm)) {
            serverIdx = i;
            break;
        }
    }

    auto *purchasedServer = new PurchasedServer();
    std::vector<ExtendResult> &extendResult = result[today].extendResult;
    uint16_t id = totalPurchasedServerNum;
    int resultIdx = 0, n = extendResult.size();
    bool exist = false;
    for (int i = 0; i < n; ++i) {
        id += extendResult[i].purchaseNum;
        if (extendResult[i].serverIdx == serverIdx) {   // 买过同一型号的 server
            ++extendResult[i].purchaseNum;   // 数量加一表示多买一个
            exist = true;
            resultIdx = i;
            break;
        }
    }

    // 已经购买过这一类型的服务器
    if (exist) {
        if (resultIdx < n - 1) {  // 不是 extendResult 中的最后一个种类
            for (DeployedVirtualMachine &deployedVM : deployedVMs) {
                if (deployedVM.purchasedServerId >= id) ++deployedVM.purchasedServerId;
            }
            for (PurchasedServer &item : purchasedServers) {
                if (item.id >= id) ++item.id;
            }
            ReMapPurchasedServerIdx();

            std::vector<MigrateResult> &migrateResult = result[today].migrateResult;
            for (MigrateResult &res : migrateResult) {
                if (res.purchasedServerId >= id) ++res.purchasedServerId;
            }
            std::vector<RequestResult> &requestResult = result[today].requestResult;
            for (RequestResult &res : requestResult) {
                if (res.purchasedServerId >= id) ++res.purchasedServerId;
            }
        }
    } else {
        extendResult.emplace_back(serverIdx, 1u);
    }

    purchasedServer->id = id;
    purchasedServer->serverIdx = serverIdx;
    purchasedServer->remainCpuCoreA = (servers[serverIdx].cpuCore >> 1u);
    purchasedServer->remainCpuCoreB = (servers[serverIdx].cpuCore >> 1u);
    purchasedServer->remainMemorySizeA = (servers[serverIdx].memorySize >> 1u);
    purchasedServer->remainMemorySizeB = (servers[serverIdx].memorySize >> 1u);
    purchasedServerIdxMap[id] = purchasedServers.size();
    purchasedServers.emplace_back(*purchasedServer);

#ifdef TEST
    totalHardwareCost += servers[serverIdx].hardwareCost;
#endif

    return purchasedServerIdxMap[id];
}

void Handler::DeleteVirtualMachine(const int &vmId) {
    uint32_t deployedVMIdx = idDeployedVMMap[vmId];
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
    idDeployedVMMap.erase(vmId);
    idVirtualMachineMap.erase(vmId);
    purchasedServer.deployedVM.erase(deployedVMIdx);
}

LOCATION Handler::CheckCapacity(const PurchasedServer &purchasedServer, const VirtualMachine &vm) {
    // 双节点
    if (vm.nodeType) return CheckCapacity(purchasedServer, vm, ALL) ? ALL : NONE;

    // 单节点
    bool a = CheckCapacity(purchasedServer, vm, NODE_A);
    bool b = CheckCapacity(purchasedServer, vm, NODE_B);
    if (a && b) return ALL;
    else if (a) return NODE_A;
    else if (b) return NODE_B;
    else return NONE;
}

bool Handler::CheckCapacity(const PurchasedServer &purchasedServer, const VirtualMachine &vm, const LOCATION &location) {
    uint16_t cpuCore = vm.cpuCore;
    uint16_t memorySize = vm.memorySize;
    switch (location) {
        case NODE_A:
            return purchasedServer.remainCpuCoreA >= cpuCore && purchasedServer.remainMemorySizeA >= memorySize;
        case NODE_B:
            return purchasedServer.remainCpuCoreB >= cpuCore && purchasedServer.remainMemorySizeB >= memorySize;
        case ALL:
            return purchasedServer.remainCpuCoreA >= (cpuCore >> 1u) && purchasedServer.remainMemorySizeA >= (memorySize >> 1u) &&
                   purchasedServer.remainCpuCoreB >= (cpuCore >> 1u) && purchasedServer.remainMemorySizeB >= (memorySize >> 1u);
        default:
            return false;
    }
}

inline bool Handler::CheckCapacity(const Server &server, const VirtualMachine &vm) {
    return vm.nodeType ? server.cpuCore >= vm.cpuCore && server.memorySize >= vm.memorySize :
           (server.cpuCore >> 1u) >= vm.cpuCore && (server.memorySize >> 1u) >= vm.memorySize;
}

double_t Handler::CalculatePurchasedServerValue(const PurchasedServer &purchasedServer, const VirtualMachine &vm, const LOCATION &location) {
    uint16_t energyCost = servers[purchasedServer.serverIdx].energyCost;
    uint16_t cpuCore = vm.cpuCore;
    uint16_t memorySize = vm.memorySize;

    if (location == ALL && CheckCapacity(purchasedServer, vm, location)) {
        uint16_t halfCpuCore = cpuCore >> 1u;
        uint16_t halfMemorySize = memorySize >> 1u;
        uint16_t remainCpuCoreA = purchasedServer.remainCpuCoreA - halfCpuCore;
        uint16_t remainMemorySizeA = purchasedServer.remainMemorySizeA - halfMemorySize;
        uint16_t remainCpuCoreB = purchasedServer.remainCpuCoreB - halfCpuCore;
        uint16_t remainMemorySizeB = purchasedServer.remainMemorySizeB - halfMemorySize;
        return CalculatePurchasedServerValue(remainCpuCoreA + remainCpuCoreB, remainMemorySizeA + remainMemorySizeB, energyCost);
    }

    if (location == NODE_A && CheckCapacity(purchasedServer, vm, location)) {
        uint16_t remainCpuCoreA = purchasedServer.remainCpuCoreA - cpuCore;
        uint16_t remainMemorySizeA = purchasedServer.remainMemorySizeA - memorySize;
        return CalculatePurchasedServerValue(remainCpuCoreA, remainMemorySizeA, energyCost);
    }

    if (location == NODE_B && CheckCapacity(purchasedServer, vm, location)) {
        uint16_t remainCpuCoreB = purchasedServer.remainCpuCoreB - cpuCore;
        uint16_t remainMemorySizeB = purchasedServer.remainMemorySizeB - memorySize;
        return CalculatePurchasedServerValue(remainCpuCoreB, remainMemorySizeB, energyCost);
    }

    return 0;
}

inline double_t Handler::CalculatePurchasedServerValue(uint16_t cpuCore, uint16_t memorySize, uint16_t energyCost) {
    // return 0.75 * (double_t) cpuCore + 0.25 * (double_t) memorySize;
    return 0.75 * (double_t) cpuCore + 0.23 * (double_t) memorySize + 0.02 * (double_t) energyCost;
}

void Handler::DeployVirtualMachine(const uint16_t &purchasedServerIdx, const Request &req, const LOCATION &location) {
    PurchasedServer &purchasedServer = purchasedServers[purchasedServerIdx];
    uint16_t purchasedServerId = purchasedServer.id;
    uint16_t vmIdx = req.vmIdx;
    int vmId = req.vmId;
    uint32_t addIdx = req.addIdx;
    VirtualMachine &vm = virtualMachines[vmIdx];

    auto *deployedVM = new DeployedVirtualMachine();
    auto *requestResult = new RequestResult();
    uint32_t n = deployedVMs.size();
    uint16_t cpuCore = vm.cpuCore;
    uint16_t memorySize = vm.memorySize;

    switch (location) {
        case NODE_A:
            purchasedServer.remainCpuCoreA -= cpuCore;
            purchasedServer.remainMemorySizeA -= memorySize;
            deployedVM->location = true;
            requestResult->location = NODE_A;
            break;
        case NODE_B:
            purchasedServer.remainCpuCoreB -= cpuCore;
            purchasedServer.remainMemorySizeB -= memorySize;
            deployedVM->location = false;
            requestResult->location = NODE_B;
            break;
        case ALL:
            purchasedServer.remainCpuCoreA -= cpuCore >> 1u;
            purchasedServer.remainCpuCoreB -= cpuCore >> 1u;
            purchasedServer.remainMemorySizeA -= memorySize >> 1u;
            purchasedServer.remainMemorySizeB -= memorySize >> 1u;
            requestResult->location = ALL;
            break;
        default:
            return;
    }

    ++deployedVMNum;
    idDeployedVMMap[vmId] = n;
    purchasedServer.deployedVM.emplace(n);
    deployedVM->purchasedServerId = purchasedServerId;
    deployedVM->vmIdx = vmIdx;
    deployedVM->vmId = vmId;
    deployedVMs.emplace_back(*deployedVM);
    requestResult->purchasedServerId = purchasedServerId;
    result[today].requestResult[addIdx] = *requestResult;
}
