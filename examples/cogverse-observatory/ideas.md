# CogVerse Village Dashboard — Design Brainstorm

## Three Approaches

### 1. Retro Terminal Matrix
- **Very Brief Intro**: Full terminal/CRT aesthetic — green phosphor text on black, scanlines, blinking cursors. Data presented as raw terminal output.
- **Probability**: 0.04

### 2. Isometric Pixel Observatory
- **Very Brief Intro**: Dark space observatory with the floating islands hero as the centerpiece, surrounded by glowing data panels that feel like control consoles in a pixel-art space station. Neon accents pulse with live data.
- **Probability**: 0.07

### 3. Brutalist Data Shrine
- **Very Brief Intro**: Raw concrete textures with harsh monospace type, exposed grid lines, and data presented as sacred artifacts in a digital temple.
- **Probability**: 0.03

---

## Chosen Approach: Isometric Pixel Observatory

### Design Movement
Dark cyberpunk observatory — inspired by sci-fi bridge consoles, retro pixel art games, and astronomical observation decks. The village is observed from a cosmic vantage point.

### Core Principles
1. **Data as living light** — every metric glows, pulses, or flows; nothing is static
2. **Spatial hierarchy** — the hero image is the village itself; panels orbit it like instruments
3. **Pixel precision** — pixel fonts for labels, monospace for data, clean geometric shapes for containers
4. **Cosmic depth** — starfield background creates infinite depth; panels float at different z-levels

### Color Philosophy
The palette mirrors the village topology: **cyan (#00f0ff)** for CogCity/compute/cold intelligence, **amber (#ffb347)** for CogHood/warmth/origin. Against absolute black (#050510), these two temperatures create the emotional axis of the entire village — growth vs. home, expansion vs. return.

### Layout Paradigm
**Orbital layout** — the hero image sits at the gravitational center. Data panels orbit around it in a loose constellation pattern. On mobile they stack vertically; on desktop they float in asymmetric positions around the hero. No grid. No sidebar. The village is the center of gravity.

### Signature Elements
1. **Pulsing gear indicators** — circular SVG elements that rotate at speeds proportional to actual gear train RPMs
2. **Thought stream** — a scrolling ticker of resident thoughts that flows across the bottom like a news crawl, but with pixel-art speech bubbles
3. **Attentional Focus halo** — residents in the AF get a glowing ring that brightens with their STI value

### Interaction Philosophy
Hover reveals depth — hovering a resident shows their episodic memory; hovering a gear train shows its members. Click triggers stimulus — clicking a resident sends a stimulus event to the bus. The dashboard is not just observation — it's participation.

### Animation
- Starfield: 60 dots drifting slowly upward (parallax layers at 0.2px/s, 0.5px/s, 1px/s)
- Gear indicators: CSS rotation with `animation-duration` bound to RPM data
- Thought stream: horizontal scroll with `@keyframes` marquee, pausing on hover
- Data updates: numbers count up/down with easing when new data arrives
- Resident avatars: subtle `animate-pulse` when in Attentional Focus
- Panel entrance: `scale(0.95) → scale(1)` with `opacity 0→1` staggered 80ms per panel

### Typography System
- **Headings**: "Press Start 2P" — pixel font, uppercase, letter-spacing 2px
- **Data values**: "Space Mono" — monospace, bold, for all numbers and metrics
- **Body text**: "Space Mono" — regular weight, for descriptions and thought content
- **Labels**: "Press Start 2P" — 10px, muted cyan, uppercase

### Brand Essence
**CogVerse Observatory** — a live window into a cognitive village where AI residents think, converse, and create together. For builders of autonomous systems who want to see their village breathe. Different because it shows real-time cognitive state, not just logs.

Personality: **observant, alive, cosmic**

### Brand Voice
Headlines sound like astronomical observations or ship's log entries. CTAs are invitations to participate, not commands.

- "Nine minds. Five gear trains. One village."
- "The bridge hums both ways."

Ban: "Welcome to", "Get started", "Explore our", "Innovative solution"

### Wordmark & Logo
A stylized gear with 9 teeth (one per resident), the center void forming a subtle eye/lens shape. Rendered in pixel art at 32x32, glowing cyan.

### Signature Brand Color
**#00f0ff** — electric cyan. The color of active thought, live connections, and the bridge between islands.

## Style Decisions
- Background is always #050510 — never lighter
- All panels have `border: 1px dashed` with low-opacity cyan
- No rounded corners > 2px — pixel precision
- Data that changes gets a 200ms count-up animation
- Residents in AF pulse; others are dim
