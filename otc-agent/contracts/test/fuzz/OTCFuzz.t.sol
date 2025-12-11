// SPDX-License-Identifier: MIT
pragma solidity ^0.8.26;

import {Test, console} from "forge-std/Test.sol";
import {OTC} from "../../contracts/OTC.sol";
import {MockERC20} from "../../contracts/MockERC20.sol";
import {MockAggregatorV3} from "../../contracts/mocks/MockAggregator.sol";
import {IAggregatorV3} from "../../contracts/interfaces/IAggregatorV3.sol";
import {IERC20} from "@openzeppelin/contracts/token/ERC20/IERC20.sol";

/**
 * @title OTCFuzzTest
 * @notice Fuzz tests for OTC contract edge cases
 */
contract OTCFuzzTest is Test {
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
        
        // Fund accounts generously
        token.transfer(consigner, 10_000_000e18);
        usdc.transfer(buyer, 10_000_000e6);
        vm.deal(consigner, 1000 ether);
        vm.deal(buyer, 1000 ether);
        
        vm.stopPrank();
    }

    /**
     * @notice Fuzz test consignment creation with various amounts
     */
    function testFuzz_CreateConsignment(uint256 amount) public {
        // Bound to reasonable range
        amount = bound(amount, 100e18, 1_000_000e18);
        
        vm.startPrank(consigner);
        token.approve(address(otc), amount);
        
        uint256 consignmentId = otc.createConsignment{value: 0.001 ether}(
            tokenId,
            amount,
            true,
            0, 0,
            0, 1000,
            0, 30,
            100e18, amount,
            500
        );
        
        (,, uint256 total, uint256 remaining,,,,,,,,,,,,) = otc.consignments(consignmentId);
        assertEq(total, amount);
        assertEq(remaining, amount);
        vm.stopPrank();
    }

    /**
     * @notice Fuzz test offer creation with various discounts
     */
    function testFuzz_CreateOfferWithDiscount(uint16 discountBps) public {
        // Bound discount to valid range (0-10%)
        discountBps = uint16(bound(discountBps, 0, 1000));
        
        // Setup consignment
        vm.startPrank(consigner);
        token.approve(address(otc), 10000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 10000e18, true, 0, 0, 0, 1000, 0, 30, 100e18, 10000e18, 500
        );
        vm.stopPrank();
        
        // Create offer with fuzzed discount
        vm.startPrank(buyer);
        uint256 offerId = otc.createOfferFromConsignment(
            1, 100e18, discountBps, OTC.PaymentCurrency.USDC, 0
        );
        
        (,,, uint256 tokenAmount, uint256 offerDiscount,,,,,,,,,,,,) = otc.offers(offerId);
        assertEq(tokenAmount, 100e18);
        assertEq(offerDiscount, discountBps);
        vm.stopPrank();
    }

    /**
     * @notice Fuzz test lockup period handling
     */
    function testFuzz_LockupPeriod(uint256 lockupDays) public {
        // Bound to valid range (0-30 days as configured)
        lockupDays = bound(lockupDays, 0, 30);
        uint256 lockupSeconds = lockupDays * 1 days;
        
        // Setup consignment with 0-30 day lockup range
        vm.startPrank(consigner);
        token.approve(address(otc), 10000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 10000e18, true, 0, 0, 0, 1000, 0, 30, 100e18, 10000e18, 500
        );
        vm.stopPrank();
        
        // Create offer with fuzzed lockup
        vm.startPrank(buyer);
        uint256 offerId = otc.createOfferFromConsignment(
            1, 100e18, 0, OTC.PaymentCurrency.USDC, lockupSeconds
        );
        
        (,,,,,, uint256 unlockTime,,,,,,,,,,) = otc.offers(offerId);
        assertEq(unlockTime, block.timestamp + lockupSeconds);
        vm.stopPrank();
    }

    /**
     * @notice Fuzz test price volatility checks
     */
    function testFuzz_PriceVolatility(uint256 newPriceRaw) public {
        // Bound price to reasonable range (avoid negative/zero)
        int256 newPrice = int256(bound(newPriceRaw, 1e6, 100e8));
        
        // Setup consignment with 5% max volatility (500 bps)
        vm.startPrank(consigner);
        token.approve(address(otc), 10000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 10000e18, false, 0, 0, 0, 0, 0, 0, 100e18, 10000e18, 500
        );
        vm.stopPrank();
        
        // Create offer at current price
        vm.startPrank(buyer);
        uint256 offerId = otc.createOfferFromConsignment(
            1, 100e18, 0, OTC.PaymentCurrency.USDC, 0
        );
        vm.stopPrank();
        
        // Change price before approval
        vm.prank(owner);
        tokenFeed.setAnswer(newPrice);
        
        // Calculate expected behavior
        int256 originalPrice = 1e8;
        uint256 priceDiff = newPrice > originalPrice ? 
            uint256(newPrice - originalPrice) : uint256(originalPrice - newPrice);
        uint256 deviationBps = (priceDiff * 10000) / uint256(originalPrice);
        
        vm.startPrank(approver);
        if (deviationBps <= 500) {
            // Should succeed
            otc.approveOffer(offerId);
        } else {
            // Should revert
            vm.expectRevert("price volatility exceeded");
            otc.approveOffer(offerId);
        }
        vm.stopPrank();
    }

    /**
     * @notice Fuzz test ETH payment amounts
     */
    function testFuzz_ETHPayment(uint256 tokenAmount) public {
        // Bound to minimum deal amount and reasonable max
        tokenAmount = bound(tokenAmount, 100e18, 10000e18);
        
        // Setup
        vm.startPrank(consigner);
        token.approve(address(otc), 100000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 100000e18, false, 0, 0, 0, 0, 0, 0, 100e18, 100000e18, 500
        );
        vm.stopPrank();
        
        // Create ETH offer
        vm.startPrank(buyer);
        uint256 offerId = otc.createOfferFromConsignment(
            1, tokenAmount, 0, OTC.PaymentCurrency.ETH, 0
        );
        vm.stopPrank();
        
        // Approve
        vm.prank(approver);
        otc.approveOffer(offerId);
        
        // Calculate required ETH
        uint256 requiredEth = otc.requiredEthWei(offerId);
        
        // Fulfill with exact amount
        vm.prank(buyer);
        otc.fulfillOffer{value: requiredEth}(offerId);
        
        // Verify paid
        (,,,,,,,,,,,, bool paid,,,, ) = otc.offers(offerId);
        assertTrue(paid);
    }

    /**
     * @notice Fuzz test multiple consignment interactions
     */
    function testFuzz_MultipleConsignments(uint8 numConsignments) public {
        // Bound to reasonable number
        numConsignments = uint8(bound(numConsignments, 1, 10));
        
        vm.startPrank(consigner);
        uint256 totalAmount = uint256(numConsignments) * 1000e18;
        token.approve(address(otc), totalAmount);
        
        for (uint256 i = 0; i < numConsignments; i++) {
            otc.createConsignment{value: 0.001 ether}(
                tokenId, 1000e18, false, 0, 0, 0, 0, 0, 0, 100e18, 1000e18, 500
            );
        }
        
        // Verify all created
        assertEq(otc.nextConsignmentId(), uint256(numConsignments) + 1);
        assertEq(otc.tokenDeposited(tokenId), totalAmount);
        vm.stopPrank();
    }

    /**
     * @notice Fuzz test cancellation timing
     */
    function testFuzz_CancellationTiming(uint256 waitTime) public {
        // Bound wait time
        waitTime = bound(waitTime, 0, 7 days);
        
        // Setup
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 1000e18, false, 0, 0, 0, 0, 0, 0, 100e18, 1000e18, 500
        );
        vm.stopPrank();
        
        vm.prank(buyer);
        uint256 offerId = otc.createOfferFromConsignment(1, 100e18, 0, OTC.PaymentCurrency.USDC, 0);
        
        // Try to cancel after wait time
        vm.warp(block.timestamp + waitTime);
        
        uint256 quoteExpiry = otc.quoteExpirySeconds();
        
        vm.startPrank(buyer);
        if (waitTime >= quoteExpiry) {
            // Should succeed - past expiry
            otc.cancelOffer(offerId);
            (,,,,,,,,,,,,,,bool cancelled,,) = otc.offers(offerId);
            assertTrue(cancelled);
        } else {
            // Should revert - not yet expired for buyer
            vm.expectRevert("not expired");
            otc.cancelOffer(offerId);
        }
        vm.stopPrank();
    }

    /**
     * @notice Fuzz test claim timing after lockup
     */
    function testFuzz_ClaimTiming(uint256 claimTime, uint256 lockupDays) public {
        lockupDays = bound(lockupDays, 0, 30);
        uint256 lockupSeconds = lockupDays * 1 days;
        claimTime = bound(claimTime, 0, 365 days);
        
        // Setup
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 1000e18, true, 0, 0, 0, 1000, 0, 30, 100e18, 1000e18, 500
        );
        vm.stopPrank();
        
        vm.startPrank(buyer);
        uint256 offerId = otc.createOfferFromConsignment(1, 100e18, 0, OTC.PaymentCurrency.USDC, lockupSeconds);
        vm.stopPrank();
        
        vm.prank(approver);
        otc.approveOffer(offerId);
        
        vm.startPrank(buyer);
        usdc.approve(address(otc), 100e6);
        otc.fulfillOffer(offerId);
        vm.stopPrank();
        
        // Try to claim after claimTime
        vm.warp(block.timestamp + claimTime);
        
        vm.startPrank(buyer);
        if (claimTime >= lockupSeconds) {
            // Should succeed
            otc.claim(offerId);
            assertEq(token.balanceOf(buyer), 100e18);
        } else {
            // Should revert
            vm.expectRevert("locked");
            otc.claim(offerId);
        }
        vm.stopPrank();
    }
}

