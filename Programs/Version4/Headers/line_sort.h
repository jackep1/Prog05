#ifndef LINE_SORT_H
#define LINE_SORT_H

#include <string>
#include <map>

/// Reorders fragments of text while removing line number labels.
///
/// @param fragments The fragments of text to reorder, indexed by line number.
/// @return A string containing the ordered text.
std::string unlabeledReorder(const std::map<int, std::string>& fragments);

/// Reorders fragments of text while retaining line number labels.
///
/// @param fragments The fragments of text to reorder, indexed by line number.
/// @return A string containing the ordered text.
std::string labeledReorder(const std::map<int, std::string>& fragments);

#endif //LINE_SORT_H
