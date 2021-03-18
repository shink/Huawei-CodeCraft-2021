/*
 * @author: shenke
 * @date: 2021/3/12
 * @project: HuaWei_CodeCraft_2021
 * @desp:
 */

#ifndef HUAWEI_CODECRAFT_2021_SOLVER_H
#define HUAWEI_CODECRAFT_2021_SOLVER_H

#include "Statement.h"


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

    string ToString() {
        return "type: " + type + ", cpuCore: " + std::to_string(cpuCore) + ", memorySize: " + std::to_string(memorySize)
               + ", hardwareCost: " + std::to_string(hardwareCost) + ", energyCost: " + std::to_string(energyCost);
    }
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

    string ToString() const {
        return "add: " + std::to_string(add) + ", type: " + type + ", id: " + std::to_string(id);
    }
};

struct PurchasedServer {
    uint8_t serverId;
    uint16_t remainCpuCoreA{};
    uint16_t remainCpuCoreB{};
    uint16_t remainMemorySizeA{};
    uint16_t remainMemorySizeB{};

    PurchasedServer(uint8_t serverId, uint16_t remainCpuCoreA, uint16_t remainCpuCoreB, uint16_t remainMemorySizeA, uint16_t remainMemorySizeB) :
            serverId(serverId), remainCpuCoreA(remainCpuCoreA), remainCpuCoreB(remainCpuCoreB),
            remainMemorySizeA(remainMemorySizeA), remainMemorySizeB(remainMemorySizeB) {}
};

struct DeployedVirtualMachine {
    uint32_t purchasedServerId{};
    uint16_t vmId{};
    bool location{}; // 1: A, 0: B
};


class Solver {
public:
    Solver();

    void Init();

    void Input();

    void Solve();

    void Output();

    ~Solver();

private:
    void InputServer();

    void InputVirtualMachine();

    void InputRequest();

    void SortServer(uint8_t left, uint8_t right);

    inline bool CompareServer(const Server &server1, const Server &server2);

    uint16_t AddVirtualMachine(uint16_t day, int id, const string &type);

    void DeleteVirtualMachine(int id);

    bool CheckCapacity(uint16_t day);

    uint16_t PurchaseServer();

    void Expand(uint8_t num);

    void Migrate(uint8_t num);

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

    std::vector<PurchasedServer> purchasedServers;
    std::vector<DeployedVirtualMachine> deployedVMs;
    std::vector<string> res;
    std::vector<string> ans;

#ifdef TEST
    uint64_t cost = 0u;
#endif
};

#endif //HUAWEI_CODECRAFT_2021_SOLVER_H
