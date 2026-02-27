#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_PLUGIN_BIRDEYE_SRC_TYPES_API_TRADER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_PLUGIN_BIRDEYE_SRC_TYPES_API_TRADER_H
#include "core.hpp"

class GainersLosersParams;
class GainersLosersResponse;
class TraderTransactionsSeekParams;
class TraderTransactionsSeekResponse;

class GainersLosersParams : public object, public std::enable_shared_from_this<GainersLosersParams> {
public:
    using std::enable_shared_from_this<GainersLosersParams>::shared_from_this;
    any type;

    string sort_by;

    any sort_type;

    double offset;

    double limit;
};

class GainersLosersResponse : public object, public std::enable_shared_from_this<GainersLosersResponse> {
public:
    using std::enable_shared_from_this<GainersLosersResponse>::shared_from_this;
    boolean success;

    object data;
};

class TraderTransactionsSeekParams : public object, public std::enable_shared_from_this<TraderTransactionsSeekParams> {
public:
    using std::enable_shared_from_this<TraderTransactionsSeekParams>::shared_from_this;
    string address;

    double offset;

    double limit;

    any tx_type;

    double before_time;

    double after_time;
};

class TraderTransactionsSeekResponse : public object, public std::enable_shared_from_this<TraderTransactionsSeekResponse> {
public:
    using std::enable_shared_from_this<TraderTransactionsSeekResponse>::shared_from_this;
    boolean success;

    object data;
};

#endif
