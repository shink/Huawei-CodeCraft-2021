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
    double_t scale{};

    Server() = default;

    Server(string model, uint16_t cpuCore, uint16_t memorySize, uint32_t hardwareCost, uint16_t energyCost, double_t scale) :
            model(std::move(model)), cpuCore(cpuCore), memorySize(memorySize), hardwareCost(hardwareCost), energyCost(energyCost),
            scale(scale) {}

    string ToString() const {
        return "model: " + model + ", cpuCore: " + std::to_string(cpuCore) + ", memorySize: " + std::to_string(memorySize)
               + ", totalHardwareCost: " + std::to_string(hardwareCost) + ", totalEnergyCost: " + std::to_string(energyCost)
               + ", scale: " + std::to_string(scale);
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
    string model{};
    int id{};

    Request() = default;

    Request(bool add, string model, int id) : add(add), model(std::move(model)), id(id) {}

    string ToString() const {
        return "add: " + std::to_string(add) + ", model: " + model + ", id: " + std::to_string(id);
    }
};

struct PurchasedServer {
    bool type{};
    uint8_t serverIdx{};
    uint16_t remainCpuCoreA{};
    uint16_t remainCpuCoreB{};
    uint16_t remainMemorySizeA{};
    uint16_t remainMemorySizeB{};
    double_t remainScaleA{};
    double_t remainScaleB{};
    std::vector<uint32_t> deployedVMA{};
    std::vector<uint32_t> deployedVMB{};

    PurchasedServer() = default;

    PurchasedServer(bool type, uint8_t serverIdx, uint16_t cpuCore, uint16_t memorySize, double_t remainScaleA, double_t remainScaleB) :
            type(type), serverIdx(serverIdx), remainCpuCoreA(cpuCore >> 1u), remainCpuCoreB(cpuCore >> 1u),
            remainMemorySizeA(memorySize >> 1u), remainMemorySizeB(memorySize >> 1u),
            remainScaleA(remainScaleA), remainScaleB(remainScaleB) {}

    string ToString() const {
        return "type: " + std::to_string(type) + ", serverIdx: " + std::to_string(serverIdx)
               + ", remainCpuCoreA: " + std::to_string(remainCpuCoreA) + ", remainCpuCoreB: " + std::to_string(remainCpuCoreB)
               + ", remainMemorySizeA: " + std::to_string(remainMemorySizeA) + ", remainMemorySizeB: " + std::to_string(remainMemorySizeB)
               + ", remainScaleA: " + std::to_string(remainScaleA) + ", remainScaleB: " + std::to_string(remainScaleB);
    }
};

struct DeployedVirtualMachine {
    uint32_t purchasedServerIdx{};
    uint16_t vmIdx{};
    int vmId{};
    bool location{}; // 1: A, 0: B

    DeployedVirtualMachine() = default;

    DeployedVirtualMachine(uint32_t purchasedServerIdx, uint16_t vmIdx, int vmId, bool location) :
            purchasedServerIdx(purchasedServerIdx), vmIdx(vmIdx), vmId(vmId), location(location) {}
};

struct Quantity {
    int cpuCore{};
    int memorySize{};

    Quantity() = default;

    Quantity(uint32_t cpuCore, uint32_t memorySize) : cpuCore(cpuCore), memorySize(memorySize) {}
};


#endif //HUAWEI_CODECRAFT_2021_STATEMENT_H
