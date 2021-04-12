/*
 * @author: shenke
 * @date: 2021/3/30
 * @project: HuaWei_CodeCraft_2021
 * @desp:
 */

#include "Handler.h"

Handler::Handler() {
}

void Handler::Init() {
#ifdef TEST
    // 清空 log.txt 和 output.txt
    struct stat output{};
    if (stat(OUTPUT_FILE, &output) == 0) {
        std::fstream file(OUTPUT_FILE, std::ios::out);
    }

    struct stat log{};
    if (stat(LOG_FILE, &log) == 0) {
        std::fstream file(LOG_FILE, std::ios::out);
    }

    LOG.open(LOG_FILE, std::ios::app);
    freopen(INPUT_FILE, "r", stdin);
    freopen(OUTPUT_FILE, "w", stdout);

    start = std::chrono::system_clock::now();
#endif
}

void Handler::Input() {
    InputServer();
    InputVirtualMachine();
}

void Handler::Handle() {
    // 将 servers 按照 energyCost 的和进行升序排列
    std::sort(servers.begin(), servers.end(), [](const Server &a, const Server &b) {
        return a.energyCost < b.energyCost;
    });

    string TStr, KStr;
    std::cin >> TStr >> KStr;
    T = (uint16_t) std::stoi(TStr);
    K = (uint16_t) std::stoi(KStr);
    requests.resize(T);
    addRequestCounts.resize(T);
    result.resize(T);

    // 输入 k 天
    InputRequest(0u, K);

    uint16_t n = T - K;
    for (uint16_t i = 0u; i < n; ++i) {
        // 处理 1 天
        Handle(i, i + 1u);

        // 输出 1 天
        Output(i, i + 1u);

        // 输入 1 天
        InputRequest(K + i, K + i + 1u);
    }

    // 最后处理第 T - K + 1 天到第 T 天
    Handle(n, T);

    // 最后输出第 T - K + 1 天到第 T 天
    Output(n, T);
}

void Handler::Release() {
#ifdef TEST
    OutputLog("Hardware cost: " + std::to_string(totalHardwareCost));
    OutputLog("Energy cost: " + std::to_string(totalEnergyCost));
    OutputLog("Total cost: " + std::to_string(totalHardwareCost + totalEnergyCost));
    OutputLog("Total migration num: " + std::to_string(totalMigrationNum));

    std::chrono::duration<double, std::milli> duration = std::chrono::system_clock::now() - start;
    OutputLog("Handle elapsed time: " + std::to_string(duration.count()) + "ms");

    fclose(stdin);
    fclose(stdout);
    LOG.close();
#endif
}

void Handler::InputServer() {
    string NStr, modelStr, cpuCoreStr, memorySizeStr, hardwareCostStr, energyCostStr;
    uint16_t cpuCore;
    uint16_t memorySize;
    uint32_t hardwareCost;
    uint16_t energyCost;

    std::cin >> NStr;
    N = (uint8_t) std::stoi(NStr);
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

        server->scale = (double_t) memorySize / (double_t) cpuCore;
        servers[i] = *server;
    }
}

void Handler::InputVirtualMachine() {
    string MStr, modelStr, cpuCoreStr, memorySizeStr, nodeNumStr;
    uint16_t cpuCore;
    uint16_t memorySize;

    std::cin >> MStr;
    M = (uint16_t) std::stoi(MStr);
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
}

