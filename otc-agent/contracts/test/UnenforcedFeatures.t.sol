// SPDX-License-Identifier: MIT
pragma solidity ^0.8.26;

import {Test, console} from "forge-std/Test.sol";
import {OTC} from "../contracts/OTC.sol";
import {MockERC20} from "../contracts/MockERC20.sol";
import {MockAggregatorV3} from "../contracts/mocks/MockAggregator.sol";
import {IAggregatorV3} from "../contracts/interfaces/IAggregatorV3.sol";
import {IERC20} from "@openzeppelin/contracts/token/ERC20/IERC20.sol";

/**
 * @title PotentialImprovementsTest
 * @notice Tests demonstrating potential improvements and design decisions
 */
contract PotentialImprovementsTest is Test {
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
     * @notice Test: Anyone can pay for someone else's offer (by design?)
     * @dev If restrictFulfillToBeneficiaryOrApprover is false, anyone can pay
     *      This might be intentional (gift purchases) or unintended
     */
    function test_DESIGN_AnyoneCanPayForOffer() public {
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 1000e18, false, 0, 0, 0, 0, 0, 0, 100e18, 1000e18, 500
        );
        vm.stopPrank();
        
        // Buyer creates offer
        vm.prank(buyer);
        uint256 offerId = otc.createOfferFromConsignment(1, 100e18, 0, OTC.PaymentCurrency.USDC, 0);
        
        vm.prank(approver);
        otc.approveOffer(offerId);
        
        // Random person pays for buyer's offer
        address randomPayer = address(100);
        vm.prank(owner);
        usdc.transfer(randomPayer, 100e6);
        
        vm.startPrank(randomPayer);
        usdc.approve(address(otc), 100e6);
        otc.fulfillOffer(offerId);
        vm.stopPrank();
        
        // Buyer (beneficiary) still gets the tokens
        vm.prank(buyer);
        otc.claim(offerId);
        
        assertEq(token.balanceOf(buyer), 100e18, "Buyer (beneficiary) received tokens");
        
        console.log("NOTE: Third party paid for buyer's offer");
        console.log("Buyer received tokens even though they didn't pay");
        console.log("This may be intentional (gift feature) or unintended");
    }

    /**
     * @notice Test: requiredApprovals can be changed mid-offer
     * @dev Changing requiredApprovals affects all existing unapproved offers
     *      This could be problematic for consistency
     */
    function test_DESIGN_RequiredApprovalsCanChangeMidOffer() public {
        vm.startPrank(owner);
        otc.setRequiredApprovals(2);
        address approver2 = address(10);
        otc.setApprover(approver2, true);
        vm.stopPrank();
        
        vm.startPrank(consigner);
        token.approve(address(otc), 1000e18);
        otc.createConsignment{value: 0.001 ether}(
            tokenId, 1000e18, false, 0, 0, 0, 0, 0, 0, 100e18, 1000e18, 500
        );
        vm.stopPrank();
        
        vm.prank(buyer);
        uint256 offerId = otc.createOfferFromConsignment(1, 100e18, 0, OTC.PaymentCurrency.USDC, 0);
        
        // First approval
        vm.prank(approver);
        otc.approveOffer(offerId);
        
        // Offer not yet approved (needs 2)
        (,,,,,,,,,,, bool approved,,,,,) = otc.offers(offerId);
        assertFalse(approved, "Not yet approved with 1 of 2");
        
        // Admin changes requirement to 1
        vm.prank(owner);
        otc.setRequiredApprovals(1);
        
        // Offer is STILL not approved (requires another approval call to trigger check)
        (,,,,,,,,,,, approved,,,,,) = otc.offers(offerId);
        assertFalse(approved, "Still not approved after requirement change");
        
        // But next approval from same approver fails
        vm.prank(approver);
        vm.expectRevert("already approved by you");
        otc.approveOffer(offerId);
        
        // Different approver can push it through
        vm.prank(approver2);
        otc.approveOffer(offerId);
        
        (,,,,,,,,,,, approved,,,,,) = otc.offers(offerId);
        assertTrue(approved, "Now approved");
        
        console.log("NOTE: Changing requiredApprovals mid-offer requires additional approval");
        console.log("Even if new requirement is met, offer won't auto-approve");
    }
}
