# Ontogenesis: Self-Generating Kernels

## Overview

Ontogenesis is a C++ implementation of **self-generating, evolving kernels** through recursive application of differential operators. It enables kernels to:

1. **Generate themselves** through recursive self-composition
2. **Optimize themselves** through iterative improvement
3. **Reproduce** with other kernels through genetic operations
4. **Evolve** across generations to maximize fitness

## Concept

The term "ontogenesis" refers to the process of origin and development of an organism. In the context of ElizaOS C++, ontogenesis brings biological evolution to mathematical structures:

- **Kernels as Organisms**: Mathematical kernels become living entities with DNA (B-series coefficients)
- **Evolution as Optimization**: Natural selection drives kernels toward optimal computational performance
- **Genetics as Structure**: Differential operators serve as reproductive mechanisms
- **Fitness as Performance**: Grip, stability, and efficiency measure kernel quality

## Architecture

### Core Data Structures

#### 1. Ontogenetic Kernel

```cpp
struct OntogeneticKernel {
    UUID id;                           // Unique identifier
    KernelGenome genome;               // Genetic information
    OntogeneticState ontogeneticState; // Development state
    BSeries bseries;                   // B-series coefficients
};
```

#### 2. Kernel Genome

The "DNA" of a kernel:

```cpp
struct KernelGenome {
    UUID id;                         // Unique identifier
    int generation;                  // Generation number
    std::vector<UUID> lineage;       // Parent IDs
    std::vector<KernelGene> genes;   // Genetic information
    double fitness;                  // Overall fitness score
    int age;                         // Age in generations
};
```

#### 3. Development Stages

Kernels progress through life stages:

```cpp
enum class DevelopmentStage {
    EMBRYONIC,   // Just generated, basic structure
    JUVENILE,    // Developing, optimizing
    MATURE,      // Fully developed, capable of reproduction
    SENESCENT    // Declining, ready for replacement
};
```

## Quick Start

### Basic Example: Self-Generation

```cpp
#include "elizaos/ontogenesis.hpp"
using namespace elizaos::ontogenesis;

// Create a parent kernel
auto parent = initializeOntogeneticKernel(4);

// Generate offspring through self-composition
auto offspring = selfGenerate(parent);

std::cout << "Parent: " << parent.genome.fitness << std::endl;
std::cout << "Offspring: " << offspring.genome.fitness << std::endl;
```

### Optimization Example

```cpp
// Optimize a kernel through iterative improvement
auto kernel = initializeOntogeneticKernel(4);
auto optimized = selfOptimize(kernel, 20);

std::cout << "Initial fitness: " << kernel.genome.fitness << std::endl;
std::cout << "Optimized fitness: " << optimized.genome.fitness << std::endl;
```

### Reproduction Example

```cpp
// Create two parent kernels
auto parent1 = initializeOntogeneticKernel(4);
auto parent2 = initializeOntogeneticKernel(4);

// Reproduce via crossover
auto child = selfReproduce(parent1, parent2, ReproductionMethod::CROSSOVER);

std::cout << "Child generation: " << child.genome.generation << std::endl;
std::cout << "Child fitness: " << child.genome.fitness << std::endl;
```

## Evolution System

### Basic Evolution

```cpp
// Configure evolution
OntogenesisConfig config;
config.evolution.populationSize = 20;
config.evolution.mutationRate = 0.15;
config.evolution.crossoverRate = 0.7;
config.evolution.maxGenerations = 50;

// Run evolution
auto result = runOntogenesis(config);

// Analyze results
std::cout << "Best fitness: " << result.bestKernel.genome.fitness << std::endl;
std::cout << "Converged: " << (result.converged ? "YES" : "NO") << std::endl;
```

### Evolution with Seed Kernels

```cpp
OntogenesisConfig config;
config.evolution.populationSize = 20;

// Provide seed kernels
config.seedKernels.push_back(initializeOntogeneticKernel(2));
config.seedKernels.push_back(initializeOntogeneticKernel(4));

auto result = runOntogenesis(config);
```

### Tracking Progress

```cpp
auto result = runOntogenesis(config);

for (const auto& gen : result.generations) {
    std::cout << "Gen " << gen.generation 
              << ": Best=" << gen.bestFitness
              << ", Avg=" << gen.averageFitness
              << ", Diversity=" << gen.diversity
              << std::endl;
}
```

## Key Operations

### 1. Initialization

Creates a new ontogenetic kernel:

```cpp
// Order 1: Euler method
auto kernel1 = initializeOntogeneticKernel(1);

// Order 4: Classical Runge-Kutta
auto kernel4 = initializeOntogeneticKernel(4);
```

### 2. Self-Generation

Generates offspring through recursive self-composition using the chain rule:

`(f∘f)' = f'(f(x)) · f'(x)`