void Handler::InputRequest(const uint16_t &startDay, const uint16_t &endDay) {
    string RStr;
    uint16_t vmIdx;

    for (uint16_t i = startDay; i < endDay; ++i) {
        uint32_t R;
        string operationStr, modelStr, idStr;
        std::cin >> RStr;
        R = (uint16_t) std::stoi(RStr);
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
}

void Handler::Output(const uint16_t &startDay, const uint16_t &endDay) {
    for (uint16_t i = startDay; i < endDay; ++i) {
        const Result &res = result[i];

        // 扩容结果
        const std::vector<ExtendResult> &extendResult = res.extendResult;
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

    fflush(stdout);
}

void Handler::Handle(const uint16_t &startDay, const uint16_t &endDay) {
    for (uint16_t i = startDay; i < endDay; ++i) {
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
}

void Handler::Migrate() {
    uint32_t threshold = deployedVMNum * 3u / 100u;
    uint32_t migrateNum = 0u;
    bool ended = (migrateNum >= threshold);
    if (ended) return;

    // 将 purchasedServers 按照剩余容量升序排列
    std::sort(purchasedServers.begin(), purchasedServers.end(), [](const PurchasedServer &a, const PurchasedServer &b) {
        return a.vacancyRate != b.vacancyRate ? a.vacancyRate < b.vacancyRate :
               a.remainCpuCoreA + a.remainCpuCoreB + a.remainMemorySizeA + a.remainMemorySizeB <
               b.remainCpuCoreA + b.remainCpuCoreB + b.remainMemorySizeA + b.remainMemorySizeB;;
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
    SortRequest(today);
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

    LOCATION destLocation = CheckCapacity(destPurchasedServer, vm);
    if (destLocation == NONE) return NONE;

    // 双节点
    if (vm.nodeType && destLocation == ALL) {
        Migrate(originPurchasedServer, ALL, destPurchasedServer, ALL, deployedVMIdx);
        return ALL;
    }

    // 单节点
    bool flag = destPurchasedServer.remainCpuCoreA + destPurchasedServer.remainMemorySizeA > destPurchasedServer.remainCpuCoreB + destPurchasedServer.remainMemorySizeB;
    if (destLocation == NODE_A || (destLocation == ALL && flag)) {
        Migrate(originPurchasedServer, deployedVM.location ? NODE_A : NODE_B, destPurchasedServer, NODE_A, deployedVMIdx);
        return NODE_A;
    }

    if (destLocation == NODE_B || (destLocation == ALL)) {
        Migrate(originPurchasedServer, deployedVM.location ? NODE_A : NODE_B, destPurchasedServer, NODE_B, deployedVMIdx);
        return NODE_B;
    }

    return NONE;
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

    // 删除 originPurchasedServer 中的 deployedVM
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
    originPurchasedServer.vacancyRate = CalculateVacancyRate(originPurchasedServer);

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
    destPurchasedServer.vacancyRate = CalculateVacancyRate(destPurchasedServer);

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
            values[i] = CalculateDeployValue(purchasedServers[i], vm, ALL);
        }

        uint32_t minValue = 0xffffffff;
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
            values[(i << 1u)] = CalculateDeployValue(purchasedServers[i], vm, NODE_A);
            values[(i << 1u) + 1u] = CalculateDeployValue(purchasedServers[i], vm, NODE_B);
        }

        uint32_t minValue = 0xffffffff;
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
    purchasedServer.vacancyRate = CalculateVacancyRate(purchasedServer);
    deployedVM->purchasedServerId = purchasedServerId;
    deployedVM->vmIdx = vmIdx;
    deployedVM->vmId = vmId;
    deployedVMs.emplace_back(*deployedVM);
    requestResult->purchasedServerId = purchasedServerId;
    result[today].requestResult[addIdx] = *requestResult;
}

uint16_t Handler::Purchase(const VirtualMachine &vm) {
    uint8_t serverIdx = N - 1u;
    for (uint8_t i = 0u; i < N; ++i) {
        Server &server = servers[i];
        if (CheckCapacity(server, vm)) {
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
    purchasedServer.vacancyRate = CalculateVacancyRate(purchasedServer);
    purchasedServer.deployedVM.erase(deployedVMIdx);
}

void Handler::SortRequest(const uint16_t &day) {
    // 将 requests 按照 cpu + mem 大小进行降序排列
    std::vector<Request> &request = requests[day];
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

void Handler::SortDeployedVM(std::vector<uint32_t> &deployedVMIndexes) {
    std::sort(deployedVMIndexes.begin(), deployedVMIndexes.end(), [this](const uint32_t &a, const uint32_t &b) {
        const DeployedVirtualMachine &deployedVM1 = deployedVMs[a];
        const DeployedVirtualMachine &deployedVM2 = deployedVMs[b];
        const VirtualMachine &vm1 = virtualMachines[deployedVM1.vmIdx];
        const VirtualMachine &vm2 = virtualMachines[deployedVM2.vmIdx];

        if (deployedVM1.location && !deployedVM2.location) return true;
        else if (!deployedVM1.location && deployedVM2.location) return false;
        else return vm1.cpuCore + vm1.memorySize > vm2.cpuCore + vm2.memorySize;
    });
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

uint8_t Handler::CalculateVacancyRate(const PurchasedServer &purchasedServer) {
    Server &server = servers[purchasedServer.serverIdx];
    uint16_t serverCpuCore = server.cpuCore;
    uint16_t serverMemorySize = server.memorySize;
    uint16_t cpuCore = purchasedServer.remainCpuCoreA + purchasedServer.remainCpuCoreB;
    uint16_t memorySize = purchasedServer.remainMemorySizeA + purchasedServer.remainMemorySizeB;
    // return (uint8_t) (100u * cpuCore / serverCpuCore + 100u * memorySize / serverMemorySize);
    return (uint8_t) ((100u * (12u * cpuCore + memorySize)) / (12u * serverCpuCore + serverMemorySize));
}

uint32_t Handler::CalculatePurchaseValue(const PurchasedServer &purchasedServer, const VirtualMachine &vm, const LOCATION &location) {
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
        return CalculatePurchaseValue(remainCpuCoreA + remainCpuCoreB, remainMemorySizeA + remainMemorySizeB, energyCost);
    }

    if (location == NODE_A && CheckCapacity(purchasedServer, vm, location)) {
        uint16_t remainCpuCoreA = purchasedServer.remainCpuCoreA - cpuCore;
        uint16_t remainMemorySizeA = purchasedServer.remainMemorySizeA - memorySize;
        return CalculatePurchaseValue(remainCpuCoreA, remainMemorySizeA, energyCost);
    }

    if (location == NODE_B && CheckCapacity(purchasedServer, vm, location)) {
        uint16_t remainCpuCoreB = purchasedServer.remainCpuCoreB - cpuCore;
        uint16_t remainMemorySizeB = purchasedServer.remainMemorySizeB - memorySize;
        return CalculatePurchaseValue(remainCpuCoreB, remainMemorySizeB, energyCost);
    }

    return 0;
}

uint32_t Handler::CalculateMigrateValue(const PurchasedServer &purchasedServer, const VirtualMachine &vm, const LOCATION &location) {
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
        return CalculateMigrateValue(remainCpuCoreA + remainCpuCoreB, remainMemorySizeA + remainMemorySizeB, energyCost);
    }

    if (location == NODE_A && CheckCapacity(purchasedServer, vm, location)) {
        uint16_t remainCpuCoreA = purchasedServer.remainCpuCoreA - cpuCore;
        uint16_t remainMemorySizeA = purchasedServer.remainMemorySizeA - memorySize;
        return CalculateMigrateValue(remainCpuCoreA, remainMemorySizeA, energyCost);
    }

    if (location == NODE_B && CheckCapacity(purchasedServer, vm, location)) {
        uint16_t remainCpuCoreB = purchasedServer.remainCpuCoreB - cpuCore;
        uint16_t remainMemorySizeB = purchasedServer.remainMemorySizeB - memorySize;
        return CalculateMigrateValue(remainCpuCoreB, remainMemorySizeB, energyCost);
    }

    return 0u;
}

uint32_t Handler::CalculateDeployValue(const PurchasedServer &purchasedServer, const VirtualMachine &vm, const LOCATION &location) {
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
        return CalculateDeployValue(remainCpuCoreA + remainCpuCoreB, remainMemorySizeA + remainMemorySizeB, energyCost);
    }

    if (location == NODE_A && CheckCapacity(purchasedServer, vm, location)) {
        uint16_t remainCpuCoreA = purchasedServer.remainCpuCoreA - cpuCore;
        uint16_t remainMemorySizeA = purchasedServer.remainMemorySizeA - memorySize;
        return CalculateDeployValue(remainCpuCoreA, remainMemorySizeA, energyCost);
    }

    if (location == NODE_B && CheckCapacity(purchasedServer, vm, location)) {
        uint16_t remainCpuCoreB = purchasedServer.remainCpuCoreB - cpuCore;
        uint16_t remainMemorySizeB = purchasedServer.remainMemorySizeB - memorySize;
        return CalculateDeployValue(remainCpuCoreB, remainMemorySizeB, energyCost);
    }

    return 0u;
}

inline uint32_t Handler::CalculatePurchaseValue(uint16_t cpuCore, uint16_t memorySize, uint16_t energyCost) const {
    return purchaseCpuCoreParameter * (uint32_t) cpuCore + purchaseMemorySizeParameter * (uint32_t) memorySize + purchaseEnergyCostParameter * (uint32_t) energyCost;
}

inline uint32_t Handler::CalculateMigrateValue(uint16_t cpuCore, uint16_t memorySize, uint16_t energyCost) const {
    return migrateCpuCoreParameter * (uint32_t) cpuCore + migrateMemorySizeParameter * (uint32_t) memorySize + migrateEnergyCostParameter * (uint32_t) energyCost;
}

inline uint32_t Handler::CalculateDeployValue(uint16_t cpuCore, uint16_t memorySize, uint16_t energyCost) const {
    return deployCpuCoreParameter * (uint32_t) cpuCore + deployMemorySizeParameter * (uint32_t) memorySize + deployEnergyCostParameter * (uint32_t) energyCost;
}

inline void Handler::OutputLog(const string &log) {
#ifdef TEST
    LOG << log << std::endl;
#endif
}
