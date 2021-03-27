/*
 * @author: shenke
 * @date: 2021/3/12
 * @project: HuaWei_CodeCraft_2021
 * @desp:
 */

#ifndef HUAWEI_CODECRAFT_2021_HANDLER_H
#define HUAWEI_CODECRAFT_2021_HANDLER_H

#include "Statement.h"


class Handler {
public:
    Handler();

    void Input();

    void Handle();

    void Output();

private:
    void InputServer();

    void InputVirtualMachine();

    void InputRequest();

    void Init();

    void Expand(const bool &retry, const uint16_t &retryCount);

    void Migrate();

    void HandleRequests();

    void Purchase(const uint8_t &serverIdx);

    LOCATION Migrate(PurchasedServer &originPurchasedServer, PurchasedServer &destPurchasedServer, const uint32_t &deployedVMIdx);

    void ReMapPurchasedServerIdx();

    void SortDeployedVM(std::vector<uint32_t> &deployedVM);

    uint16_t AddVirtualMachine(const Request &req);

    bool DeployVirtualMachine(const uint16_t &purchasedServerIdx, const Request &req);

    void DeleteVirtualMachine(int id);

    LOCATION CheckCapacity(const PurchasedServer &purchasedServer, const VirtualMachine &vm);

    inline double_t CalculateScale(double_t memorySize, double_t cpuCore);

    const uint8_t MAX_N = 100;
    const uint16_t MAX_M = 1000;
    const uint16_t MAX_T = 1000;
    const uint16_t MAX_CPU_CORE = 1024;
    const uint16_t MAX_MEMORY_SIZE = 1024;
    const uint8_t MAX_SERVER_MODEL_LENGTH = 20;
    const uint32_t MAX_REQUEST_NUM = 100000;
    const uint16_t MAX_RETRY_COUNT = 1000;
    double_t candidateServerInterval = 0.1;

    uint8_t N;
    uint16_t M;
    uint16_t T;

    std::vector<Server> servers;
    std::vector<VirtualMachine> virtualMachines;
    std::vector<std::vector<Request>> requests;
    std::vector<uint32_t> addRequestCounts;
    Demand demand;

    std::unordered_map<string, uint16_t> virtualMachineMap;
    std::unordered_map<int, uint32_t> idDeployedVMMap;  // vmId -> deployedVMs index
    std::unordered_map<int, uint32_t> idVirtualMachineMap;  // vmId -> virtualMachines index
    std::unordered_map<uint16_t, uint16_t> purchasedServerIdxMap;  // purchasedServer id -> purchasedServers index

    std::vector<uint8_t> candidateServers;
    std::vector<PurchasedServer> purchasedServers;
    std::vector<DeployedVirtualMachine> deployedVMs;

    uint16_t today{0u};
    uint32_t deployedVMNum{0u};
    int todayCandidateServerIdx{-1};

    std::vector<Result> result;

#ifdef TEST
    uint32_t totalMigrationNum{0u};
    uint64_t totalHardwareCost{0u};
    uint64_t totalEnergyCost{0u};
#endif
};

#endif //HUAWEI_CODECRAFT_2021_HANDLER_H