```cpp
auto parent = initializeOntogeneticKernel(4);
auto child = selfGenerate(parent);

// Child inherits parent's lineage
assert(child.genome.generation == parent.genome.generation + 1);
assert(child.genome.lineage.back() == parent.genome.id);
```

### 3. Self-Optimization

Optimizes grip through iterative improvement:

```cpp
auto kernel = initializeOntogeneticKernel(4);
auto optimized = selfOptimize(kernel, 10);  // 10 iterations

// Maturity increases during optimization
assert(optimized.ontogeneticState.maturity > kernel.ontogeneticState.maturity);
```

### 4. Self-Reproduction

Combines two kernels using genetic operators:

#### Crossover

```cpp
auto child = selfReproduce(parent1, parent2, ReproductionMethod::CROSSOVER);
```

Single-point crossover on coefficient arrays:

```
Parent 1: [c1, c2, c3, c4, c5]
Parent 2: [d1, d2, d3, d4, d5]
          ^^^^^^^
           point

Child 1:  [c1, c2, c3, d4, d5]
Child 2:  [d1, d2, d3, c4, c5]
```

#### Mutation

```cpp
auto child = selfReproduce(parent1, parent2, ReproductionMethod::MUTATION);
```

Random perturbation: `coeff[i] += (random() - 0.5) * 0.2`

#### Cloning

```cpp
auto child = selfReproduce(parent1, parent2, ReproductionMethod::CLONING);
```

Direct copy of genome with new ID.

## Genetic Operations

### Gene Types

Kernels have four types of genes:

1. **Coefficient Genes**: B-series coefficients (mutable)
2. **Operator Genes**: Differential operators (mutable)
3. **Symmetry Genes**: Domain symmetries (immutable)
4. **Preservation Genes**: Conserved quantities (immutable)

### Genetic Distance

Measure similarity between kernels:

```cpp
auto kernel1 = initializeOntogeneticKernel(4);
auto kernel2 = initializeOntogeneticKernel(4);

double distance = geneticDistance(kernel1.genome, kernel2.genome);
// 0.0 = identical, 1.0 = maximum difference
```

## Fitness Evaluation

### Fitness Components

Fitness is a weighted combination:

```
fitness = 
  grip * 0.4 +          // Quality of domain fit
  stability * 0.2 +     // Numerical stability
  efficiency * 0.2 +    // Computational efficiency
  novelty * 0.1 +       // Genetic diversity
  symmetry * 0.1        // Symmetry preservation
```

### Grip Evaluation

Measures how well the kernel fits the domain:

```cpp
auto grip = evaluateGrip(kernel);

std::cout << "Contact: " << grip.contact << std::endl;
std::cout << "Coverage: " << grip.coverage << std::endl;
std::cout << "Efficiency: " << grip.efficiency << std::endl;
std::cout << "Stability: " << grip.stability << std::endl;
std::cout << "Overall: " << grip.overall << std::endl;
```

## Advanced Features

### Custom Fitness Function

Define domain-specific fitness:

```cpp
OntogenesisConfig config;
config.customFitnessFunction = [](const OntogeneticKernel& kernel) {
    double domainFit = /* evaluate domain fit */;
    double complexity = /* evaluate complexity */;
    return domainFit * 0.7 + complexity * 0.3;
};
```

### Development Schedule

Control stage transitions:

```cpp
DevelopmentSchedule schedule;
schedule.embryonicDuration = 2;    // Generations
schedule.juvenileDuration = 5;     // Generations
schedule.matureDuration = 10;      // Generations
schedule.maturityThreshold = 0.8;  // Fitness threshold
```

### Diversity Pressure

Maintain genetic diversity:

```cpp
config.evolution.diversityPressure = 0.2;  // 0.0 to 1.0
// Higher = more diverse population
```

## B-Series and Mathematical Foundation

### B-Series Expansion

The genetic code:

```
y_{n+1} = y_n + h * Σ b_i * Φ_i(f, y_n)
```

Where:
- `b_i`: coefficient genes
- `Φ_i`: elementary differentials (rooted trees)
- Trees follow A000081 sequence: 1, 1, 2, 4, 9, 20, 48, 115, ...

### Differential Operators

Reproduction mechanisms:

1. **Chain Rule** (Self-Composition):
   ```
   (f∘g)' = f'(g(x)) · g'(x)
   ```

2. **Product Rule** (Combination):
   ```
   (f·g)' = f'·g + f·g'
   ```

3. **Quotient Rule** (Refinement):
   ```
   (f/g)' = (f'·g - f·g')/g²
   ```

## Performance Characteristics

### Complexity

