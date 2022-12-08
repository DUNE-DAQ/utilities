# utilities: Utility classes for dunedaq

## Current Tools

* `Resolver` -- Performs DNS SRV record lookups
* `ReusableThread` -- Wrapper around a `std::thread` for executing short-lived tasks
* [`WorkerThread`](WorkerThread-Usage-Notes/) -- Wrapper around a `std::thread` for long-lived tasks (e.g. DAQModule work loops) 

### API Diagram

![Class Diagrams](https://github.com/DUNE-DAQ/utilities/raw/develop/docs/utilities.png)