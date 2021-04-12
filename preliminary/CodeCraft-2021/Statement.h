/*
 * @author: shenke
 * @date: 2021/3/12
 * @project: HuaWei_CodeCraft_2021
 * @desp:
 */


#ifndef HUAWEI_CODECRAFT_2021_STATEMENT_H
#define HUAWEI_CODECRAFT_2021_STATEMENT_H

#include <bits/stdc++.h>

#define TEST

#ifdef TEST
#define PrintLog(...)           \
    printf(__VA_ARGS__)
#else
#define PrintLog(...)
#endif


using std::string;


typedef enum {
    NONE, NODE_A, NODE_B, ALL
} LOCATION;

struct Server {
    string model{};
    uint16_t cpuCore{};
    uint16_t memorySize{};
    uint32_t hardwareCost{};
    uint16_t energyCost{};

    Server() = default;

    Server(string model, uint16_t cpuCore, uint16_t memorySize, uint32_t hardwareCost, uint16_t energyCost) :
            model(std::move(model)), cpuCore(cpuCore), memorySize(memorySize), hardwareCost(hardwareCost), energyCost(energyCost) {}

    string ToString() const {
        return "model: " + model + ", cpuCore: " + std::to_string(cpuCore) + ", memorySize: " + std::to_string(memorySize)
               + ", hardwareCost: " + std::to_string(hardwareCost) + ", energyCost: " + std::to_string(energyCost);
    }
};

struct VirtualMachine {
    string model{};
    uint16_t cpuCore{};
    uint16_t memorySize{};
    bool nodeType{}; // 1: 双节点, 0: 单节点

    VirtualMachine() = default;

    VirtualMachine(string model, uint16_t cpuCore, uint16_t memorySize, bool nodeType) :
            model(std::move(model)), cpuCore(cpuCore), memorySize(memorySize), nodeType(nodeType) {}

    string ToString() const {
        return "model: " + model + ", cpuCore: " + std::to_string(cpuCore) + ", memorySize: " + std::to_string(memorySize)
               + ", nodeType: " + std::to_string(nodeType);
    }
};

struct Request {
    bool add{}; // 1: add, 0: del
    uint32_t addIdx{};
    uint16_t vmIdx{};
    int vmId{};

    Request() = default;

    Request(bool add, uint32_t addIdx, uint16_t vmIdx, int vmId) : add(add), addIdx(addIdx), vmIdx(vmIdx), vmId(vmId) {}

    string ToString() const {
        return "add: " + std::to_string(add) + ", addIdx: " + std::to_string(add) + ", vmIdx: " + std::to_string(vmIdx) + ", vmId: " +
               std::to_string(vmId);
    }
};

struct PurchasedServer {
    uint16_t id{};
    uint8_t serverIdx{};
    uint16_t remainCpuCoreA{};
    uint16_t remainCpuCoreB{};
    uint16_t remainMemorySizeA{};
    uint16_t remainMemorySizeB{};
    double_t vacancyRate{};
    std::unordered_set<uint32_t> deployedVM{};

    PurchasedServer() = default;

    PurchasedServer(uint16_t id, uint8_t serverIdx, uint16_t cpuCore, uint16_t memorySize) :
            id(id), serverIdx(serverIdx), remainCpuCoreA(cpuCore >> 1u), remainCpuCoreB(cpuCore >> 1u),
            remainMemorySizeA(memorySize >> 1u), remainMemorySizeB(memorySize >> 1u) {}

    string ToString() const {
        return "id: " + std::to_string(id) + ", serverIdx: " + std::to_string(serverIdx)
               + ", remainCpuCoreA: " + std::to_string(remainCpuCoreA) + ", remainCpuCoreB: " + std::to_string(remainCpuCoreB)
               + ", remainMemorySizeA: " + std::to_string(remainMemorySizeA) + ", remainMemorySizeB: " + std::to_string(remainMemorySizeB)
               + ", vacancyRate: " + std::to_string(vacancyRate);
    }
};

struct DeployedVirtualMachine {
    uint16_t purchasedServerId{};
    uint16_t vmIdx{};
    int vmId{};
    bool location{};    // 1: A, 0: B

    DeployedVirtualMachine() = default;

    DeployedVirtualMachine(uint16_t purchasedServerId, uint16_t vmIdx, int vmId, bool location) :
            purchasedServerId(purchasedServerId), vmIdx(vmIdx), vmId(vmId), location(location) {}
};

struct ExtendResult {
    uint8_t serverIdx{};
    uint16_t purchaseNum{};

    ExtendResult(uint8_t serverIdx, uint16_t purchasedNum) : serverIdx(serverIdx), purchaseNum(purchasedNum) {}
};

struct MigrateResult {
    int vmId{};
    uint16_t purchasedServerId{};
    LOCATION location{NONE};

    MigrateResult(int vmId, uint16_t purchasedServerId, LOCATION location) : vmId(vmId), purchasedServerId(purchasedServerId), location(location) {}
};

struct RequestResult {
    uint16_t purchasedServerId{};
    LOCATION location{NONE};

    RequestResult() = default;

    RequestResult(uint16_t purchasedServerId, LOCATION location) : purchasedServerId(purchasedServerId), location(location) {}
};

struct Result {
    std::vector<ExtendResult> extendResult;
    std::vector<MigrateResult> migrateResult;
    std::vector<RequestResult> requestResult;
};


#endif //HUAWEI_CODECRAFT_2021_STATEMENT_H
