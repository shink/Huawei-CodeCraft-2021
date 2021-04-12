/*
 * @author: shenke
 * @date: 2021/3/12
 * @project: HuaWei_CodeCraft_2021
 * @desp:
 */

#include "Handler.h"

Handler::Handler() {
}

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

    // 将 purchasedServers 按照空闲率降序排列
    std::sort(purchasedServers.begin(), purchasedServers.end(), [](const PurchasedServer &a, const PurchasedServer &b) {
        return a.vacancyRate > b.vacancyRate;
    });
    // 排序后需要重新映射 purchasedServer id -> purchasedServers index
    ReMapPurchasedServerIdx();
    // 将排好序的每个 purchasedServer 的 deployedVMIdx 顺序放入 vector
    uint32_t idx = 0u;
    std::vector<uint32_t> deployedVMIdxMap(deployedVMNum);
    for (const PurchasedServer &purchasedServer : purchasedServers) {
        for (const uint32_t &deployedVMIdx : purchasedServer.deployedVM) {
            deployedVMIdxMap[idx++] = deployedVMIdx;
        }
    }

    idx = 0u;
    while (migrateNum < threshold && idx < deployedVMNum) {
        uint32_t deployedVMIdx = deployedVMIdxMap[idx++];
        DeployedVirtualMachine &deployedVM = deployedVMs[deployedVMIdx];

        const VirtualMachine &vm = virtualMachines[deployedVM.vmIdx];
        PurchasedServer &originPurchasedServer = purchasedServers[purchasedServerIdxMap[deployedVM.purchasedServerId]];
        int size = (int) purchasedServers.size();

        if (vm.nodeType) {
            // 双节点迁移
            for (int i = 0; i < size; ++i) {
                values[i] = CalculatePurchasedServerValue(purchasedServers[i], vm, ALL);
            }
            double minValue = CalculatePurchasedServerRate(originPurchasedServer, ALL);
            int minValueIdx = -1;
            for (int i = 0; i < size; ++i) {
                if (values[i] > 0 && values[i] < minValue) {
                    minValue = values[i];
                    minValueIdx = i;
                }
            }
            if (minValueIdx != -1) {
                // 可以有更好的服务器来部署,则迁移
                // 排除原地迁移的情况
                PurchasedServer &destPurchasedServer = purchasedServers[minValueIdx];
                if (originPurchasedServer.id != destPurchasedServer.id) {
                    Migrate(originPurchasedServer, ALL, destPurchasedServer, ALL, deployedVMIdx);
                    ++migrateNum;
                }
            }
        } else {
            // 单节点迁移
            uint32_t val_size = (size << 1);
            // 计算每个服务器对应的指标值
            for (int i = 0; i < size; ++i) {
                PurchasedServer &purchasedServer = purchasedServers[i];
                values[(i << 1)] = CalculatePurchasedServerValue(purchasedServer, vm, NODE_A);
                values[(i << 1) + 1] = CalculatePurchasedServerValue(purchasedServer, vm, NODE_B);
            }
            double minValue = CalculatePurchasedServerRate(originPurchasedServer, ALL);
            int minValueIdx = -1;
            for (int i = 0; i < (size << 1); ++i) {
                if (values[i] > 0 && values[i] < minValue) {
                    minValue = values[i];
                    minValueIdx = i;
                }
            }
            if (minValueIdx != -1) {
                // 可以有更好的服务器来部署,则迁移
                // 排除原地迁移的情况
                PurchasedServer &destPurchasedServer = purchasedServers[minValueIdx >> 1];
                LOCATION originLocation = deployedVM.location ? NODE_A : NODE_B;
                LOCATION destLocation = (minValueIdx & 1) == 0 ? NODE_A : NODE_B;
                if (originPurchasedServer.id != destPurchasedServer.id && originLocation != destLocation) {
                    Migrate(originPurchasedServer, originLocation, destPurchasedServer, destLocation, deployedVMIdx);
                    ++migrateNum;
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

void Handler::Migrate(PurchasedServer &originPurchasedServer, const LOCATION &originLocation,
                      PurchasedServer &destPurchasedServer, const LOCATION &destLocation, const uint32_t &deployedVMIdx) {

    if (originLocation == NONE || destLocation == NONE) return;

    DeployedVirtualMachine &deployedVM = deployedVMs[deployedVMIdx];
    VirtualMachine &vm = virtualMachines[deployedVM.vmIdx];
    int vmId = deployedVM.vmId;
    uint16_t destPurchasedServerId = destPurchasedServer.id;

    uint16_t cpuCore = vm.cpuCore;
    uint16_t memorySize = vm.memorySize;
    uint16_t halfCpuCore = vm.cpuCore >> 1u;
    uint16_t halfMemorySize = vm.memorySize >> 1u;

    // 删除原 originPurchasedServer 中的 deployedVM
    if (originLocation == ALL) {
        originPurchasedServer.remainCpuCoreA += halfCpuCore;
        originPurchasedServer.remainCpuCoreB += halfCpuCore;
        originPurchasedServer.remainMemorySizeA += halfMemorySize;
        originPurchasedServer.remainMemorySizeB += halfMemorySize;
    } else if (originLocation == NODE_A) {
        originPurchasedServer.remainCpuCoreA += cpuCore;
        originPurchasedServer.remainMemorySizeA += memorySize;
    } else if (originLocation == NODE_B) {
        originPurchasedServer.remainCpuCoreB += cpuCore;
        originPurchasedServer.remainMemorySizeB += memorySize;
    }
    originPurchasedServer.deployedVM.erase(deployedVMIdx);
    originPurchasedServer.vacancyRate = CalculatePurchasedServerRate(originPurchasedServer, ALL);

    // 将 deployedVM 添加到 destPurchasedServer
    if (destLocation == ALL) {
        destPurchasedServer.remainCpuCoreA -= halfCpuCore;
        destPurchasedServer.remainCpuCoreB -= halfCpuCore;
        destPurchasedServer.remainMemorySizeA -= halfMemorySize;
        destPurchasedServer.remainMemorySizeB -= halfMemorySize;
    } else if (destLocation == NODE_A) {
        destPurchasedServer.remainCpuCoreA -= cpuCore;
        destPurchasedServer.remainMemorySizeA -= memorySize;
        deployedVM.location = true;
    } else if (destLocation == NODE_B) {
        destPurchasedServer.remainCpuCoreB -= cpuCore;
        destPurchasedServer.remainMemorySizeB -= memorySize;
        deployedVM.location = false;
    }
    destPurchasedServer.deployedVM.emplace(deployedVMIdx);
    destPurchasedServer.vacancyRate = CalculatePurchasedServerRate(destPurchasedServer, ALL);

    deployedVM.purchasedServerId = destPurchasedServerId;
    result[today].migrateResult.emplace_back(vmId, destPurchasedServerId, destLocation);
}

void Handler::ReMapPurchasedServerIdx() {
    uint16_t n = purchasedServers.size();
    for (uint16_t i = 0u; i < n; ++i) purchasedServerIdxMap[purchasedServers[i].id] = i;
}

void Handler::AddVirtualMachine(const Request &req) {
    uint16_t n = purchasedServers.size();
    const VirtualMachine &vm = virtualMachines[req.vmIdx];
    if (vm.nodeType) {
        // 双节点
        for (uint16_t i = 0u; i < n; ++i) {
            values[i] = CalculatePurchasedServerValue(purchasedServers[i], vm, ALL);
        }

        double_t minValue = 1e10;
        bool exist = false;
        uint16_t idx = 0u;
        // 找到不为 0 的所有参数中的最小的那个
        for (uint16_t i = 0u; i < n; ++i) {
            if (values[i] > 0 && values[i] < minValue) {
                minValue = values[i];
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
        for (uint16_t i = 0u; i < n; ++i) {
            values[(i << 1u)] = CalculatePurchasedServerValue(purchasedServers[i], vm, NODE_A);
            values[(i << 1u) + 1u] = CalculatePurchasedServerValue(purchasedServers[i], vm, NODE_B);
        }

        double_t minValue = 1e10;
        bool exist = false;
        uint16_t idx = 0u;
        // 找到不为 0 的所有参数中的最小的那个
        for (uint16_t i = 0u; i < (uint16_t) (n << 1u); ++i) {
            if (values[i] > 0 && values[i] < minValue) {
                minValue = values[i];
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
    purchasedServer.vacancyRate = CalculatePurchasedServerRate(purchasedServer, ALL);
    deployedVM->purchasedServerId = purchasedServerId;
    deployedVM->vmIdx = vmIdx;
    deployedVM->vmId = vmId;
    deployedVMs.emplace_back(*deployedVM);
    requestResult->purchasedServerId = purchasedServerId;
    result[today].requestResult[addIdx] = *requestResult;
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
    int resultIdx = 0, n = (int) extendResult.size();
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
    purchasedServer->vacancyRate = CalculatePurchasedServerRate(*purchasedServer, ALL);
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
    purchasedServer.vacancyRate = CalculatePurchasedServerRate(purchasedServer, ALL);
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

double_t Handler::CalculatePurchasedServerRate(const PurchasedServer &purchasedServer, const LOCATION &location) {
    switch (location) {
        case NODE_A:
            return CalculateValue(purchasedServer.remainCpuCoreA, purchasedServer.remainMemorySizeA);
        case NODE_B:
            return CalculateValue(purchasedServer.remainMemorySizeB, purchasedServer.remainMemorySizeB);
        case ALL:
            return CalculateValue(purchasedServer.remainCpuCoreA + purchasedServer.remainCpuCoreB, purchasedServer.remainMemorySizeA + purchasedServer.remainMemorySizeB);
        default:
            return 0;
    }
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
        return CalculateValue(remainCpuCoreA + remainCpuCoreB, remainMemorySizeA + remainMemorySizeB, energyCost);
    }

    if (location == NODE_A && CheckCapacity(purchasedServer, vm, location)) {
        uint16_t remainCpuCoreA = purchasedServer.remainCpuCoreA - cpuCore;
        uint16_t remainMemorySizeA = purchasedServer.remainMemorySizeA - memorySize;
        return CalculateValue(remainCpuCoreA, remainMemorySizeA, energyCost);
    }

    if (location == NODE_B && CheckCapacity(purchasedServer, vm, location)) {
        uint16_t remainCpuCoreB = purchasedServer.remainCpuCoreB - cpuCore;
        uint16_t remainMemorySizeB = purchasedServer.remainMemorySizeB - memorySize;
        return CalculateValue(remainCpuCoreB, remainMemorySizeB, energyCost);
    }

    return 0;
}

inline double_t Handler::CalculateValue(uint16_t cpuCore, uint16_t memorySize) {
    return 0.75f * (double_t) cpuCore + 0.25f * (double_t) memorySize;
}

inline double_t Handler::CalculateValue(uint16_t cpuCore, uint16_t memorySize, uint16_t energyCost) {
//     return 0.75f * (double_t) cpuCore + 0.25f * (double_t) memorySize;
    return 0.75f * (double_t) cpuCore + 0.23f * (double_t) memorySize + 0.02f * (double_t) energyCost;
}
