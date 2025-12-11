// SPDX-License-Identifier: MIT
pragma solidity ^0.8.26;

import {Test, console} from "forge-std/Test.sol";
import {OTC} from "../contracts/OTC.sol";
import {MockERC20} from "../contracts/MockERC20.sol";
import {FeeToken} from "../contracts/mocks/FeeToken.sol";
import {MockAggregatorV3} from "../contracts/mocks/MockAggregator.sol";
import {IAggregatorV3} from "../contracts/interfaces/IAggregatorV3.sol";
import {IERC20} from "@openzeppelin/contracts/token/ERC20/IERC20.sol";

/**
 * @title CriticalEdgeCasesTest
 * @notice Tests for edge cases and potential exploits not covered by other tests
 */
contract CriticalEdgeCasesTest is Test {
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
    address public attacker = address(6);
    
    bytes32 public tokenId;

    function setUp() public {
        vm.startPrank(owner);
        
        token = new MockERC20("Test", "TST", 18, 100_000_000e18);
        usdc = new MockERC20("USDC", "USDC", 6, 100_000_000e6);
        ethFeed = new MockAggregatorV3(8, 3000e8);
        tokenFeed = new MockAggregatorV3(8, 1e8);
        
        otc = new OTC(owner, IERC20(address(usdc)), IAggregatorV3(address(ethFeed)), agent);
        otc.setApprover(approver, true);
        
        tokenId = keccak256("TST");
        otc.registerToken(tokenId, address(token), address(tokenFeed));
        
        token.transfer(consigner, 10_000_000e18);
        token.transfer(attacker, 10_000_000e18);
        usdc.transfer(buyer, 10_000_000e6);
        usdc.transfer(attacker, 10_000_000e6);
        vm.deal(consigner, 1000 ether);
        vm.deal(buyer, 1000 ether);
        vm.deal(attacker, 1000 ether);
        
        vm.stopPrank();
    }

    // ============================================================
    // EDGE CASE #1: 100% Discount Offer
    // ============================================================
    
    /**
     * @notice Test that 100% discount (10000 bps) results in 0 payment
     * @dev Should revert due to min USD check
     */
    function test_EDGE_MaxDiscountRejected() public {
        // Create consignment allowing 100% discount
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 1000e18, 
            true,  // negotiable
            0, 0,  // fixed params unused
            10000, 10000, // 100% discount allowed
            0, 30,
            100e18, 1000e18, 500
        );
        vm.stopPrank();
        
        // Try to create offer with 100% discount - should fail min USD check
        vm.prank(buyer);
        vm.expectRevert("min usd not met");
        otc.createOfferFromConsignment(1, 100e18, 10000, OTC.PaymentCurrency.USDC, 0);
    }

    // ============================================================
    // EDGE CASE #2: Emergency Refund with ETH Payment
    // ============================================================
    
    /**
     * @notice Verify emergency refund returns ETH correctly
     */
    function test_EDGE_EmergencyRefundETH() public {
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 1000e18, false, 0, 0, 0, 0, 0, 0, 100e18, 1000e18, 500
        );
        vm.stopPrank();
        
        // Create and pay ETH offer
        vm.prank(buyer);
        uint256 offerId = otc.createOfferFromConsignment(1, 100e18, 0, OTC.PaymentCurrency.ETH, 0);
        
        vm.prank(approver);
        otc.approveOffer(offerId);
        
        uint256 required = otc.requiredEthWei(offerId);
        uint256 buyerBalBefore = buyer.balance;
        
        vm.prank(buyer);
        otc.fulfillOffer{value: required}(offerId);
        
        assertEq(buyer.balance, buyerBalBefore - required, "Buyer paid ETH");
        
        // Enable emergency refund and wait
        vm.prank(owner);
        otc.setEmergencyRefund(true);
        vm.warp(block.timestamp + 31 days);
        
        // Refund
        vm.prank(buyer);
        otc.emergencyRefund(offerId);
        
        // Verify ETH returned
        assertEq(buyer.balance, buyerBalBefore, "Buyer got ETH refund");
        
        // Verify consignment restored
        (,,, uint256 remaining,,,,,,,,,,,bool active,) = otc.consignments(1);
        assertEq(remaining, 1000e18, "Tokens restored");
        assertTrue(active, "Consignment active");
    }

    // ============================================================
    // EDGE CASE #3: Pause Blocks Operations
    // ============================================================
    
    /**
     * @notice Verify pause blocks all user operations
     */
    function test_EDGE_PauseBlocksOperations() public {
        // Setup consignment first
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 1000e18, false, 0, 0, 0, 0, 0, 0, 100e18, 1000e18, 500
        );
        vm.stopPrank();
        
        vm.prank(buyer);
        uint256 offerId = otc.createOfferFromConsignment(1, 100e18, 0, OTC.PaymentCurrency.USDC, 0);
        
        vm.prank(approver);
        otc.approveOffer(offerId);
        
        // Pause contract
        vm.prank(owner);
        otc.pause();
        
        // All operations should fail
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        vm.expectRevert();
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 1000e18, false, 0, 0, 0, 0, 0, 0, 100e18, 1000e18, 500
        );
        vm.stopPrank();
        
        vm.startPrank(buyer);
        usdc.approve(address(otc), 100e6);
        vm.expectRevert();
        otc.fulfillOffer(offerId);
        vm.stopPrank();
        
        // Unpause
        vm.prank(owner);
        otc.unpause();
        
        // Now should work
        vm.startPrank(buyer);
        otc.fulfillOffer(offerId);
        vm.stopPrank();
    }

    // ============================================================
    // EDGE CASE #4: autoClaim with Invalid Offer IDs
    // ============================================================
    
    /**
     * @notice Verify autoClaim gracefully skips invalid offers
     */
    function test_EDGE_AutoClaimSkipsInvalid() public {
        // Setup valid offer
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 1000e18, false, 0, 0, 0, 0, 0, 0, 100e18, 1000e18, 500
        );
        vm.stopPrank();
        
        vm.prank(buyer);
        uint256 validId = otc.createOfferFromConsignment(1, 100e18, 0, OTC.PaymentCurrency.USDC, 0);
        
        vm.prank(approver);
        otc.approveOffer(validId);
        
        vm.startPrank(buyer);
        usdc.approve(address(otc), 100e6);
        otc.fulfillOffer(validId);
        vm.stopPrank();
        
        // Mix of valid and invalid IDs
        uint256[] memory offerIds = new uint256[](5);
        offerIds[0] = 0;        // Invalid: zero
        offerIds[1] = validId;  // Valid
        offerIds[2] = 999;      // Invalid: doesn't exist
        offerIds[3] = validId;  // Already processed (will skip second time)
        offerIds[4] = 1000;     // Invalid: doesn't exist
        
        // Should not revert, should process valid offer
        vm.prank(approver);
        otc.autoClaim(offerIds);
        
        // Verify valid offer was claimed
        (,,,,,,,,,,,,,bool fulfilled,,,) = otc.offers(validId);
        assertTrue(fulfilled, "Valid offer should be fulfilled");
        assertEq(token.balanceOf(buyer), 100e18, "Buyer received tokens");
    }

    // ============================================================
    // EDGE CASE #5: Zero Gas Deposit Requirement
    // ============================================================
    
    /**
     * @notice Verify zero gas deposit requirement works
     */
    function test_EDGE_ZeroGasDeposit() public {
        // Set required gas deposit to 0
        vm.prank(owner);
        otc.setRequiredGasDeposit(0);
        
        // Create consignment without sending ETH
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        uint256 cid = otc.createConsignment{value: 0}(
            tokenId, 1000e18, false, 0, 0, 0, 0, 0, 0, 100e18, 1000e18, 500
        );
        vm.stopPrank();
        
        assertEq(otc.consignmentGasDeposit(cid), 0, "Zero gas deposit stored");
        
        // Withdraw should work
        vm.prank(consigner);
        otc.withdrawConsignment(cid);
    }

    // ============================================================
    // EDGE CASE #6: Non-Consigner Cannot Withdraw
    // ============================================================
    
    /**
     * @notice Verify only consigner can withdraw their consignment
     */
    function test_EDGE_OnlyConsignerCanWithdraw() public {
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 1000e18, false, 0, 0, 0, 0, 0, 0, 100e18, 1000e18, 500
        );
        vm.stopPrank();
        
        // Attacker cannot withdraw
        vm.prank(attacker);
        vm.expectRevert("not consigner");
        otc.withdrawConsignment(1);
        
        // Owner cannot withdraw (unless emergency)
        vm.prank(owner);
        vm.expectRevert("not consigner");
        otc.withdrawConsignment(1);
        
        // Agent cannot withdraw
        vm.prank(agent);
        vm.expectRevert("not consigner");
        otc.withdrawConsignment(1);
        
        // Consigner can withdraw
        vm.prank(consigner);
        otc.withdrawConsignment(1);
    }

    // ============================================================
    // EDGE CASE #7: Approver Cancellation
    // ============================================================
    
    /**
     * @notice Verify approver can cancel offers (trusted role)
     */
    function test_EDGE_ApproverCanCancel() public {
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 1000e18, false, 0, 0, 0, 0, 0, 0, 100e18, 1000e18, 500
        );
        vm.stopPrank();
        
        vm.prank(buyer);
        uint256 offerId = otc.createOfferFromConsignment(1, 100e18, 0, OTC.PaymentCurrency.USDC, 0);
        
        uint256 reservedBefore = otc.tokenReserved(tokenId);
        assertEq(reservedBefore, 100e18, "Tokens reserved");
        
        // Approver can cancel without waiting for expiry
        vm.prank(approver);
        otc.cancelOffer(offerId);
        
        // Verify cancelled
        (,,,,,,,,,,,,,,bool cancelled,,) = otc.offers(offerId);
        assertTrue(cancelled, "Offer cancelled");
        
        // Tokens returned
        uint256 reservedAfter = otc.tokenReserved(tokenId);
        assertEq(reservedAfter, 0, "Tokens released");
        
        // Consignment restored
        (,,, uint256 remaining,,,,,,,,,,,bool active,) = otc.consignments(1);
        assertEq(remaining, 1000e18, "Tokens restored to consignment");
        assertTrue(active, "Consignment still active");
    }

    // ============================================================
    // EDGE CASE #8: Very Large Token Amounts
    // ============================================================
    
    /**
     * @notice Test handling of large token amounts (overflow protection)
     */
    function test_EDGE_LargeTokenAmounts() public {
        // Set high price to trigger large USD values
        vm.prank(owner);
        tokenFeed.setAnswer(1000e8); // $1000 per token
        
        vm.startPrank(consigner);
        token.approve(address(otc), 1_000_000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 1_000_000e18, false, 0, 0, 0, 0, 0, 0, 100e18, 1_000_000e18, 500
        );
        vm.stopPrank();
        
        // Create large offer: 1M tokens at $1000 = $1B
        vm.prank(buyer);
        uint256 offerId = otc.createOfferFromConsignment(1, 1_000_000e18, 0, OTC.PaymentCurrency.USDC, 0);
        
        // Check USD calculation didn't overflow
        uint256 totalUsd = otc.totalUsdForOffer(offerId);
        console.log("Total USD (8 decimals):", totalUsd);
        assertEq(totalUsd, 1_000_000_000e8, "USD = $1B");
        
        // Required USDC should be 1B (but will likely exceed buyer's balance in real scenario)
        uint256 requiredUsdc = otc.requiredUsdcAmount(offerId);
        assertEq(requiredUsdc, 1_000_000_000e6, "USDC = 1B tokens");
    }

    // ============================================================
    // EDGE CASE #9: Duplicate Token Registration Blocked
    // ============================================================
    
    /**
     * @notice Verify same tokenId cannot be registered twice
     */
    function test_EDGE_DuplicateTokenBlocked() public {
        // Try to register same tokenId again
        vm.prank(owner);
        vm.expectRevert("token exists");
        otc.registerToken(tokenId, address(token), address(tokenFeed));
    }

    // ============================================================
    // EDGE CASE #10: Offer Cannot Be Created on Inactive Consignment
    // ============================================================
    
    /**
     * @notice Verify offers cannot be created on withdrawn/inactive consignments
     */
    function test_EDGE_NoOfferOnInactiveConsignment() public {
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 1000e18, false, 0, 0, 0, 0, 0, 0, 100e18, 1000e18, 500
        );
        otc.withdrawConsignment(1);
        vm.stopPrank();
        
        // Cannot create offer on withdrawn consignment
        vm.prank(buyer);
        vm.expectRevert("consignment not active");
        otc.createOfferFromConsignment(1, 100e18, 0, OTC.PaymentCurrency.USDC, 0);
    }

    // ============================================================
    // EDGE CASE #11: Claim Before Unlock Fails
    // ============================================================
    
    /**
     * @notice Verify claim before unlock time fails
     */
    function test_EDGE_ClaimBeforeUnlockFails() public {
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 1000e18, 
            true,  // negotiable
            0, 0,
            0, 1000,
            0, 365, // 0-365 days lockup range
            100e18, 1000e18, 500
        );
        vm.stopPrank();
        
        // Create offer with 30-day lockup
        vm.prank(buyer);
        uint256 offerId = otc.createOfferFromConsignment(1, 100e18, 0, OTC.PaymentCurrency.USDC, 30 days);
        
        vm.prank(approver);
        otc.approveOffer(offerId);
        
        vm.startPrank(buyer);
        usdc.approve(address(otc), 100e6);
        otc.fulfillOffer(offerId);
        
        // Try to claim immediately - should fail
        vm.expectRevert("locked");
        otc.claim(offerId);
        
        // Warp 29 days - still locked
        vm.warp(block.timestamp + 29 days);
        vm.expectRevert("locked");
        otc.claim(offerId);
        
        // Warp 1 more day - now unlocked
        vm.warp(block.timestamp + 1 days + 1);
        otc.claim(offerId);
        
        assertEq(token.balanceOf(buyer), 100e18, "Tokens claimed after unlock");
        vm.stopPrank();
    }

    // ============================================================
    // EDGE CASE #13: Quote Expiry
    // ============================================================
    
    /**
     * @notice Verify fulfillment fails after quote expiry
     */
    function test_EDGE_FulfillAfterExpiryFails() public {
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 1000e18, false, 0, 0, 0, 0, 0, 0, 100e18, 1000e18, 500
        );
        vm.stopPrank();
        
        vm.prank(buyer);
        uint256 offerId = otc.createOfferFromConsignment(1, 100e18, 0, OTC.PaymentCurrency.USDC, 0);
        
        vm.prank(approver);
        otc.approveOffer(offerId);
        
        // Warp past quote expiry (default 30 minutes)
        vm.warp(block.timestamp + 31 minutes);
        
        vm.startPrank(buyer);
        usdc.approve(address(otc), 100e6);
        vm.expectRevert("expired");
        otc.fulfillOffer(offerId);
        vm.stopPrank();
    }

    // ============================================================
    // EDGE CASE #14: Non-Beneficiary Cannot Claim
    // ============================================================
    
    /**
     * @notice Verify only beneficiary can claim tokens
     */
    function test_EDGE_OnlyBeneficiaryCanClaim() public {
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 1000e18, false, 0, 0, 0, 0, 0, 0, 100e18, 1000e18, 500
        );
        vm.stopPrank();
        
        vm.prank(buyer);
        uint256 offerId = otc.createOfferFromConsignment(1, 100e18, 0, OTC.PaymentCurrency.USDC, 0);
        
        vm.prank(approver);
        otc.approveOffer(offerId);
        
        vm.startPrank(buyer);
        usdc.approve(address(otc), 100e6);
        otc.fulfillOffer(offerId);
        vm.stopPrank();
        
        // Attacker cannot claim
        vm.prank(attacker);
        vm.expectRevert("not beneficiary");
        otc.claim(offerId);
        
        // Owner cannot claim
        vm.prank(owner);
        vm.expectRevert("not beneficiary");
        otc.claim(offerId);
        
        // Agent cannot claim  
        vm.prank(agent);
        vm.expectRevert("not beneficiary");
        otc.claim(offerId);
        
        // Beneficiary can claim
        vm.prank(buyer);
        otc.claim(offerId);
        
        assertEq(token.balanceOf(buyer), 100e18, "Buyer received tokens");
    }
}

