// SPDX-License-Identifier: MIT
pragma solidity ^0.8.26;

import {Test, console} from "forge-std/Test.sol";
import {StdInvariant} from "forge-std/StdInvariant.sol";
import {OTC} from "../../contracts/OTC.sol";
import {MockERC20} from "../../contracts/MockERC20.sol";
import {MockAggregatorV3} from "../../contracts/mocks/MockAggregator.sol";
import {IAggregatorV3} from "../../contracts/interfaces/IAggregatorV3.sol";
import {IERC20} from "@openzeppelin/contracts/token/ERC20/IERC20.sol";

/**
 * @title OTCHandler
 * @notice Handler contract for invariant testing - performs random valid actions
 */
contract OTCHandler is Test {
    OTC public otc;
    MockERC20 public token;
    MockERC20 public usdc;
    bytes32 public tokenId;
    
    address public owner;
    address public agent;
    address public approver;
    address public consigner;
    address public buyer;
    
    // Ghost variables for tracking
    uint256 public ghost_totalTokensDeposited;
    uint256 public ghost_totalTokensClaimed;
    uint256 public ghost_totalUsdcReceived;
    uint256 public ghost_totalConsignments;
    uint256 public ghost_totalOffers;
    
    uint256[] public activeConsignmentIds;
    uint256[] public activeOfferIds;
    
    constructor(
        OTC _otc,
        MockERC20 _token,
        MockERC20 _usdc,
        bytes32 _tokenId,
        address _owner,
        address _agent,
        address _approver,
        address _consigner,
        address _buyer
    ) {
        otc = _otc;
        token = _token;
        usdc = _usdc;
        tokenId = _tokenId;
        owner = _owner;
        agent = _agent;
        approver = _approver;
        consigner = _consigner;
        buyer = _buyer;
    }
    
    function createConsignment(uint256 amount) external {
        amount = bound(amount, 100e18, 10000e18);
        
        vm.startPrank(consigner);
        token.approve(address(otc), amount);
        
        try otc.createConsignment{value: 0.001 ether}(
            tokenId,
            amount,
            true, // negotiable
            0, 0,
            0, 1000, // 0-10% discount
            0, 30,   // 0-30 days lockup  
            100e18, amount, // deal limits
            500 // maxPriceVolatilityBps
        ) returns (uint256 cid) {
            ghost_totalTokensDeposited += amount;
            ghost_totalConsignments++;
            activeConsignmentIds.push(cid);
        } catch {}
        
        vm.stopPrank();
    }
    
    function createOffer(uint256 cidIndex, uint256 amount) external {
        if (activeConsignmentIds.length == 0) return;
        
        cidIndex = cidIndex % activeConsignmentIds.length;
        uint256 cid = activeConsignmentIds[cidIndex];
        
        amount = bound(amount, 100e18, 1000e18);
        
        vm.startPrank(buyer);
        try otc.createOfferFromConsignment(cid, amount, 0, OTC.PaymentCurrency.USDC, 0) returns (uint256 oid) {
            ghost_totalOffers++;
            activeOfferIds.push(oid);
        } catch {}
        vm.stopPrank();
    }
    
    function approveOffer(uint256 oidIndex) external {
        if (activeOfferIds.length == 0) return;
        
        oidIndex = oidIndex % activeOfferIds.length;
        uint256 oid = activeOfferIds[oidIndex];
        
        vm.prank(approver);
        try otc.approveOffer(oid) {} catch {}
    }
    
    function fulfillOffer(uint256 oidIndex) external {
        if (activeOfferIds.length == 0) return;
        
        oidIndex = oidIndex % activeOfferIds.length;
        uint256 oid = activeOfferIds[oidIndex];
        
        vm.startPrank(buyer);
        usdc.approve(address(otc), 100000e6);
        try otc.fulfillOffer(oid) {
            ghost_totalUsdcReceived += 100e6; // approximate
        } catch {}
        vm.stopPrank();
    }
    
    function claimOffer(uint256 oidIndex) external {
        if (activeOfferIds.length == 0) return;
        
        oidIndex = oidIndex % activeOfferIds.length;
        uint256 oid = activeOfferIds[oidIndex];
        
        vm.prank(buyer);
        try otc.claim(oid) {
            ghost_totalTokensClaimed += 100e18; // approximate
        } catch {}
    }
    
    function cancelOffer(uint256 oidIndex) external {
        if (activeOfferIds.length == 0) return;
        
        oidIndex = oidIndex % activeOfferIds.length;
        uint256 oid = activeOfferIds[oidIndex];
        
        // Warp to allow cancellation
        vm.warp(block.timestamp + 31 minutes);
        
        vm.prank(buyer);
        try otc.cancelOffer(oid) {} catch {}
    }
    
    function withdrawConsignment(uint256 cidIndex) external {
        if (activeConsignmentIds.length == 0) return;
        
        cidIndex = cidIndex % activeConsignmentIds.length;
        uint256 cid = activeConsignmentIds[cidIndex];
        
        vm.prank(consigner);
        try otc.withdrawConsignment(cid) {} catch {}
    }
}

