#include <sdk.hpp>
#include <Server/Components/Pawn/pawn.hpp>

#include "../../src/natives.hpp"

extern void* pAMXFunctions;

namespace {

constexpr UID kPawnMetricsComponentUID = UID(0x5061776e4d747263ULL); // "PawnMtrc"

class PawnMetricsComponent final
    : public IComponent
    , public PawnEventHandler
{
public:
    PROVIDE_UID(kPawnMetricsComponentUID)

    StringView componentName() const override
    {
        return "pawn_metrics";
    }

    SemanticVersion componentVersion() const override
    {
        return SemanticVersion(0, 2, 0, 0);
    }

    void onLoad(ICore* c) override
    {
        core_ = c;
        if (core_ != nullptr) {
            core_->printLn("[pawn-metrics] component loaded (native open.mp component mode)");
        }
    }

    void onInit(IComponentList* components) override
    {
        pawn_ = components->queryComponent<IPawnComponent>();
        if (pawn_ != nullptr) {
            pAMXFunctions = const_cast<void*>(static_cast<const void*>(pawn_->getAmxFunctions().data()));
            pawn_->getEventDispatcher().addEventHandler(this);
        } else if (core_ != nullptr) {
            core_->logLn(LogLevel::Warning, "[pawn-metrics] Pawn component not available; Metrics_* natives cannot be registered");
        }
    }

    void onReady() override
    {
    }

    void onFree(IComponent* component) override
    {
        if (component == pawn_) {
            pawn_ = nullptr;
        }
    }

    void free() override
    {
        stop_pawn_metrics();

        if (pawn_ != nullptr) {
            pawn_->getEventDispatcher().removeEventHandler(this);
            pawn_ = nullptr;
        }

        if (core_ != nullptr) {
            core_->printLn("[pawn-metrics] component unloaded");
            core_ = nullptr;
        }

        pAMXFunctions = nullptr;
    }

    void reset() override
    {
    }

    // --- PawnEventHandler ---
    void onAmxLoad(IPawnScript& script) override
    {
        AMX* amx = script.GetAMX();
        if (amx != nullptr) {
            amx_Register(amx, get_pawn_metrics_natives(), -1);
        }
        script.Register(get_pawn_metrics_natives(), static_cast<int>(get_pawn_metrics_native_count()));
    }

    void onAmxUnload(IPawnScript&) override
    {
    }

private:
    ICore* core_ = nullptr;
    IPawnComponent* pawn_ = nullptr;
};

PawnMetricsComponent g_component;

} // namespace

COMPONENT_ENTRY_POINT()
{
    return &g_component;
}
