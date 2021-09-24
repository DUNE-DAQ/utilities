# toolbox -- Utility classes for dunedaq

## Current Tools

* BufferedFileReader -- Buffered File reader, supporting several compression libraries
* BufferedFileWriter -- Buffered File writer, using O_DIRECT and several compression libraries_
* FileSourceBuffer -- Reads data from a file, pretends to be a data source
* RateLimiter -- Limits actions to a configured rate
* Resolver -- Performs DNS SRV record lookups
* ReusableThread -- Wrapper around a std::thread for executing short-lived tasks
* ThreadHelper -- Executes a given function on a separate thread for long-lived tasks

## Utility Function Library

* stol -- Wrapper for use by lexical_cast
* stoul -- Wrapper for use by lexical_cast
* to_string -- Uses lexical_cast
* safe_enum_cast
* millisleep -- Sleeps for a given number of milliseconds
* strprintf -- Formats a std::string in printf fashion
* shell_expand_paths -- Performs variable subsitutition on path
* shellExpandPath -- Wraps shell_expand_paths
* throw_if_not_file -- Checks that the input path corresponds to an existing filesystem item which is a file
* join -- Converts a vector of strings in a delimiter-separated string.
* get_seconds_since_epoch
* format_timestamp
* vec_fmt
* short_vec_fmt
