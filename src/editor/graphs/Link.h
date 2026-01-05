/**
 * @file Link.h
 * @brief Link structure for connecting graph nodes
 */
#pragma once

/**
 * @brief Represents a connection between two node attributes
 */
struct Link {
    int id = 0;
    int startAttr = 0;  // Output attribute ID
    int endAttr = 0;    // Input attribute ID
    
    Link() = default;
    Link(int linkId, int start, int end) 
        : id(linkId), startAttr(start), endAttr(end) {}
};
