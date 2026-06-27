# 🚦 Adaptive Traffic Surveillance Panel & Density Analyzer

A high-throughput C++ control system designed to simulate, analyze, and optimize intersection traffic flows dynamically. 

### 🚀 Core Engineering Features
* **Asynchronous Multi-Threading:** The C++ backend utilizes detached execution threads to handle isolated live data processing streams asynchronously.
* **Anti-Starvation Scheduling Algorithm:** Uses a weighted priority scoring mechanism ($Score = Dynamic\_Count + (Waiting\_Time \times 0.5)$) to prevent high-frequency lane spikes from trapping minor lanes in perpetual red lights.
* **Max Green Overhead Guards:** Implemented algorithmic hard-stops that force-switch green lanes once a time threshold is breached, ensuring fair network resource allocation.
* **Duplex WebSockets:** Streams localized telemetry matrices over lightweight raw communication pipes directly to a responsive UI web shell at $1.2\text{ Hz}$.
