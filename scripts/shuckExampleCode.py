#!/usr/bin/python

import re
import sys, getopt, os

def main(argv):
   inputfile = ''
   outputfile = ''
   try:
      opts, args = getopt.getopt(argv,"hi:o:",["ifile=","odir="])
   except getopt.GetoptError:
      print 'shuckExampleCode.py -i <inputfile> -o <outputdirectory>'
      sys.exit(2)
   for opt, arg in opts:
      if opt == '-h':
         print 'shuckExampleCode.py -i <inputfile> -o <outputdirectory>'
         sys.exit()
      elif opt in ("-i", "--ifile"):
         sourceFile = arg
      elif opt in ("-o", "--odir"):
         destDir = arg
         
         writeState = False
         exampleFile = None

         with open( sourceFile, 'r') as source:
             print("Processing " + sourceFile + " to " + destDir)
             for line in source:
                 #print(line)
                 line = line.expandtabs(4) #tabs to spaces
                 endMarker = re.search(r"!!!END",line)
                 skipMarker = re.search(r"!!!SKIP",line)
                 startMarker = re.search(r"^(?P<leading_space>[ ]*)[^!]*!!!(?P<code_name>\w+)",line)
                 if endMarker:
                     exampleFile.close()
                     writeState = False
                     leading_space = None
                     #print("CLOSE")

                 elif skipMarker:
                    pass

                 elif startMarker:
                        leading_space = startMarker.group('leading_space')
                        if writeState:
                            #close current example file
                            exampleFile.close()

                        fileName = destDir + os.sep + startMarker.group('code_name') + ".txt"
                        exampleFile = open(fileName,'w')
                        writeState = True
                        #print("Open " + fileName)
             
                 else:
                    if writeState:
                        if leading_space and line.startswith(leading_space):
                            trimmed = line[len(leading_space):]
                            if len(trimmed) == 0:
                                trimmed = "\n"
                                print("Zero line " + line)
                        else:
                            trimmed = line

                        exampleFile.write(trimmed)
                        #print(trimmed)

                    else:
                        pass #line not part of example code

                                     
if __name__ == "__main__":
    main(sys.argv[1:])
                        