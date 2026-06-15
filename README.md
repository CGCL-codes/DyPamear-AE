# DyPamear: Efficient and Scalable Dynamic Pattern Mining with Practical Processing-in-Memory Architecture

**DyPamear** is an efficient **dynamic graph pattern mining (DGPM)** system for the UPMEM **Processing-in-Memory (PIM)** architecture.

The goal of DyPamear is to accelerate incremental pattern-aware execution of DGPM using UPMEM DPUs. It integrates **asynchronous pipelining**, **bitmap-based set intersection**, and other optimizations tailored for near-data processing architecture.

---

## 🚀 Key Features

- Skew-aware workload distribution for balanced execution across thousands of DPUs.
- Asynchronous loader–worker pipeline using WRAM FIFO.
- Bitmap-based set intersection acceleration on DPUs.
- Lightweight performance profiling and cycle analysis tools.

---

## 📁 Directory Structure

```
DyPamear-AE/
├── host/         # Host-side logic (C)
├── dpu/          # DPU-side programs (C for UPMEM)
├── python_tool/  # Python scripts for preprocessing and profiling
├── include/      # Shared headers
├── makefile      # Compilation rules
└── README.md     # Project description
```

---

## 🛠 Requirements

- **Linux environment**
- **UPMEM SDK v2025.1.0.**
- **GNU Make, C compiler (e.g., `gcc`)**
- **Python ≥ 3.8 (for analysis scripts)**

---

## ⚙️ Build and Run Instructions

To match a pattern within a graph, run:

```bash
make clean
GRAPH=<graph_name> PATTERN=<pattern_name> make test
```

Example:

```bash
GRAPH=wiki PATTERN=CLIQUE4 make test
```

> 💡 The available values for `GRAPH` and `PATTERN` are defined in `include/common.h`.  
> To add new graphs or patterns, modify `common.h` and recompile.

---

## 🧩 Customized Graphs and Matching Patterns

DyPamear supports flexible definitions of graph inputs and matching patterns.

All configuration entries are defined in `include/common.h`.

### ➕ Adding Custom Graphs

1. Place your input graph (in CSR binary format) and updated edges (.txt) into the `./data/` directory.
2. Add a macro definition in `include/common.h`:

```c
#if defined(AM0312)
#define DATA_NAME "amazon0312_adj"
#define N (1<<20)
#define M (1<<23)
#endif
```

3. Build and test:

```bash
GRAPH=AM0312 PATTERN=CLIQUE3 make test
```

## 📈 Scalability Testing

DyPamear is designed to scale from hundreds to tens of thousands of DPUs.

### 🔧 Custom DPU Count

To run DyPamear on a specific number of DPUs:

```bash
GRAPH=wiki PATTERN=CLIQUE3 EXTRA_FLAGS="-DV_NR_DPUS=5120" make test
```

### 📊 Full Scalability Sweep

To automatically benchmark DyPamear from **640** to **20,480** DPUs:

```bash
GRAPH=wiki PATTERN=CLIQUE4 make test_sc
```

This script:

- Compiles DyPamear with various DPU counts.
- Runs the benchmark for each configuration.

---

## 📊 Profiling & Visualization Tools

### `analyze_csr_graph.py`

Analyzes CSR binary and outputs graph statistics:

```bash
python3 python_tool/analyze_csr_graph.py input/graph.bin
```

Outputs include:

- Number of nodes and edges
- Degree distribution (min/avg/max)

---

### `show_cycle.py`

Visualizes DPU-level workload distribution:

```bash
python3 python_tool/show_cycle.py result.txt
```

- Left plot: Max cycle per DPU
- Right plot: Task count per DPU

