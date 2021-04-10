#! /usr/bin/python

import sys

########################################################################
# Create a tree of nodes
########################################################################
class Node:

    ####################################################################
    # Constructor for a new node.
    # A node gets a name, some data, and a pointer to its parent
    # It also gets a null list of children
    ####################################################################
    def __init__(self, name, data, parent):
        self.name     = name 
        self.data     = data
        self.children = []      # List of children nodes
        self.parent   = parent  # Parent node
    #

    ####################################################################
    # getParent
    ####################################################################
    def getParent(self):
        return self.parent
    #

    ####################################################################
    # addChild
    ####################################################################
    def addChild(self, name, data):
        child = Node(name, data, self)
        self.children.append(child)
        return child
    #

    ####################################################################
    #
    ####################################################################
    def setName(self, name):
        self.name = name
    #

    ####################################################################
    #
    ####################################################################
    def setData(self, data):
        self.data = data
    #

    ####################################################################
    # Convert to string
    ####################################################################
    def __str__(self):
        s = "Name:%s " % (self.name)
        s += "Children:%d " % (len(self.children))
        return s
    #

#




########################################################################
#
########################################################################
class App:

    ####################################################################
    #
    ####################################################################
    def __init__(self, fn):
        self.fn = fn
        self.out = "qqq.ini"

        self.dictTags = {} # Dictionary of all tags
    #

    ####################################################################
    #
    ####################################################################
    def main(self):

        fin = open(self.fn, "r")
        fout = open(self.out, "w")

        # This is the top of the tree
        root = Node("Root", None, None)

        # This the current working node
        node = root

        lastLevel = -1

        lineno = 0

        tag = Tag(0, "DW_TAG_null")

        # For each line in the DWARF listing
        for line in fin:

            # Keep track of line number for errors
            lineno += 1

            # Strip whitespace from line
            line = line.strip()

            # Skip if a blank line
            if (len(line) == 0):
                continue

            # Skip full line comments
            if (line[0] == '#'):
                continue

            # Skip partial line comments
            i = line.find("#")
            if (i > 0):
                line = line[0:i]
            #    

            # If this is a tag line
            if (line[0] == "<"):

                # Save the accumulated tag and its attributes 
                # to the dictionary of tags
                try:
                    self.dictTags[tag.getTagAdd()] = tag
                except:
                    pass
                #

                # Pull out level, add, and tag from line
                level = int(line[1:3].strip(), 0)
                add = int(line[5:15], 0)
                i = line.find("DW")
                tagName = line[i:]

                # Create the base tag for the next tag and attributes
                tag = Tag(add, tagName)

                # If this is the same level
                if (level == lastLevel):

                    # Get the parent
                    node = node.getParent()

                # If we went down one level 
                # PS: we can't go down more than one level at a time
                elif (level == lastLevel+1):

                    # Add the child to the current child
                    pass

                # If we went up n levels
                elif (level < lastLevel):

                    # Go get the right parent
                    for i in range(lastLevel-level + 1):

                        # Get the parent
                        node = node.getParent()
                    #

                else:
                    print("Should not get here!")
                    sys.exit(1)
                #

                # Add a child node to the parent node
                node = node.addChild("%d"%level, tag)

                lastLevel = level


            # Try to read attributes
            else:

                if ("DW_AT_" in line):

                    # Split line in to tokens
                    toks = line.split()

                    s = ' '.join(toks[1:])

                    # Create the attribute
                    att = Att(toks[0], s)

                    # Add attribute to tag
                    tag.addAtt(att)

                    del(att)
                #    

            # tag/att

        # for each line

        # Save the accumulated tag and its attributes 
        # to the dictionary of tags
        try:
            tag
            self.dictTags[tag.getId()] = tag
        except:
            print("bang")
            pass
        #

        for add, tag in self.dictTags.items():
            print(str(tag))
    #


    ####################################################################
    #
    ####################################################################
    def debracket(self, s):
        s = s[1:-1]
        return int(s, 0)
    #

# App class

