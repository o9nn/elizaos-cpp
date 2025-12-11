// SPDX-License-Identifier: MIT
pragma solidity ^0.8.26;

import {Test, console} from "forge-std/Test.sol";
import {OTC} from "../contracts/OTC.sol";
import {MockERC20} from "../contracts/MockERC20.sol";
import {MockAggregatorV3} from "../contracts/mocks/MockAggregator.sol";
import {IAggregatorV3} from "../contracts/interfaces/IAggregatorV3.sol";
import {IERC20} from "@openzeppelin/contracts/token/ERC20/IERC20.sol";

contract RugPullTest is Test {
    OTC public otc;
    MockERC20 public token;
    MockERC20 public usdc;
    MockAggregatorV3 public ethFeed;
    MockAggregatorV3 public tokenFeed;
    
    address public owner = address(1);
    address public agent = address(2);
    address public approver = address(3);
    address public consigner = address(4);
    address public buyer = address(5);
    
    bytes32 public tokenId;

    function setUp() public {
        vm.startPrank(owner);
        token = new MockERC20("Test", "TST", 18, 1000000e18);
        usdc = new MockERC20("USDC", "USDC", 6, 1000000e6);
        ethFeed = new MockAggregatorV3(8, 3000e8); 
        tokenFeed = new MockAggregatorV3(8, 1e8); 
        otc = new OTC(owner, IERC20(address(usdc)), IAggregatorV3(address(ethFeed)), agent);
        otc.setApprover(approver, true);
        tokenId = keccak256("TST");
        otc.registerToken(tokenId, address(token), address(tokenFeed));
        
        // Fund consigner
        token.transfer(consigner, 10000e18);
        vm.deal(consigner, 10 ether);
        // Fund buyer
        usdc.transfer(buyer, 10000e6);
        vm.deal(buyer, 10 ether);
        
        vm.stopPrank();
    }
    
    function test_BuyerRefundsLongLockup() public {
        // 1. Consigner creates 1 year lockup consignment
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 1000e18, false, // fixed
            0, 365, // 0% discount, 365 days lockup
            0, 0, 0, 0, 100e18, 1000e18, 500
        );
        vm.stopPrank();
        
        // 2. Buyer creates offer (must respect 365 days)
        vm.startPrank(buyer);
        uint256 offerId = otc.createOfferFromConsignment(
            1, 100e18, 0, OTC.PaymentCurrency.USDC, 365 days
        );
        vm.stopPrank();
        
        // 3. Approve
        vm.prank(approver);
        otc.approveOffer(offerId);
        
        // 4. Pay
        vm.startPrank(buyer);
        usdc.approve(address(otc), 100e6);
        otc.fulfillOffer(offerId);
        vm.stopPrank();
        
        // 5. Enable emergency refunds
        vm.prank(owner);
        otc.setEmergencyRefund(true);
        
        // 6. Warp 31 days (past default 30 day deadline)
        vm.warp(block.timestamp + 31 days);
        
        // 7. Buyer refunds
        vm.startPrank(buyer);
        otc.emergencyRefund(offerId);
        vm.stopPrank();
        
        // 8. Check state
        (,,,,,,,,,,,,,,bool cancelled,,) = otc.offers(offerId);
        assertTrue(cancelled, "Offer should be cancelled");
        
        // Buyer got money back?
        assertEq(usdc.balanceOf(buyer), 10000e6, "Buyer should have full refund");
        
        // This proves the Consigner was rugged (deal cancelled early)
    }
}