/**
 * @title FeeOnTransferEdgeCases
 * @notice Tests for fee-on-transfer token edge cases
 */
contract FeeOnTransferEdgeCases is Test {
    OTC public otc;
    FeeToken public feeToken; // 1% fee on transfer
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
        
        feeToken = new FeeToken(100_000_000e18);
        usdc = new MockERC20("USDC", "USDC", 6, 100_000_000e6);
        ethFeed = new MockAggregatorV3(8, 3000e8);
        tokenFeed = new MockAggregatorV3(8, 1e8);
        
        otc = new OTC(owner, IERC20(address(usdc)), IAggregatorV3(address(ethFeed)), agent);
        otc.setApprover(approver, true);
        
        tokenId = keccak256("FEE");
        otc.registerToken(tokenId, address(feeToken), address(tokenFeed));
        
        feeToken.transfer(consigner, 10_000_000e18);
        usdc.transfer(buyer, 10_000_000e6);
        vm.deal(consigner, 100 ether);
        vm.deal(buyer, 100 ether);
        
        vm.stopPrank();
    }

    /**
     * @notice Verify fee-on-transfer accounting is correct
     */
    function test_FeeTokenAccounting() public {
        uint256 depositAmount = 1000e18;
        uint256 feePercent = 1; // 1% fee
        uint256 expectedActual = depositAmount * (100 - feePercent) / 100; // 990 tokens
        
        vm.startPrank(consigner);
        feeToken.approve(address(otc), depositAmount);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, depositAmount, false, 0, 0, 0, 0, 0, 0, 100e18, 1000e18, 500
        );
        vm.stopPrank();
        
        // Verify actual amount received is tracked
        (,,uint256 total, uint256 remaining,,,,,,,,,,,,) = otc.consignments(1);
        
        // Due to 1% fee, only 990 tokens should be tracked
        assertEq(total, expectedActual, "Total should reflect fee deduction");
        assertEq(remaining, expectedActual, "Remaining should match total");
        
        // Verify contract balance matches tracked amount
        assertEq(feeToken.balanceOf(address(otc)), expectedActual, "Contract balance correct");
        assertEq(otc.tokenDeposited(tokenId), expectedActual, "tokenDeposited correct");
    }

    /**
     * @notice Full cycle with fee-on-transfer token
     */
    function test_FeeTokenFullCycle() public {
        uint256 depositAmount = 1000e18;
        uint256 actualDeposited = 990e18; // After 1% fee
        
        // Create consignment
        vm.startPrank(consigner);
        feeToken.approve(address(otc), depositAmount);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, depositAmount, false, 0, 0, 0, 0, 0, 0, 100e18, 1000e18, 500
        );
        vm.stopPrank();
        
        // Create offer for 100 tokens (will work since actualDeposited is 990)
        vm.prank(buyer);
        uint256 offerId = otc.createOfferFromConsignment(1, 100e18, 0, OTC.PaymentCurrency.USDC, 0);
        
        vm.prank(approver);
        otc.approveOffer(offerId);
        
        vm.startPrank(buyer);
        usdc.approve(address(otc), 100e6);
        otc.fulfillOffer(offerId);
        otc.claim(offerId);
        vm.stopPrank();
        
        // Buyer receives full 100 tokens (no fee on transfer out since safeTransfer doesn't trigger fee)
        // Actually FeeToken applies fee on ALL transfers
        uint256 expectedReceived = 100e18 * 99 / 100; // 99 tokens after 1% fee
        assertEq(feeToken.balanceOf(buyer), expectedReceived, "Buyer received tokens minus fee");
        
        // Verify accounting
        assertEq(otc.tokenDeposited(tokenId), actualDeposited - 100e18, "Deposited decremented by full amount");
        assertEq(otc.tokenReserved(tokenId), 0, "No tokens reserved");
    }
}

