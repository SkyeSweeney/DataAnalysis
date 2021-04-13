#! /usr/bin/python

import sys
import re




########################################################################
# Main application class
########################################################################
class App:

    ####################################################################
    # Constructor
    ####################################################################
    def __init__(self, fn):
        self.fn = fn
        self.out = "qqq.ini"

        self.Tags = {} # Dictionary of all tags
    #

    ####################################################################
    # Main entry
    ####################################################################
    def main(self):

        fin = open(self.fn, "r")
        #fout = open(self.out, "w")

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
                    self.Tags[tag.getTagAdd()] = tag
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
                node = node.addChild("Level%d"%level, tag)
                parent = node.parent
                #print(level, len(parent.children))

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
            self.Tags[tag.getId()] = tag
        except:
            pass
        #

        # Go through tags and look for those with Bodyxxx
        if (False):
            for add, tag in self.Tags.items():
                if (tag.getTagId() == Tag.DW_TAG_structure_type):
                    att = tag.attributes[18]
                    if (att.value[0:4] == "Body"):
                        print(str(att))


        # Lets find the top node for the "master.c" compilation unit
        top = self.findInTree(root, 
                               Tag.DW_TAG_compile_unit, 
                               Att.DW_AT_name, 
                               "master.c")

        if (top == None):
            print("Unable to find a compilation unit for master.c")
            sys.exit(1)
        #

        # Generate the INI file for Body messages 
        self.generateIni(top)
    #


    ####################################################################
    # Create the INI file from the node tree and attribute list
    ####################################################################
    def generateIni(self, top):

        # Lets walk the tree looking for Bodyxxx_t structures
        for node0 in top.children:
            tag = node0.data
            if (tag.id == Tag.DW_TAG_structure_type):
                att = tag.attributes[Att.DW_AT_name]
                m = re.search("Body.*_s", att.value)
                if (m):
                    self.expandStruct(node0)
                #
            #
        #
    #

    ####################################################################
    # Expand a structure tag
    ####################################################################
    def expandStruct(self, node):

        tag = node.data
        att = tag.attributes[Att.DW_AT_name]

        print("*********** Structure: %s Size: %d" % 
                (att.value, 
                 int(tag.attributes[Att.DW_AT_byte_size].value,0)))
        
        # For each item in the structure
        for childNode in node.children:

            # Get the name of each member tag
            tag = childNode.data


            # If this is a TAG_member
            if (tag.id == Tag.DW_TAG_member):

                # Expand the member tag
                self.expandMember(tag)
                pass
            # 
            else:
                print("Not a member")
                sys.exit(1)
            #
        #
    #

    ####################################################################
    # Expand a member tag
    ####################################################################
    def expandMember(self, tag):

        # Lets print the name
        print("  name: %s" % (tag.attributes[Att.DW_AT_name].value))

        # Lets get the type tag
        typeTag = tag.attributes[Att.DW_AT_type]

        # Get the raw value of the type
        typeValue = typeTag.value

        # And now the address of the type
        typeAdd = self.debracket(typeValue)

        # And now the tag itself
        typeTag = self.Tags[typeAdd]

        # If this is the base type
        if (typeTag.id == Tag.DW_TAG_base_type):
            pass

        # If this a further typedef
        elif (typeTag.id == Tag.DW_TAG_typedef):
            pass

        # If this a nested structure
        elif (typeTag.id == Tag.DW_TAG_structure_type):
            pass

        # If this a nested union
        elif (typeTag.id == Tag.DW_TAG_union_type):
            pass
        #

        try:
            typeValue = typeTag.attributes[Att.DW_AT_name].value
            print("  type: %s" % typeValue)
        except:
            print("Failed on", typeTag)
            sys.exit(1)
        #
    #



    ####################################################################
    # Find a given value for a set attribute in a given tag type
    ####################################################################
    def findInTree(self, node, tagId, attId, attValue):

        # Get the data for this node
        tag = node.data

        # If has data
        if (tag != None):

            # If this tag is the right one
            if (tag.id == tagId):

                # Get attribute
                try:
                    att = tag.attributes[attId]
                    # If it is the right value
                    if (att.value == attValue):
                        return node
                    #
                except:
                    pass


            #
        #

        # If we got here, this is not the right Tag, so try children
        for child in node.children:
            found = self.findInTree(child, tagId, attId, attValue)
            if (found):
                return found
            #
        #

        return None

    #

    ####################################################################
    # Print out the node tree
    ####################################################################
    def printTree(self, node):
        print(node.name, str(node.data))
        # Now print the child nodes
        for n in node.children:
            self.printTree(n)
        #
    #


    ####################################################################
    # Remove the <> from a value and convert to an integer
    ####################################################################
    def debracket(self, s):
        s = s[1:-1]
        return int(s, 0)
    #

