// SPDX-License-Identifier: MIT
pragma solidity ^0.8.26;

import {Script} from "forge-std/Script.sol";
import {console} from "forge-std/console.sol";
import {OTC} from "../contracts/OTC.sol";
import {RegistrationHelper} from "../contracts/RegistrationHelper.sol";
import {IERC20} from "@openzeppelin/contracts/token/ERC20/IERC20.sol";
import {IAggregatorV3 as AggregatorV3Interface} from "../contracts/interfaces/IAggregatorV3.sol";

/// @title DeployOTCMainnet
/// @notice Deploy token-agnostic OTC contract and RegistrationHelper to Base mainnet
contract DeployOTCMainnet is Script {
    // Base Mainnet addresses
    address constant USDC = 0x833589fCD6eDb6E08f4c7C32D4f71b54bdA02913;
    address constant ETH_USD_FEED = 0x71041dddad3595F9CEd3DcCFBe3D1F4b0a16Bb70;
    
    function run() external {
        // Get deployer private key from environment
        uint256 deployerPrivateKey = vm.envOr("EVM_PRIVATE_KEY", vm.envUint("EVM_PRIVATE_KEY"));
        address deployer = vm.addr(deployerPrivateKey);
        
        console.log("Deploying from:", deployer);
        console.log("Balance:", deployer.balance);
        
        require(deployer.balance > 0.002 ether, "Insufficient balance for deployment");
        
        // Get deployment parameters
        address owner = vm.envOr("OWNER_ADDRESS", deployer);
        address agent = vm.envOr("AGENT_ADDRESS", deployer);
        address approver = vm.envOr("APPROVER_ADDRESS", deployer);
        
        console.log("Owner:", owner);
        console.log("Agent:", agent);
        console.log("Approver:", approver);
        
        vm.startBroadcast(deployerPrivateKey);
        
        // 1. Deploy OTC Contract (Token-Agnostic)
        console.log("\n=== Deploying OTC Contract ===");
        OTC otc = new OTC(
            owner,
            IERC20(USDC),
            AggregatorV3Interface(ETH_USD_FEED),
            agent
        );
        console.log("OTC deployed at:", address(otc));
        
        // 2. Configure OTC
        console.log("\n=== Configuring OTC ===");
        otc.setApprover(approver, true);
        console.log("Approver set:", approver);
        
        // Set limits: $5 min, 1M max tokens, 30 min expiry, 0 default unlock
        otc.setLimits(
            5 * 1e8,           // minUsdAmount: $5
            1_000_000 * 1e18,  // maxTokenPerOrder: 1M tokens
            30 * 60,           // quoteExpirySeconds: 30 minutes
            0                  // defaultUnlockDelaySeconds: 0
        );
        console.log("Limits configured");
        
        // 3. Deploy RegistrationHelper
        console.log("\n=== Deploying RegistrationHelper ===");
        RegistrationHelper helper = new RegistrationHelper(
            address(otc),
            ETH_USD_FEED
        );
        console.log("RegistrationHelper deployed at:", address(helper));
        
        // 4. Transfer ownership of OTC to owner (if deployer != owner)
        if (deployer != owner) {
            console.log("\n=== Transferring Ownership ===");
            otc.transferOwnership(owner);
            console.log("OTC ownership transferred to:", owner);
        }
        
        vm.stopBroadcast();
        
        // Print deployment summary
        console.log("\n=== Deployment Summary ===");
        console.log("Network: Base Mainnet");
        console.log("OTC Contract:", address(otc));
        console.log("RegistrationHelper:", address(helper));
        console.log("USDC:", USDC);
        console.log("ETH/USD Feed:", ETH_USD_FEED);

        // Write deployment info to JSON
        string memory deploymentJson = string.concat(
            '{\n',
            '  "network": "base-mainnet",\n',
            '  "timestamp": "', vm.toString(block.timestamp), '",\n',
            '  "contracts": {\n',
            '    "otc": "', vm.toString(address(otc)), '",\n',
            '    "registrationHelper": "', vm.toString(address(helper)), '",\n',
            '    "usdc": "', vm.toString(USDC), '",\n',
            '    "ethUsdFeed": "', vm.toString(ETH_USD_FEED), '"\n',
            '  }\n',
            '}'
        );
        vm.writeFile("deployments/mainnet-evm.json", deploymentJson);
        console.log("Wrote deployment to deployments/mainnet-evm.json");

        console.log("");
        console.log("=== Environment Variables to Add ===");
        console.log("NEXT_PUBLIC_BASE_OTC_ADDRESS=%s", address(otc));
        console.log("NEXT_PUBLIC_REGISTRATION_HELPER_ADDRESS=%s", address(helper));
        console.log("");
        console.log("=== Next Steps ===");
        console.log("1. Verify contracts on Basescan:");
        console.log("   forge verify-contract %s OTC --chain-id 8453 --watch", address(otc));
        console.log("   forge verify-contract %s RegistrationHelper --chain-id 8453 --watch", address(helper));
        console.log("");
        console.log("2. Update Vercel environment variables with the addresses above");
        console.log("");
        console.log("3. Start backend event listeners for token registrations");
    }
}

