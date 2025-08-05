# AutoFun IDL C++ Implementation

This module provides C++ bindings for the auto.fun protocol Interface Definition Language (IDL). It enables C++ applications to interact with the auto.fun smart contract on Solana by providing type-safe structures and serialization utilities.

## Features

- **Type Definitions**: Complete C++ structures matching the auto.fun IDL
- **JSON Support**: Serialization/deserialization to/from JSON using nlohmann::json
- **Binary Support**: Binary serialization using Borsh-like format  
- **Instruction Building**: Helper functions to build Solana instructions
- **Validation**: Pubkey validation and other utility functions
- **Template Support**: Generic AmountConfig supporting Range and Enum variants

## Core Types

### Config
Configuration structure for the auto.fun platform:
```cpp
struct Config {
    Pubkey authority;
    Pubkey pending_authority;
    Pubkey team_wallet;
    f64 init_bonding_curve;
    u128 platform_buy_fee;
    u128 platform_sell_fee;
    u64 curve_limit;
    AmountConfig<u64> lamport_amount_config;
    AmountConfig<u64> token_supply_config;
    AmountConfig<u8> token_decimals_config;
};
```

### BondingCurve
Represents token economics and bonding curve state:
```cpp
struct BondingCurve {
    Pubkey token_mint;
    Pubkey creator;
    u64 init_lamport;
    u64 reserve_lamport;
    u64 reserve_token;
    u64 curve_limit;
    bool is_completed;
};
```

### AmountConfig
Generic configuration supporting both Range and Enum variants:
```cpp
// Range example
AmountConfig<u64> range_config(1000000ULL, 100000000ULL);

// Enum example  
std::vector<u8> decimals = {6, 8, 9, 18};
AmountConfig<u8> enum_config(decimals);
```

## Usage Examples

### JSON Serialization
```cpp
#include "elizaos/autofun_idl.hpp"
using namespace elizaos::autofun_idl;

// Create and configure a Config
Config config;
config.authority = "9WzDXwBbmkg8ZTbNMqUxvQRAyrZzDsGYdLVL9zYtAWWM";
config.init_bonding_curve = 0.85;

// Serialize to JSON
std::string json = IdlProcessor::configToJson(config);

// Deserialize from JSON
Config parsed = IdlProcessor::parseConfigFromJson(json);
```

### Binary Serialization
```cpp
// Serialize to binary
std::vector<u8> binary_data = IdlProcessor::serializeBondingCurve(curve);

// Deserialize from binary
BondingCurve deserialized = IdlProcessor::deserializeBondingCurve(binary_data);
```

### Instruction Building
```cpp
// Build accept authority instruction
std::string new_admin = "DjVE6JNiYqPL2QXyCUUh8rNjHrbz9hXHNYt99MQ59qw1";
std::string global_config = "11111111111111111111111111111111";

Instruction instr = IdlProcessor::buildAcceptAuthorityInstruction(new_admin, global_config);
```

### Validation
```cpp
// Validate pubkey format
bool valid = IdlProcessor::validatePubkey("9WzDXwBbmkg8ZTbNMqUxvQRAyrZzDsGYdLVL9zYtAWWM");

// Get instruction discriminator
auto disc = IdlProcessor::getInstructionDiscriminator("swap");
```

## Available Instructions

The module supports all auto.fun protocol instructions:

- `accept_authority` - Accept pending authority transfer
- `configure` - Configure platform parameters
- `launch` - Launch a new token
- `launch_and_swap` - Launch a token and perform initial swap
- `nominate_authority` - Nominate new authority
- `swap` - Perform token swap
- `withdraw` - Withdraw funds

## Building

The module is automatically built as part of the ElizaOS-CPP project:

```bash
mkdir build && cd build
cmake ..
make elizaos-autofun_idl
```

## Running Tests

```bash
# Run all autofun_idl tests
./cpp/tests/elizaos_tests --gtest_filter="AutofunIdlTest.*"

# Run the demo
./cpp/autofun_idl/examples/autofun_idl_demo
```

## Integration

To use this module in your C++ code:

```cpp
#include "elizaos/autofun_idl.hpp"

// Link against:
// elizaos-autofun_idl
// elizaos-core  
// nlohmann_json::nlohmann_json
```

## IDL Compatibility

This implementation is fully compatible with the auto.fun IDL specification version 0.1.0. The types and structures match exactly with the TypeScript definitions found in the `autofun-idl` directory.

## Error Handling

The module uses `IdlException` for error handling:

```cpp
try {
    auto config = IdlProcessor::parseConfigFromJson(invalid_json);
} catch (const IdlException& e) {
    std::cout << "Error: " << e.what() << std::endl;
    // Handle specific error type: e.getError()
}
```

## Thread Safety

The module is designed to be thread-safe for read operations. Serialization and deserialization functions are stateless and can be called from multiple threads concurrently.