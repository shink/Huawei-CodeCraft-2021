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

    void Solve();

    void Output();

private:
    void InputServer();

    void InputVirtualMachine();

    void InputRequest();

    void Init();

    void Expand(uint16_t day);

    void Migrate(uint16_t day);

    void HandleRequests(uint16_t day);

    void GatherAnswer();

    void SortServer(uint8_t left, uint8_t right, bool (*compare)(const Server &, const Server &));

    void Purchase(uint16_t startDay, uint16_t endDay);

    void Migrate(const VirtualMachine &vm, DeployedVirtualMachine &deployedVM, uint16_t destPurchasedServerIdx, bool destLocation);

    void SortDeployedVM();

    uint16_t AddVirtualMachine(uint16_t day, int id, const string &model);

    void DeployVirtualMachine(uint16_t purchasedServerIdx, uint16_t vmIdx, int vmId, const LOCATION &location);

    void DeleteVirtualMachine(int id);

    inline LOCATION CheckCapacity(const PurchasedServer &purchasedServer, const VirtualMachine &vm);

    inline LOCATION CheckCapacity(const PurchasedServer &purchasedServer);

    inline double_t CalculateScale(double_t memorySize, double_t cpuCore);

    const uint8_t MAX_N = 100;
    const uint16_t MAX_M = 1000;
    const uint16_t MAX_T = 1000;
    const uint16_t MAX_CPU_CORE = 1024;
    const uint16_t MAX_MEMORY_SIZE = 1024;
    const uint8_t MAX_SERVER_MODEL_LENGTH = 20;
    const uint32_t MAX_REQUEST_NUM = 100000;
    const uint16_t MAX_RETRY_COUNT = 1000;

    uint8_t N;
    uint16_t M;
    uint16_t T;

    std::unordered_map<string, uint8_t> serverMap;
    std::unordered_map<string, uint16_t> virtualMachineMap;
    std::unordered_map<int, uint32_t> idDeployedVMMap;  // id -> deployedVMs index
    std::unordered_map<int, uint32_t> idVirtualMachineMap;  // id -> virtualMachines index

    std::vector<Server> servers;
    std::vector<VirtualMachine> virtualMachines;
    std::vector<DeployedVirtualMachine> deployedVMs;
    std::vector<std::vector<Request>> requests;
    std::vector<PurchasedServer> purchasedServers;

    uint8_t serverIdx{0u};
    uint32_t deployedVMNum{0u};
    double_t serverMemoryCpuScale{0.0};
    double_t requestMemoryCpuScale{0.0};

    std::vector<string> migrationResult;
    std::vector<string> extendResult;
    std::vector<string> requestResult;
    std::vector<string> ans;

#ifdef TEST
    uint32_t totalMigrationNum{0u};
    uint64_t totalHardwareCost{0u};
    uint64_t totalEnergyCost{0u};
#endif
};

#endif //HUAWEI_CODECRAFT_2021_HANDLER_H