########################################################################
#
########################################################################
class Tag:

    DW_TAG_null             = 0
    DW_TAG_array_type       = 1
    DW_TAG_base_type        = 2
    DW_TAG_compile_unit     = 3
    DW_TAG_const_type       = 4
    DW_TAG_enumeration_type = 5
    DW_TAG_enumerator       = 6
    DW_TAG_formal_parameter = 7
    DW_TAG_member           = 8
    DW_TAG_pointer_type     = 9
    DW_TAG_structure_type   = 10
    DW_TAG_subprogram       = 11
    DW_TAG_subrange_type    = 12
    DW_TAG_typedef          = 13
    DW_TAG_union_type       = 14
    DW_TAG_variable         = 15

    DW_TAG = {"DW_TAG_array_type":       DW_TAG_array_type,
              "DW_TAG_base_type":        DW_TAG_base_type,
              "DW_TAG_compile_unit":     DW_TAG_compile_unit,
              "DW_TAG_const_type":       DW_TAG_const_type,
              "DW_TAG_enumeration_type": DW_TAG_enumeration_type,
              "DW_TAG_enumerator":       DW_TAG_enumerator,
              "DW_TAG_formal_parameter": DW_TAG_formal_parameter,
              "DW_TAG_member":           DW_TAG_member,
              "DW_TAG_pointer_type":     DW_TAG_pointer_type,
              "DW_TAG_structure_type":   DW_TAG_structure_type,
              "DW_TAG_subprogram":       DW_TAG_subprogram,
              "DW_TAG_subrange_type":    DW_TAG_subrange_type,
              "DW_TAG_typedef":          DW_TAG_typedef,
              "DW_TAG_union_type":       DW_TAG_union_type,
              "DW_TAG_variable":         DW_TAG_variable}

    ####################################################################
    #
    ####################################################################
    def __init__(self, add, tagName):
        self.add          = add   # Address of tag
        self.tagId        = self.nameToId(tagName) # Tag ID from above list
        self.listAtt      = []    # List of Att objects for Tag
    #

    ####################################################################
    #
    ####################################################################
    def setTagId(self, tagId):
        self.tagId  = tagId # Tag ID from above list
    #

    ####################################################################
    #
    ####################################################################
    def getTagId(self):
        return self.tagId
    #

    ####################################################################
    #
    ####################################################################
    def getTagAdd(self):
        return self.add
    #

    ####################################################################
    #
    ####################################################################
    def addAtt(self, att):
        self.listAtt.append(att)
    #

    ####################################################################
    #
    ####################################################################
    def addChild(self, child):
        self.listChildren.append(child)
    #

    ####################################################################
    #
    ####################################################################
    def __str__(self):
        s = "Add:%x " % (self.add)
        s = "Tag:%s(%d) " % (self.idToName(self.tagId), self.tagId)
        s += "NumAtt:%d " % (len(self.listAtt))
        s += '\n'
        for i in range(len(self.listAtt)):
            s += "  " + str(self.listAtt[i]) + '\n'
        #
        return s
    #

    ####################################################################
    #
    ####################################################################
    def nameToId(self, tag):

        retval = self.DW_TAG_null
        if (tag in self.DW_TAG):
            retval = self.DW_TAG[tag]
        #

        return retval
    #

    ####################################################################
    #
    ####################################################################
    def idToName(self, tagId):

        for key,value in self.DW_TAG.items():
            if (value == tagId):
                return key
            #
        #

        return "DW_TAG_null"
    #
#


########################################################################
#
########################################################################
class Att:

    DW_AT_bit_offset            = 0
    DW_AT_bit_size              = 1
    DW_AT_byte_size             = 2
    DW_AT_comp_dir              = 3
    DW_AT_const_value           = 4
    DW_AT_data_member_location  = 5
    DW_AT_declaration           = 6
    DW_AT_decl_column           = 7
    DW_AT_decl_file             = 8
    DW_AT_decl_line             = 9
    DW_AT_encoding              = 10
    DW_AT_external              = 11
    DW_AT_frame_base            = 12
    DW_AT_GNU_all_call_sites    = 13
    DW_AT_high_pc               = 14
    DW_AT_language              = 15
    DW_AT_location              = 16
    DW_AT_low_pc                = 17
    DW_AT_name                  = 18
    DW_AT_producer              = 19
    DW_AT_prototyped            = 20
    DW_AT_sibling               = 21
    DW_AT_stmt_list             = 22
    DW_AT_type                  = 23
    DW_AT_upper_bound           = 24
    DW_AT_null                  = 99

    DW_AT = {"DW_AT_bit_offset":           DW_AT_bit_offset,
             "DW_AT_bit_size":             DW_AT_bit_size,
             "DW_AT_byte_size":            DW_AT_byte_size,
             "DW_AT_comp_dir":             DW_AT_comp_dir,
             "DW_AT_const_value":          DW_AT_const_value,
             "DW_AT_data_member_location": DW_AT_data_member_location,
             "DW_AT_declaration":          DW_AT_declaration,
             "DW_AT_decl_column":          DW_AT_decl_column,
             "DW_AT_decl_file":            DW_AT_decl_file,
             "DW_AT_decl_line":            DW_AT_decl_line,
             "DW_AT_encoding":             DW_AT_encoding,
             "DW_AT_external":             DW_AT_external,
             "DW_AT_frame_base":           DW_AT_frame_base,
             "DW_AT_GNU_all_call_sites":   DW_AT_GNU_all_call_sites,
             "DW_AT_high_pc":              DW_AT_high_pc,
             "DW_AT_language":             DW_AT_language,
             "DW_AT_location":             DW_AT_location,
             "DW_AT_low_pc":               DW_AT_low_pc,
             "DW_AT_name":                 DW_AT_name,
             "DW_AT_producer":             DW_AT_producer,
             "DW_AT_prototyped":           DW_AT_prototyped,
             "DW_AT_sibling":              DW_AT_sibling,
             "DW_AT_stmt_list":            DW_AT_stmt_list,
             "DW_AT_type":                 DW_AT_type,
             "DW_AT_upper_bound":          DW_AT_upper_bound}


    ####################################################################
    #
    ####################################################################
    def __init__(self, attName, attValue):
        self.attId    = self.nameToId(attName)
        self.attValue = attValue
    #

    ####################################################################
    #
    ####################################################################
    def __str__(self):
        return "Att %s(%d) Value=%s" % (self.idToName(self.attId), 
                                        self.attId, 
                                        str(self.attValue))
    #

    ####################################################################
    #
    ####################################################################
    def nameToId(self, tag):

        retval = self.DW_AT_null
        if (tag in self.DW_AT):
            retval = self.DW_AT[tag]
        #

        return retval
    #

    ####################################################################
    #
    ####################################################################
    def idToName(self, attId):

        for key,value in self.DW_AT.items():
            if (value == attId):
                return key
            #
        #

        return "DW_AT_null"
    #
#

if __name__ == "__main__":
    obj = App(sys.argv[1])
    obj.main()
#