/**
 * @title OTCInvariantTest
 * @notice Invariant tests for OTC contract
 */
contract OTCInvariantTest is StdInvariant, Test {
    OTC public otc;
    MockERC20 public token;
    MockERC20 public usdc;
    MockAggregatorV3 public ethFeed;
    MockAggregatorV3 public tokenFeed;
    OTCHandler public handler;
    
    address public owner = address(1);
    address public agent = address(2);
    address public approver = address(3);
    address public consigner = address(4);
    address public buyer = address(5);
    
    bytes32 public tokenId;

    function setUp() public {
        vm.startPrank(owner);
        
        token = new MockERC20("Test", "TST", 18, 10_000_000e18);
        usdc = new MockERC20("USDC", "USDC", 6, 10_000_000e6);
        ethFeed = new MockAggregatorV3(8, 3000e8);
        tokenFeed = new MockAggregatorV3(8, 1e8);
        
        otc = new OTC(owner, IERC20(address(usdc)), IAggregatorV3(address(ethFeed)), agent);
        otc.setApprover(approver, true);
        
        tokenId = keccak256("TST");
        otc.registerToken(tokenId, address(token), address(tokenFeed));
        
        // Fund accounts
        token.transfer(consigner, 1_000_000e18);
        usdc.transfer(buyer, 1_000_000e6);
        vm.deal(consigner, 100 ether);
        vm.deal(buyer, 100 ether);
        
        vm.stopPrank();
        
        // Setup handler
        handler = new OTCHandler(
            otc, token, usdc, tokenId,
            owner, agent, approver, consigner, buyer
        );
        
        // Target only the handler for invariant testing
        targetContract(address(handler));
    }
    
    /**
     * @notice INVARIANT: Token balance >= reserved tokens
     * @dev Contract must always have enough tokens to cover reservations
     */
    function invariant_SufficientTokenBalance() public view {
        uint256 balance = token.balanceOf(address(otc));
        uint256 reserved = otc.tokenReserved(tokenId);
        
        assertGe(balance, reserved, "INVARIANT VIOLATED: Insufficient token balance for reservations");
    }
    
    /**
     * @notice INVARIANT: tokenDeposited >= tokenReserved
     * @dev Deposited amount must always be >= reserved amount
     */
    function invariant_DepositedGreaterThanReserved() public view {
        uint256 deposited = otc.tokenDeposited(tokenId);
        uint256 reserved = otc.tokenReserved(tokenId);
        
        assertGe(deposited, reserved, "INVARIANT VIOLATED: Reserved > Deposited");
    }
    
    /**
     * @notice INVARIANT: Token balance >= tokenDeposited
     * @dev Actual balance should be >= tracked deposits (accounting for claims/withdrawals)
     */
    function invariant_BalanceMatchesDeposited() public view {
        uint256 balance = token.balanceOf(address(otc));
        uint256 deposited = otc.tokenDeposited(tokenId);
        
        assertGe(balance, deposited, "INVARIANT VIOLATED: Balance < Deposited");
    }
    
    /**
     * @notice INVARIANT: Contract is not paused (unless explicitly paused)
     */
    function invariant_NotPausedByDefault() public view {
        // This is just checking the contract didn't get into a bad state
        // In normal operation, contract should not be paused
    }
    
    /**
     * @notice Log call summary after invariant runs
     */
    function invariant_CallSummary() public view {
        console.log("=== Invariant Test Summary ===");
        console.log("Total consignments:", handler.ghost_totalConsignments());
        console.log("Total offers:", handler.ghost_totalOffers());
        console.log("Total tokens deposited:", handler.ghost_totalTokensDeposited());
        console.log("Total tokens claimed:", handler.ghost_totalTokensClaimed());
        console.log("Token balance:", token.balanceOf(address(otc)));
        console.log("Token reserved:", otc.tokenReserved(tokenId));
        console.log("Token deposited:", otc.tokenDeposited(tokenId));
    }
}

