// SPDX-License-Identifier: MIT
pragma solidity ^0.8.26;

import {Test, console} from "forge-std/Test.sol";
import {OTC} from "../contracts/OTC.sol";
import {MockERC20} from "../contracts/MockERC20.sol";
import {MockAggregatorV3} from "../contracts/mocks/MockAggregator.sol";
import {IAggregatorV3} from "../contracts/interfaces/IAggregatorV3.sol";
import {IERC20} from "@openzeppelin/contracts/token/ERC20/IERC20.sol";

/**
 * @title PaymentFlow Tests
 * @notice Tests the complete payment flow: buyer pays -> desk collects -> treasury withdrawal
 * @dev Verifies that payments are correctly tracked and can be withdrawn by treasury
 */
contract PaymentFlowTest is Test {
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
    address public treasury = address(6);
    
    bytes32 public tokenId;

    function setUp() public {
        vm.startPrank(owner);
        
        token = new MockERC20("Test", "TST", 18, 1_000_000e18);
        usdc = new MockERC20("USDC", "USDC", 6, 1_000_000e6);
        ethFeed = new MockAggregatorV3(8, 2000e8); // $2000 ETH
        tokenFeed = new MockAggregatorV3(8, 1e8);    // $1 Token
        
        otc = new OTC(owner, IERC20(address(usdc)), IAggregatorV3(address(ethFeed)), agent);
        otc.setApprover(approver, true);
        
        tokenId = keccak256("TST");
        otc.registerToken(tokenId, address(token), address(tokenFeed));
        
        // Fund consigner with tokens
        token.transfer(consigner, 10_000e18);
        vm.deal(consigner, 1 ether);
        
        // Fund buyer with USDC and ETH
        usdc.transfer(buyer, 10_000e6);
        vm.deal(buyer, 10 ether);
        
        vm.stopPrank();
    }

    /**
     * @notice Test complete USDC payment flow
     * Verifies: buyer pays USDC -> contract holds it -> owner can withdraw
     */
    function test_USDCPaymentFlow() public {
        // 1. Consigner creates consignment
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId,
            1000e18,
            false, // not negotiable
            0,     // fixed discount
            0,     // fixed lockup
            0, 0,  // no ranges
            0, 0,  // no ranges
            100e18, 1000e18,
            500    // max volatility
        );
        vm.stopPrank();

        // 2. Buyer creates offer for 100 tokens at $1 each = $100 USDC
        vm.startPrank(buyer);
        uint256 offerId = otc.createOfferFromConsignment(
            1,       // consignment ID
            100e18,  // 100 tokens
            0,       // no discount
            OTC.PaymentCurrency.USDC,
            0        // no lockup
        );
        vm.stopPrank();

        // 3. Approver approves
        vm.prank(approver);
        otc.approveOffer(offerId);

        // 4. Buyer pays with USDC
        vm.startPrank(buyer);
        uint256 buyerUsdcBefore = usdc.balanceOf(buyer);
        usdc.approve(address(otc), 100e6); // $100 USDC (6 decimals)
        otc.fulfillOffer(offerId);
        uint256 buyerUsdcAfter = usdc.balanceOf(buyer);
        vm.stopPrank();

        // Verify buyer paid
        uint256 paymentAmount = buyerUsdcBefore - buyerUsdcAfter;
        assertEq(paymentAmount, 100e6, "Buyer should have paid $100 USDC");
        
        // Verify contract received USDC
        uint256 contractUsdcBalance = usdc.balanceOf(address(otc));
        assertEq(contractUsdcBalance, 100e6, "Contract should hold $100 USDC");

        // 5. Owner withdraws USDC to treasury
        vm.startPrank(owner);
        uint256 treasuryBefore = usdc.balanceOf(treasury);
        otc.withdrawStable(treasury, 100e6, 0);
        uint256 treasuryAfter = usdc.balanceOf(treasury);
        vm.stopPrank();

        // Verify treasury received payment
        assertEq(treasuryAfter - treasuryBefore, 100e6, "Treasury should receive $100 USDC");
        assertEq(usdc.balanceOf(address(otc)), 0, "Contract should have 0 USDC after withdrawal");

        // 6. Buyer claims tokens
        vm.prank(buyer);
        otc.claim(offerId);

        // Verify buyer got tokens
        assertEq(token.balanceOf(buyer), 100e18, "Buyer should have 100 tokens");

        console.log("USDC Payment Flow Complete:");
        console.log("  Buyer paid: $100 USDC");
        console.log("  Treasury received: $100 USDC");
        console.log("  Buyer received: 100 tokens");
    }

    /**
     * @notice Test complete ETH payment flow
     * Verifies: buyer pays ETH -> contract holds it -> owner can withdraw
     */
    function test_ETHPaymentFlow() public {
        // 1. Consigner creates consignment
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId,
            1000e18,
            false, 0, 0, 0, 0, 0, 0, 100e18, 1000e18, 500
        );
        vm.stopPrank();

        // 2. Buyer creates offer for 100 tokens, paying in ETH
        // At $1/token and $2000/ETH, 100 tokens = $100 = 0.05 ETH
        vm.startPrank(buyer);
        uint256 offerId = otc.createOfferFromConsignment(
            1, 100e18, 0, OTC.PaymentCurrency.ETH, 0
        );
        vm.stopPrank();

        // 3. Approver approves
        vm.prank(approver);
        otc.approveOffer(offerId);

        // 4. Calculate required ETH
        uint256 requiredEth = otc.requiredEthWei(offerId);
        assertGt(requiredEth, 0, "Should require ETH payment");

        // 5. Buyer pays with ETH
        uint256 contractEthBefore = address(otc).balance;
        vm.prank(buyer);
        otc.fulfillOffer{value: requiredEth}(offerId);
        
        // Verify contract received ETH
        uint256 contractEthAfter = address(otc).balance;
        assertEq(contractEthAfter - contractEthBefore, requiredEth, "Contract should hold ETH payment");

        // 6. Owner withdraws ETH to treasury
        uint256 treasuryEthBefore = treasury.balance;
        vm.prank(owner);
        otc.withdrawStable(treasury, 0, requiredEth);
        uint256 treasuryEthAfter = treasury.balance;

        // Verify treasury received ETH
        assertEq(treasuryEthAfter - treasuryEthBefore, requiredEth, "Treasury should receive ETH");

        // 7. Buyer claims tokens
        vm.prank(buyer);
        otc.claim(offerId);
        assertEq(token.balanceOf(buyer), 100e18, "Buyer should have 100 tokens");

        console.log("ETH Payment Flow Complete:");
        console.log("  Buyer paid:", requiredEth, "wei");
        console.log("  Treasury received:", requiredEth, "wei");
        console.log("  Buyer received: 100 tokens");
    }

    /**
     * @notice Test multiple offers accumulate payments
     */
    function test_MultiplePaymentsAccumulate() public {
        // Setup consignment
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 1000e18, false, 0, 0, 0, 0, 0, 0, 100e18, 1000e18, 500
        );
        vm.stopPrank();

        // Create and pay for 3 offers
        for (uint256 i = 0; i < 3; i++) {
            vm.startPrank(buyer);
            uint256 offerId = otc.createOfferFromConsignment(
                1, 100e18, 0, OTC.PaymentCurrency.USDC, 0
            );
            vm.stopPrank();

            vm.prank(approver);
            otc.approveOffer(offerId);

            vm.startPrank(buyer);
            usdc.approve(address(otc), 100e6);
            otc.fulfillOffer(offerId);
            vm.stopPrank();
        }

        // Verify contract accumulated all payments
        uint256 totalPayments = usdc.balanceOf(address(otc));
        assertEq(totalPayments, 300e6, "Contract should hold $300 USDC from 3 offers");

        // Owner can withdraw all at once
        vm.prank(owner);
        otc.withdrawStable(treasury, 300e6, 0);
        assertEq(usdc.balanceOf(treasury), 300e6, "Treasury should receive $300 USDC");
    }

    /**
     * @notice Test only owner can withdraw
     */
    function test_OnlyOwnerCanWithdraw() public {
        // Setup and pay
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 1000e18, false, 0, 0, 0, 0, 0, 0, 100e18, 1000e18, 500
        );
        vm.stopPrank();

        vm.startPrank(buyer);
        uint256 offerId = otc.createOfferFromConsignment(1, 100e18, 0, OTC.PaymentCurrency.USDC, 0);
        vm.stopPrank();

        vm.prank(approver);
        otc.approveOffer(offerId);

        vm.startPrank(buyer);
        usdc.approve(address(otc), 100e6);
        otc.fulfillOffer(offerId);
        vm.stopPrank();

        // Non-owner cannot withdraw
        vm.prank(buyer);
        vm.expectRevert();
        otc.withdrawStable(buyer, 100e6, 0);

        vm.prank(consigner);
        vm.expectRevert();
        otc.withdrawStable(consigner, 100e6, 0);

        // Owner can withdraw
        vm.prank(owner);
        otc.withdrawStable(treasury, 100e6, 0);
        assertEq(usdc.balanceOf(treasury), 100e6);
    }

    /**
     * @notice Test discount applied correctly
     */
    function test_DiscountAppliedToPayment() public {
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        // Create consignment with 10% discount
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 1000e18, false, 
            1000, // 10% discount
            0,    // no lockup
            0, 0, 0, 0, 100e18, 1000e18, 500
        );
        vm.stopPrank();

        vm.startPrank(buyer);
        uint256 offerId = otc.createOfferFromConsignment(
            1, 100e18, 1000, OTC.PaymentCurrency.USDC, 0
        );
        vm.stopPrank();

        vm.prank(approver);
        otc.approveOffer(offerId);

        // At $1/token with 10% discount: 100 tokens = $90
        vm.startPrank(buyer);
        uint256 buyerBefore = usdc.balanceOf(buyer);
        usdc.approve(address(otc), 100e6);
        otc.fulfillOffer(offerId);
        uint256 buyerAfter = usdc.balanceOf(buyer);
        vm.stopPrank();

        uint256 paid = buyerBefore - buyerAfter;
        assertEq(paid, 90e6, "Buyer should pay $90 USDC (10% discount)");
        
        console.log("Discount test:");
        console.log("  Token price: $1");
        console.log("  Discount: 10%");
        console.log("  Expected payment: $90");
        console.log("  Actual payment: $", paid / 1e6);
    }
}

