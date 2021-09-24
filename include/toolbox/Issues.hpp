/**
 * @file Issues.hpp ERS issues used by the toolbox functions and classes
 *
 * This is part of the DUNE DAQ , copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */
#ifndef TOOLBOX_INCLUDE_TOOLBOX_ISSUES_HPP_
#define TOOLBOX_INCLUDE_TOOLBOX_ISSUES_HPP_

#include <ers/Issue.hpp>

#include <string>

namespace dunedaq {

// Disable coverage collection LCOV_EXCL_START
ERS_DECLARE_ISSUE(toolbox,
                  BufferedReaderWriterConfigurationError,
                  "Configuration Error: " << conferror,
                  ((std::string)conferror))

ERS_DECLARE_ISSUE(toolbox, GenericConfigurationError, "Configuration Error: " << conferror, ((std::string)conferror))

ERS_DECLARE_ISSUE(toolbox, CannotOpenFile, "Couldn't open binary file: " << filename, ((std::string)filename))
ERS_DECLARE_ISSUE(toolbox,
                  BufferedReaderWriterCannotOpenFile,
                  "Couldn't open file: " << filename,
                  ((std::string)filename))

ERS_DECLARE_ISSUE(toolbox,                         ///< Namespace
                  FileNotFound,                    ///< Issue class name
                  file_path << " does not exist!", ///< Message
                  ((std::string)file_path)         ///< Message parameters
)
ERS_DECLARE_ISSUE(toolbox,                   ///< Namespace
                  CorruptedFile,             ///< Issue class name
                  file_path << " corrupted", ///< Message
                  ((std::string)file_path)   ///< Message parameters
)
ERS_DECLARE_ISSUE_BASE(toolbox,                        ///< Namespace
                       FileIsDirectory,                ///< Issue class name
                       CorruptedFile,                  ///< Base class of the issue
                       file_path << " is a directory", ///< Log Message from the issue
                       ((std::string)file_path),       ///< Base class attributes
                       ERS_EMPTY                       ///< Attribute of this class
)

ERS_DECLARE_ISSUE(toolbox,                             // Namespace
                  ThreadingIssue,                      // Issue Class Name
                  "Threading Issue detected: " << err, // Message
                  ((std::string)err))                  // Message parameters
// Reenable coverage collection LCOV_EXCL_STOP
} // namespace dunedaq

#endif // TOOLBOX_INCLUDE_TOOLBOX_ISSUES_HPP_
