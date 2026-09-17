# BusinessDashboard

A modern C++ & Qt desktop application designed specifically for **Customer & Order Overview Management**, featuring an extensible Python plugin architecture.

---

## About The Project

This desktop application was built for a partner business (**GbR**) to provide a centralized **Customer Overview System**. It gives business owners a clear, at-a-glance view of all active clients, assigned projects, industries, and pricing structures in a single dashboard.

> **Note on Portfolio Usage:**  
> This project is published as a showcase for my developer portfolio with explicit permission from the business owners. All sensitive company data, database credentials, and production records have been anonymized or removed.

---

## Current Focus: C++ & Python Plugin System (Work in Progress)

I am currently expanding the application with a dynamic **C++ & Python Plugin Architecture** to allow custom extensions without recompiling the main executable.

* **Embedded Python Engine:** Embedded Python interpreter (`PythonBridge`) to execute custom scripts directly within the C++ environment.
* **Plugin Architecture:** Dynamic plugin manager (`pluginManager`, `pythonPluginAdapter`) designed for custom automated reports and integrations.

---

## Tech Stack

* **Language:** C++ (Core Application) & Python (Plugin System)
* **Framework:** Qt (Qt Widgets, Qt SQL, Qt Resources)
* **Build System:** CMake
* **IDE:** Qt Creator
* **Platform:** Linux / Cross-Platform

---
