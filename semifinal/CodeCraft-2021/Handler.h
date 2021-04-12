/*
 * @author: shenke
 * @date: 2021/3/30
 * @project: HuaWei_CodeCraft_2021
 * @desp:
 */

#ifndef HUAWEI_CODECRAFT_2021_HANDLER_H
#define HUAWEI_CODECRAFT_2021_HANDLER_H

#include "Statement.h"


class Handler {
public:
    Handler();

    void Init();

    void Input();

    void Handle();

    void Release();

private:
    void InputServer();

    void InputVirtualMachine();

    void InputRequest(const uint16_t &startDay, const uint16_t &endDay);

    void Output(const uint16_t &startDay, const uint16_t &endDay);

    void Handle(const uint16_t &startDay, const uint16_t &endDay);

    void Migrate();

    void HandleRequests();

    void IncreasePurchaseNumber();

    uint16_t Purchase(const VirtualMachine &vm);

    void Migrate(PurchasedServer &originPurchasedServer, const LOCATION &originLocation,
                 PurchasedServer &destPurchasedServer, const LOCATION &destLocation, const uint32_t &deployedVMIdx);

    LOCATION Migrate(PurchasedServer &originPurchasedServer, PurchasedServer &destPurchasedServer, const uint32_t &deployedVMIdx);

    void ReMapPurchasedServerIdx();

    void AddVirtualMachine(const Request &req);

    void DeployVirtualMachine(const uint16_t &purchasedServerIdx, const Request &req, const LOCATION &location);

    void DeleteVirtualMachine(const int &vmId);

    void SortRequest(const uint16_t &day);

    void SortDeployedVM(std::vector<uint32_t> &deployedVMIndexes);

    static LOCATION CheckCapacity(const PurchasedServer &purchasedServer, const VirtualMachine &vm);

    static bool CheckCapacity(const PurchasedServer &purchasedServer, const VirtualMachine &vm, const LOCATION &location);

    static inline bool CheckCapacity(const Server &server, const VirtualMachine &vm);

    uint8_t CalculateVacancyRate(const PurchasedServer &purchasedServer);

    uint32_t CalculatePurchaseValue(const PurchasedServer &purchasedServer, const VirtualMachine &vm, const LOCATION &location);

    uint32_t CalculateMigrateValue(const PurchasedServer &purchasedServer, const VirtualMachine &vm, const LOCATION &location);

    uint32_t CalculateDeployValue(const PurchasedServer &purchasedServer, const VirtualMachine &vm, const LOCATION &location);

    inline uint32_t CalculatePurchaseValue(uint16_t cpuCore, uint16_t memorySize, uint16_t energyCost) const;

    inline uint32_t CalculateMigrateValue(uint16_t cpuCore, uint16_t memorySize, uint16_t energyCost) const;

    inline uint32_t CalculateDeployValue(uint16_t cpuCore, uint16_t memorySize, uint16_t energyCost) const;

    inline void OutputLog(const string &log);


    static const uint8_t MAX_N = 100u;
    static const uint16_t MAX_M = 1000u;
    static const uint16_t MAX_T = 1000u;
    static const uint16_t MAX_CPU_CORE = 1024u;
    static const uint16_t MAX_MEMORY_SIZE = 1024u;
    static const uint8_t MAX_SERVER_MODEL_LENGTH = 20u;
    static const uint32_t MAX_REQUEST_NUM = 100000u;
    static const uint8_t MAX_RETRY_COUNT = 18u;
    static const uint16_t MAX_PURCHASED_SERVER_NUM = 10000u;

    // purchase parameters
    uint32_t purchaseCpuCoreParameter{75u};
    uint32_t purchaseMemorySizeParameter{25u};
    uint32_t purchaseEnergyCostParameter{0u};

    // migrate parameters
    uint32_t migrateCpuCoreParameter{75u};
    uint32_t migrateMemorySizeParameter{25u};
    uint32_t migrateEnergyCostParameter{0u};

    // deploy parameters
    uint32_t deployCpuCoreParameter{75u};
    uint32_t deployMemorySizeParameter{25u};
    uint32_t deployEnergyCostParameter{0u};


    uint8_t N{0u};
    uint16_t M{0u};
    uint16_t T{0u};
    uint16_t K{0u};

    std::vector<Server> servers;
    std::vector<VirtualMachine> virtualMachines;
    std::vector<std::vector<Request>> requests;
    std::vector<uint32_t> addRequestCounts;

    std::unordered_map<string, uint16_t> virtualMachineMap;
    std::unordered_map<int, uint32_t> idDeployedVMMap;  // vmId -> deployedVMs index
    std::unordered_map<int, uint32_t> idVirtualMachineMap;  // vmId -> virtualMachines index
    uint16_t purchasedServerIdxMap[MAX_PURCHASED_SERVER_NUM];   // purchasedServer id -> purchasedServers index
    uint32_t values[MAX_REQUEST_NUM];    // purchasedServer values

    std::vector<PurchasedServer> purchasedServers;
    std::vector<DeployedVirtualMachine> deployedVMs;

    uint16_t today{0u};
    uint32_t deployedVMNum{0u};
    uint16_t totalPurchasedServerNum{0u};

    std::vector<Result> result;

#ifdef TEST
    std::ofstream LOG;

    std::chrono::system_clock::time_point start;
    uint32_t totalMigrationNum{0u};
    uint64_t totalHardwareCost{0u};
    uint64_t totalEnergyCost{0u};
#endif
};

#endif //HUAWEI_CODECRAFT_2021_HANDLER_H
