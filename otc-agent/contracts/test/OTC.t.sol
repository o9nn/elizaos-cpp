// SPDX-License-Identifier: MIT
pragma solidity ^0.8.26;

import {Test, console} from "forge-std/Test.sol";
import {OTC} from "../contracts/OTC.sol";
import {MockERC20} from "../contracts/MockERC20.sol";
import {MockAggregatorV3} from "../contracts/mocks/MockAggregator.sol";
import {IAggregatorV3} from "../contracts/interfaces/IAggregatorV3.sol";
import {IERC20} from "@openzeppelin/contracts/token/ERC20/IERC20.sol";

contract OTCTest is Test {
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
        ethFeed = new MockAggregatorV3(8, 3000e8); // $3000 ETH
        tokenFeed = new MockAggregatorV3(8, 1e8); // $1 Token
        
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
    
    function test_CreateConsignment() public {
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        
        uint256 gasDeposit = 0.001 ether;
        otc.createConsignment{value: gasDeposit}(
            tokenId,
            1000e18,
            true, // negotiable
            0, 0,
            0, 1000, // 0-10% discount
            0, 30, // 0-30 days lockup
            100e18, 1000e18, // limits
            500      // max volatility
        );
        
        (,, uint256 total, uint256 remaining,,,,,,,,,,,,) = otc.consignments(1);
        assertEq(total, 1000e18);
        assertEq(remaining, 1000e18);
        assertEq(otc.tokenDeposited(tokenId), 1000e18);
        vm.stopPrank();
    }
    
    function test_DoubleReservationBug() public {
        // Setup consignment
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 1000e18, true, 0, 0, 0, 1000, 0, 30, 100e18, 1000e18, 500
        );
        vm.stopPrank();
        
        // Initial reserved
        assertEq(otc.tokenReserved(tokenId), 0);
        
        // Create offer
        vm.startPrank(buyer);
        uint256 offerId = otc.createOfferFromConsignment(
            1, 100e18, 0, OTC.PaymentCurrency.USDC, 0
        );
        vm.stopPrank();
        
        // Reserved should be 100
        assertEq(otc.tokenReserved(tokenId), 100e18);
        
        // Approve offer
        vm.prank(approver);
        otc.approveOffer(offerId);
        
        // Fulfill offer
        vm.startPrank(buyer);
        usdc.approve(address(otc), 10000e6); // ample
        otc.fulfillOffer(offerId);
        vm.stopPrank();
        
        // Check reserved - IF BUG EXISTS, it will be 200
        uint256 reserved = otc.tokenReserved(tokenId);
        console.log("Reserved:", reserved);
        
        // Fail if bug exists (we expect 100)
        assertEq(reserved, 100e18, "Double reservation bug detected");
        
        // Claim
        vm.warp(block.timestamp + 1);
        vm.prank(buyer);
        otc.claim(offerId);
        
        // Reserved should be 0
        assertEq(otc.tokenReserved(tokenId), 0, "Leaked reservation after claim");
    }
    
    function test_CancelRestoresInventory() public {
         // Setup consignment
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 1000e18, true, 0, 0, 0, 1000, 0, 30, 100e18, 1000e18, 500
        );
        vm.stopPrank();
        
        // Create offer
        vm.prank(buyer);
        uint256 offerId = otc.createOfferFromConsignment(
            1, 100e18, 0, OTC.PaymentCurrency.USDC, 0
        );
        
        // Check remaining reduced
        (,,, uint256 remaining,,,,,,,,,,,,) = otc.consignments(1);
        assertEq(remaining, 900e18);
        assertEq(otc.tokenReserved(tokenId), 100e18);
        
        // Warping to expiry
        vm.warp(block.timestamp + 30 minutes + 1);

        // Cancel
        vm.prank(buyer);
        otc.cancelOffer(offerId);
        
        // Check restored
        (,,, remaining,,,,,,,,,,,,) = otc.consignments(1);
        assertEq(remaining, 1000e18, "Inventory not restored on cancel");
        assertEq(otc.tokenReserved(tokenId), 0, "Reservation not cleared on cancel");
    }
}

