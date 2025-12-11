// SPDX-License-Identifier: MIT
pragma solidity ^0.8.26;

import {Test, console} from "forge-std/Test.sol";
import {OTC} from "../contracts/OTC.sol";
import {MockERC20} from "../contracts/MockERC20.sol";
import {FeeToken} from "../contracts/mocks/FeeToken.sol";
import {MockAggregatorV3} from "../contracts/mocks/MockAggregator.sol";
import {IAggregatorV3} from "../contracts/interfaces/IAggregatorV3.sol";
import {IERC20} from "@openzeppelin/contracts/token/ERC20/IERC20.sol";

contract OTCFeeTokenTest is Test {
    OTC public otc;
    FeeToken public token;
    MockERC20 public usdc;
    MockAggregatorV3 public ethFeed;
    MockAggregatorV3 public tokenFeed;
    
    address public owner = address(1);
    address public agent = address(2);
    address public approver = address(3);
    address public consigner = address(4);
    
    bytes32 public tokenId;

    function setUp() public {
        vm.startPrank(owner);
        
        token = new FeeToken(1000000e18);
        usdc = new MockERC20("USDC", "USDC", 6, 1000000e6);
        ethFeed = new MockAggregatorV3(8, 3000e8); 
        tokenFeed = new MockAggregatorV3(8, 1e8); 
        
        otc = new OTC(owner, IERC20(address(usdc)), IAggregatorV3(address(ethFeed)), agent);
        otc.setApprover(approver, true);
        
        tokenId = keccak256("FEE");
        otc.registerToken(tokenId, address(token), address(tokenFeed));
        
        token.transfer(consigner, 10000e18);
        vm.deal(consigner, 10 ether);
        
        vm.stopPrank();
    }
    
    function test_FeeOnTransferConsignment() public {
        vm.startPrank(consigner);
        uint256 amount = 1000e18;
        token.approve(address(otc), amount);
        
        // Create consignment with 1000 tokens
        // 1% fee means OTC gets 990 tokens
        otc.createConsignment{value: 0.001 ether}(
            tokenId,
            amount,
            true, 0, 0, 0, 1000, 0, 30, 100e18, 1000e18, 500
        );
        
        // Check consignment details
        (,, uint256 total, uint256 remaining,,,,,,,,,,,,) = otc.consignments(1);
        
        // If bug exists, total == 1000, but actual balance == 990
        console.log("Consignment Total:", total);
        console.log("Consignment Remaining:", remaining);
        console.log("Actual Balance:", token.balanceOf(address(otc)));
        
        // This assertion expects the FIX to be in place (tracking actual received)
        // If I haven't fixed it yet, this test might fail or I should assert the broken behavior to demonstrate it.
        // Let's assert the CORRECT behavior and see it fail.
        
        assertEq(remaining, 990e18, "Should track actual received amount");
        
        vm.stopPrank();
    }
}

