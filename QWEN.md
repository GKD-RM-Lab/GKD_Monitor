
# GKD Monitor Project Context

## Project Overview

GKD Monitor is a Qt-based GUI application designed to monitor the runtime status of a system. It receives data over a TCP connection, where each data point consists of a double-precision floating-point value and a hierarchical name (e.g., "system.cpu.temperature"). The application parses these names and displays the data in a tree structure within a table view.

Key features include:
- Real-time data reception via TCP.
- Hierarchical data display in a tree view.
- Ability to create charts (like line charts) for specific data points for detailed monitoring.
- Planned future features include custom Lua scripting for data processing, alerts, and receiving notifications.

The project uses XMake as its build system and relies on Qt libraries, specifically QtCore, QtGui, QtNetwork, and QtCharts.

## Building and Running

The project uses XMake for building.

### Prerequisites

- XMake
- Qt development libraries (likely Qt5 or Qt6, including Widgets and Charts modules)

### Build Commands

- **Build the project:**
  ```bash
  xmake build
  ```
  The executable will be located in the `build` directory.

- **Run the project:**
  ```bash
  xmake run
  ```

## Codebase Structure and Key Components

- **`src/mainwindow.cpp` / `include/mainwindow.h`**: The main application window. It sets up the UI, initializes the data receiver, creates the dockable tree view for data display, and manages chart widgets. It connects a timer to periodically update the displayed data and simulate some values.
- **`src/value_tree_model.cpp` / `include/value_tree_model.h`**: Implements a `QAbstractItemModel` to represent the hierarchical data received. It uses a custom `ValueTreeItem` class to build the tree structure. The `update()` slot is responsible for integrating new data points into the model and notifying views of changes.
- **`src/chart_widget.cpp` / `include/chart_widget.h`**: Provides a widget for displaying charts. It uses a `QChartView` and can switch between different chart types (currently only Line chart is implemented). It holds a pointer to a `ChartBase` object.
- **`src/charts.cpp` / `include/charts.h`**: Defines the `ChartBase` abstract class and concrete implementations like `LineChart`. These classes handle the QtCharts series and axes, and the logic for updating the chart with new data points, including removing old ones.
- **`xmake.lua`**: The XMake configuration file defining the target, source files, Qt rules, and required frameworks.
- **`config.ini`**: Likely contains application settings, such as the TCP port to listen on.
- **`ui/`**: Contains Qt Designer `.ui` files for the main window and chart widget.

## Development Conventions

- **Language:** C++23 (as specified in `xmake.lua`).
- **Framework:** Qt, using Widgets and Charts modules.
- **Build System:** XMake.
- **UI Design:** UI files (`.ui`) are likely created with Qt Designer and integrated into the C++ code.
- **Data Handling:** Data is managed centrally, likely through a `valueManager` (referenced in `mainwindow.cpp` and `value_tree_model.cpp`) which seems to be a global or singleton for storing and retrieving named values.
- **Charts:** Charting is abstracted through a base class (`ChartBase`) allowing for different chart types.