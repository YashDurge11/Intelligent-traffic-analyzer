# 🚦 Adaptive Traffic Surveillance Panel & Density Analyzer

A high-throughput C++ control system designed to simulate, analyze, and optimize intersection traffic flows dynamically. 

### 🚀 Core Engineering Features
* **Asynchronous Multi-Threading:** The C++ backend utilizes detached execution threads to handle isolated live data processing streams asynchronously.
* **Algorithmic Signal Allocation:** Implemented a real-time scheduling algorithm to analyze queue structures and clear high-density lanes dynamically.
* **Duplex WebSockets:** Streams localized telemetry matrices over lightweight raw communication pipes directly to a responsive UI web shell.