- **Initialization**: O(n) where n = coefficient count
- **Self-Generation**: O(n²)
- **Self-Optimization**: O(k·n) where k = iterations
- **Crossover**: O(n)
- **Mutation**: O(1)
- **Evolution**: O(g·p·n) where g = generations, p = population

### Memory

- **Kernel**: ~1KB (genome + state)
- **Population**: p × 1KB
- **History**: Limited to recent events

### Convergence

Typical evolution converges in 20-50 generations with:
- Population size: 20-50
- Mutation rate: 0.1-0.2
- Crossover rate: 0.7-0.9

## Examples

### Complete Examples

See the `examples/` directory for complete working examples:

1. **ontogenesis_simple_demo.cpp**: Basic self-generation and optimization
2. **ontogenesis_evolution_demo.cpp**: Multi-generation evolution
3. **ontogenesis_lineage_demo.cpp**: Lineage tracking and family trees

### Building and Running

```bash
# Build
cd build
make ontogenesis_simple_demo
make ontogenesis_evolution_demo
make ontogenesis_lineage_demo

# Run
./ontogenesis_simple_demo
./ontogenesis_evolution_demo
./ontogenesis_lineage_demo
```

## Integration with ElizaOS

Ontogenesis integrates with other ElizaOS components:

```cpp
// Include headers
#include "elizaos/ontogenesis.hpp"
#include "elizaos/core.hpp"

// Link library
// target_link_libraries(your_target elizaos-ontogenesis)
```

## API Reference

### Initialization

- `OntogeneticKernel initializeOntogeneticKernel(int order)`

### Core Operations

- `OntogeneticKernel selfGenerate(const OntogeneticKernel& parent)`
- `OntogeneticKernel selfOptimize(const OntogeneticKernel& kernel, int iterations = 10)`
- `OntogeneticKernel selfReproduce(const OntogeneticKernel& parent1, const OntogeneticKernel& parent2, ReproductionMethod method = ReproductionMethod::CROSSOVER)`

### Genetic Operations

- `std::pair<KernelGenome, KernelGenome> crossover(const KernelGenome& genome1, const KernelGenome& genome2)`
- `KernelGenome mutate(const KernelGenome& genome, double mutationRate)`
- `double geneticDistance(const KernelGenome& genome1, const KernelGenome& genome2)`
- `KernelGenome cloneGenome(const KernelGenome& genome)`

### Fitness Evaluation

- `GripEvaluation evaluateGrip(const OntogeneticKernel& kernel)`
- `double evaluateFitness(const OntogeneticKernel& kernel, const std::vector<OntogeneticKernel>* population = nullptr)`
- `double calculateNovelty(const OntogeneticKernel& kernel, const std::vector<OntogeneticKernel>& population)`

### Evolution

- `EvolutionResult runOntogenesis(const OntogenesisConfig& config)`
- `const OntogeneticKernel& tournamentSelect(const std::vector<OntogeneticKernel>& population, int tournamentSize = 3)`
- `void updateDevelopmentStage(OntogeneticKernel& kernel, const DevelopmentSchedule& schedule)`
- `double calculatePopulationDiversity(const std::vector<OntogeneticKernel>& population)`

### Utilities

- `UUID generateKernelId()`
- `std::string developmentStageToString(DevelopmentStage stage)`
- `std::string reproductionMethodToString(ReproductionMethod method)`

## Best Practices

### Population Size

- Small problems: 10-20 individuals
- Medium problems: 20-50 individuals
- Large problems: 50-100 individuals

### Mutation Rate

- Low diversity: 0.15-0.25
- Normal diversity: 0.10-0.15
- High diversity: 0.05-0.10

### Crossover Rate

- Exploitation: 0.5-0.6
- Balance: 0.7-0.8
- Exploration: 0.8-0.9

### Convergence

- Set `fitnessThreshold` based on problem
- Use `maxGenerations` as safety limit
- Monitor `diversity` to detect premature convergence

## Troubleshooting

### Slow Convergence

- Increase population size
- Increase mutation rate
- Adjust fitness function weights

### Premature Convergence

- Increase diversity pressure
- Reduce elitism rate
- Increase mutation rate

### Poor Fitness

- Check fitness function definition
- Verify grip evaluation
- Increase optimization iterations

## References

- Butcher, J.C. (2016). Numerical Methods for Ordinary Differential Equations
- Hairer, E., Nørsett, S.P., Wanner, G. (1993). Solving Ordinary Differential Equations I
- Holland, J.H. (1992). Adaptation in Natural and Artificial Systems
- von Neumann, J. (1966). Theory of Self-Reproducing Automata
- Cayley, A. (1857). On the Theory of the Analytical Forms called Trees (A000081)

## License

MIT License - see [LICENSE](../../LICENSE) for details.

---

**Ontogenesis**: Where mathematics becomes life, and kernels evolve themselves through the pure language of differential calculus.