/**
 * @title AdminEmergencyWithdrawTest
 * @notice Tests for admin emergency withdraw functionality
 */
contract AdminEmergencyWithdrawTest is Test {
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
        
        token = new MockERC20("Test", "TST", 18, 100_000_000e18);
        usdc = new MockERC20("USDC", "USDC", 6, 100_000_000e6);
        ethFeed = new MockAggregatorV3(8, 3000e8);
        tokenFeed = new MockAggregatorV3(8, 1e8);
        
        otc = new OTC(owner, IERC20(address(usdc)), IAggregatorV3(address(ethFeed)), agent);
        otc.setApprover(approver, true);
        
        tokenId = keccak256("TST");
        otc.registerToken(tokenId, address(token), address(tokenFeed));
        
        token.transfer(consigner, 10_000_000e18);
        usdc.transfer(buyer, 10_000_000e6);
        vm.deal(consigner, 100 ether);
        vm.deal(buyer, 100 ether);
        
        vm.stopPrank();
    }

    /**
     * @notice Verify admin cannot emergency withdraw too early
     */
    function test_AdminEmergencyWithdrawTooEarly() public {
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 1000e18, false, 0, 0, 0, 0, 0, 0, 100e18, 1000e18, 500
        );
        vm.stopPrank();
        
        vm.prank(buyer);
        uint256 offerId = otc.createOfferFromConsignment(1, 100e18, 0, OTC.PaymentCurrency.USDC, 0);
        
        vm.prank(approver);
        otc.approveOffer(offerId);
        
        vm.startPrank(buyer);
        usdc.approve(address(otc), 100e6);
        otc.fulfillOffer(offerId);
        vm.stopPrank();
        
        // Try admin emergency withdraw immediately - should fail
        vm.prank(owner);
        vm.expectRevert("must wait 180 days after unlock");
        otc.adminEmergencyWithdraw(offerId);
        
        // Warp 179 days after unlock (unlock is immediate since no lockup)
        vm.warp(block.timestamp + 179 days);
        
        vm.prank(owner);
        vm.expectRevert("must wait 180 days after unlock");
        otc.adminEmergencyWithdraw(offerId);
    }

    /**
     * @notice Verify admin emergency withdraw works after 180 days
     */
    function test_AdminEmergencyWithdrawAfterWait() public {
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 1000e18, false, 0, 0, 0, 0, 0, 0, 100e18, 1000e18, 500
        );
        vm.stopPrank();
        
        vm.prank(buyer);
        uint256 offerId = otc.createOfferFromConsignment(1, 100e18, 0, OTC.PaymentCurrency.USDC, 0);
        
        vm.prank(approver);
        otc.approveOffer(offerId);
        
        vm.startPrank(buyer);
        usdc.approve(address(otc), 100e6);
        otc.fulfillOffer(offerId);
        vm.stopPrank();
        
        // Warp 181 days
        vm.warp(block.timestamp + 181 days);
        
        uint256 buyerBalBefore = token.balanceOf(buyer);
        
        // Admin emergency withdraw should succeed
        vm.prank(owner);
        otc.adminEmergencyWithdraw(offerId);
        
        // Tokens go to beneficiary (buyer)
        assertEq(token.balanceOf(buyer), buyerBalBefore + 100e18, "Buyer received tokens");
        
        // Verify state
        (,,,,,,,,,,,,,bool fulfilled,,,) = otc.offers(offerId);
        assertTrue(fulfilled, "Offer marked fulfilled");
        
        assertEq(otc.tokenReserved(tokenId), 0, "Tokens unreserved");
        assertEq(otc.tokenDeposited(tokenId), 900e18, "Deposited decremented");
    }

    /**
     * @notice Verify non-owner cannot admin emergency withdraw
     */
    function test_OnlyOwnerCanAdminEmergencyWithdraw() public {
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 1000e18, false, 0, 0, 0, 0, 0, 0, 100e18, 1000e18, 500
        );
        vm.stopPrank();
        
        vm.prank(buyer);
        uint256 offerId = otc.createOfferFromConsignment(1, 100e18, 0, OTC.PaymentCurrency.USDC, 0);
        
        vm.prank(approver);
        otc.approveOffer(offerId);
        
        vm.startPrank(buyer);
        usdc.approve(address(otc), 100e6);
        otc.fulfillOffer(offerId);
        vm.stopPrank();
        
        vm.warp(block.timestamp + 181 days);
        
        // Non-owner attempts should fail
        vm.prank(agent);
        vm.expectRevert();
        otc.adminEmergencyWithdraw(offerId);
        
        vm.prank(approver);
        vm.expectRevert();
        otc.adminEmergencyWithdraw(offerId);
        
        vm.prank(buyer);
        vm.expectRevert();
        otc.adminEmergencyWithdraw(offerId);
    }
}