# App class

########################################################################
# Definition of a Tag
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
    # Constructor
    ####################################################################
    def __init__(self, add, name):
        self.add          = add   # Address of tag
        self.id           = self.nameToId(name) # Tag ID from above list
        self.attributes   = {}    # Dictionary of Attributes by attId
    #

    ####################################################################
    # Get the Tag id
    ####################################################################
    def getTagId(self):
        return self.id
    #

    ####################################################################
    # Get the address of a tag
    ####################################################################
    def getTagAdd(self):
        return self.add
    #

    ####################################################################
    # Add an attribute to a tag
    ####################################################################
    def addAtt(self, att):
        self.attributes[att.id] = att
    #

    ####################################################################
    # Add a child Tag to the current tag
    ####################################################################
    def addChild(self, child):
        self.listChildren.append(child)
    #

    ####################################################################
    # Convert to a string
    ####################################################################
    def __str__(self):
        s = "TAG Add:%x " % (self.add)
        s += "Tag:%s(%d) " % (self.idToName(self.id), self.id)
        s += "NumAtt:%d " % (len(self.attributes))
        s += '\n'
        for key,value in self.attributes.items():
            s += "  " + str(value) + '\n'
        #
        return s
    #

    ####################################################################
    # Convert a tag name to an enumeration
    ####################################################################
    def nameToId(self, name):

        retval = self.DW_TAG_null # If bad name
        if (name in self.DW_TAG):
            retval = self.DW_TAG[name]
        #

        return retval
    #

    ####################################################################
    # Convert Tag enumeration to text string
    ####################################################################
    def idToName(self, id):

        for key,value in self.DW_TAG.items():
            if (value == id):
                return key
            #
        #

        return "DW_TAG_null"
    #
#


########################################################################
# Attribute
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
    # Constructore
    ####################################################################
    def __init__(self, attName, value):
        self.id    = self.nameToId(attName)
        self.value = value
    #

    ####################################################################
    # Convert to string
    ####################################################################
    def __str__(self):
        return "ATT %s(%d) Value=%s" % (self.idToName(self.id), 
                                        self.id, 
                                        str(self.value))
    #

    ####################################################################
    # Convert text name to enumeration
    ####################################################################
    def nameToId(self, name):

        retval = self.DW_AT_null
        if (name in self.DW_AT):
            retval = self.DW_AT[name]
        #

        return retval
    #

    ####################################################################
    # Convert enumeration to text string
    ####################################################################
    def idToName(self, id):

        for key,value in self.DW_AT.items():
            if (value == id):
                return key
            #
        #

        return "DW_AT_null"
    #
#

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
    # Convert to string
    ####################################################################
    def __str__(self):
        s = "NODE Name:%s " % (self.name)
        s += "Children:%d " % (len(self.children))
        s += "Data:%s " % (str(self.data))
        return s
    #

# end Node

########################################################################
#
########################################################################
if __name__ == "__main__":
    obj = App(sys.argv[1])
    obj.main()
#
