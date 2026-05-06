// SPDX-License-Identifier: MIT
pragma solidity ^0.8.26;

import {Test, console} from "forge-std/Test.sol";
import {OTC} from "../contracts/OTC.sol";
import {MockERC20} from "../contracts/MockERC20.sol";
import {MockAggregatorV3} from "../contracts/mocks/MockAggregator.sol";
import {IAggregatorV3} from "../contracts/interfaces/IAggregatorV3.sol";
import {IERC20} from "@openzeppelin/contracts/token/ERC20/IERC20.sol";

contract FullLifecycleTest is Test {
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
    address public otherBuyer = address(6);
    
    bytes32 public tokenId;

    function setUp() public {
        vm.startPrank(owner);
        
        // Deploy dependencies
        token = new MockERC20("Test", "TST", 18, 1_000_000e18);
        usdc = new MockERC20("USDC", "USDC", 6, 1_000_000e6);
        ethFeed = new MockAggregatorV3(8, 2000e8); // $2000 ETH
        tokenFeed = new MockAggregatorV3(8, 1e8);    // $1 Token
        
        // Deploy OTC
        otc = new OTC(owner, IERC20(address(usdc)), IAggregatorV3(address(ethFeed)), agent);
        otc.setApprover(approver, true);
        
        // Register Token
        tokenId = keccak256("TST");
        otc.registerToken(tokenId, address(token), address(tokenFeed));
        
        // Fund accounts
        token.transfer(consigner, 10_000e18);
        usdc.transfer(buyer, 10_000e6);
        usdc.transfer(otherBuyer, 10_000e6);
        vm.deal(consigner, 1 ether); // For gas deposits
        vm.deal(buyer, 10 ether);    // For ETH payments
        
        vm.stopPrank();
    }
    
    function test_FullCycle_HappyPath() public {
        // 1. Create Consignment
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        
        uint256 gasDeposit = 0.001 ether;
        
        uint256 consignmentId = otc.createConsignment{value: gasDeposit}(
            tokenId,
            1000e18, // amount
            true,    // negotiable
            0, 0,    // fixed params
            0, 1000, // discount range (0-10%)
            0, 30,   // lockup range
            100e18, 1000e18, // deal size limits
            500      // max volatility
        );
        
        // Verify Consignment State
        (
            bytes32 c_tokenId,
            address c_consigner,
            uint256 c_total,
            uint256 c_remaining,
            ,
            ,
            ,
            ,
            ,
            ,
            ,
            ,
            ,
            ,
            bool c_isActive,
            
        ) = otc.consignments(consignmentId);
        
        assertEq(c_tokenId, tokenId);
        assertEq(c_consigner, consigner);
        assertEq(c_total, 1000e18);
        assertEq(c_remaining, 1000e18);
        assertTrue(c_isActive);
        
        // Verify OTC Internal Accounting
        assertEq(otc.tokenDeposited(tokenId), 1000e18);
        assertEq(otc.tokenReserved(tokenId), 0);
        assertEq(otc.consignmentGasDeposit(consignmentId), gasDeposit);
        
        vm.stopPrank();
        
        // 2. Create Offer
        vm.startPrank(buyer);
        uint256 offerAmount = 100e18;
        uint256 discountBps = 500; // 5%
        
        uint256 offerId = otc.createOfferFromConsignment(
            consignmentId,
            offerAmount,
            discountBps,
            OTC.PaymentCurrency.USDC,
            0 // 0 lockup
        );
        
        // Verify Offer State
        (
            uint256 o_cid,
            bytes32 o_tid,
            address o_ben,
            uint256 o_amt,
            uint256 o_disc,
            ,
            ,
            ,
            ,
            ,
            OTC.PaymentCurrency o_curr,
            bool o_appr,
            bool o_paid,
            ,
            ,
            ,
            
        ) = otc.offers(offerId);
        
        assertEq(o_cid, consignmentId);
        assertEq(o_tid, tokenId);
        assertEq(o_ben, buyer);
        assertEq(o_amt, offerAmount);
        assertEq(o_disc, discountBps);
        assertTrue(o_curr == OTC.PaymentCurrency.USDC);
        assertFalse(o_appr);
        assertFalse(o_paid);
        
        // Verify Counters Updated
        (,,, uint256 c_remaining_after,,,,,,,,,,,,) = otc.consignments(consignmentId);
        assertEq(c_remaining_after, 1000e18 - offerAmount);
        assertEq(otc.tokenReserved(tokenId), offerAmount); // Reserved explicitly
        
        vm.stopPrank();
        
        // 3. Approve Offer
        vm.prank(approver);
        otc.approveOffer(offerId);
        
        (,,,,,,,,,,, bool o_appr_after,,,,,) = otc.offers(offerId);
        assertTrue(o_appr_after);
        
        // 4. Fulfill Offer (Pay)
        vm.startPrank(buyer);
        // Calculate expected USD: $1 * 100 * 0.95 = $95
        uint256 expectedUsdc = 95e6; 
        usdc.approve(address(otc), expectedUsdc);
        
        uint256 buyerUsdcBefore = usdc.balanceOf(buyer);
        
        otc.fulfillOffer(offerId);
        
        uint256 buyerUsdcAfter = usdc.balanceOf(buyer);
        assertEq(buyerUsdcBefore - buyerUsdcAfter, expectedUsdc);
        
        (,,,,,,,,,,,, bool o_paid_after,,,,) = otc.offers(offerId);
        assertTrue(o_paid_after);
        
        // Verify Reserved did NOT increase (bug check)
        assertEq(otc.tokenReserved(tokenId), offerAmount);
        
        vm.stopPrank();
        
        // 5. Claim Tokens
        vm.startPrank(buyer);
        uint256 buyerTokenBefore = token.balanceOf(buyer);
        
        otc.claim(offerId);
        
        uint256 buyerTokenAfter = token.balanceOf(buyer);
        assertEq(buyerTokenAfter - buyerTokenBefore, offerAmount);
        
        (,,,,,,,,,,,,, bool o_full_after,,,) = otc.offers(offerId);
        assertTrue(o_full_after);
        
        // Verify Reserved Cleared
        assertEq(otc.tokenReserved(tokenId), 0);
        
        vm.stopPrank();
        
        // 6. Consigner Withdraws Remaining
        vm.startPrank(consigner);
        
        uint256 consignerTokenBefore = token.balanceOf(consigner);
        otc.withdrawConsignment(consignmentId);
        uint256 consignerTokenAfter = token.balanceOf(consigner);
        
        uint256 expectedRefund = 1000e18 - 100e18; // 900
        assertEq(consignerTokenAfter - consignerTokenBefore, expectedRefund);
        
        // Verify Gas Deposit Refunded
        // Hard to check exact ETH balance due to gas costs of tx, 
        // but verify internal accounting is cleared
        assertEq(otc.consignmentGasDeposit(consignmentId), 0);
        
        (,,, uint256 c_rem_final,,,,,,,,,,,bool c_active_final,) = otc.consignments(consignmentId);
        assertEq(c_rem_final, 0);
        assertFalse(c_active_final);
        
        // Check totalDeposited reduced properly
        // original 1000. 100 claimed. 900 withdrawn.
        // tokenDeposited now tracks actual balance accurately.
        // 1000 - 100 (claimed) - 900 (withdrawn) = 0.
        assertEq(otc.tokenDeposited(tokenId), 0);
        
        vm.stopPrank();
    }
    
    function test_CancelFlow() public {
        // Setup
        vm.startPrank(consigner);
        token.approve(address(otc), 100e18);
        uint256 cid = otc.createConsignment{value: 0.001 ether}(
            tokenId, 100e18, false, 0, 0, 0, 0, 0, 0, 100e18, 100e18, 500
        );
        vm.stopPrank();
        
        // Create Offer (Takes all inventory)
        vm.startPrank(buyer);
        uint256 oid = otc.createOfferFromConsignment(cid, 100e18, 0, OTC.PaymentCurrency.USDC, 0);
        vm.stopPrank();
        
        // Verify consignment inactive (depleted)
        (,,, uint256 rem,,,,,,,,,,,bool active,) = otc.consignments(cid);
        assertEq(rem, 0);
        assertFalse(active);
        assertEq(otc.tokenReserved(tokenId), 100e18);
        
        // Cancel Offer
        vm.prank(approver); // Approver can cancel
        otc.cancelOffer(oid);
        
        // Verify Inventory Restored
        (,,, rem,,,,,,,,,,, active,) = otc.consignments(cid);
        assertEq(rem, 100e18);
        assertTrue(active);
        assertEq(otc.tokenReserved(tokenId), 0);
    }
    
    function test_SolvencyProtection() public {
        // Test that one consignment cannot withdraw another's tokens
        vm.startPrank(consigner);
        token.approve(address(otc), 200e18);
        // C1
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 100e18, false, 0, 0, 0, 0, 0, 0, 100e18, 100e18, 500
        );
        // C2
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 100e18, false, 0, 0, 0, 0, 0, 0, 100e18, 100e18, 500
        );
        vm.stopPrank();
        
        // Buyer buys from C1
        vm.startPrank(buyer);
        uint256 oid = otc.createOfferFromConsignment(1, 100e18, 0, OTC.PaymentCurrency.USDC, 0);
        vm.stopPrank();
        
        vm.prank(approver);
        otc.approveOffer(oid);
        
        vm.startPrank(buyer);
        usdc.approve(address(otc), 10000e6);
        otc.fulfillOffer(oid);
        otc.claim(oid);
        vm.stopPrank();
        
        // C1 is empty/sold. C2 is full.
        // Total Balance should be 100.
        assertEq(token.balanceOf(address(otc)), 100e18);
        
        // Consigner tries to withdraw C2
        vm.startPrank(consigner);
        otc.withdrawConsignment(2);
        assertEq(token.balanceOf(address(otc)), 0);
        vm.stopPrank();
        
        // Consigner tries to withdraw C1 (should fail/empty)
        // If remainingAmount == 0, isActive is likely false (set by createOfferFromConsignment)
        // withdrawConsignment checks isActive first.
        
        vm.startPrank(consigner);
        vm.expectRevert("not active");
        otc.withdrawConsignment(1);
        vm.stopPrank();
    }
}

