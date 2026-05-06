// SPDX-License-Identifier: MIT
pragma solidity ^0.8.26;

import {Test, console} from "forge-std/Test.sol";
import {SimplePoolOracle} from "../contracts/SimplePoolOracle.sol";
import {MockERC20} from "../contracts/MockERC20.sol";
import {MockAggregatorV3} from "../contracts/mocks/MockAggregator.sol";

contract MockUniswapV3Pool {
    address public token0;
    address public token1;
    int24 public currentTick;
    int56 public tickCumulative;
    uint32 public lastTime;
    
    int24 public observeTick;
    
    constructor(address _token0, address _token1) {
        token0 = _token0;
        token1 = _token1;
    }
    
    function setTick(int24 _tick) external {
        currentTick = _tick;
        observeTick = _tick;
    }
    
    function setSpotTick(int24 _tick) external {
        currentTick = _tick;
    }
    
    function observe(uint32[] calldata secondsAgos) 
        external view returns (int56[] memory tickCumulatives, uint160[] memory secondsPerLiquidityCumulativeX128s) 
    {
        tickCumulatives = new int56[](secondsAgos.length);
        secondsPerLiquidityCumulativeX128s = new uint160[](secondsAgos.length);
        
        for(uint i=0; i<secondsAgos.length; i++) {
            if (secondsAgos[i] == 0) {
                tickCumulatives[i] = int56(observeTick) * 1000; 
            } else {
                tickCumulatives[i] = int56(observeTick) * 1000 - (int56(observeTick) * int56(uint56(secondsAgos[i])));
            }
        }
    }
    
    function slot0() external view returns (
        uint160 sqrtPriceX96,
        int24 tick,
        uint16 observationIndex,
        uint16 observationCardinality,
        uint16 observationCardinalityNext,
        uint8 feeProtocol,
        bool unlocked
    ) {
        return (0, currentTick, 0, 0, 0, 0, true);
    }
}

contract SimplePoolOracleTest is Test {
    SimplePoolOracle public oracle;
    MockUniswapV3Pool public pool;
    MockERC20 public token;
    MockERC20 public usdc;
    MockAggregatorV3 public ethFeed;
    
    function setUp() public {
        token = new MockERC20("Test", "TST", 18, 1000e18);
        usdc = new MockERC20("USDC", "USDC", 6, 1000e6);
        ethFeed = new MockAggregatorV3(8, 3000e8);
        
        pool = new MockUniswapV3Pool(address(token), address(usdc));
        
        // Tick for 1:1 price (if decimals equal)
        // 1.0001^tick = price.
        // TST (18) / USDC (6).
        // Price = 1 USDC per TST?
        // 1 TST = 1e18 units. 1 USDC = 1e6 units.
        // price = (sqrtPrice / 2^96)^2 * 10^18 / 10^6? No.
        // Logic in Oracle:
        // isToken0 = (token0 == target) = true.
        // price = priceX192 * 10^18 / 2^192 / 10^6.
        
        // Let's just set a tick and see what happens.
        pool.setTick(0); // Tick 0 => ratio 1.
        
        oracle = new SimplePoolOracle(address(pool), address(token), address(ethFeed));
    }
    
    function test_PriceCalculation() public {
        // At tick 0, sqrtPrice is 2^96. Ratio 1.
        // token0 = TST (18), token1 = USDC (6).
        // Price = 1 * 10^18 / 10^6 = 10^12.
        // 1 TST = 10^12 base units (USDC units?).
        // Wait. Oracle logic:
        // isUSDC = true.
        // price (USD 8 decimals) = basePerTarget * 1e8 / 1e6.
        // basePerTarget = 10^12.
        // price = 10^12 * 1e2 = 10^14.
        // $1,000,000. High.
        
        // Try tick corresponding to $1.
        // $1 = 1 USDC per TST.
        // base units = 1e6.
        // target units = 1e18.
        // ratio = 1e6 / 1e18 = 1e-12.
        // 1.0001^tick = 1e-12.
        // tick ~= log(1e-12) / log(1.0001) ~= -276310.
        
        pool.setTick(-276325); 
        
        uint256 price = oracle.getTWAPPrice();
        console.log("Price:", price);
        // Should be close to 1e8 ($1).
        assertApproxEqRel(price, 1e8, 0.01e18); // 1% tolerance
    }
    
    function test_SpotDeviationRevert() public {
        pool.setTick(-276325); // $1
        oracle.getTWAPPrice(); // Should pass
        
        // Move spot tick drastically (flash loan)
        // -276325 is $1. 
        // If we move to tick 0 (Ratio 1 => Price $1M), deviation is huge.
        pool.setSpotTick(0);
        
        vm.expectRevert("spot price deviation high");
        oracle.getTWAPPrice();
    }
}

